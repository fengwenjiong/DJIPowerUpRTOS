#include "Thread_GUI.h"
#include "GUI_TouchCorrect.h"
#include "Board_LCD.h"
#include "GUI_Shell_Module.h"
osThreadId_t ThreadIDGUI;
uint64_t gui_thread_stk[4096/8] = {0};
const osThreadAttr_t GUIThreadControl = {
	.stack_mem = &gui_thread_stk[0],
  .stack_size =  sizeof(gui_thread_stk) , 
	.priority = osPriorityLow,
};
static void SystemInitFinish_Callback(void* UserParameters) {
	lv_obj_clean(lv_scr_act());
    //进入用户GUI界面
    GUI_TouchCorrect_create();
}
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
	lv_indev_t * mouse_indev = lv_indev_drv_register(&indev_drv);
    //为输入设备设定显示图标
    LV_IMG_DECLARE(img_cursor);                          /*Declare the image file.*/
    lv_obj_t * cursor_obj =  lv_img_create(lv_scr_act(), NULL); /*Create an image object for the cursor */
    lv_img_set_src(cursor_obj, &img_cursor);             /*Set the image source*/
    lv_indev_set_cursor(mouse_indev, cursor_obj);
    //初始化lvglshell模块
    GUI_Shell_Init();
    //主题配置
	lv_theme_t *th = lv_theme_night_init(210, NULL);
	lv_theme_set_current(th);
    //加载启动动画
    lv_obj_t *preload = lv_preload_create(lv_scr_act(),NULL);
    lv_preload_set_spin_time(preload,2000);
    lv_obj_t *label = lv_label_create(lv_scr_act(),NULL);
    lv_label_set_text(label,"System starting...");
    lv_obj_align(preload,lv_scr_act(),LV_ALIGN_CENTER,0,-20);
    lv_obj_align(label,preload,LV_ALIGN_OUT_BOTTOM_MID,0,5);
    //开启启动完毕回调定时器
    RTE_RoundRobin_CreateTimer(1,"InitOver",2000,0,1,SystemInitFinish_Callback,NULL);
	for (;;) 
	{
		lv_refr_now();
		RTE_RoundRobin_Run(RTE_RoundRobin_GetGroupID("GUIGroup"));
		osDelay(5);
	}
}
