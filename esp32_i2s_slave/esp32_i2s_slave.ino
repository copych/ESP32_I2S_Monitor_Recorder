#include "driver/i2s.h"
#include "config.h"
#include "FS.h"
#include "SD_MMC.h"
#include "SPI.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h> 

#include "biquad.h"

Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int16_t ylim = SCREEN_HEIGHT - 1;                                     // OLED y-axis drawing boundary limit
float k = SCREEN_WIDTH / FFT_SIZE;
float k1 = FFT_SIZE / (SCREEN_WIDTH*2);

bool inRecording = false;
bool sdOk = true;
size_t dataWritten = 0;
File file;

const size_t headerSize = 44;
byte header[headerSize];

int16_t re[SAMPLES], im[SAMPLES], disp[SAMPLES];                                // real and imaginary FFT result arrays

int old_dat[SCREEN_WIDTH] , posOffset[SCREEN_WIDTH+1];

union {
  byte bytes[4];
  uint32_t raw_data;
  int16_t channels[2];
} samples[SAMPLES];

int16_t mono[SAMPLES];
int16_t aWeighted[SAMPLES];

int sampMin = 0;
int sampMax = 0;
size_t minMs;
size_t maxMs;
const size_t holdMs = 15000;
int peakVal = 0x7fff; // 100%

const float downscale = 0.95f * SCREEN_HEIGHT / 0x10000  ; 
float scaler = downscale;
const int offset = SCREEN_HEIGHT / 2;
int16_t offset1=0;
size_t bytes_read;
static int32_t avg=0;

/*
 * // weighting curves for fft drawing
// a-weighting
Biquad BQ1(1.0, -1.984243452024144, 0.9842938017035889, 0.9921343134319333, -1.9842686268638665, 0.9921343134319333 );
Biquad BQ2(1.0, -1.1600661576224323, 0.24530901534380437, 0.37734549232809783, 0.0, -0.37734549232809783);

// k-weighting
Biquad BQ1(1.0, -1.6604216374364655, 0.7102394870461437, 1.5890090791074565, -2.7559893102826307, 1.216798080784852 );
Biquad BQ2(1.0, -1.9892010416922554, 0.9892301960673886, 0.994607809439911, -1.989215618879822, 0.994607809439911);

// b-weighting
Biquad BQ1(1.0, 0.7834210806668332, -0.13630637970332993, 0.41177867524087575, 0.8235573504817515, 0.41177867524087575);
Biquad BQ2(1.0, -1.9752758935835153, 0.975374134952398, 0.9876625071339783, -1.9753250142679566, 0.9876625071339783);

*/
// d-weighting
Biquad BQ1(1.0, -1.3701514355674782, 0.5482878830715046, 1.6732931054114215, -1.3701514355674782, -0.12500522233991684);
Biquad BQ2(1.0, 1.00383414763463, 0.16248913085817168, 0.5415808196232005, 1.083161639246401, 0.5415808196232005);
Biquad BQ3(1.0, -1.9718553118085056, 0.9719053471420027, 0.985940164737627, -1.971880329475254, 0.985940164737627);
Biquad BQ4(1.0, -1.8105617349742942, 0.832209755409449, 0.9690391212829926, -1.8105617349742942, 0.8631706341264563);


TaskHandle_t Task1;
TaskHandle_t Task2;


void setup() {
  Serial.begin(115200); 
  btStop(); // we don't want bluetooth to consume our precious cpu time 

  initButtons();
  initSDCard();
 
  I2S_Init(I2S_MODE, I2S_BITS_PER_SAMPLE_16BIT);
 
  display.begin(i2c_Address, true); // Address 0x3C default
  display.clearDisplay();                            // blank the display
  display.setTextSize(1);                             // configure text properties
  display.setTextColor(WHITE);
  display.display(); 

  xTaskCreatePinnedToCore( task1, "Task1", 8000, NULL, 1, &Task1, 0 );
  xTaskCreatePinnedToCore( task2, "Task2", 8000, NULL, 1, &Task2, 1 );
  xTaskNotifyGive(Task1);
  xTaskNotifyGive(Task2);
}

void loop() {
  taskYIELD();
  processButtons();
}


static void task1(void *userData) { // core 0 
  while (true) {
    bytes_read = I2S_Read((char *)&samples, bytesToRead);
    if (inRecording) {
      processRec();
    } else {
      for (int i=0; i< SAMPLES ; ++i) {
        mono[i] = ( (float)(samples[i].channels[0]) + (float)(samples[i].channels[1]) ) * 0.5f - avg ;
      }
    }
    xTaskNotifyGive(Task2);
  }
}


static void task2(void *userData) { // core 1 along with Arduino
  size_t lastTime ;
  lastTime = millis();
  while (true) {
    if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY) ) {
      if (millis()-lastTime>FRAME_TIME) {
        taskYIELD();
        if (!inRecording) {
          for (int i=0; i< SAMPLES ; ++i) {
            avg = 0.001f * ( mono[i] + avg * 999) ;
            aWeighted[i] = BQ4.filter(BQ3.filter(BQ2.filter(BQ1.filter((float)(mono[i])))));
            // aWeighted[i] = BQ3.filter(BQ2.filter(BQ1.filter((float)(mono[i]))));
            // aWeighted[i] = BQ2.filter(BQ1.filter((float)(mono[i])));
            // aWeighted[i] = mono[i];
          }
          
          
          for (int i = 0; i < FFT_SIZE; ++i) {
            int j = i * DOWNSAMPLE;
            re[i] = (int)(( aWeighted[j] ) );
            disp[i] = (int)( mono[j] * scaler);
            im[i] = 0;
            int absVal = abs(mono[j]);
            if (absVal > sampMax) { sampMax = absVal ; maxMs = millis(); }
          }
          
          if (millis() > maxMs + holdMs) { sampMax *= 0.98; }

          scaler = downscale * 0x7fff / (float)sampMax ;
          
          fix_fft(re, im, FFT_BIT, 0);                              // send the samples for FFT conversion, returning the real/imaginary results in the same arrays
           
          taskYIELD();
          
          drawPeak();
          drawWave();
          drawFFT();
        } else {
          drawRec();
        }
        display.display();
        lastTime = millis();
        display.clearDisplay();
        taskYIELD();
      } else {
        taskYIELD();
      }
    }
  }
}
