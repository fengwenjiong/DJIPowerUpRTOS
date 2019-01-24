#include "GUI_OverAll.h"
#include "GUI_Wifi.h"
#include "GUI_BenchMark.h"
#include "GUI_Clock.h"
#include "GUI_TCPServer.h"
//#include "Board_RTC.h"
GUI_OverAll_t OverAllPage = {0};
/*********************
 * Create new styles
 ********************/
/*Create a scroll bar style*/
lv_style_t overall_style_sb = {0};
/*Create styles for the buttons*/
lv_style_t overall_style_btn_rel = {0};
lv_style_t overall_style_btn_pr = {0};

static lv_res_t btn_wifi_action(lv_obj_t *obj)
{
    GUI_WIFI_PageInit();
    return LV_RES_OK;
}
static lv_res_t btn_bench_action(lv_obj_t *obj)
{
    GUI_BenchMark_Init();
    return LV_RES_OK;
}
static lv_res_t btn_clock_action(lv_obj_t *obj)
{
		GUI_Clock_PageInit();
    return LV_RES_OK;
}
static lv_res_t btn_tcpserver_action(lv_obj_t *obj)
{
		GUI_TCPServer_PageInit();
    return LV_RES_OK;
}
static void GUI_CPU_TimerCallback(void *arg)
{
	lv_obj_t *label = (lv_obj_t *)arg;
	if(label)
	{
		extern volatile uint8_t   StaticsCPUUsage;
		char buf[16]={0};
		usprintf(buf,"CPU:%d%%",StaticsCPUUsage);
		lv_label_set_text(label,buf);
	}
}
void GUI_OverAll_Init(void)
{
	//桌面图标按钮风格
	static lv_style_t btn_style_rel,btn_style_pr;
	lv_style_copy(&btn_style_rel,&lv_style_transp);
	lv_style_copy(&btn_style_pr,&lv_style_plain_color);
	btn_style_pr.body.opa = 80;
	lv_style_copy(&overall_style_sb, &lv_style_plain);
	overall_style_sb.body.main_color = LV_COLOR_BLACK;
	overall_style_sb.body.grad_color = LV_COLOR_BLACK;
	overall_style_sb.body.border.color = LV_COLOR_WHITE;
	overall_style_sb.body.border.width = 1;
	overall_style_sb.body.border.opa = LV_OPA_70;
	overall_style_sb.body.radius = LV_RADIUS_CIRCLE;
	overall_style_sb.body.opa = LV_OPA_60;
	lv_style_copy(&overall_style_btn_rel, &lv_style_btn_rel);
	overall_style_btn_rel.body.main_color = LV_COLOR_MAKE(112,128,144);
	overall_style_btn_rel.body.grad_color = LV_COLOR_MAKE(0x30, 0x30, 0x30);
	overall_style_btn_rel.body.border.color = LV_COLOR_SILVER;
	overall_style_btn_rel.body.border.width = 2;
	overall_style_btn_rel.body.border.opa = LV_OPA_50;
	overall_style_btn_rel.body.radius = 0;
	lv_style_copy(&overall_style_btn_pr, &overall_style_btn_rel);
	overall_style_btn_pr.body.main_color = LV_COLOR_MAKE(0x55, 0x96, 0xd8);
	overall_style_btn_pr.body.grad_color = LV_COLOR_MAKE(0x37, 0x62, 0x90);
	overall_style_btn_pr.text.color = LV_COLOR_MAKE(0xbb, 0xd5, 0xf1);
	
	//overall tab
	OverAllPage.tabview = lv_tabview_create(lv_scr_act(), NULL);
	lv_obj_set_size(OverAllPage.tabview, LV_HOR_RES, LV_VER_RES);
	//-数据显示tabview
	lv_tabview_add_tab(OverAllPage.tabview, "Data Centor");


	//-设置tabview
	lv_tabview_add_tab(OverAllPage.tabview, "Settings");
	//--wifi app
	//---图标
	LV_IMG_DECLARE(img_internet);
	OverAllPage.iconwifi = lv_img_create(lv_tabview_get_tab(OverAllPage.tabview, 1), NULL);
	lv_img_set_src(OverAllPage.iconwifi, &img_internet);
	lv_obj_align(OverAllPage.iconwifi,lv_tabview_get_tab(OverAllPage.tabview, 1),LV_ALIGN_IN_TOP_LEFT,15,-5);
	//---按键
	OverAllPage.btnwifi = lv_btn_create(lv_tabview_get_tab(OverAllPage.tabview, 1), NULL);
	lv_obj_set_size(OverAllPage.btnwifi,64+5,64+5);
	lv_obj_align(OverAllPage.btnwifi,OverAllPage.iconwifi,LV_ALIGN_CENTER,0,0);
	lv_btn_set_style(OverAllPage.btnwifi,LV_BTN_STYLE_REL,&btn_style_rel);
	lv_btn_set_style(OverAllPage.btnwifi,LV_BTN_STYLE_PR,&btn_style_pr);
	lv_btn_set_action(OverAllPage.btnwifi,LV_BTN_ACTION_CLICK,btn_wifi_action);
	//--benchmark app
	//---图标
	LV_IMG_DECLARE(img_benchmark);
	OverAllPage.iconbench = lv_img_create(lv_tabview_get_tab(OverAllPage.tabview, 1), NULL);
	lv_img_set_src(OverAllPage.iconbench, &img_benchmark);
	lv_obj_align(OverAllPage.iconbench,OverAllPage.iconwifi,LV_ALIGN_OUT_RIGHT_MID,10,0);
	//---按键
	OverAllPage.btnwifi = lv_btn_create(lv_tabview_get_tab(OverAllPage.tabview, 1), NULL);
	lv_obj_set_size(OverAllPage.btnwifi,64+5,64+5);
	lv_obj_align(OverAllPage.btnwifi,OverAllPage.iconbench,LV_ALIGN_CENTER,0,0);
	lv_btn_set_style(OverAllPage.btnwifi,LV_BTN_STYLE_REL,&btn_style_rel);
	lv_btn_set_style(OverAllPage.btnwifi,LV_BTN_STYLE_PR,&btn_style_pr);
	lv_btn_set_action(OverAllPage.btnwifi,LV_BTN_ACTION_CLICK,btn_bench_action);
	//--clock app
	//---图标
	LV_IMG_DECLARE(img_clock);
	OverAllPage.iconclock = lv_img_create(lv_tabview_get_tab(OverAllPage.tabview, 1), NULL);
	lv_img_set_src(OverAllPage.iconclock, &img_clock);
	lv_obj_align(OverAllPage.iconclock,OverAllPage.iconbench,LV_ALIGN_OUT_RIGHT_MID,10,0);
	//---按键
	OverAllPage.btnclock = lv_btn_create(lv_tabview_get_tab(OverAllPage.tabview, 1), NULL);
	lv_obj_set_size(OverAllPage.btnclock,64+5,64+5);
	lv_obj_align(OverAllPage.btnclock,OverAllPage.iconclock,LV_ALIGN_CENTER,0,0);
	lv_btn_set_style(OverAllPage.btnclock,LV_BTN_STYLE_REL,&btn_style_rel);
	lv_btn_set_style(OverAllPage.btnclock,LV_BTN_STYLE_PR,&btn_style_pr);
	lv_btn_set_action(OverAllPage.btnclock,LV_BTN_ACTION_CLICK,btn_clock_action);
	//--TCPServer app
	//---图标
	LV_IMG_DECLARE(img_tcpserver);
	OverAllPage.icontcpserver = lv_img_create(lv_tabview_get_tab(OverAllPage.tabview, 1), NULL);
	lv_img_set_src(OverAllPage.icontcpserver, &img_tcpserver);
	lv_obj_align(OverAllPage.icontcpserver,OverAllPage.iconclock,LV_ALIGN_OUT_RIGHT_MID,10,0);
	//---按键
	OverAllPage.btntcpserver = lv_btn_create(lv_tabview_get_tab(OverAllPage.tabview, 1), NULL);
	lv_obj_set_size(OverAllPage.btntcpserver,64+5,64+5);
	lv_obj_align(OverAllPage.btntcpserver,OverAllPage.icontcpserver,LV_ALIGN_CENTER,0,0);
	lv_btn_set_style(OverAllPage.btntcpserver,LV_BTN_STYLE_REL,&btn_style_rel);
	lv_btn_set_style(OverAllPage.btntcpserver,LV_BTN_STYLE_PR,&btn_style_pr);
	lv_btn_set_action(OverAllPage.btntcpserver,LV_BTN_ACTION_CLICK,btn_tcpserver_action);
	
	//以lv_src_art()为父控件建立cont控件
  OverAllPage.continfor = lv_cont_create(lv_scr_act(), NULL);
	//cont控件的风格，根据默认风格修改，style每次重绘都需要被调用，因此必须是全局变量或静态变量
	static lv_style_t contstyle;
	lv_style_copy(&contstyle,lv_cont_get_style(OverAllPage.continfor));
	contstyle.body.border.width = 2;
	contstyle.body.main_color = LV_COLOR_MAKE(112,128,144);
	contstyle.body.grad_color = LV_COLOR_MAKE(112,128,144);
	contstyle.body.shadow.color = LV_COLOR_MAKE(230,230,250);
	contstyle.body.shadow.width = 3;
	//设置cont控件属性
	lv_cont_set_style(OverAllPage.continfor,&contstyle);
	lv_cont_set_layout(OverAllPage.continfor,LV_LAYOUT_OFF);
	lv_obj_set_size(OverAllPage.continfor,LV_HOR_RES-20,25);
	lv_obj_align(OverAllPage.continfor,lv_scr_act(),LV_ALIGN_IN_BOTTOM_MID,0,-15);
	//-电量图标，以GUI自带的SYMBOL_BATTERY_FULL作为图标
	OverAllPage.iconinfor[0] = lv_img_create(OverAllPage.continfor,NULL);
	lv_img_set_src(OverAllPage.iconinfor[0],SYMBOL_BATTERY_FULL);
	lv_obj_align(OverAllPage.iconinfor[0],OverAllPage.continfor,LV_ALIGN_IN_RIGHT_MID,-15,0);
	//-WIFI状态图标，以GUI自带的SYMBOL_CLOSE作为图标（默认关闭）
	OverAllPage.iconinfor[1] = lv_img_create(OverAllPage.continfor,NULL);
	lv_img_set_src(OverAllPage.iconinfor[1],SYMBOL_CLOSE);
	lv_obj_align(OverAllPage.iconinfor[1],OverAllPage.continfor,LV_ALIGN_IN_LEFT_MID,15,0);
	
	//-时间label，显示当前时间
	OverAllPage.labelinfor[0] = lv_label_create(OverAllPage.continfor, NULL);
	//label的风格，使用小字体
	static lv_style_t labelstyle;
	lv_style_copy(&labelstyle,lv_label_get_style(OverAllPage.labelinfor[0]));
	labelstyle.text.font = &lv_font_dejavu_10;
	labelstyle.text.color = LV_COLOR_MAKE(248,248,255);
	lv_label_set_style(OverAllPage.labelinfor[0],&labelstyle);
	lv_label_set_text(OverAllPage.labelinfor[0], "13:58");
	lv_obj_align(OverAllPage.labelinfor[0],OverAllPage.continfor,LV_ALIGN_CENTER,-20,0);
	//-日期label，显示当前日期
	OverAllPage.labelinfor[1] = lv_label_create(OverAllPage.continfor, NULL);
	lv_label_set_style(OverAllPage.labelinfor[1],&labelstyle);
	lv_label_set_text(OverAllPage.labelinfor[1], "2019/01/15");
	lv_obj_align(OverAllPage.labelinfor[1],OverAllPage.labelinfor[0],LV_ALIGN_OUT_RIGHT_MID,10,0);
	//-电量label，显示当前电量
	OverAllPage.labelinfor[2] = lv_label_create(OverAllPage.continfor, NULL);
	lv_label_set_style(OverAllPage.labelinfor[2],&labelstyle);
	lv_label_set_text(OverAllPage.labelinfor[2], "100%");
	//-CPU label，显示当前CPU占用率
	lv_obj_align(OverAllPage.labelinfor[2],OverAllPage.iconinfor[0],LV_ALIGN_OUT_LEFT_MID,-5,0);
	OverAllPage.labelinfor[3] = lv_label_create(OverAllPage.continfor, NULL);
	lv_label_set_style(OverAllPage.labelinfor[3],&labelstyle);
	lv_label_set_text(OverAllPage.labelinfor[3], "CPU:NA%");
	lv_obj_align(OverAllPage.labelinfor[3],OverAllPage.iconinfor[1],LV_ALIGN_OUT_RIGHT_MID,10,0);
	//-CPU label刷新定时器
	RTE_RoundRobin_CreateTimer(1,"GUICPU",25,1,1,GUI_CPU_TimerCallback,OverAllPage.labelinfor[3]);	
}


