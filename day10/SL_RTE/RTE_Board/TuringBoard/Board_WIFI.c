#include "Board_WIFI.h"
#include "Board_SPI.h"
#include "Board_HALCallback.h"
#define WIFI_STR "[WIFI]"
static rw_DriverParams_t WIFIDriver = {0};
/*************************************************
*** Function: WIFI模块控制
*************************************************/
static void Board_WIFI_PowerSet(uint8_t status)
{    
  if (status) 
	{
		HAL_GPIO_WritePin(WIFI_RESET_GPIO_Port,WIFI_RESET_Pin,GPIO_PIN_SET);
  } 
	else 
	{
		HAL_GPIO_WritePin(WIFI_RESET_GPIO_Port,WIFI_RESET_Pin,GPIO_PIN_RESET);
  }
}
/*************************************************
*** Function: 初始化WIFIIO接口
*************************************************/
static uint32_t Board_WIFI_InterfaceInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
	
  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = WIFI_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(WIFI_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = WIFI_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(WIFI_RESET_GPIO_Port, &GPIO_InitStruct);
	
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(WIFI_CS_GPIO_Port, WIFI_CS_Pin, GPIO_PIN_RESET);
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(WIFI_RESET_GPIO_Port, WIFI_RESET_Pin, GPIO_PIN_RESET);
	osDelay(500);
	
	
	__HAL_RCC_GPIOD_CLK_ENABLE();
  /*Configure GPIO pin : PtPin */
  /* EXTI interrupt init*/
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_NVIC_SetPriority(EXTI4_IRQn, WIFI_IO_PreemptPriority, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);
	HAL_GPIO_Init(WIFI_INT_GPIO_Port, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	return 0;
}
/*************************************************
*** Function: 失能WIFIIO接口
*************************************************/
static uint32_t Board_WIFI_InterfaceDeInit(void)
{
	return 0;
}
/*************************************************
*** Function: WIFI模块 SPI传输接口
*************************************************/
#if RTE_USE_OS
osEventFlagsId_t EvtIDWIFI;
#else
volatile bool WIFISPIReadyFlag;
#endif
static void Board_WIFI_SPITransRec(uint8_t* write, uint8_t* read, uint16_t len)
{
  uint8_t dummy = 0x00;
  HAL_GPIO_WritePin(WIFI_CS_GPIO_Port, WIFI_CS_Pin, GPIO_PIN_RESET);
  if(read == NULL) 
	{
		if(write == NULL)  
		{
			retry1:
			HAL_SPI_Transmit_IT(&SpiHandle[SPI_WIFI].SpiHalHandle,&dummy,len);
			uint32_t flags = osEventFlagsWait(EvtIDWIFI,0x00000001u,osFlagsWaitAny,osWaitForever);
			if(flags != 0x00000001u)
			{
				goto retry1;
			}
		}
		else 
		{
			retry2:
			HAL_SPI_Transmit_IT(&SpiHandle[SPI_WIFI].SpiHalHandle,write,len);
			uint32_t flags = osEventFlagsWait(EvtIDWIFI,0x00000001u,osFlagsWaitAny,osWaitForever);
			if(flags != 0x00000001u)
				goto retry2;
		}
  } 
  else 
	{
		if(write == NULL) 
		{
			retry3:
			HAL_SPI_Receive_IT(&SpiHandle[SPI_WIFI].SpiHalHandle,read,len);
			uint32_t flags = osEventFlagsWait(EvtIDWIFI,0x00000002u,osFlagsWaitAny,osWaitForever);
			if(flags != 0x00000002u)
				goto retry3;
		}
		else 
		{
			retry4:
			HAL_SPI_TransmitReceive_IT(&SpiHandle[SPI_WIFI].SpiHalHandle,write,read,len);
			uint32_t flags = osEventFlagsWait(EvtIDWIFI,0x00000003u,osFlagsWaitAny,osWaitForever);
			if(flags != 0x00000003u)
				goto retry4;
		}
  }
	HAL_GPIO_WritePin(WIFI_CS_GPIO_Port, WIFI_CS_Pin, GPIO_PIN_SET);
}
/*************************************************
*** Function: 延时接口
*************************************************/
static void Board_WIFI_Delay(int ms)
{
	osDelay(ms);
}
/*************************************************
*** Function: 获取系统运行时钟
*************************************************/
static uint32_t Board_WIFI_GetTick(void)
{
	return RTE_RoundRobin_GetTick();
}
/*************************************************
*** Function: 中断设置
*************************************************/
void Board_WIFI_InterruptSet(uint8_t enable)
{
	if (enable)
		NVIC_EnableIRQ(EXTI4_IRQn);
	else
		NVIC_DisableIRQ(EXTI4_IRQn);
}
/*************************************************
*** Function: 内存分配接口
*************************************************/
static void Board_WIFI_Free( void *pv )
{
	RTE_MEM_Free(MEM_RTE,pv);
}
static void *Board_WIFI_Malloc(size_t xWantedSize)
{
	return RTE_MEM_Alloc(MEM_RTE,xWantedSize);
}
/*************************************************
*** Function: 断言接口
*************************************************/
static void Board_WIFI_Assert(const char *file, int line)
{
	RTE_Assert((char*)file,(uint32_t)line);
}
/*************************************************
*** Function: WIFI连接用户回调函数
*************************************************/
static void User_WIFIConnect_Callback(uint8_t event,rw_WlanConnect_t* wlan_info, RW_DISCONNECT_REASON dis_code)
{
	RTE_Printf("%10s    Connect_callback event = 0x%x\r\n",WIFI_STR,event);
	switch(event) 
	{
		//连接到路由
		case CONN_STATUS_STA_CONNECTED:
		{
			if (wlan_info != NULL) 
			{
				RTE_Printf("%10s    Connected to AP as STA success!\r\n",WIFI_STR);
				RTE_Printf("%10s    bssid = %02X:%02X:%02X:%02X:%02X:%02X\r\n",WIFI_STR
																													 , wlan_info->bssid[0]
																													 , wlan_info->bssid[1] 
																													 , wlan_info->bssid[2]  
																													 , wlan_info->bssid[3]  
																													 , wlan_info->bssid[4]  
																													 , wlan_info->bssid[5] );
				RTE_Printf("%10s    channel = %d\r\n",WIFI_STR, wlan_info->channel);
				RTE_Printf("%10s    ssid = %s\r\n",WIFI_STR, wlan_info->ssid);
				RTE_Printf("%10s    psk = %s\r\n",WIFI_STR, wlan_info->psk);
				RTE_Printf("%10s    sec_mode = %d\r\n",WIFI_STR, wlan_info->sec_mode);
				RTE_Printf("%10s    auth_mode = %d\r\n",WIFI_STR, wlan_info->auth_mode);         
			}
			WIFI_CTX.rw_connect_status =STATUS_OK;
		}break;
		//与路由断开连接
		case CONN_STATUS_STA_DISCONNECT:
		{
			WIFI_CTX.rw_connect_status =STATUS_FAIL;
			RTE_Printf("%10s    Disconnect to AP as STA wifi = %d\r\n",WIFI_STR,dis_code);
		}break;
		//AP建立成功
		case CONN_STATUS_AP_ESTABLISH:
		{
			WIFI_CTX.rw_connect_status = STATUS_OK;
			RTE_Printf("%10s    AP establish\r\n",WIFI_STR);
		}break;
		//有客户端连接到AP
		case CONN_STATUS_AP_CLT_CONNECTED:
		{
			RTE_Printf("%10s    AP client connected!\r\n",WIFI_STR);
		}break;
		//客户端与AP断开连接
		case CONN_STATUS_AP_CLT_DISCONNECT:
			RTE_Printf("%10s    AP client disconnected Code = %d\r\n",WIFI_STR,dis_code);
		break;
		default:
			RTE_Printf("%10s    User_WIFIConnect_Callback error = %d\r\n",WIFI_STR,event);
		break;
	}
}
/*************************************************
*** Function: DHCP用户回调函数 非操作系统库下需要
*************************************************/
static void User_WIFIDHCP_Callback(rw_IpConfig_t* ip_addr, int status)
{
	if(status ==RW_OK)
	{
		RTE_Printf("%10s    IP get success addr = 0x%x\r\n",WIFI_STR,ip_addr->addr);
		WIFI_CTX.rw_ipquery_status =STATUS_OK; 
	}
	else
	{   
		RTE_Printf("%10s    IP get fail code = %d\r\n",WIFI_STR,status);
		WIFI_CTX.rw_ipquery_status =STATUS_FAIL; 
	}
}
/*************************************************
*** Function: DNS用户回调函数 非操作系统库下需要
*************************************************/
static void User_WIFIDNS_Callback(int dns_ip)
{
	if(dns_ip !=0)
		RTE_Printf("%10s    DNS success ip addr = 0x%x\r\n",WIFI_STR,dns_ip);
	else
		RTE_Printf("%10s    DNS fail,please retry!!!\r\n",WIFI_STR);
}
/*************************************************
*** Function: easyconfig用户回调函数
*************************************************/
static void User_WIFI_WPS_EASY_Callback(rw_WlanEasyConfigWpsResponse_t* info , int status)
{
	if (status == 0) 
	{      
		RTE_Printf("%10s    bssid = %02X:%02X:%02X:%02X:%02X:%02X\r\n",WIFI_STR
		                                                  , info->bssid[0]
																											, info->bssid[1] 
																											, info->bssid[2]  
																											, info->bssid[3]  
																											, info->bssid[4]  
																											, info->bssid[5] );
		RTE_Printf("%10s    channel =%d\r\n",WIFI_STR, info->channel);
		RTE_Printf("%10s    ssid =%s\r\n",WIFI_STR, info->ssid);
		RTE_Printf("%10s    psk =%s\r\n",WIFI_STR, info->psk); 
		WIFI_CTX.rw_easywps_status = STATUS_OK;
		memcpy(&WIFI_EasyWPSInfo,info,sizeof(WIFI_EasyWPSInfo));
		memset(&WIFI_ConnectInfo, 0, sizeof(WIFI_ConnectInfo));
		if(WIFI_CTX.easywps_mode ==CONFIG_WPS)
		{
			WIFI_ConnectInfo.bssid = NULL;
			WIFI_ConnectInfo.channel = 0;
		}
		else
		{
			WIFI_ConnectInfo.bssid = WIFI_EasyWPSInfo.bssid; 
			WIFI_ConnectInfo.channel = WIFI_EasyWPSInfo.channel;
		}         
		WIFI_ConnectInfo.ssid = (char*)WIFI_EasyWPSInfo.ssid; 
		WIFI_ConnectInfo.psk = (char*)WIFI_EasyWPSInfo.psk;
		WIFI_ConnectInfo.pmk = NULL;
		if(info->psk[0] != 0)
		{
			WIFI_ConnectInfo.sec_mode = RW_SEC_TYPE_SEC;
		}
		else
		{
			WIFI_ConnectInfo.sec_mode = RW_SEC_TYPE_OPEN;
		}    
		WIFI_ConnectInfo.auth_mode = RW_AUTH_TYPE_AUTO;
		WIFI_ConnectInfo.role_mode = ROLE_STA;     
	}
	else
	{    
		WIFI_CTX.rw_easywps_status = STATUS_FAIL;
		RTE_Printf("%10s    wps_easy config fail \r\n",WIFI_STR);
	}
}
/*************************************************
*** Function: 初始化WIFI驱动 
							各类回调函数在操作系统框架下由.lib创建的线程调用
							裸机环境下由WIFI线程轮询
*************************************************/
void Board_WIFI_Driver_Init(rw_DriverParams_t* params)
{
	//平台接口
	params->driver_cb.hw_init = Board_WIFI_InterfaceInit;    
	params->driver_cb.hw_deinit = Board_WIFI_InterfaceDeInit;
  params->driver_cb.hw_power = Board_WIFI_PowerSet;
  params->driver_cb.spi_io_buffer = Board_WIFI_SPITransRec;    
	params->driver_cb.time_delay = Board_WIFI_Delay;
	params->driver_cb.Stamp_get = Board_WIFI_GetTick;   
	params->driver_cb.toggle_irq = Board_WIFI_InterruptSet;   
	params->driver_cb.driver_free = Board_WIFI_Free;
	params->driver_cb.driver_malloc = Board_WIFI_Malloc;
	params->driver_cb.driver_assert = Board_WIFI_Assert;
  //应用回调接口
	params->app_cb.conn_cb = User_WIFIConnect_Callback; 

//#if WIFI_USE_OS == 0
//	params->app_cb.scan_cb = User_WIFIScan_Callback;
//	params->app_cb.dhcp_cb = User_WIFIDHCP_Callback;
//	params->app_cb.dns_cb = User_WIFIDNS_Callback;
//#endif
	params->app_cb.easy_wps_cb = User_WIFI_WPS_EASY_Callback;
	//中断使能
	params->spi_int_enable = true;
	//RX Buffer个数 一个大小为1664字节
	//适当增加接收buffer的个数可以提高模块双向数据时的吞吐率
	params->rx_queue_num = 8;
	params->tx_queue_num = 8;
	params->tx_max_len = MAX_SEND_PACKET_LEN;

	//扫描buffer个数 一个为44字节
	params->scan_max_num = 32;
	//tcp重传次数
	params->tcp_retry_num = 5;
	//socket个数 一个buffer为48字节
	params->socket_max_num = 8;
	params->country_code = "CN";
	//主机名
	params->host_name = "TuringBoard";
}
/*************************************************
*** Function: WIFI模块初始化
*************************************************/
int Board_WIFI_Init(void)
{
	EvtIDWIFI = osEventFlagsNew(NULL);
  int ret = RW_OK;
  char libVersion[20] = {0};
  char module_mac[6] ={0};
	/*RAK439用户参数配置*/
  Board_WIFI_Driver_Init(&WIFIDriver);
	/*RAK439驱动初始化 库函数调用*/
  ret = rw_sysDriverInit(&WIFIDriver);
	while(ret != RW_OK)
  {
    RTE_Printf("%10s    RAK module platform init...failed code=%d\r\n",WIFI_STR, ret);
    ret = rw_sysDriverInit(&WIFIDriver);
		osDelay(200);
  }
	RTE_Printf("%10s    RAK module platform init success\r\n",WIFI_STR);
  rw_getLibVersion(libVersion); 
  RTE_Printf("%10s    RAK wifi LibVersion:%s\r\n",WIFI_STR, libVersion);
  rw_getMacAddr(module_mac);
  RTE_Printf("%10s    RAK wifi module-MAC:%02X:%02X:%02X:%02X:%02X:%02X\r\n",WIFI_STR, module_mac[0],module_mac[1],module_mac[2],module_mac[3],module_mac[4],module_mac[5]);
 
	return RW_OK;
}
bool Board_WIFI_ScanHotPots(rw_WlanNetworkInfoList_t *ScanResult)
{
	int ret = 0; 
	ret = rw_wlanNetworkScan(NULL,0);
	if(ret == RW_OK)
		ret = rw_wlanGetScanInfo(ScanResult);
	return (ret==RW_OK?true:false);
}
bool Board_WIFI_Connect2AP(char *ssid,char *pasword)
{
	int ret = RW_OK;
	rw_WlanConnect_t coninfor = {0};
	coninfor.role_mode = ROLE_STA;//STA模式
	coninfor.ssid = ssid;
	coninfor.psk = pasword;
	coninfor.pmk = NULL;
	coninfor.channel = 0;//不指定信道
	coninfor.sec_mode = RW_SEC_TYPE_SEC;//连接到加密网络
	coninfor.auth_mode = RW_AUTH_TYPE_AUTO;//加密类型自动
	uint8_t timeout = 0;
	while ((ret =rw_wlanConnect(&coninfor)) != RW_OK) {
    osDelay(50);
	}
	//等待连接建立 这里的连接状态在上面的回调函数中会被修改
	while(WIFI_CTX.rw_connect_status != STATUS_OK){
		timeout++;
		osDelay(500);
		if(timeout>=100)
			return false;
	}
	return  true;
}
bool Board_WIFI_GetIPAsSTA(char *ipbuffer)
{
	int ret = RW_OK;
	rw_IpConfig_t ipconfig = {0};
	while ((ret = rw_ipConfig(&ipconfig, DHCP_CLIENT)) != RW_OK){
    osDelay(50);
	}
  WIFI_CTX.rw_ipquery_status = STATUS_OK;
	uint8_t IP0,IP1,IP2,IP3 = 0;
	IP0 = (ipconfig.addr >> 24)&0xFF;
	IP1 = (ipconfig.addr >> 16)&0xFF;
	IP2 = (ipconfig.addr >> 8)&0xFF;
	IP3 = (ipconfig.addr)&0xFF;
	usprintf(ipbuffer,"%d.%d.%d.%d",IP0,IP1,IP2,IP3);
	return true;
}
int8_t Board_WIFI_FindHotsPot(rw_WlanNetworkInfoList_t *resultarray,const char *ssid,uint8_t cnt)
{
		for(int8_t i=0;i<cnt;i++)
		{
			if(ustrstr((char *)resultarray->WlanNetworkInfo[i].ssid,ssid))
				return i;
		}
		return -1;
}
int Board_WIFI_OpenServer(uint16_t Port)
{
	SOCKADDR_IN     LocalAddr = {0};
	int             sockfd = -1;
	int             ret = RW_OK;
	//filling the TCP server socket address
	LocalAddr.sin_family = AF_INET;
	LocalAddr.sin_port = htons((uint16_t)Port);
	LocalAddr.sin_addr = 0;
	// creating a TCP socket
	sockfd = socket(AF_INET,SOCK_STREAM, 0);
	if (sockfd < RW_OK )
	{
		close(sockfd);
		return sockfd;
	}
	// binding the TCP socket to the TCP server address
	ret = bind(sockfd, (SOCKADDR_IN *)&LocalAddr, sizeof(SOCKADDR_IN));
	if (ret < RW_OK )
	{
		close(sockfd);
		return ret;
	}
	// putting the socket for listening to the incoming TCP connection
	ret = listen(sockfd, 1);
	if (ret < RW_OK )
	{
		close(sockfd);
		return ret;
	}
	WIFI_CTX.ltcps_sockfd = sockfd;
	return sockfd;
}
