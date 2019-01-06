#include "MV_File.h"
#include "Board_FatFS.h"
#define FILE_DEBUG_STR    "[FILE]"
uint8_t File_List_Directory(char **DirectoryBuffer,uint8_t DirectoryBufferNum,const char* DirName)
{
  FRESULT res;
  FILINFO fno;
  DIR dir;
	uint8_t DirectoryCnt = 0;
  /* Open directory */
  res = f_opendir(&dir, (TCHAR const*)DirName);
  if (res == FR_OK)
  {
    for (;;)
    {
      res = f_readdir(&dir, &fno);
      if (res != FR_OK || fno.fname[0] == 0 || DirectoryCnt>DirectoryBufferNum)
        break;
      if (fno.fname[0] == '.')
        continue;
			DirectoryBuffer[DirectoryCnt] = RTE_MEM_Realloc(MEM_RTE,DirectoryBuffer[DirectoryCnt],strlen(fno.fname)+1);
			memset(DirectoryBuffer[DirectoryCnt],0,RTE_MEM_GetDataSize(DirectoryBuffer[DirectoryCnt]));
			memcpy(DirectoryBuffer[DirectoryCnt],fno.fname,strlen(fno.fname)+1);
			DirectoryCnt++;
    }
  }
	return DirectoryCnt;
}

