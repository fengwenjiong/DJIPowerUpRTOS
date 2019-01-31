#ifndef __GUI_MANAGE_H
#define __GUI_MANAGE_H
#include "RTE_Include.h"
typedef enum
{
    GUI_NONE = -1,
    GUI_ARC = 0,
    GUI_BAR = 1,
    GUI_BTN = 2,
    GUI_BTNM = 3,
    GUI_CALENDAR = 4,
    GUI_CB = 5,
    GUI_CHART = 6,
    GUI_CONT = 7,
    GUI_DDLIST = 8,
    GUI_GAUGE = 9,
    GUI_IMG = 10,
    GUI_IMGBTN = 11,
    GUI_KB = 12,
    GUI_LABEL = 13,
    GUI_LED = 14,
    GUI_LINE = 15,
    GUI_LIST = 16,
    GUI_LMETER = 17,
    GUI_MBOX = 18,
    GUI_PAGE = 19,
    GUI_PRELOAD = 20,
    GUI_ROLLER = 21,
    GUI_SLIDER = 22,
    GUI_SW = 23,
    GUI_TA = 24,
    GUI_TABVIEW = 25,
    GUI_WIN = 26,
    GUI_OBJ_CNT,
}GUI_Objx_e;
typedef struct
{
    const char *name;
    GUI_Objx_e type;
    lv_obj_t* (*design)(lv_obj_t * par, const lv_obj_t * copy);
}GUI_str2objTable_t;
extern const GUI_str2objTable_t str2objTable[GUI_OBJ_CNT];
typedef struct
{
    int8_t id;
    int8_t type;
    lv_obj_t *body;
}GUI_SubObj_t;
typedef struct
{
    int8_t type;
    uint8_t length;
    uint8_t capbility;
    lv_obj_t *rootobj;
    GUI_SubObj_t **datatabel;
}GUI_RootObj_t;
void GUI_Manage_NewRoot(GUI_Objx_e type,GUI_RootObj_t *root,lv_obj_t *father,lv_obj_t *templet,uint8_t capbility);
void GUI_Manage_DelRoot(GUI_RootObj_t *root);

GUI_SubObj_t *GUI_Manage_NewSub(GUI_Objx_e type,GUI_RootObj_t *root,lv_obj_t *father,lv_obj_t *templet);
GUI_SubObj_t *GUI_Manage_FindSub(GUI_RootObj_t *root,uint8_t id);
void GUI_Manage_DelSub(GUI_RootObj_t *root,uint8_t id);


#endif // __GUI_MANAGE_H
