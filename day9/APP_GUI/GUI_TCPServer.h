#ifndef __GUI_TCPSERVER_H
#define __GUI_TCPSERVER_H
#include "RTE_Include.h"
typedef struct
{
	//root¿Ø¼þ
	lv_obj_t * wintcpserver;
	//sub
	lv_obj_t *labelip;
	lv_obj_t *labelportnum;
	lv_obj_t *taportnum;
	lv_obj_t *mboxall;
	lv_obj_t *btnm;
	lv_obj_t *labelopen;
	lv_obj_t *btnopen;
	lv_obj_t *labelclose;
	lv_obj_t *btnclose;
}GUI_TCPServer_t;
extern void GUI_TCPServer_PageInit(void);




#endif
