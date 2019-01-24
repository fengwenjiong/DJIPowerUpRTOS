#include "GUI_Wifi.h"
#include "GUI_OverAll.h"
#include "WIFI_SM.h"
/*************************************************
***              |--switch控件(切换WIFI状态)-->wifi_sw_action--|mbox控件(提示正在扫描)-->wifi_scancancel_action-----|
***              |                                           -->WifiScanCheckCallback --|                  <关闭<---|
***              |                                                                      |--扫描到    --|删除之前得mbox，更新list------------------------------------------------|
***              |                                                                      |--没有扫描到--|mbox控件(提示失败)                                                      |
***              |                          |                                                                                                                                   |
***    WIFI窗口--|--list控件(显示扫描结果)--|btnm控件(连接选择得WIFI)-->wifi_select_action-->WifiConnectCheckCallback--|                                                   <----|
***              |                          |                                             --|mbox控件(提示正在链接)    |--扫描到    --|删除之前得mbox，更新list
***              |                                                                                                     |--没有扫描到--|mbox控件(提示失败)
***              |
***              |
***              |--btn控件(关闭窗口)
*************************************************/
GUI_WIFI_t WIFI = {0};
static void wifi_close_action_animation_cb(lv_obj_t * obj)
{
	if(WIFI.mboxall)
	{
		RTE_Printf("%08x\r\n",WIFI.mboxall);
		lv_obj_del(WIFI.mboxall);
		WIFI.mboxall = 0;
		RTE_RoundRobin_RemoveTimer(1,RTE_RoundRobin_GetTimerID(1,"WifiScanCheck"));
		RTE_RoundRobin_RemoveTimer(1,RTE_RoundRobin_GetTimerID(1,"WifiConnectCheck"));
	}
  lv_obj_del(obj);
	obj = 0;
}
static lv_res_t wifi_close_action_animation(lv_obj_t * btn)
{
    lv_obj_t * win = lv_win_get_from_btn(btn);
    /* Float in the button using a built-in function
    * Delay the animation with 50 ms and float in 300 ms. NULL means no end callback*/
    lv_obj_animate(win, LV_ANIM_FLOAT_TOP | LV_ANIM_OUT, 300, 50, wifi_close_action_animation_cb);
    return LV_RES_OK;
}
/*************************************************
*** Function: mbox关闭按钮回调函数
*************************************************/
static lv_res_t wifi_mbox_close_action(lv_obj_t * btn, const char * txt)
{
	lv_obj_t * mbox = lv_mbox_get_from_btn(btn);
	if(txt[0] != '\0') {
		lv_obj_del(mbox);
		WIFI.mboxall = 0;
		return LV_RES_INV;
	}
	return LV_RES_OK;
}
/*************************************************
*** Function: 连接WIFI定时器回调函数
*************************************************/
static void WifiConnectCheckCallback(void *arg)
{
	lv_obj_t * btn = (lv_obj_t*)arg;
	char btnlabel[128] = {0};
	if(WIFISMHandle.ConnectFlag)
	{
		//清除mbox
		lv_obj_del(WIFI.mboxall);
		WIFI.mboxall = 0;
		//更新状态栏WIFI图标
		lv_img_set_src(OverAllPage.iconinfor[1],SYMBOL_WIFI);
		//清除原有选择
		if(WIFISMHandle.SelectSSIDNum!=-1&&WIFISMHandle.SelectListBtn)
		{
			lv_label_set_text(lv_list_get_btn_label(WIFISMHandle.SelectListBtn),(char *)WIFISMHandle.ScanResult.WlanNetworkInfo[WIFISMHandle.SelectSSIDNum].ssid);
		}
		//更新List选择
		WIFISMHandle.SelectListBtn = btn;
		WIFISMHandle.SelectSSIDNum = Board_WIFI_FindHotsPot(&WIFISMHandle.ScanResult,lv_label_get_text(lv_list_get_btn_label(btn)),WIFISMHandle.ScanResult.num);
		strcat(btnlabel,lv_label_get_text(lv_list_get_btn_label(btn)));
		strcat(btnlabel,"   ");
		strcat(btnlabel,SYMBOL_OK);
		lv_label_set_text(lv_list_get_btn_label(btn),btnlabel);
	}
	else
	{
		//清除mbox
		lv_obj_del(WIFI.mboxall);
		WIFI.mboxall = 0;
		//消息提示mbox
		WIFI.mboxall = lv_mbox_create(lv_scr_act(),NULL);
		strcat(btnlabel,"Connect to\n");
		strcat(btnlabel,lv_label_get_text(lv_list_get_btn_label(btn)));
		strcat(btnlabel," Fail!");
		lv_mbox_set_text( WIFI.mboxall, btnlabel);
		lv_obj_set_width(WIFI.mboxall,LV_HOR_RES*2/3);
		lv_obj_align(WIFI.mboxall,lv_scr_act(),LV_ALIGN_CENTER,0,0);
	}
}
/*************************************************
*** Function: kb ×按钮回调函数
*************************************************/
static lv_res_t kb_clean_action_animation(lv_obj_t * btn)
{
	lv_obj_del(WIFI.kbta);
	lv_obj_del(WIFI.mboxall);
	WIFI.mboxall = 0;
	lv_obj_del(WIFI.keyboard);
	RTE_RoundRobin_RemoveTimer(1,RTE_RoundRobin_GetTimerID(1,"WifiConnectCheck"));
	return LV_RES_OK;
}
/*************************************************
*** Function: kb √按钮回调函数
*************************************************/
static lv_res_t kb_ok_action_animation(lv_obj_t * btn)
{
	if(!WIFISMHandle.ConnectFlag)
	{
		memset(WIFISMHandle.SelectPassword,0,16);
		strcat(WIFISMHandle.SelectPassword,lv_ta_get_text(WIFI.kbta));
		lv_obj_del(WIFI.kbta);
		lv_obj_del(WIFI.keyboard);
		lv_obj_t * preload3 = lv_preload_create(WIFI.mboxall, NULL);
		lv_preload_set_arc_length(preload3, 90);
		lv_preload_set_spin_time(preload3, 5000);
		lv_obj_align(WIFI.mboxall,lv_scr_act(),LV_ALIGN_CENTER,0,0);	
		RTE_StateMachine_ChangeTo(&WIFISMHandle.SM,WIFI_CONNECT);
		RTE_RoundRobin_ResumeTimer(1,RTE_RoundRobin_GetTimerID(1,"WifiConnectCheck"));
	}
	return LV_RES_OK;
}
/*************************************************
*** Function: 选择WIFI按钮回调函数
*************************************************/
static lv_res_t wifi_select_action(lv_obj_t * btn)
{
	char btnlabel[128] = {0};
	//如果选择了新的
	if(btn!=WIFISMHandle.SelectListBtn)
	{
		//消息提示mbox
		strcat(btnlabel,"Connect to\n");
		strcat(btnlabel,lv_label_get_text(lv_list_get_btn_label(btn)));
		strcat(btnlabel,"...");
		memset(WIFISMHandle.SelectSSID,0,16);
		strcat(WIFISMHandle.SelectSSID,lv_label_get_text(lv_list_get_btn_label(btn)));
    WIFI.mboxall = lv_mbox_create(lv_scr_act(),NULL);
    lv_obj_set_width(WIFI.mboxall,lv_obj_get_width(WIFI.winwifi)-30);
    lv_mbox_set_text(WIFI.mboxall, btnlabel);
		lv_obj_align(WIFI.mboxall,lv_scr_act(),LV_ALIGN_IN_TOP_MID,0,10);
		//输入密码
		WIFI.keyboard = lv_kb_create(lv_scr_act(), NULL);
		lv_kb_set_cursor_manage(WIFI.keyboard, true);
		WIFI.kbta = lv_ta_create(WIFI.mboxall, NULL);
		lv_obj_set_height(WIFI.kbta,40);
		lv_ta_set_pwd_mode(WIFI.kbta,true);
		lv_ta_set_text(WIFI.kbta, "");
		lv_kb_set_style(WIFI.keyboard, LV_KB_STYLE_BG, &lv_style_transp_tight);
		lv_kb_set_style(WIFI.keyboard, LV_KB_STYLE_BTN_REL, &lv_style_btn_rel);
		lv_kb_set_style(WIFI.keyboard, LV_KB_STYLE_BTN_PR, &lv_style_btn_pr);
		lv_kb_set_ta(WIFI.keyboard, WIFI.kbta);
		lv_kb_set_ok_action(WIFI.keyboard,kb_ok_action_animation);
		lv_kb_set_hide_action(WIFI.keyboard,kb_clean_action_animation);
		lv_obj_set_size(WIFI.keyboard,260,100);
		lv_obj_align(WIFI.keyboard,lv_scr_act(),LV_ALIGN_IN_BOTTOM_MID,0,0);	
		//创建定时器
		RTE_RoundRobin_CreateTimer(1,"WifiConnectCheck",10000,0,0,WifiConnectCheckCallback,btn);
	}
	return LV_RES_OK;
}
/*************************************************
*** Function: 扫描WIFI定时器回调函数
*************************************************/
static void WifiScanCheckCallback(void *arg)
{
	if(WIFISMHandle.ScanResult.num)
	{
		WIFISMHandle.ScanFlag = true;
		lv_obj_del(WIFI.mboxall);
		WIFI.mboxall = 0;
		WIFI.listssid = lv_list_create( WIFI.winwifi,NULL);
		lv_obj_set_width( WIFI.listssid,LV_HOR_RES-80);
		lv_obj_set_height( WIFI.listssid,110);
		for(uint8_t i=0;i<WIFISMHandle.ScanResult.num;i++)
		{
			lv_list_add(WIFI.listssid, SYMBOL_WIFI, (char *)WIFISMHandle.ScanResult.WlanNetworkInfo[i].ssid,wifi_select_action);
		}
		lv_list_set_style(WIFI.listssid, LV_LIST_STYLE_BG, &lv_style_transp_tight);
		lv_list_set_style(WIFI.listssid, LV_LIST_STYLE_SCRL, &lv_style_transp_tight);
		lv_list_set_style(WIFI.listssid, LV_LIST_STYLE_BTN_REL, &overall_style_btn_rel);
		lv_list_set_style(WIFI.listssid, LV_LIST_STYLE_BTN_PR, &overall_style_btn_pr);
		lv_list_set_style(WIFI.listssid, LV_LIST_STYLE_SB, &overall_style_sb);
		lv_obj_align(WIFI.listssid,WIFI.winwifi,LV_ALIGN_IN_TOP_MID,0,120);
		lv_obj_animate(WIFI.listssid, LV_ANIM_FLOAT_RIGHT | LV_ANIM_IN, 300, 50, NULL);
	}
	else
	{
    lv_obj_del(WIFI.mboxall);
		WIFI.mboxall = 0;
    WIFI.mboxall = lv_mbox_create(lv_scr_act(),NULL);
    lv_mbox_set_text( WIFI.mboxall, "No Vaild HotPots!");
		lv_obj_set_width(WIFI.mboxall,LV_HOR_RES*2/3);
    static const char * btns3[] = {"OK",""};
    lv_mbox_add_btns(WIFI.mboxall, btns3, wifi_mbox_close_action);
    lv_obj_align(WIFI.mboxall,lv_scr_act(),LV_ALIGN_CENTER,0,0);
    lv_sw_off(WIFI.swwifi);
	}
}
/*************************************************
*** Function: 扫描WiFi取消按钮回调函数
*************************************************/
static lv_res_t wifi_scancancel_action(lv_obj_t * btnm, const char *txt)
{
	lv_sw_off(WIFI.swwifi);
	lv_obj_del(WIFI.mboxall);
	WIFI.mboxall = 0;
	RTE_RoundRobin_RemoveTimer(1,RTE_RoundRobin_GetTimerID(1,"WifiScanCheck"));
	return LV_RES_INV;
}
/*************************************************
*** Function: 断开WIFI定时器回调函数
*************************************************/
static void WifiDisconnectCheckCallback(void *arg)
{
	char btnlabel[128] = {0};
	if(WIFISMHandle.ConnectFlag==false)
	{
		//清除已连接
		WIFISMHandle.SelectListBtn = 0;
		WIFISMHandle.SelectSSIDNum = -1;
		//断开连接
		RTE_MEM_Free(MEM_RTE,WIFISMHandle.ScanResult.WlanNetworkInfo);
		//颜色反转
		lv_img_set_src(OverAllPage.iconinfor[1],SYMBOL_CLOSE);
		WIFISMHandle.ScanFlag = false;
		lv_obj_animate(WIFI.listssid, LV_ANIM_GROW_H | LV_ANIM_OUT, 200, 0, NULL);
		lv_obj_animate(WIFI.listssid, LV_ANIM_GROW_V | LV_ANIM_OUT, 200, 0, wifi_close_action_animation_cb);
	}
	else
	{
		//清除mbox
		lv_obj_del(WIFI.mboxall);
		WIFI.mboxall = 0;
		//消息提示mbox
		WIFI.mboxall = lv_mbox_create(lv_scr_act(),NULL);
		strcat(btnlabel,"Close Connect:\n");
		strcat(btnlabel,WIFISMHandle.SelectSSID);
		strcat(btnlabel," Fail!");
		lv_mbox_set_text( WIFI.mboxall, btnlabel);
		lv_obj_set_width(WIFI.mboxall,LV_HOR_RES*2/3);
		lv_obj_align(WIFI.mboxall,lv_scr_act(),LV_ALIGN_CENTER,0,0);
	}
}
/*************************************************
*** Function: WiFi启停开关回调函数
*************************************************/
static lv_res_t wifi_sw_action(lv_obj_t * sw)
{
	if(lv_sw_get_state(sw))
	{
		WIFI.mboxall = lv_mbox_create(lv_scr_act(),NULL);
		lv_mbox_set_text( WIFI.mboxall, "Hotpots Scanning...");
		lv_obj_set_width(WIFI.mboxall,LV_HOR_RES*2/3);
		static const char * btns3[] = {"Cancel",""};
		lv_obj_t * preload3 = lv_preload_create(WIFI.mboxall, NULL);
		lv_preload_set_arc_length(preload3, 90);
		lv_preload_set_spin_time(preload3, 5000);
		lv_mbox_add_btns(WIFI.mboxall, btns3, wifi_scancancel_action);
		lv_obj_align(WIFI.mboxall,lv_scr_act(),LV_ALIGN_CENTER,0,0);
		RTE_StateMachine_ChangeTo(&WIFISMHandle.SM,WIFI_SCAN);
		RTE_RoundRobin_CreateTimer(1,"WifiScanCheck",4000,0,1,WifiScanCheckCallback,NULL);
	}
	else
	{
		WIFI.mboxall = lv_mbox_create(lv_scr_act(),NULL);
		lv_mbox_set_text( WIFI.mboxall, "Close Connect...");
		lv_obj_set_width(WIFI.mboxall,LV_HOR_RES*2/3);
		lv_obj_t * preload3 = lv_preload_create(WIFI.mboxall, NULL);
		lv_preload_set_arc_length(preload3, 90);
		lv_preload_set_spin_time(preload3, 5000);
		lv_obj_align(WIFI.mboxall,lv_scr_act(),LV_ALIGN_CENTER,0,0);
		RTE_StateMachine_ChangeTo(&WIFISMHandle.SM,WIFI_DISCONNECT);
		RTE_RoundRobin_CreateTimer(1,"WifiDisconnectCheck",2000,0,1,WifiDisconnectCheckCallback,NULL);
	}
	return LV_RES_OK;
}
void GUI_WIFI_PageInit(void)
{
	//WIFI窗口
	WIFI.winwifi = lv_win_create(lv_scr_act(), NULL);
	lv_obj_set_size(WIFI.winwifi,LV_HOR_RES,LV_VER_RES);
	lv_win_set_layout(WIFI.winwifi, LV_LAYOUT_OFF); //列布局
	lv_win_set_title(WIFI.winwifi,"Internet");
	lv_win_set_btn_size(WIFI.winwifi,40);
	lv_win_set_sb_mode(WIFI.winwifi, LV_SB_MODE_AUTO);
	lv_win_add_btn(WIFI.winwifi,SYMBOL_CLOSE,wifi_close_action_animation);
	lv_obj_align(WIFI.winwifi,lv_scr_act(),LV_ALIGN_CENTER,0,0);
	lv_obj_set_opa_scale_enable(WIFI.winwifi,true);
	lv_obj_set_opa_scale(WIFI.winwifi,0xF0);

	WIFI.lablesw = lv_label_create(WIFI.winwifi, NULL);
	static lv_style_t labelstyle;
	lv_style_copy(&labelstyle,lv_label_get_style(WIFI.lablesw));
	labelstyle.text.font = &lv_font_dejavu_20;
	lv_label_set_style(WIFI.lablesw,&labelstyle);
	lv_label_set_text(WIFI.lablesw,"WIFI Switch");
	lv_obj_align(WIFI.lablesw, WIFI.winwifi, LV_ALIGN_IN_TOP_LEFT, 30, 50);

	WIFI.swwifi = lv_sw_create(WIFI.winwifi, NULL);
	if(WIFISMHandle.ScanFlag == false)
		lv_sw_off(WIFI.swwifi);
	else
	{
		lv_sw_on(WIFI.swwifi);
		WIFI.listssid = lv_list_create( WIFI.winwifi,NULL);
		lv_obj_set_height( WIFI.listssid,110);
		lv_obj_set_width( WIFI.listssid,LV_HOR_RES-80);
		for(uint8_t i=0;i<WIFISMHandle.ScanResult.num;i++)
		{
			if(WIFISMHandle.SelectSSIDNum == i)
			{
				char btnlabel[128] = {0};
				strcat(btnlabel,(char *)WIFISMHandle.ScanResult.WlanNetworkInfo[i].ssid);
				strcat(btnlabel,"   ");
				strcat(btnlabel,SYMBOL_OK);
				WIFISMHandle.SelectListBtn = lv_list_add(WIFI.listssid, SYMBOL_WIFI, btnlabel,wifi_select_action);
			}
			else
				lv_list_add(WIFI.listssid, SYMBOL_WIFI, (char *)WIFISMHandle.ScanResult.WlanNetworkInfo[i].ssid,wifi_select_action);
		}
		lv_list_set_style(WIFI.listssid, LV_LIST_STYLE_BG, &lv_style_transp_tight);
		lv_list_set_style(WIFI.listssid, LV_LIST_STYLE_SCRL, &lv_style_transp_tight);
		lv_list_set_style(WIFI.listssid, LV_LIST_STYLE_BTN_REL, &overall_style_btn_rel);
		lv_list_set_style(WIFI.listssid, LV_LIST_STYLE_BTN_PR, &overall_style_btn_pr);
		lv_list_set_style(WIFI.listssid, LV_LIST_STYLE_SB, &overall_style_sb);
		lv_obj_align(WIFI.listssid,WIFI.winwifi,LV_ALIGN_IN_TOP_MID,0,120);
		lv_obj_animate(WIFI.listssid, LV_ANIM_FLOAT_RIGHT | LV_ANIM_IN, 300, 50, NULL);
	}
	lv_sw_set_action(WIFI.swwifi,wifi_sw_action);
	lv_obj_align(WIFI.swwifi, WIFI.lablesw, LV_ALIGN_OUT_RIGHT_MID, 100, 0);

	//下拉效果动画
	/* Float in the button using a built-in function
	 * Delay the animation with 50 ms and float in 300 ms. NULL means no end callback*/
	lv_obj_animate(WIFI.winwifi, LV_ANIM_FLOAT_TOP | LV_ANIM_IN, 300, 50, NULL);
}
