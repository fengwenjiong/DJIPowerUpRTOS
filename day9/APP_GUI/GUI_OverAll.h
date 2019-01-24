#ifndef __GUI_OVER_ALL
#define __GUI_OVER_ALL
#include "RTE_Include.h"
typedef struct
{
	//root object
	lv_obj_t *tabview;
	//sub object
	lv_obj_t *iconwifi;
	lv_obj_t *btnwifi;
	
	lv_obj_t *iconbench;
	lv_obj_t *btnbench;

	lv_obj_t *iconclock;
	lv_obj_t *btnclock;
	
	lv_obj_t *icontcpserver;
	lv_obj_t *btntcpserver;
	
	
	lv_obj_t *iconinfor[4];
	lv_obj_t *labelinfor[8];
	lv_obj_t *continfor;
	
	lv_style_t style_wp;
}GUI_OverAll_t;
extern GUI_OverAll_t OverAllPage;
extern void GUI_OverAll_Init(void);
/*Create a scroll bar style*/
extern lv_style_t overall_style_sb;
/*Create styles for the buttons*/
extern lv_style_t overall_style_btn_rel;
extern lv_style_t overall_style_btn_pr;
#endif
