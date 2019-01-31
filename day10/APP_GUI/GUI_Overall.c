#include "GUI_Overall.h"
#include "GUI_Filebrowser.h"
#include "MV_Support/MV_BMP.h"
lv_img_dsc_t img_test = {
  .header.always_zero = 0,
  .header.w = 0,
  .header.h = 0,
  .data_size = 0,
  .header.cf = LV_IMG_CF_TRUE_COLOR,
  .data = NULL,
};
GUI_RootObj_t OverallRoot = {0};
static lv_res_t btn_file_action(lv_obj_t *obj)
{
	GUI_Filebrowser_Entrance();
	return LV_RES_OK;
}
void GUI_Overall(void)
{
    //根控件
    GUI_Manage_NewRoot(GUI_TABVIEW,&OverallRoot,lv_scr_act(),NULL,16);
    //-数据tab
    lv_tabview_add_tab(OverallRoot.rootobj,"Data Center");
    GUI_SubObj_t *vimgdis = GUI_Manage_NewSub(GUI_IMG,&OverallRoot,lv_tabview_get_tab(OverallRoot.rootobj, 0),NULL);
    //文件读取测试
    image_t img = {0};
    bmp_read(&img,"Image/BMP_128x160/10_16.bmp");
    img_test.header.w = img.w;
    img_test.header.h = img.h;
    img_test.data_size = img.w*img.h*sizeof(lv_color_int_t);
    img_test.data = RTE_MEM_Alloc0(MEM_RTE,img_test.data_size);
    uint16_t *pSrcLine = (uint16_t *)img.pixels;
    lv_color_int_t *pDstLine = (lv_color_int_t *)img_test.data;
	for(uint16_t y=0;y<img_test.header.h;y++)
	{
		for(uint16_t x=0;x<img_test.header.w;x++)
		{
			uint16_t pixel = (pSrcLine)[x];
#if LV_COLOR_DEPTH == 32
			pDstLine[x] = LV_COLOR_MAKE(IM_R565(pixel)<<3,IM_G565(pixel)<<2,IM_B565(pixel)<<3).full;
#elif LV_COLOR_DEPTH == 16
            pDstLine[x] = pixel;
#endif // LV_COLOR_DEPTH
		}
	    pSrcLine += img.w;
	    pDstLine += img_test.header.w;
	}
    lv_img_set_src(vimgdis->body,&img_test);

    //-系统tab
    lv_tabview_add_tab(OverallRoot.rootobj,"System");
	//--file app
	//---图标
	LV_IMG_DECLARE(img_file);
	GUI_SubObj_t *viconfile = GUI_Manage_NewSub(GUI_IMG,&OverallRoot,lv_tabview_get_tab(OverallRoot.rootobj, 1),NULL);
	lv_img_set_src(viconfile->body, &img_file);
	lv_obj_align(viconfile->body,lv_tabview_get_tab(OverallRoot.rootobj, 1),LV_ALIGN_IN_TOP_LEFT,15,0);
	//---文字
	GUI_SubObj_t *vlabelfile = GUI_Manage_NewSub(GUI_LABEL,&OverallRoot,lv_tabview_get_tab(OverallRoot.rootobj, 1),NULL);
	lv_label_set_text(vlabelfile->body,"File");
	lv_obj_align(vlabelfile->body,viconfile->body,LV_ALIGN_OUT_BOTTOM_MID,0,5);
	//---按钮
	GUI_SubObj_t *vbtnfile = GUI_Manage_NewSub(GUI_BTN,&OverallRoot,lv_tabview_get_tab(OverallRoot.rootobj, 1),NULL);
    lv_obj_set_size(vbtnfile->body,lv_obj_get_width(viconfile->body)+5,lv_obj_get_height(viconfile->body)+5);
    lv_obj_align(vbtnfile->body,viconfile->body,LV_ALIGN_CENTER,0,0);
    //---入口函数
    lv_btn_set_action(vbtnfile->body,LV_BTN_ACTION_CLICK,btn_file_action);

    //风格
    static lv_style_t stylebtnpre = {0};
    lv_style_copy(&stylebtnpre,&lv_style_plain_color);
    stylebtnpre.body.opa = 80;
	lv_btn_set_style(vbtnfile->body,LV_BTN_STYLE_REL,&lv_style_transp);
	lv_btn_set_style(vbtnfile->body,LV_BTN_STYLE_PR,&stylebtnpre);
}
