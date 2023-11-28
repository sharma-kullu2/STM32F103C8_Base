/*******************************************************************************
* @file    Main.c
* @author  GS
* @brief   This file contains the entry point
******************************************************************************/

#include "main.h"
#include "sd_card.h"
/* Private function prototypes -----------------------------------------------*/
static void Error_Handler(void);
static void SystemClock_Config(void);
static void SD_command(SD_Command cmd,uint32_t args, uint8_t response);

/*Entry Point-----------------------------------------------------------------*/
int main(void) {
  HAL_Init();
  SystemClock_Config();
  //LED_Init();
  BSP_LED_Init(LED2);
  if(BSP_UART_Init()!=HAL_OK){
    Error_Handler();
  }
  
  /* Output a message on Hyperterminal using printf function */
  printf("\n\r*********************************\n\r");
  printf("\n\r* STM32 F103C8 SD Card Test App *\n\r");
  printf("\n\r*********************************\n\r");
  /**
   * @note: Systems with SD card as peripheral should call SD_IO_Init()
   * prior to other SPI peripherals
   * Initialises SPI
   * SPI Speed / BaudRate=250Khz
   * SD S1: card enter Native Mode
  */
  SD_IO_Init();
  
  uint32_t tickstart = HAL_GetTick();
  /*SD S2: Card enter SPI Mode*/
  SD_IO_CSState(0);
  BSP_SPI_Speed(SPI_SPEED_16M);
    /* send CMD 0
     * cmd[0]=0x40 | 0
     * cmd[1..4]=args
     * cmd[5]=crc | 0x01
     * response = R1
    */
  uint8_t response=0;
  while(response!=0x01){
      uint32_t tick = HAL_GetTick();
      if(tick-tickstart > SD_INIT_TIMEOUT) goto END;
      SD_command(CMD_0,0,response);
      printf("Response R1= 0x%x\n",response);
  }
  /*SD S3: High Speed Card Init*/
  SD_command(CMD_8,0x1AA,response);
  if(response==0b00000100){ //Illegal Command
      while(response!=0x0){
        SD_command(CMD_55,0x0,response);
        SD_command(CMD_41,0x0,response);
        uint32_t tick = HAL_GetTick();
        if(tick-tickstart > SD_INIT_TIMEOUT){
          while(response!=0x0){
              SD_command(CMD_1,0x0,response);
              uint32_t tick = HAL_GetTick();
              if(tick-tickstart > SD_INIT_TIMEOUT) goto END;
          }
          printf("Found MMC v3\n");
          break;
        }
      }
      printf("Found SD v1\n");
  }
  else{ //check lower 12 bits
      uint8_t resp[4];
      for (int i=0;i<4;i++){
        resp[i]=SD_IO_WriteByte(SD_DUMMY_BYTE);
      }
      if ((resp[3]<<8|resp[4])==0x01AA){
          while(response!=0x0){
            SD_command(CMD_55,0x0,response);
            SD_command(CMD_41,0X40000000,response);
            uint32_t tick = HAL_GetTick();
            if(tick-tickstart > SD_INIT_TIMEOUT) goto END;
          }
          SD_command(CMD_58,0x0,response);
          if(response==0x0){
            for (int i=0;i<4;i++){
              resp[i]=SD_IO_WriteByte(SD_DUMMY_BYTE);
            }
            if ((resp[0]>>7&1)==1) printf("SD Version 2+| BLOCK ADDRESSING \n");
            else printf("SD Version 2+ | BYTE ADDRESSING \n");
          }
      }
      else goto END;
  }
  
  

  /* Infinite loop */
  while (1)
  {  
    BSP_LED_Off(LED2);
    printf("LED OFF \n\r");
    HAL_Delay(500);
    BSP_LED_On(LED2);
    printf("LED ON \n\r");
    HAL_Delay(500);
  }

  END:
  printf("NO/FAULTY CARD");
  return 0;
}

/**
  * @brief  EXTI line detection callbacks
  * @param  GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == GPIO_PIN_1)
  {
    //@Todo
  }
}


/*Private Function definitions-----------------------------------------------*/

/**
  * @brief  SD_command : send CMD to SD Card | obtain response specific to command
  * @param  cmdno :
  * @param args :
  * @param crc :
  * @param response : void pointer to response given the CMD
  * @retval None
  */
static void SD_command(SD_Command cmdno,uint32_t args, uint8_t response){
  /*create command*/
  uint8_t cmd[6];
  cmd[0]=0x40|cmdno;
  for(int i=1;i<5;i++){
    cmd[i]=(args>>(32-(i*(8))))&0xff;
  }
  switch(cmdno){
    case CMD_0:
      cmd[5]= (0b1001010<<1)|0x01; //0x95
      break;
    case CMD_8:
      cmd[5]= (0b1000011<<1)|0x01; //0X87
      break;
    default:
      cmd[5]= 0x01;
  }
  
  /*send command*/
  SD_IO_WriteData(cmd,6);
  /*obtain response for CMD*/
#if 0
  switch (cmdno)
  {
    case CMD_8:
    case CMD_58:
      /*R1+32bit*/
      uint8_t res[5];
      for(int i=0;i<5;i++){
        res[i]=SD_IO_WriteByte(SD_DUMMY_BYTE);
      }
      *response = &res;
      break;
    case CMD_12:
      /*R1+ 1bit*/
      uint8_t resp=0;
      uint8_t flag=0;
      for(;;){
        resp=SD_IO_WriteByte(SD_DUMMY_BYTE);
        flag=SD_IO_WriteByte(SD_DUMMY_BYTE);
        if((flag>>7&1)==0) break;
      }
      *response = &resp;
      break;
    default:
      uint8_t resp =0; 
      resp=SD_IO_WriteByte(SD_DUMMY_BYTE); //@check
      *response =&resp;
      break;
  }
#else
  /*The loop will exit when either the MSB of R1 is not set or when i reaches 255*/
  for (uint8_t i = 0; ((response = SD_IO_WriteByte(SD_DUMMY_BYTE)) & 0X80) && i != 0XFF; i++);
#endif
}

/**
  * @brief  Error_Handler : indicates UART could not start hence turns the LED 2 on
  * @param  void
  * @retval None
  */
static void Error_Handler(void){
  while(1){
    BSP_LED_On(LED2);
  }
}
/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 64000000
  *            HCLK(Hz)                       = 64000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            PLLMUL                         = 16
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef clkinitstruct = {0};
  RCC_OscInitTypeDef oscinitstruct = {0};
  
  /* Configure PLL ------------------------------------------------------*/
  /* PLL configuration: PLLCLK = (HSI / 2) * PLLMUL = (8 / 2) * 16 = 64 MHz */
  /* PREDIV1 configuration: PREDIV1CLK = PLLCLK / HSEPredivValue = 64 / 1 = 64 MHz */
  /* Enable HSI and activate PLL with HSi_DIV2 as source */
  oscinitstruct.OscillatorType  = RCC_OSCILLATORTYPE_HSI;
  oscinitstruct.HSEState        = RCC_HSE_OFF;
  oscinitstruct.LSEState        = RCC_LSE_OFF;
  oscinitstruct.HSIState        = RCC_HSI_ON;
  oscinitstruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  oscinitstruct.HSEPredivValue    = RCC_HSE_PREDIV_DIV1;
  oscinitstruct.PLL.PLLState    = RCC_PLL_ON;
  oscinitstruct.PLL.PLLSource   = RCC_PLLSOURCE_HSI_DIV2;
  oscinitstruct.PLL.PLLMUL      = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&oscinitstruct)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
  clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;  
  if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }
}




