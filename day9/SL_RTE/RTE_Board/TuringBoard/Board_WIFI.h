#ifndef __BOARD_WIFI_H
#define __BOARD_WIFI_H
#include "stm32h7xx_hal.h"
#include "Board_Config.h"
#include "RTE_Include.h"
#include "rak_wifi_include.h"
#include "rak_wifi_socket.h"
#include "rak_wifi_ap_sta.h"
extern int Board_WIFI_Init(void);
extern bool Board_WIFI_ScanHotPots(rw_WlanNetworkInfoList_t *ScanResult);
extern bool Board_WIFI_Connect2AP(char *ssid,char *pasword);
extern bool Board_WIFI_GetIPAsSTA(char *ipbuffer);
extern int8_t Board_WIFI_FindHotsPot(rw_WlanNetworkInfoList_t *resultarray,const char *ssid,uint8_t cnt);
extern int Board_WIFI_OpenServer(uint16_t Port);
#endif
