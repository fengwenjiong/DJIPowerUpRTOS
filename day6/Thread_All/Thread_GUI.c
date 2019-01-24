#include "Thread_GUI.h"
#include "Board_LCD.h"
osThreadId_t ThreadIDGUI;
uint64_t gui_thread_stk[2048/8] = {0};
const osThreadAttr_t GUIThreadControl = {
	.stack_mem = &gui_thread_stk[0],
  .stack_size =  sizeof(gui_thread_stk) , 
	.priority = osPriorityBelowNormal,
};
extern void benchmark_create();
__NO_RETURN void ThreadTaskGUI (void *argument) {
	RTE_RoundRobin_CreateGroup("GUIGroup");
	Board_LCD_Init();
	lv_init();
	lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);
	disp_drv.disp_fill = Board_LCD_FillFrameNormal;
	disp_drv.disp_map = NULL;
	disp_drv.disp_flush = Board_LCD_Map;
	lv_disp_drv_register(&disp_drv);
  lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.read = Board_GUI_TouchScan;
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  lv_indev_drv_register(&indev_drv);
	benchmark_create();
  for (;;) 
	{
		lv_refr_now();
	  RTE_RoundRobin_Run(RTE_RoundRobin_GetGroupID("GUIGroup"));
		osDelay(5);
  }
}
