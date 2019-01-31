#ifndef __WIFI_SM_
#define __WIFI_SM_
#include "RTE_Include.h"
#include "Board_WIFI.h"
typedef enum
{
	WIFI_INIT = 0 ,
	WIFI_IDLE , 
	WIFI_SCAN ,
	WIFI_CONNECT , 
	WIFI_DISCONNECT ,
	WIFI_GETIP ,
	WIFI_OPENTCPSERVER ,
	WIFI_TCPSERVERWAIT ,
	WIFI_TCPSERVERREC ,
	WIFI_TCPSERVERSED ,
	WIFI_CLOSETCPSERVER ,
	WIFI_ERROR,
	WIFI_STATE_CNT ,
}WIFI_SM_State_e;
typedef enum
{
	ERROR_NONE = 0,
}WIFI_SM_Error_e;
typedef struct
{
	WIFI_SM_Error_e SMError;
	bool ScanFlag;
	bool ConnectFlag;
	bool IPFlag;
	int8_t SelectSSIDNum;
	uint16_t RecLength;
	uint8_t *RecBuf;
	int32_t ServerSocket;
	int32_t ClientSocket;
	lv_obj_t *SelectListBtn;
	rw_WlanNetworkInfoList_t ScanResult;
	SOCKADDR_IN ClientInfor;
  RTE_StateMachine_t SM;
	char PortNum[8];
	char SelectPassword[16];
	char SelectSSID[16];
	char LocalIP[16];
}WIFI_SM_Arg_t;
extern WIFI_SM_Arg_t WIFISMHandle;
extern void WIFI_SM_Init(void);
#endif
