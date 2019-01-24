#include "GUI_TCPServer.h"
#include "WIFI_SM.h"
GUI_TCPServer_t TCPServer = {0};
static void TCPServer_close_action_animation_cb(lv_obj_t * obj)
{
  lv_obj_del(obj);
	obj = 0;
}
static lv_res_t TCPServer_close_action_animation(lv_obj_t * btn)
{
	if(TCPServer.mboxall)
	{
		lv_obj_del(TCPServer.mboxall);
		TCPServer.mboxall = 0;
	}
	lv_obj_t * win = lv_win_get_from_btn(btn);
	/* Float in the button using a built-in function
	* Delay the animation with 50 ms and float in 300 ms. NULL means no end callback*/
	lv_obj_animate(win, LV_ANIM_FLOAT_TOP | LV_ANIM_OUT, 300, 50, TCPServer_close_action_animation_cb);
	return LV_RES_OK;
}
static void ServerOpenCheckCallback(void *arg)
{
		if(WIFISMHandle.ServerSocket!=-1)
		{
			lv_obj_del(TCPServer.mboxall);
			TCPServer.mboxall = 0;
			lv_btn_set_state(TCPServer.btnopen, LV_BTN_STATE_INA); 
			lv_btn_set_state(TCPServer.btnclose, LV_BTN_STATE_REL); 
		}
		else
		{
			lv_obj_del(TCPServer.mboxall);
			TCPServer.mboxall = 0;
			TCPServer.mboxall = lv_mbox_create(lv_scr_act(),NULL);
			lv_mbox_set_text(TCPServer.mboxall, "Open Server Fail!");
			lv_obj_set_width(TCPServer.mboxall,LV_HOR_RES*2/3);
			lv_obj_align(TCPServer.mboxall,lv_scr_act(),LV_ALIGN_CENTER,0,0);
		}
}
static void ServerCloseCheckCallback(void *arg)
{
		if(WIFISMHandle.ServerSocket == -1)
		{
			lv_obj_del(TCPServer.mboxall);
			TCPServer.mboxall = 0;
			lv_btn_set_state(TCPServer.btnopen, LV_BTN_STATE_REL); 
			lv_btn_set_state(TCPServer.btnclose, LV_BTN_STATE_INA); 
		}
		else
		{
			lv_obj_del(TCPServer.mboxall);
			TCPServer.mboxall = 0;
			TCPServer.mboxall = lv_mbox_create(lv_scr_act(),NULL);
			lv_mbox_set_text( TCPServer.mboxall, "Close Server Fail!");
			lv_obj_set_width(TCPServer.mboxall,LV_HOR_RES*2/3);
			lv_obj_align(TCPServer.mboxall,lv_scr_act(),LV_ALIGN_CENTER,0,0);
		}
}
static lv_res_t tcpserver_btn_open_action(lv_obj_t *btn)
{
	  memset(WIFISMHandle.PortNum,0,8);
		strcat(WIFISMHandle.PortNum,lv_ta_get_text(TCPServer.taportnum));
		TCPServer.mboxall = lv_mbox_create(lv_scr_act(),NULL);
		lv_mbox_set_text( TCPServer.mboxall, "Open Server...");
		lv_obj_set_width(TCPServer.mboxall,LV_HOR_RES*2/3);
		lv_obj_t * preload3 = lv_preload_create(TCPServer.mboxall, NULL);
		lv_preload_set_arc_length(preload3, 90);
		lv_preload_set_spin_time(preload3, 5000);
		lv_obj_align(TCPServer.mboxall,lv_scr_act(),LV_ALIGN_CENTER,0,0);
		RTE_StateMachine_ChangeTo(&WIFISMHandle.SM,WIFI_OPENTCPSERVER);
		RTE_RoundRobin_CreateTimer(1,"ServerOpenCheck",10000,0,1,ServerOpenCheckCallback,NULL);
    return LV_RES_OK;
}
static lv_res_t tcpserver_btn_close_action(lv_obj_t *btn)
{
		TCPServer.mboxall = lv_mbox_create(lv_scr_act(),NULL);
		lv_mbox_set_text( TCPServer.mboxall, "Close Server...");
		lv_obj_set_width(TCPServer.mboxall,LV_HOR_RES*2/3);
		lv_obj_t * preload3 = lv_preload_create(TCPServer.mboxall, NULL);
		lv_preload_set_arc_length(preload3, 90);
		lv_preload_set_spin_time(preload3, 5000);
		lv_obj_align(TCPServer.mboxall,lv_scr_act(),LV_ALIGN_CENTER,0,0);
		RTE_StateMachine_ChangeTo(&WIFISMHandle.SM,WIFI_CLOSETCPSERVER);
		RTE_RoundRobin_CreateTimer(1,"ServerCloseCheck",5000,0,1,ServerCloseCheckCallback,NULL);
    return LV_RES_OK;
}
static lv_res_t tcpserver_btnm_action(lv_obj_t * btnm, const char *txt)
{
		if(ustrstr(txt,"DEL"))
		{
			if(strlen(lv_ta_get_text(TCPServer.taportnum)))
			{
				char buf[8] = {0};
				memcpy(buf,lv_ta_get_text(TCPServer.taportnum),strlen(lv_ta_get_text(TCPServer.taportnum))-1);
				lv_ta_set_text(TCPServer.taportnum,buf);
			}
		}
		else
		{
			if(strlen(lv_ta_get_text(TCPServer.taportnum))<4)
			{
				char buf[8] = {0};
				strcat(buf,lv_ta_get_text(TCPServer.taportnum));
				strcat(buf,txt);
				lv_ta_set_text(TCPServer.taportnum,buf);
			}
		}
    return LV_RES_OK; /*Return OK because the button matrix is not deleted*/
}
void GUI_TCPServer_PageInit(void)
{
	//TCPServer窗口
	TCPServer.wintcpserver = lv_win_create(lv_scr_act(), NULL);
	lv_obj_set_size(TCPServer.wintcpserver,LV_HOR_RES,LV_VER_RES);
	lv_win_set_layout(TCPServer.wintcpserver, LV_LAYOUT_OFF);
	lv_win_set_title(TCPServer.wintcpserver,"TCPServer");
	lv_win_set_btn_size(TCPServer.wintcpserver,40);
	lv_win_set_sb_mode(TCPServer.wintcpserver, LV_SB_MODE_AUTO);
	lv_win_add_btn(TCPServer.wintcpserver,SYMBOL_CLOSE,TCPServer_close_action_animation);
	lv_obj_align(TCPServer.wintcpserver,lv_scr_act(),LV_ALIGN_CENTER,0,0);
	lv_obj_set_opa_scale_enable(TCPServer.wintcpserver,true);
	lv_obj_set_opa_scale(TCPServer.wintcpserver,0xF0);
	
	if(WIFISMHandle.IPFlag)
	{
		char tempbuf[32] = {0};
		TCPServer.labelip = lv_label_create(TCPServer.wintcpserver,NULL);
		usprintf(tempbuf,"IP:  %s",WIFISMHandle.LocalIP);
		lv_label_set_text(TCPServer.labelip,tempbuf);
		lv_obj_align(TCPServer.labelip,TCPServer.wintcpserver,LV_ALIGN_IN_TOP_LEFT,20,20);
		
		TCPServer.labelportnum = lv_label_create(TCPServer.wintcpserver,NULL);
		lv_label_set_text(TCPServer.labelportnum,"Port:");
		lv_obj_align(TCPServer.labelportnum,TCPServer.labelip,LV_ALIGN_OUT_BOTTOM_LEFT,0,15);
		
		TCPServer.taportnum = lv_ta_create(TCPServer.wintcpserver,NULL);
		lv_obj_align(TCPServer.taportnum,TCPServer.labelportnum,LV_ALIGN_OUT_RIGHT_MID,10,30);
		lv_obj_set_size(TCPServer.taportnum,100,35);
		lv_ta_set_text(TCPServer.taportnum,"");
		
		TCPServer.btnopen = lv_btn_create(TCPServer.wintcpserver,NULL);
		lv_obj_align(TCPServer.btnopen,TCPServer.labelip,LV_ALIGN_OUT_RIGHT_MID,60,20);
		lv_obj_set_size(TCPServer.btnopen,80,30);
		lv_btn_set_action(TCPServer.btnopen,LV_BTN_ACTION_CLICK,tcpserver_btn_open_action);
		
		TCPServer.labelopen = lv_label_create(TCPServer.btnopen,NULL);
		lv_label_set_text(TCPServer.labelopen,"Open");
		lv_obj_align(TCPServer.labelopen,TCPServer.btnopen,LV_ALIGN_CENTER,0,0);
		
		TCPServer.btnclose = lv_btn_create(TCPServer.wintcpserver,NULL);
		lv_obj_align(TCPServer.btnclose,TCPServer.btnopen,LV_ALIGN_OUT_BOTTOM_LEFT,0,5);
		lv_obj_set_size(TCPServer.btnclose,80,30);
		if(WIFISMHandle.ServerSocket==-1)
		{
			lv_ta_set_text(TCPServer.taportnum,WIFISMHandle.PortNum);
			lv_btn_set_state(TCPServer.btnclose, LV_BTN_STATE_INA); 
		}
		else
		{
			lv_ta_set_text(TCPServer.taportnum,WIFISMHandle.PortNum);
			lv_btn_set_state(TCPServer.btnopen, LV_BTN_STATE_INA); 
		}
		lv_btn_set_action(TCPServer.btnclose,LV_BTN_ACTION_CLICK,tcpserver_btn_close_action);
		
		TCPServer.labelclose = lv_label_create(TCPServer.btnclose,NULL);
		lv_label_set_text(TCPServer.labelclose,"Close");
		lv_obj_align(TCPServer.labelclose,TCPServer.btnclose,LV_ALIGN_CENTER,0,0);
		
		
		static const char * btnm_map[] = {"1", "2", "3", "4", "5", "\n",
															 "6", "7", "8", "9", "0", "\n",
															 "DEL", ""};
		/*Create a second button matrix with the new styles*/
		TCPServer.btnm = lv_btnm_create(TCPServer.wintcpserver, NULL);
		lv_btnm_set_map(TCPServer.btnm, btnm_map);
		lv_obj_set_size(TCPServer.btnm,240,80);
		lv_btnm_set_action(TCPServer.btnm, tcpserver_btnm_action);
		lv_obj_align(TCPServer.btnm,TCPServer.wintcpserver,LV_ALIGN_IN_BOTTOM_MID,0,0);
		/*Create a new style for the button matrix back ground*/
		static lv_style_t style_bg;
		lv_style_copy(&style_bg, &lv_style_plain);
		style_bg.body.main_color = LV_COLOR_SILVER;
		style_bg.body.grad_color = LV_COLOR_SILVER;
		style_bg.body.padding.hor = 0;
		style_bg.body.padding.ver = 0;
		style_bg.body.padding.inner = 0;

		/*Create 2 button styles*/
		static lv_style_t style_btn_rel;
		static lv_style_t style_btn_pr;
		lv_style_copy(&style_btn_rel, &lv_style_btn_rel);
		style_btn_rel.body.main_color = LV_COLOR_MAKE(176,196,222);
		style_btn_rel.body.grad_color = LV_COLOR_MAKE(119,136,153);
		style_btn_rel.body.border.color = LV_COLOR_SILVER;
		style_btn_rel.body.border.width = 1;
		style_btn_rel.body.border.opa = LV_OPA_50;
		style_btn_rel.body.radius = 0;

		lv_style_copy(&style_btn_pr, &style_btn_rel);
		style_btn_pr.body.main_color = LV_COLOR_MAKE(0x55, 0x96, 0xd8);
		style_btn_pr.body.grad_color = LV_COLOR_MAKE(0x37, 0x62, 0x90);
		style_btn_pr.text.color = LV_COLOR_MAKE(0xbb, 0xd5, 0xf1); 
		lv_btnm_set_style(TCPServer.btnm, LV_BTNM_STYLE_BG, &style_bg);
		lv_btnm_set_style(TCPServer.btnm, LV_BTNM_STYLE_BTN_REL, &style_btn_rel);
		lv_btnm_set_style(TCPServer.btnm, LV_BTNM_STYLE_BTN_PR, &style_btn_pr);
		
	}
	else
	{
		TCPServer.mboxall = lv_mbox_create(lv_scr_act(),NULL);
		lv_mbox_set_text( TCPServer.mboxall, "No Valid IP!");
		lv_obj_set_width(TCPServer.mboxall,LV_HOR_RES*2/3);
		lv_obj_align(TCPServer.mboxall,lv_scr_act(),LV_ALIGN_CENTER,0,0);
	}
	//下拉效果动画
	/* Float in the button using a built-in function
	 * Delay the animation with 50 ms and float in 300 ms. NULL means no end callback*/
	lv_obj_animate(TCPServer.wintcpserver, LV_ANIM_FLOAT_TOP | LV_ANIM_IN, 300, 50, NULL);
}
