/* It's your choice here how to do with the data. Here is just an example to print them to UART */
void AD5940_ReadWriteNBytes(unsigned char *pSendBuffer, unsigned char *pRecvBuff, unsigned long length)
{
  //set SPI settings for the following transaction
  //speedMaximum: 12MHz found to be max for Adafruit Feather M0, AD5940 rated for max 16MHz clock frequency
  //dataOrder: MSB first
  //dataMode: SCLK idles low/ data clocked on SCLK falling edge --> mode 0
  // SPI.beginTransaction(SPISettings(800000, MSBFIRST, SPI_MODE0));
  SPI.beginTransaction(SPISettings(1200000, MSBFIRST, SPI_MODE0));

  for (int i = 0; i < length; i++)
  {
    *pRecvBuff++ = SPI.transfer(*pSendBuffer++);  //do a transfer
  }

  SPI.endTransaction(); //transaction over
}

void AD5940_CsClr(void)
{
  digitalWrite(SPI_CS_AD5940_Pin, LOW);
}

void AD5940_CsSet(void)
{
  digitalWrite(SPI_CS_AD5940_Pin, HIGH);
}

void AD5940_RstSet(void)
{
  digitalWrite(AD5940_ResetPin, HIGH);
}

void AD5940_RstClr(void)
{
  digitalWrite(AD5940_ResetPin, LOW);
}

void AD5940_Delay10us(uint32_t time)
{
  //Warning: micros() only has 4us (for 16MHz boards) or 8us (for 8MHz boards) resolution - use a timer instead?
  unsigned long time_last = micros();
  while (micros() - time_last < time * 10) // subtraction handles the roll over of micros()
  {
    //wait
  }
}

// void AD5940_Delay10us(uint32_t time)
// {
// 	delayMicroseconds(10 * time);
// }


//declare the following function in the ad5940.h file if you use it in other .c files (that include ad5940.h):
//unsigned long AD5940_GetMicros(void);
//used for time tests:
// unsigned long AD5940_GetMicros()
// {
//   return micros();
// }

uint32_t AD5940_GetMCUIntFlag(void)
{
  return ucInterrupted;
}

uint32_t AD5940_ClrMCUIntFlag(void)
{
  ucInterrupted = 0;
  return 1;
}

uint32_t AD5940_MCUResourceInit(void *pCfg)
{
  /* Step1, initialize SPI peripheral and its GPIOs for CS/RST */
  //start the SPI library (setup SCK, MOSI, and MISO pins)
  SPI.begin();
  //initalize SPI chip select pin
  pinMode(SPI_CS_AD5940_Pin, OUTPUT);
  //initalize Reset pin
  pinMode(AD5940_ResetPin, OUTPUT);

  /* Step2: initialize GPIO interrupt that connects to AD5940's interrupt output pin(Gp0, Gp3, Gp4, Gp6 or Gp7 ) */
  //init AD5940 interrupt pin
  pinMode(AD5940_IntPin, INPUT_PULLUP);
  //attach ISR for falling edge
  attachInterrupt(digitalPinToInterrupt(AD5940_IntPin), Ext_Int0_Handler, FALLING);

  //chip select high to de-select AD5940 initially
  AD5940_CsSet();
  AD5940_RstSet();
  return 0;
}

/* MCU related external line interrupt service routine */
//The interrupt handler handles the interrupt to the MCU
//when the AD5940 INTC pin generates an interrupt to alert the MCU that data is ready
void Ext_Int0_Handler()
{
  ucInterrupted = 1;
  /* This example just set the flag and deal with interrupt in AD5940Main function. It's your choice to choose how to process interrupt. */
}


/* MCU related external line interrupt service routine */
//The interrupt handler handles the interrupt to the MCU
//when the AD5940 INTC pin generates an interrupt to alert the MCU that data is ready

// int32_t BIAdata(uint32_t *pData, uint32_t DataCount){

// }

