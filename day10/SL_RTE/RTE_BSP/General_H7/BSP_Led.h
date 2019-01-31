#ifndef __BSP_LED_H
#define __BSP_LED_H
#ifdef __cplusplus  
extern "C" {  
#endif  
	#include "stm32h7xx_hal.h"
	//--------------------------------------------------------------
	// LEDö��
	//--------------------------------------------------------------
	typedef enum 
	{
		LED_0 = 0,  			 // LED0
		LED_N,
	}BSP_LED_Name_t;
	//--------------------------------------------------------------
	// LED״̬ö��
	//--------------------------------------------------------------
	typedef enum {
		LED_OFF = 0,  // LED AUS
		LED_ON        // LED EIN
	}BSP_LED_Status_e;
	//--------------------------------------------------------------
	// LED���ƽṹ��
	//--------------------------------------------------------------
	typedef struct {
		BSP_LED_Name_t LedName;    // Name
		GPIO_TypeDef* LedPort; // Port
		uint16_t LedPin; // Pin
		BSP_LED_Status_e LedInitStatus;  // Init
	}BSP_LED_Handle_t;
	void BSP_LED_Init(void);
	void BSP_LED_Toggle(BSP_LED_Name_t led_name);
	void BSP_LED_On(BSP_LED_Name_t led_name);
	void BSP_LED_Off(BSP_LED_Name_t led_name);
#ifdef __cplusplus  
}  
#endif  
#endif
