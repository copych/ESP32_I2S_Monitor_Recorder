

union wavHeader{
    uint8_t wavHdr[44];
    struct{
        char riff[4] = {'R', 'I', 'F', 'F'};
        uint32_t fileSize; 
        char waveType[4] = {'W', 'A', 'V', 'E'};
        char format[4] = {'f', 'm', 't', ' '};
        uint32_t lengthOfData = 16; 			// PCM
        uint16_t audioFormat = 1;   			// PCM
        uint16_t numberOfChannels = 2; 			// stereo
        uint32_t sampleRate = 44100;
        uint32_t byteRate = 44100 * 2 * 2; 		// sample rate * number of channels * bytes per sample
        uint16_t blockAlign = 16 * 2; 			// bits per sample (all channels)
        uint16_t bitsPerSample = 16;
        char dataStr[4] = {'d', 'a', 't', 'a'};
        uint32_t dataSize;  
    };
};


void initSDCard(){
  gpio_pulldown_dis((gpio_num_t)SDMMC_D0);
  gpio_pulldown_dis((gpio_num_t)SDMMC_D1);
  gpio_pulldown_dis((gpio_num_t)SDMMC_D3);
  gpio_pulldown_dis((gpio_num_t)SDMMC_CLK);
  gpio_pulldown_dis((gpio_num_t)SDMMC_CMD);
  gpio_pullup_en((gpio_num_t)SDMMC_D0);
  gpio_pullup_en((gpio_num_t)SDMMC_D1);
  gpio_pullup_en((gpio_num_t)SDMMC_D3);
  gpio_pullup_en((gpio_num_t)SDMMC_CLK);
  gpio_pullup_en((gpio_num_t)SDMMC_CMD);

  if(!SD_MMC.begin()){
    Serial.println("Card Mount Failed");
    sdOk = false;
  } else {
    sdOk = true;
  }
  uint8_t cardType = SD_MMC.cardType();
 
  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  } 
 
  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }
  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
}


void CreateWavHeader(uint8_t * header, size_t waveDataSize){
  union wavHeader wav;
  wav.dataSize = dataWritten;
  wav.fileSize = dataWritten + headerSize;
  for (int i=0; i< headerSize; i++) {
    header[i] = wav.wavHdr[i];
  }
}

  
void startRec(const String &filename){
  if (sdOk) {
    uint8_t cardType = SD_MMC.cardType();
    if(cardType == CARD_NONE){
      Serial.println("No SD card attached");
      return;
    } else {
      CreateWavHeader(header, 100000);
      SD_MMC.remove(filename);
      file = SD_MMC.open(filename, FILE_WRITE);
      if (!file) return;
      file.write(header, headerSize);
      dataWritten = headerSize;
      inRecording = true;
    }
  }
}

void stopRec() {
  if (inRecording) {
    file.seek(0);
    CreateWavHeader(header, dataWritten);
    file.write(header, headerSize);
    
    file.close();
    Serial.println("finish");
    inRecording = false;
  }
}

void processRec() {
    if (size_t nBytes = file.write((const byte*)samples, bytesToRead)) {
      dataWritten += nBytes ;
    } else {
      DEBUG("error writing");
    }
}
