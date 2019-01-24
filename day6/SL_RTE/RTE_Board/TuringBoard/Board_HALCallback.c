#include "Board_HALCallback.h"
#include "Board_Uart.h"
#if USART1_USE_DMA_TX == 1
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	
}
#endif
