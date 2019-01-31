#include "Thread_WIFI.h"
#include "Board_WIFI.h"
#include "WIFI_SM.h"
#include "Board_SDCard.h"
/*----------------------------------------------------------------------------
 *      Thread 3 'Thread_Name': WIFI thread
 *---------------------------------------------------------------------------*/
osThreadId_t ThreadIDWIFI;
static uint64_t wifi_thread_stk[4096 / 8] = {0};
const osThreadAttr_t WIFIThreadControl = {
	.stack_mem = &wifi_thread_stk[0],
  .stack_size =  sizeof(wifi_thread_stk) , 
	.priority = osPriorityBelowNormal,
};
__NO_RETURN void ThreadTaskWIFI (void *argument) {
	if(Board_WIFI_Init()!=RW_OK)//Çý¶¯³õÊ¼»¯
		osThreadExit();
	WIFI_SM_Init();
	for (;;) 
	{
		if(WIFISMHandle.SM.RunningState != WIFI_TCPSERVERWAIT&&WIFISMHandle.SM.RunningState != WIFI_TCPSERVERSED)
			osDelay(100);
		RTE_StateMachine_Run(&WIFISMHandle.SM,&WIFISMHandle);
	}
}
