#include "File_Shell_Module.h"
#include "File_Module.h"
#define DEBUG_STR "[FILE]"
static RTE_Shell_Err_e File_Shell_ListDirectory(int argc, char *argv[])
{
    if(argc!=2)
        return SHELL_ARGSERROR;
	RTE_Printf("--------------------------------------------------\r\n");
	RTE_Printf("%10s    Current Directory:%s\r\n",DEBUG_STR,FileModuleHandle.CurrentDir);
    FileModuleHandle.CurrentDirContNum = File_Module_ListDirectory(FileModuleHandle.DirTable,
                                                                   DIR_MAX_NUM,FileModuleHandle.CurrentDir);
    for(uint8_t i=0;i<FileModuleHandle.CurrentDirContNum;i++)
        RTE_Printf("%s\r\n",FileModuleHandle.DirTable[i]);
    return(SHELL_NOERR);
}
void File_Shell_Init(void)
{
    RTE_Shell_CreateModule("fs");
    RTE_Shell_AddCommand("fs","ls",File_Shell_ListDirectory,"Show now direcory content Example:fs.ls");
}