int32_t BIAShowResult(uint32_t *pData, uint32_t DataCount)
{
  float freq;

  fImpPol_Type *pImp = (fImpPol_Type*)pData;
  AppBIACtrl(BIACTRL_GETFREQ, &freq);

  /*Process data*/
  if (debug_print) {
    // for (int i = 0; i < DataCount; i++)
    // {
    //   Serial.print("Freq: ");
    //   Serial.print(freq);
    //   Serial.print(" RzMag: ");
    //   Serial.print(pImp[i].Magnitude);
    //   Serial.print(" Ohm , RzPhase: ");
    //   Serial.println(pImp[i].Phase * 180 / MATH_PI);
    // }
      printf("Freq:%.2f ", freq);
      /*Process data*/
      for(int i=0;i<DataCount;i++)
      {
        printf("RzMag: %f Ohm , RzPhase: %f \n",pImp[i].Magnitude,pImp[i].Phase*180/MATH_PI);
      }
      return 0;
  } else {
    //save data
    mag_print = pImp[DataCount - 1].Magnitude;//yw
    freq_print = freq;
    phase_print = pImp[DataCount -1].Phase*180/MATH_PI;
    // for (int i = 0; i < DataCount; i++)
    // {
    //   freq_print  = freq;
    //   // mag_print   = pImp[DataCount - 1].Magnitude; //buffer的最后一个数？
    //   // phase_print = pImp[DataCount - 1].Phase * 180 / MATH_PI;
    //   mag_print   = pImp[i].Magnitude;
    //   phase_print = pImp[i].Phase * 180 / MATH_PI;      
    // }
  }
  return 0;
}

/* Initialize AD5940 basic blocks like clock */
static int32_t AD5940PlatformCfg(void)
{
  CLKCfg_Type clk_cfg;
  FIFOCfg_Type fifo_cfg;
  AGPIOCfg_Type gpio_cfg;

  /* Use hardware reset */
  AD5940_HWReset();
  /* Platform configuration */
  AD5940_Initialize();
  /* Step1. Configure clock */
  clk_cfg.ADCClkDiv = ADCCLKDIV_1;
  clk_cfg.ADCCLkSrc = ADCCLKSRC_HFOSC;
  clk_cfg.SysClkDiv = SYSCLKDIV_1;
  clk_cfg.SysClkSrc = SYSCLKSRC_HFOSC;
  clk_cfg.HfOSC32MHzMode = bFALSE;
  clk_cfg.HFOSCEn = bTRUE;
  clk_cfg.HFXTALEn = bFALSE;
  clk_cfg.LFOSCEn = bTRUE;
  AD5940_CLKCfg(&clk_cfg);
  /* Step2. Configure FIFO and Sequencer*/
  fifo_cfg.FIFOEn = bFALSE;
  fifo_cfg.FIFOMode = FIFOMODE_FIFO;
  fifo_cfg.FIFOSize = FIFOSIZE_4KB;                       /* 4kB for FIFO, The reset 2kB for sequencer */
  fifo_cfg.FIFOSrc = FIFOSRC_DFT;
  fifo_cfg.FIFOThresh = 4;//AppBIACfg.FifoThresh;        /* DFT result. One pair for RCAL, another for Rz. One DFT result have real part and imaginary part */
  AD5940_FIFOCfg(&fifo_cfg);                             /* Disable to reset FIFO. */
  fifo_cfg.FIFOEn = bTRUE;
  AD5940_FIFOCfg(&fifo_cfg);                             /* Enable FIFO here */

  /* Step3. Interrupt controller */

  AD5940_INTCCfg(AFEINTC_1, AFEINTSRC_ALLINT, bTRUE);           /* Enable all interrupt in Interrupt Controller 1, so we can check INTC flags */
  AD5940_INTCCfg(AFEINTC_0, AFEINTSRC_DATAFIFOTHRESH, bTRUE);   /* Interrupt Controller 0 will control GP0 to generate interrupt to MCU */
  AD5940_INTCClrFlag(AFEINTSRC_ALLINT);
  /* Step4: Reconfigure GPIO */
  gpio_cfg.FuncSet = GP6_SYNC | GP5_SYNC | GP4_SYNC | GP2_TRIG | GP1_SYNC | GP0_INT;
  gpio_cfg.InputEnSet = AGPIO_Pin2;
  gpio_cfg.OutputEnSet = AGPIO_Pin0 | AGPIO_Pin1 | AGPIO_Pin4 | AGPIO_Pin5 | AGPIO_Pin6;
  gpio_cfg.OutVal = 0;
  gpio_cfg.PullEnSet = 0;

  AD5940_AGPIOCfg(&gpio_cfg);
  AD5940_SleepKeyCtrlS(SLPKEY_UNLOCK);  /* Allow AFE to enter sleep mode. */
  return 0;
}

