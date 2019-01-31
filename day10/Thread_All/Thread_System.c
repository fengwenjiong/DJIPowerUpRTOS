#include "Thread_System.h"
#include "Thread_GUI.h"
#include "Thread_WIFI.h"
#include "BSP_Led.h"
#include "Board_FatFS.h"
#include "File_Shell_Module.h"
#include "EventRecorder.h"
#include "EventRecorderConf.h"
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Systick thread
 *---------------------------------------------------------------------------*/
osThreadId_t ThreadIDSYS;
static __IO uint32_t  StaticsIdleCnt;        /* 空闲任务计数 */
__IO uint8_t   StaticsCPUUsage;       /* CPU百分比 */
static uint32_t       StaticsIdleCntMax;     /* 1秒内最大的空闲计数 */
static uint32_t       StaticsIdleCntNow;     /* 1秒内空闲任务当前计数 */
static uint64_t system_thread_stk[1024 / 8] = {0};
const osThreadAttr_t SystemThreadControl = {
	.stack_mem = &system_thread_stk[0],
  .stack_size =  sizeof(system_thread_stk) , 
	.priority = osPriorityRealtime,
};
// OS Idle Thread
__NO_RETURN void osRtxIdleThread (void *argument) {
  (void)argument;
  for (;;) 
	{
	  __set_PRIMASK(1);
    StaticsIdleCnt++;
	  __set_PRIMASK(0);
	}
}
static void LEDTimer_Callback(void* UserParameters) {
	UNUSED(UserParameters);
	BSP_LED_Toggle(LED_0);
}
static void SystemStaticsTimer_Callback (void *UserParameters) {
	StaticsIdleCntNow = StaticsIdleCnt;    /* 获得100ms内空闲计数 */
	StaticsIdleCnt    = 0uL;          /* 复位空闲计数 */
	StaticsCPUUsage   = (100uL - (float)StaticsIdleCntNow *100 / StaticsIdleCntMax);  /* 计算100ms内的CPU利用率 */
}
__NO_RETURN void ThreadTaskSYS (void *argument) {
	osDelay(2);
	StaticsIdleCnt = 0; 
	osDelay(100);
	StaticsIdleCntMax = StaticsIdleCnt; 
	RTE_RoundRobin_CreateTimer(0,"LEDTimer",500,1,1,LEDTimer_Callback,(void *)0);
	RTE_RoundRobin_CreateTimer(0,"StaticsTimer",100,1,1,SystemStaticsTimer_Callback,(void *)0);
	RTE_Printf("%10s    TuringBoartPlus Run At %d Hz!\r\n","[SYSTEM]",SystemCoreClock);
	ThreadIDGUI = osThreadNew(ThreadTaskGUI, NULL, &GUIThreadControl); 
	ThreadIDWIFI = osThreadNew(ThreadTaskWIFI, NULL, &WIFIThreadControl); 
	//初始化文件系统模块
	Board_FatFS_Init();
	File_Module_Init();
	File_Shell_Init();
  for (;;) 
	{
		RTE_RoundRobin_TickHandler();
    osDelay(1);
  }
}
