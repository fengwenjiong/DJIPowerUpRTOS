#include "stm32h7xx_hal.h"
#include "stm32h7xx.h"
#include "Board_Uart.h"
#include "Board_SPI.h"
void USART1_IRQHandler(void)
{
	if(__HAL_UART_GET_FLAG(&UartHandle[USART_DEBUG].UartHalHandle, UART_FLAG_IDLE) != RESET)
	{
		Board_Uart_RecCallback(USART_DEBUG);
		__HAL_UART_CLEAR_IDLEFLAG(&UartHandle[USART_DEBUG].UartHalHandle);
		
	}
  HAL_UART_IRQHandler(&UartHandle[USART_DEBUG].UartHalHandle);
}
void DMA1_Stream0_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&UartHandle[USART_DEBUG].UsartHalRxDmaHandle);
}
#if USART1_USE_DMA_TX == 1
void DMA1_Stream1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&UartHandle[USART_DEBUG].UsartHalTxDmaHandle);
}
#endif
void DMA1_Stream2_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&SpiHandle[SPI_LCD].SpiHalTxDmaHandle);
}
void SPI4_IRQHandler(void)
{
  HAL_SPI_IRQHandler(&SpiHandle[SPI_LCD].SpiHalHandle);
}
void EXTI4_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}
