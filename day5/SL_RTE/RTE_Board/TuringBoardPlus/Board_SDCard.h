#ifndef __BOARD_SDCARD_H
#define __BOARD_SDCARD_H
#include "stm32h7xx_hal.h"
#include "Board_Config.h"
#include "RTE_Include.h"
/**
  * @brief  SD status structure definition  
  */     
#define   MSD_OK                        ((uint8_t)0x00)
#define   MSD_ERROR                     ((uint8_t)0x01)
#define   MSD_ERROR_SD_NOT_PRESENT      ((uint8_t)0x02)

/** 
  * @brief  SD transfer state definition  
  */     
#define   SD_TRANSFER_OK                ((uint8_t)0x00)
#define   SD_TRANSFER_BUSY              ((uint8_t)0x01)

#define   SD_PRESENT               ((uint8_t)0x01)
#define   SD_NOT_PRESENT           ((uint8_t)0x00)
#define   SD_DATATIMEOUT           ((uint32_t)100000000)
extern SD_HandleTypeDef BoardSDcard;
extern uint8_t Board_SD_Init(void);
extern uint8_t Board_SD_ReadBlocks(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks, uint32_t Timeout);
extern uint8_t Board_SD_WriteBlocks(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks, uint32_t Timeout);
extern uint8_t Board_SD_ReadBlocks_DMA(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks);
uint8_t Board_SD_WriteBlocks_DMA(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks);
extern uint8_t Board_SD_Erase(uint32_t StartAddr, uint32_t EndAddr);
extern uint8_t Board_SD_GetCardState(void);
extern void    Board_SD_GetCardInfo(HAL_SD_CardInfoTypeDef *CardInfo);
extern uint8_t Board_SD_IsDetected(void);

/* These functions can be modified in case the current settings (e.g. DMA stream)
   need to be changed for specific application needs */
extern void    Board_SD_WriteCpltCallback(void);
extern void    Board_SD_ReadCpltCallback(void);
#endif
