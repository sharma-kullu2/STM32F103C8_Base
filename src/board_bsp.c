#include "board_bsp.h"


/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


/*
* Init LED 
*/

/** @defgroup STM32F1XX_BOARD_Private_Variables STM32F1XX BOARD Private Variables
  * @{
  */ 
GPIO_TypeDef* LED_PORT[LEDn] = {LED2_GPIO_PORT};

const uint16_t LED_PIN[LEDn] = {LED2_PIN};

/**
  * @brief  Helper Fn to enable GPIO | Enable Clock before enabling GPIO Port
  * @param  Port: GPIO Port. 
  *          This parameter can be one of the following values:
  *     @arg GPIOA
  * @param  Pin: GPIO Pin. 
  *          This parameter can be one of the following values:
  *     @arg LED2
  * @param  Mode: Push PUll/ Open Drain etc 
  * @param  Pull: Pull no/up/down mode.
  * @param  Speed: Low,Medium,High Frequency.       
  */
void GPIO_Init(GPIO_TypeDef* Port, uint32_t Pin, uint32_t Mode, uint32_t Pull, uint32_t Speed)
{
  GPIO_InitTypeDef  gpioinitstruct;
  /* Configure the GPIO_LED pin */
  gpioinitstruct.Pin    = Pin;
  gpioinitstruct.Mode   = Mode;
  if(Pull != NOPULL)
    gpioinitstruct.Pull   = Pull;
  if(Speed != NOSPEED)
    gpioinitstruct.Speed  = Speed;
  HAL_GPIO_Init(Port, &gpioinitstruct);
}

/*
*@brief  Helper Fn to set GPIO to 1 |HIGH
* @param  Port: GPIO Port. 
*          This parameter can be one of the following values:
*     @arg GPIOA
* @param  Pin: GPIO Pin. 
*          This parameter can be one of the following values:
*     @arg LED2
*/
void GPIO_On(GPIO_TypeDef* Port, uint32_t Pin)
{
  HAL_GPIO_WritePin(Port,Pin, GPIO_PIN_RESET);
}

/*
*@brief  Helper Fn to set GPIO to 0|LOW
* @param  Port: GPIO Port. 
*          This parameter can be one of the following values:
*     @arg GPIOA
* @param  Pin: GPIO Pin. 
*          This parameter can be one of the following values:
*     @arg LED2
*/
void GPIO_Off(GPIO_TypeDef* Port, uint32_t Pin)
{
  HAL_GPIO_WritePin(Port,Pin, GPIO_PIN_SET);
}

/**
  * @brief  Configures LED GPIO.
  * @param  Led: Led to be configured. 
  *          This parameter can be one of the following values:
  *     @arg LED2
  */
void BSP_LED_Init(Led_TypeDef Led)
{
  /* Enable the GPIO_LED Clock */
  LEDx_GPIO_CLK_ENABLE(Led);

  /* Configure the GPIO_LED pin */
  GPIO_Init(LED_PORT[Led],LED_PIN[Led],GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH);
  /* Reset PIN to switch off the LED */
  GPIO_Off(LED_PORT[Led],LED_PIN[Led]);
}

/**
  * @brief  DeInit LEDs.
  * @param  Led: LED to be de-init. 
  *   This parameter can be one of the following values:
  *     @arg  LED2
  * @note Led DeInit does not disable the GPIO clock nor disable the Mfx 
  */
void BSP_LED_DeInit(Led_TypeDef Led)
{
  GPIO_InitTypeDef  gpio_init_structure;

  /* Turn off LED */
  GPIO_Off(LED_PORT[Led],LED_PIN[Led]);
  /* DeInit the GPIO_LED pin */
  gpio_init_structure.Pin = LED_PIN[Led];
  HAL_GPIO_DeInit(LED_PORT[Led], gpio_init_structure.Pin);
}

/**
  * @brief  Turns selected LED On.
  * @param  Led: Specifies the Led to be set on. 
  *   This parameter can be one of following parameters:
  *     @arg LED2
  */
void BSP_LED_On(Led_TypeDef Led)
{
  //@Todo assert if led is valid before proceeding
  HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET); 
}

/**
  * @brief  Turns selected LED Off.
  * @param  Led: Specifies the Led to be set off. 
  *   This parameter can be one of following parameters:
  *     @arg LED2
  */
void BSP_LED_Off(Led_TypeDef Led)
{
  //@Todo assert if led is valid before proceeding
  HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_RESET); 
}

/**
  * @brief  Toggles the selected LED.
  * @param  Led: Specifies the Led to be toggled. 
  *   This parameter can be one of following parameters:
  *            @arg  LED2
  */
void BSP_LED_Toggle(Led_TypeDef Led)
{
  HAL_GPIO_TogglePin(LED_PORT[Led], LED_PIN[Led]);
}

/*
* Init UART
*/

/* UART handler declaration */
static UART_HandleTypeDef UartHandle;

