#include "Board_HALCallback.h"
#include "Board_Uart.h"
#include "Board_SPI.h"
#include "Board_WIFI.h"
#include "Board_SDCard.h"
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
extern volatile bool WIFISPIReadyFlag;
#endif
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi == &SpiHandle[SPI_LCD].SpiHalHandle)
	{
#if RTE_USE_OS
		osThreadFlagsSet(ThreadIDGUI,0x00000001U);
#else
		LCDSPIReadyFlag = true;
#endif
	}
	if(hspi == &SpiHandle[SPI_WIFI].SpiHalHandle)
	{
#if RTE_USE_OS
		osEventFlagsSet(EvtIDWIFI,0x00000001U);
#else
		WIFISPIReadyFlag = true;
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
		WIFISPIReadyFlag = true;
#endif
	}
}
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi == &SpiHandle[SPI_WIFI].SpiHalHandle)
	{
		RTE_Printf("HAL_SPI_ErrorCallback[SPI_WIFI]:%x\r\n",hspi->ErrorCode);
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
void HAL_SD_ErrorCallback(SD_HandleTypeDef *hsd)
{
	RTE_Printf("HAL_SD_ErrorCallback:%x\r\n",hsd->ErrorCode);
}
#if RTE_USE_OS
extern osEventFlagsId_t EvtIDSD;
#else
extern volatile bool SDRxReadyFlag,SDTxReadyFlag;
#endif
void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd)
{
  if(Board_SD_WaitReady() != true)
  {
    RTE_Assert(__FILE__, __LINE__);
  }
#if RTE_USE_OS
	osEventFlagsSet(EvtIDSD,0x00000001u);
#else
  SDRxReadyFlag = true;
#endif
}
void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd)
{
  if(Board_SD_WaitReady() != true)
  {
    RTE_Assert(__FILE__, __LINE__);
  }
#if RTE_USE_OS
	osEventFlagsSet(EvtIDSD,0x00000002u);
#else
  SDTxReadyFlag = true;
#endif
}
