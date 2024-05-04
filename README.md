# ESP32_I2S_Monitor_Recorder

Sometimes the only way to debug realtime I2S output is to get the exact signal recorded and analyze it with a "magnifying glass", count samples and measure amplitudes. This simple tool is what I've made for such cases.

OLED display shows some helpful info: D-weighted frequency response, momentary signal form and used dynamic range percentage.

On clicking "BOOT" key of the devboard the recording is launched, display shows samples count. Second press stops the recording and finalizes the writing of "sound.wav" in the root of the SD (microSD) card.


## Connections:

PIN_I2S_BCLK 25

 PIN_I2S_DIN 26

 PIN_I2S_LRC 27
 
_
 
 SDMMC_D0  2

 SDMMC_D1  4

 SDMMC_D2  12
 
 SDMMC_D3  13

_

 SDA_PIN 21
 
 SCL_PIN 22

 SDMMC_CLK 14

 SDMMC_CMD 15
