#include "main.h"
#include "RTE_Components.h"
#include "RTE_Include.h"
#include "EventRecorder.h"
#include "EventRecorderConf.h"
#include <string.h>
#include "RTE_Include.h"
#include "BSP_Systick.h"
#include "BSP_Led.h"
#include "Board_Initial.h"
#include "Board_Uart.h"
#include "Board_MSPInit.h"
#if RTE_USE_OS == 0
static void LEDTimer_Callback(void* UserParameters) {
	EventStopA(0); 
	UNUSED(UserParameters);
	BSP_LED_Toggle(LED_0);
	EventStartA(0); 
}
#else
#include "Thread_System.h"
#endif
void RTE_Puts (const char *pcString,uint16_t length)
{
	HAL_UART_Transmit(&UartHandle[USART_DEBUG].UartHalHandle, (uint8_t *)pcString,length,HAL_MAX_DELAY);
}
int main(void)
{
	Board_Initial();
	BSP_Systick_Init();
	BSP_LED_Init();
	EventRecorderInitialize(EventRecordAll, 1U); 
	EventRecorderStart();
	RTE_Init();
	Board_Uart_Init(USART_DEBUG,Board_Uart1_MSPInitCallback,Board_Uart1_MSPDeInitCallback);
	RTE_Reg_Puts(RTE_Puts);
	RTE_Printf("HelloWorld!\r\n");
#if RTE_USE_OS == 0
	RTE_RoundRobin_CreateTimer(0,"LEDTimer",500,1,1,LEDTimer_Callback,(void *)0);
	for(;;)
	{
		RTE_RoundRobin_Run(0);
  }
#else
	osKernelInitialize();                 // Initialize CMSIS-RTOS
	ThreadIDSYS = osThreadNew(ThreadTaskSYS, NULL, &SystemThreadControl); 
  osKernelStart();   
#endif
}


