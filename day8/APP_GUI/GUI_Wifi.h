#ifndef GUI_FILEBROSWER_H
#define GUI_FILEBROSWER_H
#include "RTE_Include.h"
typedef struct
{
	//root控件
	lv_obj_t * winwifi;
	//sub控件
	lv_obj_t * swwifi;
	lv_obj_t * lablesw;
	lv_obj_t * listssid;
	//属于lv_src()的sub控件
	lv_obj_t * mboxall;
	lv_obj_t * keyboard;
	lv_obj_t * kbta;
}GUI_WIFI_t;
extern GUI_WIFI_t WIFI;
extern void GUI_WIFI_PageInit(void);
#endif // GUI_FILEBROSWER_H
