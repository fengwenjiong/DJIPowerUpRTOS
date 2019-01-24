#include "Board_MSPInit.h"
#include "Board_Uart.h"
void Board_Uart1_MSPInitCallback(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
	/* Peripheral clock enable */
	__HAL_RCC_USART1_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
	/**USART1 GPIO Configuration    
	PA9     ------> USART1_TX
	PA10     ------> USART1_RX 
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, USART1_DMARX_PreemptPriority, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
#if USART1_USE_DMA_TX == 1
  /* DMA1_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
#endif
	/* USART1 DMA Init */
	/* USART1_RX Init */
	UartHandle[USART_DEBUG].UsartHalRxDmaHandle.Instance = DMA1_Stream0;
	UartHandle[USART_DEBUG].UsartHalRxDmaHandle.Init.Request = DMA_REQUEST_USART1_RX;
	UartHandle[USART_DEBUG].UsartHalRxDmaHandle.Init.Direction = DMA_PERIPH_TO_MEMORY;
	UartHandle[USART_DEBUG].UsartHalRxDmaHandle.Init.PeriphInc = DMA_PINC_DISABLE;
	UartHandle[USART_DEBUG].UsartHalRxDmaHandle.Init.MemInc = DMA_MINC_ENABLE;
	UartHandle[USART_DEBUG].UsartHalRxDmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	UartHandle[USART_DEBUG].UsartHalRxDmaHandle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	UartHandle[USART_DEBUG].UsartHalRxDmaHandle.Init.Mode = DMA_NORMAL;
	UartHandle[USART_DEBUG].UsartHalRxDmaHandle.Init.Priority = DMA_PRIORITY_LOW;
	UartHandle[USART_DEBUG].UsartHalRxDmaHandle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	if (HAL_DMA_Init(&UartHandle[USART_DEBUG].UsartHalRxDmaHandle) != HAL_OK)
	{
		RTE_Assert(__FILE__, __LINE__);
	}

	__HAL_LINKDMA(&UartHandle[USART_DEBUG].UartHalHandle,hdmarx,UartHandle[USART_DEBUG].UsartHalRxDmaHandle);
#if USART1_USE_DMA_TX == 1
	/* USART1_TX Init */
	UartHandle[USART_DEBUG].UsartHalTxDmaHandle.Instance = DMA1_Stream1;
	UartHandle[USART_DEBUG].UsartHalTxDmaHandle.Init.Request = DMA_REQUEST_USART1_TX;
	UartHandle[USART_DEBUG].UsartHalTxDmaHandle.Init.Direction = DMA_MEMORY_TO_PERIPH;
	UartHandle[USART_DEBUG].UsartHalTxDmaHandle.Init.PeriphInc = DMA_PINC_DISABLE;
	UartHandle[USART_DEBUG].UsartHalTxDmaHandle.Init.MemInc = DMA_MINC_ENABLE;
	UartHandle[USART_DEBUG].UsartHalTxDmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	UartHandle[USART_DEBUG].UsartHalTxDmaHandle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	UartHandle[USART_DEBUG].UsartHalTxDmaHandle.Init.Mode = DMA_NORMAL;
	UartHandle[USART_DEBUG].UsartHalTxDmaHandle.Init.Priority = DMA_PRIORITY_LOW;
	UartHandle[USART_DEBUG].UsartHalTxDmaHandle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	if (HAL_DMA_Init(&UartHandle[USART_DEBUG].UsartHalTxDmaHandle) != HAL_OK)
	{
		RTE_Assert(__FILE__, __LINE__);
	}

	__HAL_LINKDMA(&UartHandle[USART_DEBUG].UartHalHandle,hdmatx,UartHandle[USART_DEBUG].UsartHalTxDmaHandle);
#endif
	/* USART1 interrupt Init */
	HAL_NVIC_SetPriority(USART1_IRQn, USART1_PreemptPriority, 0);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
}
void Board_Uart1_MSPDeInitCallback(void)
{
	/* Peripheral clock disable */
	__HAL_RCC_USART1_CLK_DISABLE();
	/**USART1 GPIO Configuration    
	PA9     ------> USART1_TX
	PA10     ------> USART1_RX 
	*/
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);
	/* USART1 DMA DeInit */
	HAL_DMA_DeInit(UartHandle[USART_DEBUG].UartHalHandle.hdmarx);
#if USART1_USE_DMA_TX == 1
	HAL_DMA_DeInit(UartHandle[USART_DEBUG].UartHalHandle.hdmatx);
#endif
	/* USART1 interrupt DeInit */
	HAL_NVIC_DisableIRQ(USART1_IRQn);
}
void Board_Spi4_MSPInitCallback(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
	/* Peripheral clock enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_SPI4_CLK_ENABLE();
	/**SPI4 GPIO Configuration    
	PE2     ------> SPI4_SCK
	PE5     ------> SPI4_MISO
	PE14     ------> SPI4_MOSI 
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_5|GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI4;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}
void Board_Spi4_MSPDeInitCallback(void)
{
	__HAL_RCC_SPI4_CLK_DISABLE();
	/**SPI4 GPIO Configuration    
	PE2     ------> SPI4_SCK
	PE5     ------> SPI4_MISO
	PE14     ------> SPI4_MOSI 
	*/
	HAL_GPIO_DeInit(GPIOE, GPIO_PIN_2|GPIO_PIN_5|GPIO_PIN_14);
	/* SPI4 interrupt DeInit */
	HAL_NVIC_DisableIRQ(SPI4_IRQn);
}
