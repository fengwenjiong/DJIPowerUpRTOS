#ifndef __FILE_MODULE_H
#define __FILE_MODULE_H
#include "RTE_Include.h"
#include "Board_FatFS.h"
#define FILE_RW_BUFFER_SIZE 16*1024
#define DIR_MAX_NUM 64
#define DIR_MAX_LEN 256
typedef struct
{
    uint8_t CurrentDirContNum;
    char *CurrentDir;
    char *LastDir;
    char *FatherDir;
    char **DirTable;
    uint8_t *RWBuffer;
    uint8_t *RWBufferPointer;
    uint32_t RWBufferOffset;
    uint32_t RWBufferSize;
    uint32_t RWBufferIndex;
}File_Module_t;
extern File_Module_t FileModuleHandle;
extern void File_Module_Error(FIL *fp,const char * dsc,...);
extern void File_Module_OpenRead(FIL *fp, const char *path);
extern void File_Module_Open(FIL *fp, const char *path);
extern void File_Module_Close(FIL *fp);
extern void File_Module_Seek(FIL *fp, UINT offset);
extern void File_Module_Truncate(FIL *fp);
extern void File_Module_Sync(FIL *fp);
extern void File_Module_RWBufOn(FIL *fp);
extern void File_Module_RWBufOff(FIL *fp);
extern uint32_t File_Module_TellWBuf(FIL *fp);
extern uint32_t File_Module_SizeWBuf(FIL *fp);
extern void File_Module_ReadByte(FIL *fp, uint8_t *value);
extern void File_Module_CompareByte(FIL *fp, uint8_t value);
extern void File_Module_IgnoreByte(FIL *fp);
extern void File_Module_ReadWord(FIL *fp, uint16_t *value);
extern void File_Module_CompareWord(FIL *fp, uint16_t value);
extern void File_Module_IgnoreWord(FIL *fp);
extern void File_Module_ReadLong(FIL *fp, uint32_t *value);
extern void File_Module_CompareLong(FIL *fp, uint32_t value);
extern void File_Module_IgnoreLong(FIL *fp);
extern void File_Module_ReadData(FIL *fp, void *data, UINT size);
extern void File_Module_WriteByte(FIL *fp, uint8_t value);
extern void File_Module_WriteWord(FIL *fp, uint16_t value);
extern void File_Module_WriteLong(FIL *fp, uint32_t value);

extern uint8_t File_Module_ListDirectory(char **DirectoryBuffer,uint8_t DirectoryBufferNum,const char* DirName);
extern void File_Module_CleanDirectoryBuffer(char **DirectoryBuffer,uint8_t DirectoryBufferNum);
extern void File_Module_Init(void);
#endif // __FILE_MODULE_h
