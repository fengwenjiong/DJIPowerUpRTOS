#include "Board_SDRAM.h"
#include "Board_Uart.h"
#include "Board_SDCard.h"
#include "Board_RGBLCD.h"
extern osMessageQueueId_t RGBLCDMsgQueue;
void HAL_SDRAM_DMA_XferCpltCallback(MDMA_HandleTypeDef *hmdma)
{
	static uint8_t MDMAWriteOverMsg = 0x00;
	if(hmdma->Init.SourceInc == MDMA_SRC_INC_WORD)
		MDMAWriteOverMsg = 0x01;
	else if(hmdma->Init.SourceInc == MDMA_SRC_INC_DISABLE)
		MDMAWriteOverMsg = 0x02;
	osMessageQueuePut (RGBLCDMsgQueue, &MDMAWriteOverMsg, 0, NULL);
}
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
void HAL_SD_AbortCallback(SD_HandleTypeDef *hsd)
{
  RTE_Assert(__FILE__, __LINE__);	
}

void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd)
{
  Board_SD_WriteCpltCallback();
}
void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd)
{
  Board_SD_ReadCpltCallback();
}
extern osThreadId_t ThreadIDSensor;
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
	osThreadFlagsSet(ThreadIDSensor,0x00000001U);
}