/**
  * @brief UART MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param huart: UART handle pointer
  * @retval None
  */

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef  GPIO_InitStruct;


  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  USARTx_TX_GPIO_CLK_ENABLE();
  USARTx_RX_GPIO_CLK_ENABLE();


  /* Enable USARTx clock */
  USARTx_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/
  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = USARTx_TX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;

  HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = USARTx_RX_PIN;

  HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStruct);
}

/**
  * @brief UART MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO and NVIC configuration to their default state
  * @param huart: UART handle pointer
  * @retval None
  */

void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
  /*##-1- Reset peripherals ##################################################*/
  USARTx_FORCE_RESET();
  USARTx_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks #################################*/
  /* Configure UART Tx as alternate function  */
  HAL_GPIO_DeInit(USARTx_TX_GPIO_PORT, USARTx_TX_PIN);
  /* Configure UART Rx as alternate function  */
  HAL_GPIO_DeInit(USARTx_RX_GPIO_PORT, USARTx_RX_PIN);

}

HAL_StatusTypeDef BSP_UART_Init(){
  HAL_StatusTypeDef Status;
  UartHandle.Instance        = USARTx;
  UartHandle.Init.BaudRate   = 9600;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits   = UART_STOPBITS_1;
  UartHandle.Init.Parity     = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode       = UART_MODE_TX_RX;
  Status=HAL_UART_Init(&UartHandle);
  return Status;
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}

/*
* Init SPI
*/

#ifdef HAL_SPI_MODULE_ENABLED
static uint32_t SpixTimeout = BOARD_SPIx_TIMEOUT_MAX;        /*<! Value of Timeout when SPI communication fails */

static SPI_HandleTypeDef hf103_Spi;
static void SPIx_Error (void);
/**
  * @brief  Initialize SPI MSP.
  */
static void SPIx_MspInit(void)
{
  GPIO_InitTypeDef  gpioinitstruct = {0};
  
  /*** Configure the GPIOs ***/  
  /* Enable GPIO clock */
  BOARD_SPIx_SCK_GPIO_CLK_ENABLE();
  BOARD_SPIx_MISO_MOSI_GPIO_CLK_ENABLE();

  /* Configure SPI SCK */
  gpioinitstruct.Pin        = BOARD_SPIx_SCK_PIN;
  gpioinitstruct.Mode       = GPIO_MODE_AF_PP;
  gpioinitstruct.Speed      = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(BOARD_SPIx_SCK_GPIO_PORT, &gpioinitstruct);

  /* Configure SPI MISO and MOSI */ 
  gpioinitstruct.Pin        = BOARD_SPIx_MOSI_PIN;
  HAL_GPIO_Init(BOARD_SPIx_MISO_MOSI_GPIO_PORT, &gpioinitstruct);
  
  gpioinitstruct.Pin        = BOARD_SPIx_MISO_PIN;
  gpioinitstruct.Mode       = GPIO_MODE_INPUT;
  HAL_GPIO_Init(BOARD_SPIx_MISO_MOSI_GPIO_PORT, &gpioinitstruct);

  /*** Configure the SPI peripheral ***/ 
  /* Enable SPI clock */
  BOARD_SPIx_CLK_ENABLE();
}

/**
  * @brief  Initialize SPI HAL.
  */
void BSP_SPI_Init(void)
{
  if(HAL_SPI_GetState(&hf103_Spi) == HAL_SPI_STATE_RESET)
  {
    /* SPI Config */
    hf103_Spi.Instance = BOARD_SPIx;
      /* SPI baudrate is set to 8 MHz maximum (PCLK2/SPI_BaudRatePrescaler = 64/8 = 8 MHz) 
       to verify these constraints:
          - ST7735 LCD SPI interface max baudrate is 15MHz for write and 6.66MHz for read
            Since the provided driver doesn't use read capability from LCD, only constraint 
            on write baudrate is considered.
          - SD card SPI interface max baudrate is 25MHz for write/read
          - PCLK2 max frequency is 32 MHz 
       */
    hf103_Spi.Init.BaudRatePrescaler  = SPI_BAUDRATEPRESCALER_8;
    hf103_Spi.Init.Direction          = SPI_DIRECTION_2LINES;
    hf103_Spi.Init.CLKPhase           = SPI_PHASE_1EDGE;
    hf103_Spi.Init.CLKPolarity        = SPI_POLARITY_LOW;
    hf103_Spi.Init.CRCCalculation     = SPI_CRCCALCULATION_DISABLE;
    hf103_Spi.Init.CRCPolynomial      = 7;
    hf103_Spi.Init.DataSize           = SPI_DATASIZE_8BIT;
    hf103_Spi.Init.FirstBit           = SPI_FIRSTBIT_MSB;
    hf103_Spi.Init.NSS                = SPI_NSS_SOFT;
    hf103_Spi.Init.TIMode             = SPI_TIMODE_DISABLE;
    hf103_Spi.Init.Mode               = SPI_MODE_MASTER;
    SPIx_MspInit();
    HAL_SPI_Init(&hf103_Spi);
  }
}