/* !!Change the application parameters here if you want to change it to none-default value */
void AD5940BIAStructInit(void)
{
  AppBIACfg_Type *pBIACfg;

  AppBIAGetCfg(&pBIACfg);

  pBIACfg->SeqStartAddr = 0;
  pBIACfg->MaxSeqLen = 512; /** @todo add checker in function */

  pBIACfg->RcalVal = 10000.0;
  pBIACfg->DftNum = DFTNUM_1024;

  // pBIACfg->NumOfData = -1;      /* Never stop until you stop it manually by AppBIACtrl() function */
  pBIACfg->NumOfData = 1;
  //NOD = 5;                     /*注意这里把上面的Number of data 抄下来。main里要用*/
  pBIACfg->BiaODR = 20;         /* ODR(Sample Rate) 20Hz */
  //pBIACfg->BiaODR = 2048;
  pBIACfg->FifoThresh = 4;      /* 4 */
  pBIACfg->ADCSinc3Osr = ADCSINC3OSR_2;
}
///////////////////////////////////////////////////////////////////////

void AD5940_BIA_Setup() {
  //init baud rate of UART, irrelevant for SAMD21 because it uses USB CDC serial port (built in USB controller)
  //pinMode(A2, INPUT_PULLUP); //allow AD5940 to set the LED on this pin

  //init GPIOs (SPI, AD5940 Reset and Interrupt)
  AD5940_MCUResourceInit(0);
  AD5940PlatformCfg();
  AD5940BIAStructInit();
  // AppBIAInit(AppBuff, APPBUFF_SIZE);  /* Initialize BIA application. Provide a buffer, which is used to store sequencer commands */
  AppBIAInit(AppBuff, APPBUFF_SIZE);  /* Initialize BIA application. Provide a buffer, which is used to store sequencer commands */
  AppBIACtrl(BIACTRL_START, 0);         /* Control BIA measurement to start. Second parameter has no meaning with this command. */
}

void AD5940_Sample_Main(bool started_flag) {
  if (!started_flag) {
    AD5940PlatformCfg();
    AD5940BIAStructInit();
    AppBIAInit(AppBuff, APPBUFF_SIZE);  /* Initialize BIA application. Provide a buffer, which is used to store sequencer commands */
    AppBIACtrl(BIACTRL_START, 0);         /* Control BIA measurement to start. Second parameter has no meaning with this command. */
  }
  // else{//初始化以后再run一次AD5940_Sample_Main(1)，这样这个else就不用了
  //   AD5940PlatformCfg();
  //   AppBIAInit(AppBuff, APPBUFF_SIZE);
  //   AppBIACtrl(BIACTRL_START, 0);
  //   delay(10);//delay is needed 
  //   delayMicroseconds(1000);   
  // }

  
  /* Check if interrupt flag which will be set when interrupt occurred. */
  if (AD5940_GetMCUIntFlag())
  {
    IntCount++;
    AD5940_ClrMCUIntFlag(); /* Clear this flag */
    temp = APPBUFF_SIZE;
    // temp= 1;
    AppBIAISR(AppBuff, &temp); /* Deal with it and provide a buffer to store data we got */
    BIAShowResult(AppBuff, temp); /* Show the results to UART */
    // if(IntCount == 240)
    // {
    //   IntCount = 0;
    //   AppBIACtrl(BIACTRL_SHUTDOWN, 0);
    // }
  }

  // count++;
  // if(count > 1000000)
  // {
  //   count = 0;
  //   //AppBIAInit(0, 0);    /* Re-initialize BIA application. Because sequences are ready, no need to provide a buffer, which is used to store sequencer commands */
  //   //AppBIACtrl(BIACTRL_START, 0);          /* Control BIA measurement to start. Second parameter has no meaning with this command. */
  // }

}
