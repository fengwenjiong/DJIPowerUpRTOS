#include "Board_FatFS.h"
#include "Board_SDCard.h"
#define FS_STR "[FATFS]"
Board_FatFS_Control_t FatFSHandle = {0};
static volatile DSTATUS Stat = STA_NOINIT;
static DSTATUS SD_CheckStatus(BYTE lun);
static DSTATUS SD_initialize (BYTE);
static DSTATUS SD_status (BYTE);
static DRESULT SD_read (BYTE, BYTE*, DWORD, UINT);
#if _USE_WRITE == 1
static DRESULT SD_write (BYTE, const BYTE*, DWORD, UINT);
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
static DRESULT SD_ioctl (BYTE, BYTE, void*);
#endif  /* _USE_IOCTL == 1 */
const Diskio_drvTypeDef  SD_Driver =
{
  SD_initialize,
  SD_status,
  SD_read,
#if  _USE_WRITE == 1
  SD_write,
#endif /* _USE_WRITE == 1 */

#if  _USE_IOCTL == 1
  SD_ioctl,
#endif /* _USE_IOCTL == 1 */
};
static DSTATUS SD_CheckStatus(BYTE lun)
{
  Stat = STA_NOINIT;
  if(Board_SD_GetCardState() == true)
  {
    Stat &= ~STA_NOINIT;
  }
  return Stat;
}
DSTATUS SD_initialize(BYTE lun)
{
  Stat = STA_NOINIT;
	Stat = SD_CheckStatus(lun);
  return Stat;
}
DSTATUS SD_status(BYTE lun)
{
  return SD_CheckStatus(lun);
}
DRESULT SD_read(BYTE lun, BYTE *buff, DWORD sector, UINT count)
{
  DRESULT res = RES_ERROR;

  if(Board_SD_ReadBlocks((uint32_t*)buff,
                       (uint32_t) (sector),
                       count, SDMMC_DATATIMEOUT) == true)
  {
    res = RES_OK;
  }

  return res;
}
#if _USE_WRITE == 1
DRESULT SD_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count)
{
  DRESULT res = RES_ERROR;

  if(Board_SD_WriteBlocks((uint32_t*)buff,
                        (uint32_t)(sector),
                        count, SDMMC_DATATIMEOUT) == true)
  {
    res = RES_OK;
  }

  return res;
}
#endif

