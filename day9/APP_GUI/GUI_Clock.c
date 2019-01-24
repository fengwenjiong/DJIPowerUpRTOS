#include "GUI_Clock.h"
GUI_Clock_t Clock = {0};
static void clock_close_action_animation_cb(lv_obj_t * obj)
{
  lv_obj_del(obj);
	obj = 0;
}
static lv_res_t clock_close_action_animation(lv_obj_t * btn)
{
    lv_obj_t * win = lv_win_get_from_btn(btn);
    /* Float in the button using a built-in function
    * Delay the animation with 50 ms and float in 300 ms. NULL means no end callback*/
    lv_obj_animate(win, LV_ANIM_FLOAT_TOP | LV_ANIM_OUT, 300, 50, clock_close_action_animation_cb);
    return LV_RES_OK;
}
void GUI_Clock_PageInit(void)
{
	//clock窗口
	Clock.winclock = lv_win_create(lv_scr_act(), NULL);
	lv_obj_set_size(Clock.winclock,LV_HOR_RES,LV_VER_RES);
	lv_win_set_layout(Clock.winclock, LV_LAYOUT_PRETTY); //无布局
	lv_win_set_title(Clock.winclock,"Clock");
	lv_win_set_btn_size(Clock.winclock,40);
	lv_win_set_sb_mode(Clock.winclock, LV_SB_MODE_AUTO);
	lv_win_add_btn(Clock.winclock,SYMBOL_CLOSE,clock_close_action_animation);
	lv_obj_align(Clock.winclock,lv_scr_act(),LV_ALIGN_CENTER,0,0);
	lv_obj_set_opa_scale_enable(Clock.winclock,true);
	lv_obj_set_opa_scale(Clock.winclock,0xF0);
	
	lv_obj_t *rl = lv_roller_create(Clock.winclock,NULL);
	lv_obj_set_height(rl,30);
	//下拉效果动画
	/* Float in the button using a built-in function
	 * Delay the animation with 50 ms and float in 300 ms. NULL means no end callback*/
	lv_obj_animate(Clock.winclock, LV_ANIM_FLOAT_TOP | LV_ANIM_IN, 300, 50, NULL);
}
