#include "File_Module.h"
#define DEBUG_STR "[FILE]"
File_Module_t FileModuleHandle = {0};
void File_Module_Error(FIL *fp,const char * dsc,...)
{
    if(fp)
        f_close(fp);
    RTE_Printf("%10s    ",DEBUG_STR);
	va_list vaArgP;
	// Start the varargs processing.
	va_start(vaArgP, dsc);
    RTE_Vprintf(dsc, vaArgP);
    va_end(vaArgP);
    while(1);
}
uint8_t File_Module_ListDirectory(char **DirectoryBuffer,uint8_t DirectoryBufferNum,const char* DirName)
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
            DirectoryBuffer[DirectoryCnt] = RTE_MEM_Realloc(MEM_AXIM,DirectoryBuffer[DirectoryCnt],(strlen(fno.fname)+1*2/2));
            memset(DirectoryBuffer[DirectoryCnt],0,RTE_MEM_GetDataSize(DirectoryBuffer[DirectoryCnt]));
            memcpy(DirectoryBuffer[DirectoryCnt],fno.fname,strlen(fno.fname)+1);
            DirectoryCnt++;
        }
    }

	return DirectoryCnt;
}
void File_Module_CleanDirectoryBuffer(char **DirectoryBuffer,uint8_t DirectoryBufferNum)
{
    for(uint8_t i=0;i<DirectoryBufferNum;i++)
    {
        if(DirectoryBuffer[i])
            RTE_MEM_Free(MEM_AXIM,DirectoryBuffer[i]);
    }
}
void File_Module_OpenRead(FIL *fp, const char *path)
{
    FRESULT res = f_open(fp, path, FA_READ|FA_OPEN_EXISTING);
    if (res != FR_OK)
        File_Module_Error(fp,"%s fail:%d",__FUNCTION__,res);
}
void File_Module_Open(FIL *fp, const char *path)
{
    FRESULT res = f_open(fp, path, FA_WRITE|FA_CREATE_ALWAYS);
    if (res != FR_OK)
        File_Module_Error(fp,"%s fail:%d",__FUNCTION__,res);
}
void File_Module_Close(FIL *fp)
{
    FRESULT res = f_close(fp);
    if (res != FR_OK)
        File_Module_Error(fp,"%s fail:%d",__FUNCTION__,res);
}
void File_Module_Seek(FIL *fp, UINT offset)
{
    FRESULT res = f_lseek(fp, offset);
    if (res != FR_OK)
        File_Module_Error(fp,"%s fail:%d",__FUNCTION__,res);
}
void File_Module_Truncate(FIL *fp)
{
    FRESULT res = f_truncate(fp);
    if (res != FR_OK)
        File_Module_Error(fp,"%s fail:%d",__FUNCTION__,res);
}
void File_Module_Sync(FIL *fp)
{
    FRESULT res = f_sync(fp);
    if (res != FR_OK)
        File_Module_Error(fp,"%s fail:%d",__FUNCTION__,res);
}
void File_Module_RWBufOn(FIL *fp)
{
    FileModuleHandle.RWBufferOffset = f_tell(fp) % 4;
    FileModuleHandle.RWBufferSize = FILE_RW_BUFFER_SIZE;
    FileModuleHandle.RWBuffer = (uint8_t *)RTE_MEM_Alloc0(MEM_AXIM,FILE_RW_BUFFER_SIZE);
    RTE_AssertParam(FileModuleHandle.RWBuffer);
    FileModuleHandle.RWBufferPointer = FileModuleHandle.RWBuffer + FileModuleHandle.RWBufferOffset;
    FileModuleHandle.RWBufferSize -= FileModuleHandle.RWBufferOffset;
    FileModuleHandle.RWBufferIndex = 0;
    if (fp->flag & FA_READ)
    {
        uint32_t file_remaining = f_size(fp) - f_tell(fp);
        uint32_t can_do = MATH_MIN(FileModuleHandle.RWBufferSize, file_remaining);
        UINT bytes;
        FRESULT res = f_read(fp, FileModuleHandle.RWBufferPointer, can_do, &bytes);
        if (res != FR_OK)
            File_Module_Error(fp,"%s fail:%d",__FUNCTION__,res);
        if (bytes != can_do)
            File_Module_Error(fp,"%s f_read lenth err!",__FUNCTION__);
    }
}
void File_Module_RWBufOff(FIL *fp)
{
    if ((fp->flag & FA_WRITE) && FileModuleHandle.RWBufferIndex) {
        UINT bytes;
        FRESULT res = f_write(fp, FileModuleHandle.RWBufferPointer, FileModuleHandle.RWBufferIndex, &bytes);
        if (res != FR_OK)
            File_Module_Error(fp,"%s fail:%d",__FUNCTION__,res);
        if (bytes != FileModuleHandle.RWBufferIndex)
            File_Module_Error(fp,"%s f_write lenth err!",__FUNCTION__);
    }
    FileModuleHandle.RWBufferPointer = 0;
    RTE_MEM_Free(MEM_AXIM,FileModuleHandle.RWBuffer);
}
inline static void File_Module_FillBuffer(FIL *fp)
{
    if (FileModuleHandle.RWBufferIndex == FileModuleHandle.RWBufferSize) {
        FileModuleHandle.RWBufferPointer -= FileModuleHandle.RWBufferOffset;
        FileModuleHandle.RWBufferSize += FileModuleHandle.RWBufferOffset;
        FileModuleHandle.RWBufferOffset = 0;
        FileModuleHandle.RWBufferIndex = 0;
        uint32_t file_remaining = f_size(fp) - f_tell(fp);
        uint32_t can_do = MATH_MIN(FileModuleHandle.RWBufferSize, file_remaining);
        UINT bytes;
        FRESULT res = f_read(fp, FileModuleHandle.RWBufferPointer, can_do, &bytes);
        if (res != FR_OK)
            File_Module_Error(fp,"%s fail:%d",__FUNCTION__,res);
        if (bytes != can_do)
            File_Module_Error(fp,"%s f_read lenth err!",__FUNCTION__);
    }
}
inline static void File_Module_FlushBuffer(FIL *fp)
{
    if (FileModuleHandle.RWBufferIndex == FileModuleHandle.RWBufferSize) {
        UINT bytes;
        FRESULT res = f_write(fp, FileModuleHandle.RWBufferPointer, FileModuleHandle.RWBufferIndex, &bytes);
        if (res != FR_OK)
            File_Module_Error(fp,"%s fail:%d",__FUNCTION__,res);
        if (bytes != FileModuleHandle.RWBufferIndex)
            File_Module_Error(fp,"%s f_write lenth err!",__FUNCTION__);
        FileModuleHandle.RWBufferPointer -= FileModuleHandle.RWBufferOffset;
        FileModuleHandle.RWBufferSize += FileModuleHandle.RWBufferOffset;
        FileModuleHandle.RWBufferOffset = 0;
        FileModuleHandle.RWBufferIndex = 0;
    }
}
uint32_t File_Module_TellWBuf(FIL *fp)
{
    if (fp->flag & FA_READ) {
        return f_tell(fp) - FileModuleHandle.RWBufferSize + FileModuleHandle.RWBufferIndex;
    } else {
        return f_tell(fp) + FileModuleHandle.RWBufferIndex;
    }
}
uint32_t File_Module_SizeWBuf(FIL *fp)
{
    if (fp->flag & FA_READ) {
        return f_size(fp);
    } else {
        return f_size(fp) + FileModuleHandle.RWBufferIndex;
    }
}
void File_Module_ReadByte(FIL *fp, uint8_t *value)
{
    if (FileModuleHandle.RWBufferPointer) {
        // We get a massive speed boost by buffering up as much data as possible
        // via massive reads. So much so that the time wasted by
        // all these operations does not cost us.
        for (size_t i = 0; i < sizeof(*value); i++) {
            File_Module_FillBuffer(fp);
            ((uint8_t *) value)[i] = FileModuleHandle.RWBufferPointer[FileModuleHandle.RWBufferIndex++];
        }
    } else {
        UINT bytes;
        FRESULT res = f_read(fp, value, sizeof(*value), &bytes);
        if (res != FR_OK)
            File_Module_Error(fp,"%s f_read:%d",__FUNCTION__,res);
        if (bytes != sizeof(*value))
            File_Module_Error(fp,"%s f_read lenth err!",__FUNCTION__);
    }
}
void File_Module_CompareByte(FIL *fp, uint8_t value)
{
    uint8_t compare;
    File_Module_ReadByte(fp, &compare);
    if (value != compare)
        File_Module_Error(fp,"%s real:%x expect:%x",__FUNCTION__,compare,value);
}
void File_Module_IgnoreByte(FIL *fp)
{
    uint8_t trash;
    File_Module_ReadByte(fp, &trash);
}

