#include "Board_HALCallback.h"
#include "Board_Uart.h"
#include "Board_SPI.h"
#include "Board_WIFI.h"
#if USART1_USE_DMA_TX == 1
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	
}
#endif
#if RTE_USE_OS 
extern osThreadId_t ThreadIDGUI;
extern osEventFlagsId_t EvtIDWIFI;
#else
extern volatile bool LCDSPIReadyFlag;
#endif
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi == &SpiHandle[SPI_LCD].SpiHalHandle)
	{
#if RTE_USE_OS
		osThreadFlagsSet(ThreadIDGUI,0x00000001U);
#else
		if(LCDSPIReadyFlag == false)
			LCDSPIReadyFlag = true;
#endif
	}
	if(hspi == &SpiHandle[SPI_WIFI].SpiHalHandle)
	{
#if RTE_USE_OS
		osEventFlagsSet(EvtIDWIFI,0x00000001U);
#else
		if(LCDSPIReadyFlag == false)
			LCDSPIReadyFlag = true;
#endif
	}
}
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi == &SpiHandle[SPI_WIFI].SpiHalHandle)
	{
#if RTE_USE_OS
		osEventFlagsSet(EvtIDWIFI,0x00000002U);
#else
		if(LCDSPIReadyFlag == false)
			LCDSPIReadyFlag = true;
#endif
	}
}
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi == &SpiHandle[SPI_WIFI].SpiHalHandle)
	{
#if RTE_USE_OS
		osEventFlagsSet(EvtIDWIFI,0x00000003U);
#else
		if(LCDSPIReadyFlag == false)
			LCDSPIReadyFlag = true;
#endif
	}
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == GPIO_PIN_4)
  {
		DRIVER_INT_HANDLE();
  }
}

