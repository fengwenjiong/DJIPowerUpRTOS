#ifndef __BOARD_SDCARD_H
#define __BOARD_SDCARD_H
#include "stm32h7xx_hal.h"
#include "Board_Config.h"
#include "RTE_Include.h"
extern SD_HandleTypeDef BoardSDcard;
extern void Board_SD_Init(void);
extern bool Board_SD_IsDetected(void);
extern bool Board_SD_ReadBlocks(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks, uint32_t Timeout);
extern bool Board_SD_WriteBlocks(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks, uint32_t Timeout);
extern bool Board_SD_GetCardState(void);
extern void Board_SD_GetCardInfo(HAL_SD_CardInfoTypeDef *CardInfo);
extern bool Board_SD_WaitReady(void);
extern void Board_SD_Test(void);
#endif
