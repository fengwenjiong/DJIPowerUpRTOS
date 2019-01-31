#include "MV_TXT.h"
void txt_read_all(uint8_t **buf,const char *path)
{
    FIL fp;
    File_Module_OpenRead(&fp, path);
    File_Module_RWBufOn(&fp);
    if (!(*buf))
        *buf = RTE_MEM_Alloc0(MEM_AXIM,f_size(&fp));
    File_Module_ReadData(&fp,*buf,f_size(&fp));
    File_Module_RWBufOff(&fp);
    File_Module_Close(&fp);
}
