#ifndef __BOARD_FATFS_H
#define __BOARD_FATFS_H
#include "stm32h7xx_hal.h"
#include "Board_Config.h"
#include "RTE_Include.h"
#include "ff.h"
#include "ff_gen_drv.h"
typedef struct {
	uint32_t Total; /*!< Total size of memory */
	uint32_t Free;  /*!< Free size of memory */
	char SDPath[4];   /* SD logical drive path */
	FATFS SDFatFS;    /* File system object for SD logical drive */
	FIL SDFile;       /* File object for SD */
} Board_FatFS_Control_t;
extern Board_FatFS_Control_t FatFSHandle;
void Board_FatFS_Init(void);
void Board_FatFS_Test(void);
#endif
