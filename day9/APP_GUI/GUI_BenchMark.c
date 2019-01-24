#include "GUI_BenchMark.h"
GUI_BenchMark_t BenchMark;
#define TEST_CYCLE_NUM  10              /*How many times run the test (will calculate the average)*/
#define SHADOW_WIDTH    (LV_DPI / 8)
#define OPACITY         LV_OPA_60
static void bench_close_action_animation_cb(lv_obj_t * obj)
{
  lv_obj_del(obj);
	obj = 0;
}
static lv_res_t bench_close_action_animation(lv_obj_t * btn)
{
    lv_obj_t * win = lv_win_get_from_btn(btn);
    /* Float in the button using a built-in function
    * Delay the animation with 50 ms and float in 300 ms. NULL means no end callback*/
    lv_obj_animate(win, LV_ANIM_FLOAT_TOP | LV_ANIM_OUT, 300, 50, bench_close_action_animation_cb);
    return LV_RES_OK;
}
/**********************

 *  STATIC PROTOTYPES

 **********************/
static void refr_monitor(uint32_t time_ms, uint32_t px_num);
static lv_res_t run_test_click(lv_obj_t * btn);
/**********************

 *  STATIC VARIABLES

 **********************/
static uint32_t time_sum;
static uint32_t refr_cnt;
static uint32_t cpu_consume;
void GUI_BenchMark_Init(void)
{
	//benchmark窗口
	BenchMark.winbenchmark = lv_win_create(lv_scr_act(), NULL);
	lv_obj_set_size(BenchMark.winbenchmark,LV_HOR_RES,LV_VER_RES);
	lv_win_set_layout(BenchMark.winbenchmark, LV_LAYOUT_PRETTY); //无布局
	lv_win_set_title(BenchMark.winbenchmark,"Benchmark");
	lv_win_set_btn_size(BenchMark.winbenchmark,40);
	lv_win_set_sb_mode(BenchMark.winbenchmark, LV_SB_MODE_AUTO);
	lv_win_add_btn(BenchMark.winbenchmark,SYMBOL_CLOSE,bench_close_action_animation);
	lv_obj_align(BenchMark.winbenchmark,lv_scr_act(),LV_ALIGN_CENTER,0,0);
	lv_obj_set_opa_scale_enable(BenchMark.winbenchmark,true);
	lv_obj_set_opa_scale(BenchMark.winbenchmark,0xF0);
	
	/*Create a label to show the test result*/
	BenchMark.result_label = lv_label_create(BenchMark.winbenchmark, NULL);
	lv_label_set_text(BenchMark.result_label, "Run the test");
	lv_label_set_body_draw(BenchMark.result_label, true);
	lv_label_set_style(BenchMark.result_label, &lv_style_pretty);

	/*Create a "Run test" button*/
	lv_obj_t * btn;
	btn = lv_btn_create(BenchMark.winbenchmark, NULL);
	lv_page_glue_obj(btn, true);
	lv_cont_set_fit(btn, true, true);
	lv_btn_set_action(btn, LV_BTN_ACTION_CLICK, run_test_click);
	lv_obj_set_protect(btn, LV_PROTECT_FOLLOW);     /*Line break in layout*/
	lv_obj_t *btnlabel;
	btnlabel = lv_label_create(btn, NULL);
	lv_label_set_text(btnlabel, "Run\ntest!");
	//下拉效果动画
	/* Float in the button using a built-in function
	 * Delay the animation with 50 ms and float in 300 ms. NULL means no end callback*/
	lv_obj_animate(BenchMark.winbenchmark, LV_ANIM_FLOAT_TOP | LV_ANIM_IN, 300, 50, NULL);
}
void benchmark_start(void)
{
    lv_refr_set_monitor_cb(refr_monitor);
    lv_obj_invalidate(lv_scr_act());
    time_sum = 0;
    refr_cnt = 0;
		cpu_consume = 0;
}
bool benchmark_is_ready(void)
{
    if(refr_cnt == TEST_CYCLE_NUM) return true;
    else return false;
}
uint32_t benchmark_get_refr_time(void)
{
    if(benchmark_is_ready()) return time_sum / TEST_CYCLE_NUM;
    else return 0;
}
/*--------------------

 * OTHER FUNCTIONS

 ---------------------*/
/**

 * Called when a the library finished rendering to monitor its performance

 * @param time_ms time of rendering in milliseconds

 * @param px_num Number of pixels drawn

 */
#include <stdio.h>
static void refr_monitor(uint32_t time_ms, uint32_t px_num)
{
    (void) px_num   ; /*Unused*/
		extern volatile uint8_t   StaticsCPUUsage;
    time_sum += time_ms;
		cpu_consume += StaticsCPUUsage;
    refr_cnt ++;
    lv_obj_invalidate(lv_scr_act());
    if(refr_cnt >= TEST_CYCLE_NUM) {
        float time_avg = (float)time_sum / (float)TEST_CYCLE_NUM;
				float cpu_avg = (float)cpu_consume / (float)TEST_CYCLE_NUM;
        char buf[256];
				if(time_sum != 0) 
					sprintf(buf, "Screen load: %0.1f ms\nCPU Consume: %0.1f%%\nAverage of %d", time_avg,cpu_avg,TEST_CYCLE_NUM);
        lv_label_set_text(BenchMark.result_label, buf);
        lv_refr_set_monitor_cb(NULL);
    } else {
        char buf[256];
        usprintf(buf, "Running %d/%d", refr_cnt, TEST_CYCLE_NUM);
        lv_label_set_text(BenchMark.result_label, buf);
    }
}
/**

 * Called when the "Run test" button is clicked

 * @param btn pointer to the button

 * @return LV_RES_OK because the button is not deleted in the function

 */

static lv_res_t run_test_click(lv_obj_t * btn)
{
    (void) btn; /*Unused*/
    benchmark_start();
    return LV_RES_OK;
}
