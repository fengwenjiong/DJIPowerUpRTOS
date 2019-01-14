#include "rak_wifi_ap_sta.h"
#define AP_STA_STR   "[AP&STA]"
RW_APP_CTX                           WIFI_CTX = {0};//管理WIFI模块的连接
rw_WlanConnect_t                     WIFI_ConnectInfo = {0};//WIFI模块的连接状态结构体
rw_WlanEasyConfigWpsResponse_t       WIFI_EasyWPSInfo = {0};//easywps信息结构体
/*************************************************
*** Args:   无
*** Function: 初始化WIFI_APP
*************************************************/
void WIFI_APP_Init(void)
{
   int i =0;
   memset((void* )&WIFI_CTX, 0, sizeof(WIFI_CTX));
   WIFI_CTX.ltcps_sockfd = INVAILD_SOCK_FD;
   WIFI_CTX.ludps_sockfd = INVAILD_SOCK_FD;
   WIFI_CTX.tcp_cloud_sockfd = INVAILD_SOCK_FD;   
   WIFI_CTX.easy_sockfd = INVAILD_SOCK_FD;
   WIFI_CTX.udpc_sockfd = INVAILD_SOCK_FD;
   for(i=0; i<ALLOW_MAX_NUMS; i++)
   {
      WIFI_CTX.ltcps_clientinfos[i].tcpc_sockfd =INVAILD_SOCK_FD;
   }
}
/*************************************************
*** Args:   pconn ipmode ipconfig
*** Function: 初始化网络 在非easyconfig下自动调用
*************************************************/
int WIFI_NetWork_Init(rw_WlanConnect_t* pconn, rw_IpConfigMode_t ipmode, rw_IpConfig_t* ipconfig)
{
  int ret = RW_OK;
	uint8_t timeout = 0;
	while ((ret =rw_wlanConnect(pconn)) != RW_OK) {
		RTE_Printf("%10s    Connect to %s rw_wlanConnect error:%d\r\n",AP_STA_STR,pconn->ssid,ret);
    osDelay(50);
	}
	//等待连接建立
	while(WIFI_CTX.rw_connect_status != STATUS_OK){
		timeout++;
		osDelay(500);
		if(timeout>=100)
		{
			RTE_Printf("%10s    app_demo_ctx.rw_connect_status != STATUS_OK\r\n",AP_STA_STR);
			return RW_ERR;
		}
	}
	while ((ret = rw_ipConfig(ipconfig, ipmode)) != RW_OK){
		RTE_Printf("%10s    rw_ipConfig error=%d\r\n",AP_STA_STR, ret);  
    osDelay(50);
	}
  WIFI_CTX.rw_ipquery_status = STATUS_OK;
	RTE_Printf("%10s    Local ip addr = 0x%x\r\n",AP_STA_STR, ipconfig->addr);
	if(pconn->role_mode ==ROLE_AP){	
		if ((ret =rw_ipConfig(ipconfig, DHCP_SERVER)) != RW_OK){
			RTE_Printf("%10s    As ap rw_ipConfig error=%d\r\n",AP_STA_STR, ret); 
		}	
	}	
  if (ipmode != DHCP_CLIENT) 
	{
    WIFI_CTX.rw_ipquery_status =STATUS_OK; 
  }
  return ret;
}
/*************************************************
*** Args:   无
*** Function: 建立一个AP
*************************************************/
int WIFI_Start_AP(void)
{
	int ret = RW_OK;
	rw_IpConfig_t ipinfo = {0};
	memset(&WIFI_ConnectInfo, 0, sizeof(WIFI_ConnectInfo));
	WIFI_ConnectInfo.role_mode = ROLE_AP;
	WIFI_ConnectInfo.ssid = "TuringBoard";
	WIFI_ConnectInfo.psk = "1234567890";
	WIFI_ConnectInfo.pmk = NULL;    //set NULL
	WIFI_ConnectInfo.channel = 6;
	WIFI_ConnectInfo.sec_mode = RW_SEC_TYPE_SEC;
	WIFI_ConnectInfo.auth_mode = RW_AUTH_TYPE_AUTO;

	
	ipinfo.addr = 0xC0A80701;
	ipinfo.mask = 0xFFFFFF00;
	ipinfo.gw   = 0xC0A80701;
	ipinfo.svr1 = 0xC0A80701;

	ret =WIFI_NetWork_Init(&WIFI_ConnectInfo, IP_CONFIG_STATIC, &ipinfo);  
	if (ret != RW_OK) 
	{
		RTE_Printf("rw_startAP error =%d\r\n", ret);
	}  
	return  ret;
}
/*************************************************
*** Args:   无
*** Function: 建立一个STA
*************************************************/
int WIFI_Start_Station(void)
{
	int ret = RW_OK;
	rw_IpConfig_t ipinfo = {0};
	//    uint8_t ap_pmk[32]={0xa0, 0xd6, 0xd8, 0x52, 0x4b, 0x98, 0xf5, 0x5b, 0xd3, 0x6b, 0xa2, 0xb7, 0x59, 0x0a, 0x27, 0xfb, 0xf8, 0xe8, 0x51, 0xb0, 0x21, 0x31, 0x7c, 0x3f, 0xb8, 0xc5, 0x27, 0x21, 0x53, 0xd2, 0x42, 0x89};
	//    uint8_t ap_pmk[32]={0xFD, 0xC1, 0x78, 0xE3, 0x47, 0xAB, 0x07, 0xB9, 0xA0, 0xd3, 0xa0, 0xec, 0x00, 0xce, 0x0d, 0x3c, 0x30, 0x98, 0x1c, 0xa2, 0x14, 0xb5, 0xe1, 0xfc, 0xde, 0xa9, 0xa8, 0xca, 0x9d, 0xae, 0xd6, 0x6d};	

	memset(&WIFI_ConnectInfo, 0, sizeof(WIFI_ConnectInfo));
	WIFI_ConnectInfo.role_mode = ROLE_STA;
	WIFI_ConnectInfo.ssid = "ELEVATOR_REMOTE";//"RAK_Wireless"  "1HEA-PC_Network_4" "mw_raktest"
	WIFI_ConnectInfo.psk = "transportation";
	WIFI_ConnectInfo.pmk = NULL; //ap_pmk;NULL;
	WIFI_ConnectInfo.channel = 0;
	WIFI_ConnectInfo.sec_mode = RW_SEC_TYPE_SEC;
	WIFI_ConnectInfo.auth_mode = RW_AUTH_TYPE_AUTO;

	ret = WIFI_NetWork_Init(&WIFI_ConnectInfo, DHCP_CLIENT, &ipinfo);	
	if (ret != RW_OK) 
	{
		RTE_Printf("rw_startSTA error =%d\r\n", ret);
	}  
	return  ret;
}
/*************************************************
*** Args:   无
*** Function: 智能配网
*************************************************/
int WIFI_Start_SmartConfig(rw_ConfigMode_t mode)
{
	int ret = RW_OK;
	WIFI_CTX.rw_easywps_status =STATUS_INIT;
	ret =rw_startEasyWps(mode);
	if (ret != RW_OK) 
	{
		RTE_Printf("rw_startEasyWps error =%d\r\n", ret);
	}  
	return  ret;
}

