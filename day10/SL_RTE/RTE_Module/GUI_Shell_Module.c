#include "GUI_Shell_Module.h"
#include <stdlib.h>
#define DEBUG_STR "[LVGL]"
static GUI_Obj_Vet_t GUIModuleGroup = {0};
static GUI_Objx_t* GUI_Obj_New(GUI_Obj_Vet_t *Group,GUI_Objx_e type,lv_obj_t *father,lv_obj_t *templet)
{
    GUI_Objx_t v = {0};
    v.id = Group->length;
    v.type = type;
    v.body = str2objTable[type].design(father,templet);
    vec_push(Group, v);
    GUI_Objx_t *retval =  &vec_last(Group);
    return retval;
}
static void GUI_Obj_Del(GUI_Obj_Vet_t *Group,uint8_t id)
{
    if(id >= Group->length)
        return;
    lv_obj_del(Group->data[id].body);
    Group->data[id].body = NULL;
    Group->data[id].id = -1;
    Group->data[id].type = GUI_NONE;
    vec_splice(Group, id, 1);
    for(uint8_t i = id;i<Group->length;i++)
    {
        Group->data[id].id--;
    }
}
static GUI_Objx_e GUI_Shell_str2type(char *str)
{
    GUI_Objx_e type = GUI_NONE;
    for(uint8_t i=0;i<GUI_OBJ_CNT;i++)
    {
        if(ustrcmp(str,str2objTable[i].name)==0)
        {
            type = str2objTable[i].type;
            break;
        }
    }
    return type;
}
static RTE_Shell_Err_e GUI_Shell_Help(int argc, char *argv[])
{
    if(argc!=2)
        return SHELL_ARGSERROR;
	RTE_Printf("--------------------------------------------------\r\n");
	RTE_Printf("%10s    Available Objx\r\n",DEBUG_STR);
    for(uint8_t i=0;i<GUI_OBJ_CNT;i++)
    {
         RTE_Printf("%12s",str2objTable[i].name);
         if(i%8==0&&i!=0)
            RTE_Printf("\r\n");
    }
    RTE_Printf("\r\n");
    return(SHELL_NOERR);
}
static RTE_Shell_Err_e GUI_Shell_NewOBJ(int argc, char *argv[])
{
    if(argc!=5&&argc!=7&&argc!=3)
        return SHELL_ARGSERROR;
    GUI_Objx_e type = GUI_Shell_str2type(argv[2]);
    if(type == GUI_NONE)
        RTE_Printf("Can't identify such objx!\r\n");
    else
    {
        GUI_Objx_t *v = GUI_Obj_New(&GUIModuleGroup,type,lv_scr_act(),NULL);
        RTE_Printf("%10s    New Objx ID:%d Type:%s\r\n",DEBUG_STR,v->id,argv[2]);
        lv_obj_set_drag(v->body,true);
        if(argc==5)
            lv_obj_set_pos(v->body,(lv_coord_t)atoi(argv[3]),(lv_coord_t)atoi(argv[4]));
        if(argc==7)
            lv_obj_set_size(v->body,(lv_coord_t)atoi(argv[5]),(lv_coord_t)atoi(argv[6]));
    }
	return(SHELL_NOERR);
}
static RTE_Shell_Err_e GUI_Shell_DelOBJ(int argc, char *argv[])
{
    if(argc!=3)
        return SHELL_ARGSERROR;
    int8_t id = atoi(argv[2]);
    if(id!=-1)
    {
        RTE_Printf("%10s    Del Objx ID:%d\r\n",DEBUG_STR,id);
        GUI_Obj_Del(&GUIModuleGroup,id);
    }
    else
        RTE_Printf("%10s    No suce Objx!\r\n",DEBUG_STR);
	return(SHELL_NOERR);
}
static RTE_Shell_Err_e GUI_Shell_ListOBJ(int argc, char *argv[])
{
    if(argc!=2)
        return SHELL_ARGSERROR;
	RTE_Printf("--------------------------------------------------\r\n");
	RTE_Printf("%10s    All Objx\r\n",DEBUG_STR);
    for(uint8_t i = 0;i<GUIModuleGroup.length;i++)
    {
        RTE_Printf("%10s    Objx ID:%d Type:%s\r\n",DEBUG_STR,GUIModuleGroup.data[i].id,
                   str2objTable[GUIModuleGroup.data[i].type].name);
    }
	return(SHELL_NOERR);
}
void GUI_Shell_Init(void)
{
    RTE_Shell_CreateModule("lvgl");
    RTE_Shell_AddCommand("lvgl","help",GUI_Shell_Help,"Show all support lvgl obj Example:lvgl.help");
    RTE_Shell_AddCommand("lvgl","newobj",GUI_Shell_NewOBJ,"Create a new obj in lvgl srceen Example:lvgl.newobj(type,x1,y1,w,h)");
    RTE_Shell_AddCommand("lvgl","delobj",GUI_Shell_DelOBJ,"Deleat a obj in lvgl srceen Example:lvgl.delobj(id)");
    RTE_Shell_AddCommand("lvgl","listobj",GUI_Shell_ListOBJ,"List all objs in lvgl srceen Example:lvgl.listobj");
    vec_init(&GUIModuleGroup);
}