#if _USE_IOCTL == 1
DRESULT SD_ioctl(BYTE lun, BYTE cmd, void *buff)
{
  DRESULT res = RES_ERROR;
  HAL_SD_CardInfoTypeDef CardInfo;

  if (Stat & STA_NOINIT) return RES_NOTRDY;

  switch (cmd)
  {
  /* Make sure that no pending write process */
  case CTRL_SYNC :
    res = RES_OK;
    break;

  /* Get number of sectors on the disk (DWORD) */
  case GET_SECTOR_COUNT :
    Board_SD_GetCardInfo(&CardInfo);
    *(DWORD*)buff = CardInfo.LogBlockNbr;
    res = RES_OK;
    break;

  /* Get R/W sector size (WORD) */
  case GET_SECTOR_SIZE :
    Board_SD_GetCardInfo(&CardInfo);
    *(WORD*)buff = CardInfo.LogBlockSize;
    res = RES_OK;
    break;

  /* Get erase block size in unit of sector (DWORD) */
  case GET_BLOCK_SIZE :
    Board_SD_GetCardInfo(&CardInfo);
    *(DWORD*)buff = CardInfo.LogBlockSize / 512;
    res = RES_OK;
    break;

  default:
    res = RES_PARERR;
  }

  return res;
}
#endif /* _USE_IOCTL == 1 */
void Board_FatFS_Init(void) 
{
	uint8_t retSD;    /* Return value for SD */
  /*## FatFS: Link the SD driver ###########################*/
  retSD = FATFS_LinkDriver(&SD_Driver, FatFSHandle.SDPath);
	/*##-1- Register the file system object to the FatFs module ##############*/
	retSD = f_mount(&FatFSHandle.SDFatFS, FatFSHandle.SDPath, 0);
	if(retSD)
	{
		RTE_Printf("%10s    mount error : %d \r\n",FS_STR,retSD);
		RTE_Assert(__FILE__, __LINE__);
	}
	RTE_Printf("%10s    Logic path:%s \r\n",FS_STR,FatFSHandle.SDPath);
	FATFS *fs;
	DWORD fre_clust;
  /* Get volume information and free clusters of drive */
  if ((retSD = f_getfree(FatFSHandle.SDPath, &fre_clust, &fs)) != FR_OK) {
		RTE_Printf("%10s    Read sd infor error : %d \r\n",FS_STR,retSD);
		RTE_Assert(__FILE__, __LINE__);
	}
	/* Get total sectors and free sectors */
	FatFSHandle.Total = (fs->n_fatent - 2) * fs->csize * 0.5;
	FatFSHandle.Free = fre_clust * fs->csize * 0.5;
	/* Format string */
	RTE_Printf("%10s    Total card size: %u kBytes\r\n",FS_STR, FatFSHandle.Total);
	/* Format string for free card size */
	RTE_Printf("%10s    Free card size:  %u kBytes\r\n",FS_STR, FatFSHandle.Free);
}
void Board_FatFS_Test(void)
{
	FIL fil;                  // file objects
	uint32_t byteswritten;                /* File write counts */
	uint32_t bytesread;                   /* File read counts */
	uint8_t wtext[] = "This is STM32 working with FatFs"; /* File write buffer */
	uint8_t rtext[100];                     /* File read buffers */
	char filename[] = "STM32cube.txt";
	/*##-2- Create and Open new text file objects with write access ######*/
	FRESULT retSD = f_open(&fil, filename, FA_CREATE_ALWAYS | FA_WRITE);
	if(retSD)
			RTE_Printf(" open file error : %d\r\n",retSD);
	else
			RTE_Printf(" open file sucess!!! \r\n");
	 
	/*##-3- Write data to the text files ###############################*/
	retSD = f_write(&fil, wtext, sizeof(wtext), (void *)&byteswritten);
	if(retSD)
			RTE_Printf(" write file error : %d\r\n",retSD);
	else
	{
			RTE_Printf(" write file sucess!!! \r\n");
			RTE_Printf(" write Data : %s\r\n",wtext);
	}
	 
	/*##-4- Close the open text files ################################*/
	retSD = f_close(&fil);
	if(retSD)
			RTE_Printf(" close error : %d\r\n",retSD);
	else
			RTE_Printf(" close sucess!!! \r\n");
	 
	/*##-5- Open the text files object with read access ##############*/
	retSD = f_open(&fil, filename, FA_READ);
	if(retSD)
			RTE_Printf(" open file error : %d\r\n",retSD);
	else
			RTE_Printf(" open file sucess!!! \r\n");
	 
	/*##-6- Read data from the text files ##########################*/
	retSD = f_read(&fil, rtext, sizeof(rtext), (UINT*)&bytesread);
	if(retSD)
			RTE_Printf(" read error!!! %d\r\n",retSD);
	else
	{
			RTE_Printf(" read sucess!!! \r\n");
			RTE_Printf(" read Data : %s\r\n",rtext);
	}
	 
	/*##-7- Close the open text files ############################*/
	retSD = f_close(&fil);
	if(retSD)  
			RTE_Printf(" close error!!! %d\r\n",retSD);
	else
			RTE_Printf(" close sucess!!! \r\n");
	 
	/*##-8- Compare read data with the expected data ############*/
	if(bytesread == byteswritten)
	{ 
			printf(" FatFs is working well!!!\r\n");
	}
}
DWORD get_fattime(void)
{
	struct tm time;
	ulocaltime(RTE_RoundRobin_GetTick(),&time);
  return ((DWORD)(2019 - 1980) << 25 | (DWORD)1 << 21 | (DWORD)23 << 16 | (DWORD)time.tm_hour<<11 | (DWORD)time.tm_min<<5 | (DWORD)time.tm_sec);
}