void File_Module_ReadWord(FIL *fp, uint16_t *value)
{
    if (FileModuleHandle.RWBufferPointer) {
        // We get a massive speed boost by buffering up as much data as possible
        // via massive reads. So much so that the time wasted by
        // all these operations does not cost us.
        for (size_t i = 0; i < sizeof(*value); i++) {
            File_Module_FillBuffer(fp);
            ((uint8_t *) value)[i] = FileModuleHandle.RWBufferPointer[FileModuleHandle.RWBufferIndex++];
        }
    } else {
        UINT bytes;
        FRESULT res = f_read(fp, value, sizeof(*value), &bytes);
        if (res != FR_OK)
            File_Module_Error(fp,"%s f_read:%d",__FUNCTION__,res);
        if (bytes != sizeof(*value))
            File_Module_Error(fp,"%s f_read lenth err!",__FUNCTION__);
    }
}
void File_Module_CompareWord(FIL *fp, uint16_t value)
{
    uint16_t compare;
    File_Module_ReadWord(fp, &compare);
    if (value != compare)
        File_Module_Error(fp,"%s real:%x expect:%x",__FUNCTION__,compare,value);
}
void File_Module_IgnoreWord(FIL *fp)
{
    uint16_t trash;
    File_Module_ReadWord(fp, &trash);
}

