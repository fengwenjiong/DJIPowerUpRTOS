#include "Board_Uart.h"
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  for(Board_Uart_Name_e USART_Name=(Board_Uart_Name_e)0;USART_Name<USART_N;USART_Name++) 
	{
		if(huart == &UartHandle[USART_Name].UartHalHandle)
		{
			Board_Uart_RecCallback(USART_Name);
		}
	}
}
#if USART1_USE_DMA_TX == 1
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	
}
#endif
