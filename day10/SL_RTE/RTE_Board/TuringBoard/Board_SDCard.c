#include "Board_SDCard.h"
#define SD_STR "[SDCARD]"
SD_HandleTypeDef BoardSDcard = {0};
HAL_SD_CardInfoTypeDef SDCardInfo1 = {0};
#if RTE_USE_OS
osEventFlagsId_t EvtIDSD;
#else
volatile bool SDRxReadyFlag,SDTxReadyFlag = false;
#endif
bool Board_SD_WaitReady(void)
{
  uint32_t loop = 0x00100000U;
  
  /* Wait for the Erasing process is completed */
  /* Verify that SD card is ready to use after the Erase */
  while(loop > 0)
  {
    loop--;
    if(Board_SD_GetCardState())
    {
        return true;
    }
  }
  return false;
}
void Board_SD_Init(void)
{
	//初始化时的时钟不能大于400KHZ 
  GPIO_InitTypeDef GPIO_InitStruct;		
	__HAL_RCC_GPIOC_CLK_ENABLE();
	/**SDMMCx GPIO Configuration **/
	GPIO_InitStruct.Pin = SD_CD_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(SD_CD_GPIO_Port, &GPIO_InitStruct);
	if(Board_SD_IsDetected()==true)
	{
		//时钟为400M（PLL1）/(3+1) = 100M
		BoardSDcard.Instance = SDMMC1;
		BoardSDcard.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
		BoardSDcard.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
		BoardSDcard.Init.BusWide = SDMMC_BUS_WIDE_4B;
		BoardSDcard.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
		BoardSDcard.Init.ClockDiv = 3;
		/* Configure SD Bus width (4 bits mode selected) */
		if (HAL_SD_Init(&BoardSDcard) != HAL_OK)
		{
			RTE_Assert(__FILE__, __LINE__);
		}
		HAL_SD_GetCardInfo(&BoardSDcard,&SDCardInfo1);
		RTE_Printf("%10s    SD card information! \r\n",SD_STR);
		RTE_Printf("%10s    LogBlockSize  : %d \r\n",SD_STR,SDCardInfo1.LogBlockSize );
		RTE_Printf("%10s    BlockSize  : %d \r\n",SD_STR,SDCardInfo1.BlockSize );
		RTE_Printf("%10s    BlockNbr :   %d \r\n",SD_STR,SDCardInfo1.BlockNbr);
		RTE_Printf("%10s    RCA           : %d \r\n",SD_STR,SDCardInfo1.RelCardAdd);
		RTE_Printf("%10s    CardType      : %d \r\n",SD_STR,SDCardInfo1.CardType);
#if RTE_USE_OS
		EvtIDSD = osEventFlagsNew(NULL);
#endif
	}
	else
		RTE_Printf("%10s    No SDCard!\r\n","[SDCARD]");
		return;
}
bool Board_SD_ReadBlocks(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks, uint32_t Timeout)
{
#if !RTE_USE_OS
	SDRxReadyFlag = false;
#endif 
	if(HAL_SD_ReadBlocks_IT(&BoardSDcard, (uint8_t *)pData, ReadAddr, NumOfBlocks) != HAL_OK)
		return false;
#if RTE_USE_OS
	osEventFlagsWait(EvtIDSD,0x00000001u,osFlagsWaitAny,Timeout);
#else 
	while(SDRxReadyFlag!=true);
#endif
//	/*
//		 the SCB_InvalidateDCache_by_Addr() requires a 32-Byte aligned address,
//		 adjust the address and the D-Cache size to invalidate accordingly.
//	 */
//	uint32_t alignedAddr = (uint32_t)pData & ~0x1F;
//	SCB_InvalidateDCache_by_Addr((uint32_t*)alignedAddr, NumOfBlocks*SDCardInfo1.BlockSize + ((uint32_t)pData - alignedAddr));
	return true; 
}
bool Board_SD_WriteBlocks(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks, uint32_t Timeout)
{
#if !RTE_USE_OS
	SDTxReadyFlag = false;
#endif 
//  /*
//   the SCB_CleanDCache_by_Addr() requires a 32-Byte aligned address
//   adjust the address and the D-Cache size to clean accordingly.
//   */
//  uint32_t alignedAddr = (uint32_t)pData &  ~0x1F;
//  SCB_CleanDCache_by_Addr((uint32_t*)alignedAddr, NumOfBlocks*SDCardInfo1.BlockSize + ((uint32_t)pData - alignedAddr));
	if(HAL_SD_WriteBlocks_IT(&BoardSDcard, (uint8_t *)pData, WriteAddr, NumOfBlocks) != HAL_OK)
		return false;
#if RTE_USE_OS
	osEventFlagsWait(EvtIDSD,0x00000002u,osFlagsWaitAny,Timeout);
#else 
	while(SDTxReadyFlag!=true);
#endif
	return true; 
}
bool Board_SD_Erase(uint32_t Addr, uint32_t EraseSize)
{
  if(HAL_SD_Erase(&BoardSDcard, Addr, Addr + EraseSize) == HAL_OK)
	{
		while(Board_SD_WaitReady() != true);
		return true;
	}
	return false;
}
bool Board_SD_GetCardState(void)
{
  return ((HAL_SD_GetCardState(&BoardSDcard) == HAL_SD_CARD_TRANSFER ) ? true : false);
}
void Board_SD_GetCardInfo(HAL_SD_CardInfoTypeDef *CardInfo)
{
  /* Get SD card Information */
  HAL_SD_GetCardInfo(&BoardSDcard, CardInfo);
}
bool Board_SD_IsDetected(void) 
{
  bool status = false;
  /* Check SD card detect pin */
  if (HAL_GPIO_ReadPin(SD_CD_GPIO_Port,SD_CD_Pin) == GPIO_PIN_RESET) {
    status = true;
  }
  /* USER CODE BEGIN 1 */
  /* user code can be inserted here */
  /* USER CODE END 1 */ 
  return status;
}  
void Board_SD_Test(void)
{
	//读写测试
	uint32_t *aTxBuffer = RTE_MEM_Alloc0(MEM_RTE,1024*4);
	uint32_t *aRxBuffer = RTE_MEM_Alloc0(MEM_RTE,1024*4);
	for (uint32_t index = 0; index < 1024; index++)
	{
		aTxBuffer[index] = 0xB5F3A5F3U + index;
	}
	for (uint32_t index = 0; index < 1024; index++)
	{
		RTE_Printf("%8x ",aTxBuffer[index]);
	}
	RTE_Printf("\r\n");
	RTE_Printf(" ****************** Start Write test ******************* \r\n");
	RTE_Printf(" - Buffer size to write: %d Bytes   at:%d\r\n", (1024*4),RTE_RoundRobin_GetTick());
	RTE_Printf("Write State:%d\r\n",(Board_SD_WriteBlocks(aTxBuffer, 0x00000400U, 1024*4/512,osWaitForever)));
	RTE_Printf(" - Buffer size end write: %d Bytes   at:%d\r\n", (1024*4),RTE_RoundRobin_GetTick());
	RTE_Printf(" - Buffer size to read: %d Bytes   at:%d\r\n", (1024*4),RTE_RoundRobin_GetTick());
	RTE_Printf("Read State:%d\r\n",(Board_SD_ReadBlocks(aRxBuffer, 0x00000400U, 1024*4/512,osWaitForever)));
	RTE_Printf(" - Buffer size end read: %d Bytes   at:%d\r\n", (1024*4),RTE_RoundRobin_GetTick()); 
	for (uint32_t index = 0; index < 1024; index++)
	{
		RTE_Printf("%8x ",aRxBuffer[index]);
	}
	RTE_Printf("\r\n");
	RTE_MEM_Free(MEM_RTE,aTxBuffer);
	RTE_MEM_Free(MEM_RTE,aRxBuffer);
	//擦除测试
	aRxBuffer = RTE_MEM_Alloc0(MEM_RTE,1024*4);
	RTE_Printf("Erase State:%x\r\n",Board_SD_Erase(0x00000400U,1024*4));
	RTE_Printf("Read State:%d\r\n",(Board_SD_ReadBlocks(aRxBuffer, 0x00000400U, 1024*4/512,osWaitForever)));
	RTE_Printf(" - Buffer size end read: %d Bytes   at:%d\r\n", (1024*4),RTE_RoundRobin_GetTick()); 
	for (uint32_t index = 0; index < 1024; index++)
	{
		RTE_Printf("%8x ",aRxBuffer[index]);
	}
	RTE_Printf("\r\n");
	RTE_MEM_Free(MEM_RTE,aRxBuffer);
}
void HAL_SD_MspInit(SD_HandleTypeDef* sdHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(sdHandle->Instance==SDMMC1)
  {
  /* USER CODE BEGIN SDMMC1_MspInit 0 */

  /* USER CODE END SDMMC1_MspInit 0 */
    /* SDMMC1 clock enable */
    __HAL_RCC_SDMMC1_CLK_ENABLE();
  
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**SDMMC1 GPIO Configuration    
    PC8     ------> SDMMC1_D0
    PC9     ------> SDMMC1_D1
    PC10     ------> SDMMC1_D2
    PC11     ------> SDMMC1_D3
    PC12     ------> SDMMC1_CK
    PD2     ------> SDMMC1_CMD 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11 
                          |GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO1;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
		
		__HAL_RCC_SDMMC1_FORCE_RESET();
		__HAL_RCC_SDMMC1_RELEASE_RESET();
    /* SDMMC1 interrupt Init */
    HAL_NVIC_SetPriority(SDMMC1_IRQn, SD_PreemptPriority, 0);
    HAL_NVIC_EnableIRQ(SDMMC1_IRQn);
  }
}

void HAL_SD_MspDeInit(SD_HandleTypeDef* sdHandle)
{

  if(sdHandle->Instance==SDMMC1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_SDMMC1_CLK_DISABLE();
    /**SDMMC1 GPIO Configuration    
    PC8     ------> SDMMC1_D0
    PC9     ------> SDMMC1_D1
    PC10     ------> SDMMC1_D2
    PC11     ------> SDMMC1_D3
    PC12     ------> SDMMC1_CK
    PD2     ------> SDMMC1_CMD 
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11 
                          |GPIO_PIN_12);
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2);
    /* SDMMC1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(SDMMC1_IRQn);
  }
} 
