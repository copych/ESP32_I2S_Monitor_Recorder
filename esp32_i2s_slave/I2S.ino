
/* implementation */


void I2S_Init(i2s_mode_t MODE, i2s_bits_per_sample_t BPS)
{
  i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_SLAVE | I2S_MODE_RX   ),
      .sample_rate = SAMPLE_RATE,
      .bits_per_sample = BPS,
      .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
      .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
      .intr_alloc_flags = 0,
      .dma_buf_count = DMA_BUF_NUM,
      .dma_buf_len = DMA_BUF_LEN
    };
  if (MODE == I2S_MODE_RX || MODE == I2S_MODE_TX) {
    Serial.println("using I2S_MODE");
    i2s_pin_config_t pin_config;
    pin_config.bck_io_num = PIN_I2S_BCLK;
    pin_config.ws_io_num = PIN_I2S_LRC;
    if (MODE == I2S_MODE_RX) {
      pin_config.data_out_num = I2S_PIN_NO_CHANGE;
      pin_config.data_in_num = PIN_I2S_DIN;
    } 
    else if (MODE == I2S_MODE_TX) {
      pin_config.data_out_num = PIN_I2S_DOUT;
      pin_config.data_in_num = I2S_PIN_NO_CHANGE;
    }

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
    i2s_set_clk(I2S_NUM_0, SAMPLE_RATE, BPS, I2S_CHANNEL_STEREO);
  } 
  else if (MODE == I2S_MODE_DAC_BUILT_IN || MODE == I2S_MODE_ADC_BUILT_IN) {
    Serial.println("using ADC_builtin");
    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    // GPIO36, VP
    i2s_set_adc_mode(ADC_UNIT_1, ADC1_CHANNEL_0);  
  }
}

size_t I2S_Read(char *data, size_t numData)
{
  size_t bytes_read;
  // i2s_read(i2s_port_ti2s_num, void *dest, size_t size, size_t *bytes_read, TickType_t ticks_to_wait)
  i2s_read(I2S_NUM_0, (char *)data, numData, &bytes_read, portMAX_DELAY);
  return bytes_read;
}

/*
size_t I2S_Write(char &data, size_t numData)
{
  size_t bytes_written;
//  i2s_write(i2s_port_ti2s_num, const void *src, size_t size, size_t *bytes_written, TickType_t ticks_to_wait)
  i2s_write(I2S_NUM_0, (const char *)data, numData, &bytes_written, portMAX_DELAY);
  return bytes_written;
}
*/
