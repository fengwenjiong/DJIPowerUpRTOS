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
#include "Board_SPI.h"
#include "Board_LCD.h"
#include "Board_SDCard.h"
#if RTE_USE_OS == 0
static void LEDTimer_Callback(void* UserParameters) {
	EventStopA(0); 
	UNUSED(UserParameters);
	BSP_LED_Toggle(LED_0);
	EventStartA(0); 
}
static void GUITimer_Callback(void* UserParameters) {
	RTE_RoundRobin_Run(1);
	lv_refr_now();
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
	Board_Spi_Init(SPI_LCD,Board_Spi4_MSPInitCallback,Board_Spi4_MSPDeInitCallback);
	Board_Spi_Init(SPI_WIFI,Board_Spi1_MSPInitCallback,Board_Spi1_MSPDeInitCallback);
	Board_SD_Init();
//	Board_SD_Test();
#if RTE_USE_OS == 0
	RTE_RoundRobin_CreateTimer(0,"LEDTimer",500,1,1,LEDTimer_Callback,(void *)0);
	RTE_RoundRobin_CreateGroup("GUIGroup");
	RTE_RoundRobin_CreateTimer(0,"GUIGroupTimer",5,1,1,GUITimer_Callback,(void *)0);
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
	lv_obj_t * label1 =  lv_label_create(lv_scr_act(), NULL);
	lv_label_set_text(label1, "Welcome to DJI-POWER-UP!");
	lv_obj_align(label1, NULL, LV_ALIGN_CENTER, 0, 0);
	lv_obj_t * button = lv_btn_create(lv_scr_act(), NULL);
	for(;;)
	{
		RTE_RoundRobin_Run(0);
  }
#else
	osKernelInitialize();                 // Initialize CMSIS-RTOS
	ThreadIDSYS = osThreadNew(ThreadTaskSYS, NULL, &SystemThreadControl); 
  osKernelStart();   
	for(;;)
	{
		
	}
#endif
  
}