void File_Module_ReadLong(FIL *fp, uint32_t *value)
{
    if (FileModuleHandle.RWBufferPointer) {
        // We get a massive speed boost by buffering up as much data as possible
        // via massive reads. So much so that the time wasted by
        // all these operations does not cost us.
        for (size_t i = 0; i < sizeof(*value); i++) {
            File_Module_FillBuffer(fp);
            ((uint8_t *) value)[i] = FileModuleHandle.RWBufferPointer[FileModuleHandle.RWBufferIndex++];
        }
    } else {
        UINT bytes;
        FRESULT res = f_read(fp, value, sizeof(*value), &bytes);
        if (res != FR_OK)
            File_Module_Error(fp,"%s f_read:%d",__FUNCTION__,res);
        if (bytes != sizeof(*value))
            File_Module_Error(fp,"%s f_read lenth err!",__FUNCTION__);
    }
}
void File_Module_CompareLong(FIL *fp, uint32_t value)
{
    uint32_t compare;
    File_Module_ReadLong(fp, &compare);
    if (value != compare)
        File_Module_Error(fp,"%s real:%x expect:%x",__FUNCTION__,compare,value);
}
void File_Module_IgnoreLong(FIL *fp)
{
    uint32_t trash;
    File_Module_ReadLong(fp, &trash);
}
void File_Module_ReadData(FIL *fp, void *data, UINT size)
{
    if (FileModuleHandle.RWBufferPointer) {
        // We get a massive speed boost by buffering up as much data as possible
        // via massive reads. So much so that the time wasted by
        // all these operations does not cost us.
        while (size) {
            File_Module_FillBuffer(fp);
            uint32_t file_buffer_space_left = FileModuleHandle.RWBufferSize - FileModuleHandle.RWBufferIndex;
            uint32_t can_do = MATH_MIN(size, file_buffer_space_left);
            memcpy(data, FileModuleHandle.RWBufferPointer + FileModuleHandle.RWBufferIndex, can_do);
            FileModuleHandle.RWBufferIndex += can_do;
            data += can_do;
            size -= can_do;
        }
    } else {
        UINT bytes;
        FRESULT res = f_read(fp, data, size, &bytes);
        if (res != FR_OK)
            File_Module_Error(fp,"%s f_read:%d",__FUNCTION__,res);
        if (bytes != size)
            File_Module_Error(fp,"%s f_read lenth err!",__FUNCTION__);
    }
}
void File_Module_WriteByte(FIL *fp, uint8_t value)
{
    if (FileModuleHandle.RWBufferPointer) {
        // We get a massive speed boost by buffering up as much data as possible
        // before a write to the SD card. So much so that the time wasted by
        // all these operations does not cost us.
        for (size_t i = 0; i < sizeof(value); i++) {
            FileModuleHandle.RWBufferPointer[FileModuleHandle.RWBufferIndex++] = ((uint8_t *) &value)[i];
            File_Module_FlushBuffer(fp);
        }
    } else {
        UINT bytes;
        FRESULT res = f_write(fp, &value, sizeof(value), &bytes);
        if (res != FR_OK)
            File_Module_Error(fp,"%s f_write:%d",__FUNCTION__,res);
        if (bytes != sizeof(value))
            File_Module_Error(fp,"%s f_write lenth err!",__FUNCTION__);
    }
}
void File_Module_WriteWord(FIL *fp, uint16_t value)
{
    if (FileModuleHandle.RWBufferPointer) {
        // We get a massive speed boost by buffering up as much data as possible
        // before a write to the SD card. So much so that the time wasted by
        // all these operations does not cost us.
        for (size_t i = 0; i < sizeof(value); i++) {
            FileModuleHandle.RWBufferPointer[FileModuleHandle.RWBufferIndex++] = ((uint8_t *) &value)[i];
            File_Module_FlushBuffer(fp);
        }
    } else {
        UINT bytes;
        FRESULT res = f_write(fp, &value, sizeof(value), &bytes);
        if (res != FR_OK)
            File_Module_Error(fp,"%s f_write:%d",__FUNCTION__,res);
        if (bytes != sizeof(value))
            File_Module_Error(fp,"%s f_write lenth err!",__FUNCTION__);
    }
}
void File_Module_WriteLong(FIL *fp, uint32_t value)
{
    if (FileModuleHandle.RWBufferPointer) {
        // We get a massive speed boost by buffering up as much data as possible
        // before a write to the SD card. So much so that the time wasted by
        // all these operations does not cost us.
        for (size_t i = 0; i < sizeof(value); i++) {
            FileModuleHandle.RWBufferPointer[FileModuleHandle.RWBufferIndex++] = ((uint8_t *) &value)[i];
            File_Module_FlushBuffer(fp);
        }
    } else {
        UINT bytes;
        FRESULT res = f_write(fp, &value, sizeof(value), &bytes);
        if (res != FR_OK)
            File_Module_Error(fp,"%s f_write:%d",__FUNCTION__,res);
        if (bytes != sizeof(value))
            File_Module_Error(fp,"%s f_write lenth err!",__FUNCTION__);
    }
}
void File_Module_WriteData(FIL *fp, const void *data, UINT size)
{
    if (FileModuleHandle.RWBufferPointer) {
        // We get a massive speed boost by buffering up as much data as possible
        // before a write to the SD card. So much so that the time wasted by
        // all these operations does not cost us.
        while (size) {
            uint32_t file_buffer_space_left = FileModuleHandle.RWBufferSize - FileModuleHandle.RWBufferIndex;
            uint32_t can_do = MATH_MIN(size, file_buffer_space_left);
            memcpy(FileModuleHandle.RWBufferPointer+FileModuleHandle.RWBufferIndex, data, can_do);
            FileModuleHandle.RWBufferIndex += can_do;
            data += can_do;
            size -= can_do;
            File_Module_FlushBuffer(fp);
        }
    } else {
        UINT bytes;
        FRESULT res = f_write(fp, data, size, &bytes);
        if (res != FR_OK)
            File_Module_Error(fp,"%s f_write:%d",__FUNCTION__,res);
        if (bytes != size)
            File_Module_Error(fp,"%s f_write lenth err!",__FUNCTION__);
    }
}
void File_Module_Init(void)
{
    FileModuleHandle.DirTable = (char **)RTE_MEM_Alloc0(MEM_AXIM,sizeof(char *)*DIR_MAX_NUM);
    FileModuleHandle.CurrentDir = (char *)RTE_MEM_Alloc0(MEM_AXIM,DIR_MAX_LEN);
    FileModuleHandle.LastDir = (char *)RTE_MEM_Alloc0(MEM_AXIM,DIR_MAX_LEN);
    FileModuleHandle.FatherDir = (char *)RTE_MEM_Alloc0(MEM_AXIM,DIR_MAX_LEN);
    strcat(FileModuleHandle.CurrentDir,FatFSHandle.SDPath);
    strcat(FileModuleHandle.FatherDir,FatFSHandle.SDPath);
}


