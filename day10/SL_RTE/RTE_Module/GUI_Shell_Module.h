#ifndef __GUI_MODULE_H
#define __GUI_MODULE_H
#include "GUI_Manage.h"
typedef struct
{
    int8_t id;
    GUI_Objx_e type;
    lv_obj_t *body;
}GUI_Objx_t;
typedef vec_t(GUI_Objx_t) GUI_Obj_Vet_t;


extern void GUI_Shell_Init(void);
#endif // __GUI_ENTRANCE_H
