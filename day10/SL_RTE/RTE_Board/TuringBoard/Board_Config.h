/**
  ******************************************************************************
  * @file    Board_Config.h
  * @author  Shan Lei ->>lurenjia.tech ->>https://github.com/sudashannon
  * @brief   Ӳ����������ͷ�ļ�������KEIL��ʽ��д��Ϊ���ӵ�Ӳ�������������á�
  * @version V1.0 2018/11/02 ��һ��
  ******************************************************************************
  */
#ifndef __BOARD_CONFIG_H
#define __BOARD_CONFIG_H
//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------
//<s> BOARD_VERSION
//<i> Ӳ�����Ӱ汾�ţ����嶨�������ĵ���
#define BOARD_VERSION "1.0"
//=======================
//<h>ϵͳʱ������
//=======================
#if HSE_VALUE == 8000000
	//<o> PLL M��Ƶ����
	//<i> ����/M��Ϊ����PLL��ʱ�ӡ�
	#define PLL_M    		         (4U)
	//<o> PLL N��Ƶ����
	//<i> PLL����ʱ��*NΪ��һ�׶�ʱ�ӡ�
	#define PLL_N    		         (400U)
#elif HSE_VALUE == 12000000
	//<o> PLL M��Ƶ����
	//<i> ����/M��Ϊ����PLL��ʱ�ӡ�
	#define PLL_M    		         (3U)
	//<o> PLL N��Ƶ����
	//<i> PLL����ʱ��*NΪ��һ�׶�ʱ�ӡ�
	#define PLL_N    		         (200U)
#elif HSE_VALUE == 25000000
	//<o> PLL M��Ƶ����
	//<i> ����/M��Ϊ����PLL��ʱ�ӡ�
	#define PLL_M    		         (2U)
	//<o> PLL N��Ƶ����
	//<i> PLL����ʱ��*NΪ��һ�׶�ʱ�ӡ�
	#define PLL_N    		         (64U)
#endif
//</h>
//=======================
//<h>Systick����
//=======================
	//<o> Systick�ж����ȼ�
	//<i> Ĭ����͡�
	#define SYSTICK_PreemptPriority    		 (0x15)
//</h>
//==================================================================
//<h>����1�й�����
//==================================================================
	//<o> ����1�ж����ȼ�
	#define USART1_PreemptPriority           0x12
	//<o> ����1�����ж����ȼ�
	#define USART1_DMARX_PreemptPriority     0x13
	//<e>ʹ��DMA����
	#define USART1_USE_DMA_TX                0
	#if USART1_USE_DMA_TX == 1
	//<o> ���ڷ���DMA�ж����ȼ�
	#define USART1_DMATX_PreemptPriority     0x12
	#endif
	//</e>
//</h>
//==================================================================
//<h>SDCARD�й�����
//==================================================================
	//==================================================================
	//<h>SDCARD GPIO�й�����
	//==================================================================
	#define SD_CD_Pin GPIO_PIN_13
	#define SD_CD_GPIO_Port GPIOC
	//</h>
	//==================================================================
	//<h>SDCARD �ж����ȼ��й�����
	//==================================================================
	#define SD_PreemptPriority       0x04
	//</h>
//</h>
//==================================================================
//<h>WIFI�й�����
//==================================================================
	//==================================================================
	//<h>WIFI GPIO�й�����
	//==================================================================
	#define WIFI_INT_Pin GPIO_PIN_4
	#define WIFI_INT_GPIO_Port GPIOD
	#define WIFI_CS_Pin GPIO_PIN_6
	#define WIFI_CS_GPIO_Port GPIOD
	#define WIFI_RESET_Pin GPIO_PIN_5
	#define WIFI_RESET_GPIO_Port GPIOB
		//</h>
	//==================================================================
	//<h>WIFI SPI�ж����ȼ��й����� SPI�ж����ȼ������DMA�ĸ�
	//==================================================================
	//<o> SPI1�ж����ȼ�
	#define SPI1_PreemptPriority             0x05
	//<o> SPI1DMA�����ж����ȼ�
	#define SPI1_DMA_PreemptPriority         0x06
	//<o> WIFI IO�ж����ȼ�
	#define WIFI_IO_PreemptPriority          0x07
	//</h>
//</h>
//==================================================================
//<h>LCD�й�����
//==================================================================
	//==================================================================
	//<h>LCD GPIO�й�����
	//==================================================================
	#define SPI4_NSS_Pin                     GPIO_PIN_3
	#define SPI4_NSS_GPIO_Port               GPIOE
	#define LCD_DC_Pin                       GPIO_PIN_12
	#define LCD_DC_GPIO_Port                 GPIOE
	#define LCD_RST_Pin                      GPIO_PIN_15
	#define LCD_RST_GPIO_Port                GPIOE
	#define LCD_BLK_Pin                      GPIO_PIN_10
	#define LCD_BLK_GPIO_Port                GPIOE
	#define LCD_PEN_Pin                      GPIO_PIN_8
	#define LCD_PEN_GPIO_Port                GPIOE
	#define LCD_CS2_Pin                      GPIO_PIN_7
	#define LCD_CS2_GPIO_Port                GPIOE
	//</h>
	//==================================================================
	//<h>LCD SPI�ж����ȼ��й����� SPI�ж����ȼ������DMA�ĸ�
	//==================================================================
	//<o> SPI4�ж����ȼ�
	#define SPI4_PreemptPriority             0x08
	//<o> SPI4DMA�����ж����ȼ�
	#define SPI4_DMATX_PreemptPriority       0x09
	//</h>
//</h>

#endif
/****************** (C) COPYRIGHT SuChow University Shannon********************/
