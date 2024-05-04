#define DEBUG_ON

/* Uncomment the initialize the I2C address , uncomment only one, If you get a totally blank screen try the other*/
#define i2c_Address 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's
//#define i2c_Address 0x3d //initialize with the I2C addr 0x3D Typically Adafruit OLED's

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define WHITE 1
#define OLED_RESET -1   //   QT-PY / XIAO


#define PIN_I2S_BCLK 25
#define PIN_I2S_DIN 26
#define PIN_I2S_LRC 27
#define PIN_I2S_DOUT -1
 
#define SDMMC_D0  2
#define SDMMC_D1  4
#define SDMMC_D2  12
#define SDMMC_D3  13
#define SDMMC_CLK 14
#define SDMMC_CMD 15
 

#define I2S_MODE I2S_MODE_RX 
#define SAMPLE_RATE 44100

#define SAMPLES_BIT 10 // 8 - 10
#define DOWNSAMPLE_BIT 1 // we don't actualy need 10-20khz range for visializing. if not so, set to 0
#define SAMPLES (1<<SAMPLES_BIT)
#define DOWNSAMPLE (1<<DOWNSAMPLE_BIT)
#define FFT_BIT (SAMPLES_BIT-DOWNSAMPLE_BIT )
#define FFT_SIZE (1<<FFT_BIT)

const size_t bytesToRead = (SAMPLES * 2 * 2 ); // 16 bit stereo
char communicationData[bytesToRead];

#define DMA_BUF_LEN SAMPLES
#define DMA_BUF_NUM 2

#define FPS 30
const int FRAME_TIME = (1000 / FPS) ;

// Debugging macros
#ifndef MIDI_VIA_SERIAL
  #ifndef DEB
    #ifdef DEBUG_ON
      #define DEB(...) Serial.print(__VA_ARGS__) 
      #define DEBF(...) Serial.printf(__VA_ARGS__) 
      #define DEBUG(...) Serial.println(__VA_ARGS__) 
    #else
      #define DEB(...)
      #define DEBF(...)
      #define DEBUG(...)
    #endif
  #endif
#else
      #define DEB(...)
      #define DEBF(...)
      #define DEBUG(...)
#endif
