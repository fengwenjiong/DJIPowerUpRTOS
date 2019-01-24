#include "WIFI_SM.h"
#include "GUI_Wifi.h"
#include "Board_WIFI.h"

WIFI_SM_Arg_t WIFISMHandle = 
{
	.SMError = ERROR_NONE,
	.SelectSSIDNum = -1,
	.ServerSocket = -1,
};
static uint8_t WIFI_INIT_Function(void *Args);
static uint8_t WIFI_IDLE_Function(void *Args);
static uint8_t WIFI_SCAN_Fnction(void *Args);
static uint8_t WIFI_CONNECT_Fnction(void *Args);
static uint8_t WIFI_DISCONNECT_Fnction(void *Args);
static uint8_t WIFI_GETIP_Fnction(void *Args);
static uint8_t WIFI_OPENTCPSERVER_Fnction(void *Args);
static uint8_t WIFI_TCPSERVERWAIT_Fnction(void *Args);
static uint8_t WIFI_TCPSERVERREC_Fnction(void *Args);
static uint8_t WIFI_TCPSERVERSED_Fnction(void *Args);
static uint8_t WIFI_CLOSERTCPSERVER_Fnction(void *Args);
static uint8_t WIFI_ERROR_Function(void *Args); 
void WIFI_SM_Init(void)
{
	RTE_StateMachine_Init(&WIFISMHandle.SM,WIFI_INIT);
	RTE_StateMachine_Add(&WIFISMHandle.SM,WIFI_INIT,WIFI_INIT_Function);
	RTE_StateMachine_Add(&WIFISMHandle.SM,WIFI_IDLE,WIFI_IDLE_Function);
	RTE_StateMachine_Add(&WIFISMHandle.SM,WIFI_SCAN,WIFI_SCAN_Fnction);
	RTE_StateMachine_Add(&WIFISMHandle.SM,WIFI_CONNECT,WIFI_CONNECT_Fnction);
	RTE_StateMachine_Add(&WIFISMHandle.SM,WIFI_DISCONNECT,WIFI_DISCONNECT_Fnction);
	RTE_StateMachine_Add(&WIFISMHandle.SM,WIFI_GETIP,WIFI_GETIP_Fnction);
	RTE_StateMachine_Add(&WIFISMHandle.SM,WIFI_OPENTCPSERVER,WIFI_OPENTCPSERVER_Fnction);
	RTE_StateMachine_Add(&WIFISMHandle.SM,WIFI_TCPSERVERWAIT,WIFI_TCPSERVERWAIT_Fnction);
	RTE_StateMachine_Add(&WIFISMHandle.SM,WIFI_TCPSERVERREC,WIFI_TCPSERVERREC_Fnction);
	RTE_StateMachine_Add(&WIFISMHandle.SM,WIFI_TCPSERVERSED,WIFI_TCPSERVERSED_Fnction);
	RTE_StateMachine_Add(&WIFISMHandle.SM,WIFI_CLOSETCPSERVER,WIFI_CLOSERTCPSERVER_Fnction);
	RTE_StateMachine_Add(&WIFISMHandle.SM,WIFI_ERROR,WIFI_ERROR_Function);
}
static uint8_t WIFI_INIT_Function(void *Args)
{
	WIFI_SM_Arg_t *RunArgs = (WIFI_SM_Arg_t *)Args;	
	uint8_t RetState = RunArgs->SM.RunningState;
	WIFI_APP_Init();//初始化APP变量
	RunArgs->RecBuf = RTE_MEM_Alloc0(MEM_RTE,MAX_RECV_PACKET_LEN);
	RunArgs->RecLength = 1024;
	memset(RunArgs->RecBuf,1,1024);
	RetState = WIFI_IDLE;
	return RetState;
}
static uint8_t WIFI_IDLE_Function(void *Args)
{
	WIFI_SM_Arg_t *RunArgs = (WIFI_SM_Arg_t *)Args;	
	uint8_t RetState = RunArgs->SM.RunningState;

	return RetState;
}
static uint8_t WIFI_SCAN_Fnction(void *Args)
{
	WIFI_SM_Arg_t *RunArgs = (WIFI_SM_Arg_t *)Args;	
	uint8_t RetState = RunArgs->SM.RunningState;
	if(Board_WIFI_ScanHotPots(&RunArgs->ScanResult))
	{
		RetState = WIFI_IDLE;
	}
	else
		RTE_MEM_Free(MEM_RTE,RunArgs->ScanResult.WlanNetworkInfo);
	return RetState;
}
static uint8_t WIFI_CONNECT_Fnction(void *Args)
{
	WIFI_SM_Arg_t *RunArgs = (WIFI_SM_Arg_t *)Args;	
	uint8_t RetState = RunArgs->SM.RunningState;
	RunArgs->ConnectFlag = Board_WIFI_Connect2AP(RunArgs->SelectSSID,RunArgs->SelectPassword);
	if(RunArgs->ConnectFlag)
		RetState = WIFI_GETIP;
	else
		RetState = WIFI_IDLE;
	return RetState;
}
static uint8_t WIFI_DISCONNECT_Fnction(void *Args)
{
	WIFI_SM_Arg_t *RunArgs = (WIFI_SM_Arg_t *)Args;	
	uint8_t RetState = RunArgs->SM.RunningState;
	RunArgs->ConnectFlag = false;
	rw_sysDriverReset();
	RetState = WIFI_IDLE;
	return RetState;
}
static uint8_t WIFI_GETIP_Fnction(void *Args)
{
	WIFI_SM_Arg_t *RunArgs = (WIFI_SM_Arg_t *)Args;	
	uint8_t RetState = RunArgs->SM.RunningState;
	RunArgs->IPFlag = Board_WIFI_GetIPAsSTA(RunArgs->LocalIP);
	if(RunArgs->IPFlag)
		RetState = WIFI_IDLE;
	return RetState;
}
static uint8_t WIFI_OPENTCPSERVER_Fnction(void *Args)
{
	WIFI_SM_Arg_t *RunArgs = (WIFI_SM_Arg_t *)Args;	
	uint8_t RetState = RunArgs->SM.RunningState;

	return RetState;
}
static uint8_t WIFI_TCPSERVERWAIT_Fnction(void *Args)
{
	WIFI_SM_Arg_t *RunArgs = (WIFI_SM_Arg_t *)Args;	
	uint8_t RetState = RunArgs->SM.RunningState;

	return RetState;
}
static uint8_t WIFI_TCPSERVERREC_Fnction(void *Args)
{
	WIFI_SM_Arg_t *RunArgs = (WIFI_SM_Arg_t *)Args;	
	uint8_t RetState = RunArgs->SM.RunningState;

	return RetState;
}
static uint8_t WIFI_TCPSERVERSED_Fnction(void *Args)
{
	WIFI_SM_Arg_t *RunArgs = (WIFI_SM_Arg_t *)Args;	
	uint8_t RetState = RunArgs->SM.RunningState;
	int ret = send(RunArgs->ClientSocket, RunArgs->RecBuf, RunArgs->RecLength, 0);
	if (ret <= 0 )
	{
		if (ret == RW_ERR_SOCKET_INVAILD)
		{
			RTE_Printf("send fd = %u  disconnect \r\n", RunArgs->ClientSocket);
			WIFI_CTX.ltcps_clientinfos[WIFI_CTX.tcpc_num].tcpc_sockfd = INVAILD_SOCK_FD;
			WIFI_CTX.tcpc_num --;
			RetState = WIFI_TCPSERVERWAIT;
		}
		else if(ret!=0)
		{
			RTE_Printf("send fd = %u  error:%d \r\n",RunArgs->ClientSocket,ret);
		}
	}
	else
	{
		RTE_Printf("send data %d success\r\n",ret);
//		memset(RunArgs->RecBuf,0,RunArgs->RecLength);
//		RunArgs->RecLength = 0;
//		RetState = WIFI_TCPSERVERREC;
	}
	return RetState;
}
static uint8_t WIFI_CLOSERTCPSERVER_Fnction(void *Args)
{
	WIFI_SM_Arg_t *RunArgs = (WIFI_SM_Arg_t *)Args;	
	uint8_t RetState = RunArgs->SM.RunningState;

	if(RunArgs->ServerSocket)
		RetState = WIFI_TCPSERVERREC;
	else
		RetState = WIFI_IDLE;
	return RetState;
}
static uint8_t WIFI_ERROR_Function(void *Args)
{
	WIFI_SM_Arg_t *RunArgs = (WIFI_SM_Arg_t *)Args;	
	uint8_t RetState = RunArgs->SM.RunningState;
	RTE_Printf("SM Error:%d\r\n",RunArgs->SMError);
	return RetState;
}