/**
  * @brief  SPI Write a byte to device
  * @param  Value: value to be written
*/
void BSP_SPI_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength)
{
  HAL_StatusTypeDef status = HAL_OK;
  
  status = HAL_SPI_TransmitReceive(&hf103_Spi, (uint8_t*) DataIn, DataOut, DataLength, SpixTimeout);
  
  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* Execute user timeout callback */
    SPIx_Error();
  }
}

/**
  * @brief  SPI Write an amount of data to device
  * @param  Value: value to be written
  * @param  DataLength: number of bytes to write
  */
void BSP_SPI_WriteData(uint8_t *DataIn, uint16_t DataLength)
{
  HAL_StatusTypeDef status = HAL_OK;

  status = HAL_SPI_Transmit(&hf103_Spi, DataIn, DataLength, SpixTimeout);
  
  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* Execute user timeout callback */
    SPIx_Error();
  }
}

/**
  * @brief  SPI Write a byte to device
  * @param  Value: value to be written
  */
void BSP_SPI_Write(uint8_t Value)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint8_t data;

  status = HAL_SPI_TransmitReceive(&hf103_Spi, (uint8_t*) &Value, &data, 1, SpixTimeout);

  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* Execute user timeout callback */
    SPIx_Error();
  }
}

/**
  * @brief  SPI error treatment function
  */
static void SPIx_Error (void)
{
  /* De-initialize the SPI communication BUS */
  HAL_SPI_DeInit(&hf103_Spi);

  /* Re-Initiaize the SPI communication BUS */
  BSP_SPI_Init();
}

/******************************************************************************
                            LINK OPERATIONS
*******************************************************************************/
/**
  * @brief LINK SD Card
  */
#define SD_DUMMY_BYTE            0xFF    
#define SD_NO_RESPONSE_EXPECTED  0x80

/********************************* LINK SD ************************************/
/**
  * @brief  Initialize the SD Card and put it into StandBy State (Ready for 
  *         data transfer).
  */
void SD_IO_Init(void)
{
  GPIO_InitTypeDef  gpioinitstruct = {0};
  uint8_t counter = 0;

  /* SD_CS_GPIO Periph clock enable */
  SD_CS_GPIO_CLK_ENABLE();

  /* Configure SD_CS_PIN pin: SD Card CS pin */
  gpioinitstruct.Pin    = SD_CS_PIN;
  gpioinitstruct.Mode   = GPIO_MODE_OUTPUT_PP;
  gpioinitstruct.Pull   = GPIO_PULLUP;
  gpioinitstruct.Speed  = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(SD_CS_GPIO_PORT, &gpioinitstruct);

  /*------------Put SD in SPI mode--------------*/
  /* SD SPI Config */
  BSP_SPI_Init();

  /* SD chip select high */
  SD_CS_HIGH();
  
  /* Send dummy byte 0xFF, 10 times with CS high */
  /* Rise CS and MOSI for 80 clocks cycles */
  for (counter = 0; counter <= 9; counter++)
  {
    /* Send dummy byte 0xFF */
    SD_IO_WriteByte(SD_DUMMY_BYTE);
  }
}

/**
  * @brief  Set the SD_CS pin.
  * @param  pin value.
  */
void SD_IO_CSState(uint8_t val)
{
  if(val == 1) 
  {
    SD_CS_HIGH();
}
  else
  {
    SD_CS_LOW();
  }
}
 
/**
  * @brief  Write byte(s) on the SD
  * @param  DataIn: Pointer to data buffer to write
  * @param  DataOut: Pointer to data buffer for read data
  * @param  DataLength: number of bytes to write
  */
void SD_IO_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength)
  {
  /* Send the byte */
  BSP_SPI_WriteReadData(DataIn, DataOut, DataLength);
}

/**
  * @brief  Write a byte on the SD.
  * @param  Data: byte to send.
  * @retval Data written
  */
uint8_t SD_IO_WriteByte(uint8_t Data)
{
  uint8_t tmp;

  /* Send the byte */
  BSP_SPI_WriteReadData(&Data,&tmp,1);
  return tmp;
}

/**
  * @brief  Write an amount of data on the SD.
  * @param  Data: byte to send.
  * @param  DataLength: number of bytes to write
  */
void SD_IO_ReadData(uint8_t *DataOut, uint16_t DataLength)
{
  /* Send the byte */
  SD_IO_WriteReadData(DataOut, DataOut, DataLength);
  }   
 
/**
  * @brief  Write an amount of data on the SD.
  * @param  Data: byte to send.
  * @param  DataLength: number of bytes to write
  */
void SD_IO_WriteData(const uint8_t *Data, uint16_t DataLength)
{
  /* Send the byte */
  BSP_SPI_WriteData((uint8_t *)Data, DataLength);
}

#endif /* HAL_SPI_MODULE_ENABLED */
