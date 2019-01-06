#include "stm32h7xx_hal.h"
#include "Board_Uart.h"
void USART1_IRQHandler(void)
{
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
