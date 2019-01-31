#include "GUI_Manage.h"
void GUI_Manage_NewRoot(GUI_Objx_e type,GUI_RootObj_t *root,lv_obj_t *father,lv_obj_t *templet,uint8_t capbility)
{
    root->datatabel = RTE_MEM_Alloc0(MEM_RTE,sizeof(GUI_SubObj_t *)*capbility);
    root->type = type;
    root->rootobj = str2objTable[type].design(father,templet);
    root->length = 0;
    root->capbility = capbility;
}
void GUI_Manage_DelRoot(GUI_RootObj_t *root)
{
    for(uint8_t i=0;i<root->length;i++)
    {
        if(root->datatabel[i]->body)
        {
            lv_obj_del(root->datatabel[i]->body);
            root->datatabel[i]->body = NULL;
            root->datatabel[i]->id = -1;
            root->datatabel[i]->type = GUI_NONE;
            RTE_MEM_Free(MEM_RTE,root->datatabel[i]);
        }
    }
    lv_obj_del(root->rootobj);
    root->type = GUI_NONE;
    root->rootobj = NULL;
    root->capbility = 0;
    root->length = 0;
    RTE_MEM_Free(MEM_RTE,root->datatabel);
    root->datatabel = 0;
}
GUI_SubObj_t *GUI_Manage_NewSub(GUI_Objx_e type,GUI_RootObj_t *root,lv_obj_t *father,lv_obj_t *templet)
{
    if(root->length>=root->capbility)
        return NULL;
    root->datatabel[root->length] = RTE_MEM_Alloc0(MEM_RTE,sizeof(GUI_SubObj_t));
    root->datatabel[root->length]->id = root->length;
    root->datatabel[root->length]->type = type;
    if(father == NULL)
        root->datatabel[root->length]->body = str2objTable[type].design(root->rootobj,templet);
    else
        root->datatabel[root->length]->body = str2objTable[type].design(father,templet);
    return root->datatabel[root->length++];
}
GUI_SubObj_t *GUI_Manage_FindSub(GUI_RootObj_t *root,uint8_t id)
{
    for(uint8_t i=0;i<root->length;i++)
    {
        if(id == root->datatabel[i]->id)
            return root->datatabel[i];
    }
    return NULL;
}
void GUI_Manage_DelSub(GUI_RootObj_t *root,uint8_t id)
{
    if(root->datatabel[id]->body)
        lv_obj_del(root->datatabel[id]->body);
    root->datatabel[id]->body = NULL;
    root->datatabel[id]->id = -1;
    root->datatabel[id]->type = GUI_NONE;
    for(uint8_t i = id;i<root->length;i++)
    {
        root->datatabel[i]->id--;
    }
    RTE_MEM_Free(MEM_RTE,root->datatabel[id]);
    root->datatabel[id] = 0;
    if(id!=root->length-1)
    {
        memmove(root->datatabel[id],root->datatabel[id+1],(root->length -id - 1)*sizeof(GUI_SubObj_t));
    }
    root->length--;
}
