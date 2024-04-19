i2s_config_t i2s_config = {
  .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
  .sample_rate = 100000,
  .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
  .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
  .communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_I2S_MSB,
  .intr_alloc_flags = 0,
  .dma_buf_count = 2,
  .dma_buf_len = 32,
  .use_apll = 0
};

void stopSinus(){
    dac_output_disable(DAC_CHANNEL_2);
    initDone=false;
}

void startSinus() {
  dac_output_enable(DAC_CHANNEL_2);
  SET_PERI_REG_MASK(SENS_SAR_DAC_CTRL1_REG, SENS_SW_TONE_EN);
  SET_PERI_REG_MASK(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_CW_EN2_M);
  SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_INV2, 2, SENS_DAC_INV2_S);
  initDone = true;
}

double sinusSetFrequency(double frequency) {
  if (!initDone) startSinus();
  double f, delta, delta_min = 999999999.0;
  uint16_t divi = 0, step = 1, s;
  uint8_t clk_8m_div = 0;  //0 bis 7
  for (uint8_t div = 1; div < 9; div++) {
    s = round(frequency * div / SINFAKT);
    if ((s > 0) && ((div == 1) || (s < 1024))) {
      f = SINFAKT * s / div;
      delta = abs(f - frequency);
      if (delta < delta_min) {
        step = s;
        divi = div - 1;
        delta_min = delta;
      }
    }
  }
  frequency = SINFAKT * step / (divi + 1);
  REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_DIV_SEL, divi);
  SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL1_REG, SENS_SW_FSTEP, step, SENS_SW_FSTEP_S);
  return frequency;
}
