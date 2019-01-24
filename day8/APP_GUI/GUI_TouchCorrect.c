#include "GUI_TouchCorrect.h"
#include "Board_LCD.h"
#include "GUI_OverAll.h"
/*********************
 *      DEFINES
 *********************/
#define CIRCLE_SIZE      LV_DPI/8
#define CIRCLE_OFFSET    LV_DPI/8
#define TOUCH_NUMBER     3
/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    TP_CAL_STATE_INIT,
    TP_CAL_STATE_WAIT_TOP_LEFT,
    TP_CAL_STATE_WAIT_BOTTOM_RIGHT,
    TP_CAL_STATE_WAIT_LEAVE,
    TP_CAL_STATE_READY,
} tp_cal_state_t;
/**********************
 *  STATIC PROTOTYPES
 **********************/
static void get_avr_value(lv_point_t * p);
static lv_res_t btn_click_action(lv_obj_t * scr);
/**********************
 *  STATIC VARIABLES
 **********************/
static int16_t  A1, A2, B1, B2 = 0;
static lv_point_t point[2]; /*Calibration points: [0]: top-left; [1]: top-right, [2]: bottom-right, [3]: bottom-left */
static lv_point_t avr[TOUCH_NUMBER]; /*Storage point to calculate average*/

static tp_cal_state_t state;
static lv_obj_t * prev_scr;
static lv_obj_t * big_btn;
static lv_obj_t * label_main;
static lv_obj_t * circ_area;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
/**
 * Create a touch pad calibration screen
 */
void GUI_TouchCorrect_create(void)
{
	state = TP_CAL_STATE_INIT;
	prev_scr = lv_scr_act();
	lv_obj_t * scr = lv_obj_create(NULL, NULL);
	lv_obj_set_size(scr, LV_HOR_RES, LV_VER_RES);
	lv_scr_load(scr);
	/*Create a big transparent button screen to receive clicks*/
	big_btn = lv_btn_create(lv_scr_act(), NULL);
	lv_obj_set_size(big_btn, LV_HOR_RES, LV_VER_RES);
	lv_btn_set_style(big_btn, LV_BTN_STYLE_REL, &lv_style_transp);
	lv_btn_set_style(big_btn, LV_BTN_STYLE_PR, &lv_style_transp);
	lv_btn_set_action(big_btn, LV_BTN_ACTION_CLICK, btn_click_action);
	lv_btn_set_layout(big_btn, LV_LAYOUT_OFF);

	label_main = lv_label_create(lv_scr_act(), NULL);
	char buf[64];
	usprintf(buf, "Click the circle in\n"
					"upper left-hand corner\n"
					"%u left", TOUCH_NUMBER);
	lv_label_set_text(label_main, buf);
	lv_label_set_align(label_main, LV_LABEL_ALIGN_CENTER);
	
	lv_obj_set_pos(label_main, (LV_HOR_RES - lv_obj_get_width(label_main)) / 2,
								 (LV_VER_RES - lv_obj_get_height(label_main)) / 2);

	static lv_style_t style_circ;
	lv_style_copy(&style_circ, &lv_style_pretty_color);
	style_circ.body.radius = LV_RADIUS_CIRCLE;

	circ_area = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_size(circ_area, CIRCLE_SIZE, CIRCLE_SIZE);
	lv_obj_set_style(circ_area, &style_circ);
	lv_obj_set_click(circ_area, false);

#if USE_LV_ANIMATION
	lv_anim_t a;
	a.var = circ_area;
	a.start = LV_HOR_RES / 2;
	a.end = CIRCLE_OFFSET;
	a.fp = (lv_anim_fp_t)lv_obj_set_x;
	a.path = lv_anim_path_linear;
	a.end_cb = NULL;
	a.act_time = -500;
	a.time = 200;
	a.playback = 0;
	a.playback_pause = 0;
	a.repeat = 0;
	a.repeat_pause = 0;
	lv_anim_create(&a);

	a.start = LV_VER_RES / 2;
	a.end = CIRCLE_OFFSET;
	a.fp = (lv_anim_fp_t)lv_obj_set_y;
	a.end_cb = NULL;
	a.time = 200;
	lv_anim_create(&a);
#else
	lv_obj_set_pos(circ_area, CIRCLE_OFFSET, CIRCLE_OFFSET);
#endif
	state = TP_CAL_STATE_WAIT_TOP_LEFT;
}
/**********************
 *   STATIC FUNCTIONS
 **********************/
static void get_avr_value(lv_point_t * p)
{
	int32_t x_sum = 0;
	int32_t y_sum = 0;
	uint8_t i = 0;
	for(; i < TOUCH_NUMBER ; i++) {
		x_sum += avr[i].x;
		y_sum += avr[i].y;
	}
	p->x = x_sum / TOUCH_NUMBER;
	p->y = y_sum / TOUCH_NUMBER;
}
static bool gui_input_get_calib(lv_indev_data_t *data)
{
	static int16_t last_x = 0;
	static int16_t last_y = 0;
	uint16_t x = 0;
	uint16_t y = 0;
	if(Board_LCD_TouchScan(&x,&y))
	{
		data->point.x = (((A1 * x) + B1)/1000);
		data->point.y = (((A2 * y) + B2)/1000);
		last_x = data->point.x;
		last_y = data->point.y;
		data->state = LV_INDEV_STATE_PR;			
	}
	else
	{
		data->point.x = last_x;
		data->point.y = last_y;
		data->state = LV_INDEV_STATE_REL;
	}
	return false;
}
static lv_res_t btn_click_action(lv_obj_t * scr)
{
	(void) scr;    /*Unused*/

	static uint8_t touch_nb = TOUCH_NUMBER;

	if(state == TP_CAL_STATE_WAIT_TOP_LEFT) {
			char buf[64];
			touch_nb--;
			lv_indev_t * indev = lv_indev_get_act();
			lv_indev_get_point(indev, &avr[touch_nb]);

			if(!touch_nb) {
					touch_nb = TOUCH_NUMBER;
					get_avr_value(&point[0]);
					usprintf(buf, "x: %d\ny: %d", point[0].x, point[0].y);
					lv_obj_t * label_coord = lv_label_create(lv_scr_act(), NULL);
					lv_label_set_text(label_coord, buf);
					usprintf(buf, "Click the circle in\n"
									"lower right-hand corner\n"
									" %u Left", TOUCH_NUMBER);
#if USE_LV_ANIMATION
					lv_anim_t a;
					a.var = circ_area;
					a.start = CIRCLE_OFFSET;
					a.end = LV_HOR_RES - CIRCLE_SIZE - CIRCLE_OFFSET;
					a.fp = (lv_anim_fp_t)lv_obj_set_x;
					a.path = lv_anim_path_linear;
					a.end_cb = NULL;
					a.act_time = 0;
					a.time = 200;
					a.playback = 0;
					a.playback_pause = 0;
					a.repeat = 0;
					a.repeat_pause = 0;
					lv_anim_create(&a);

					a.start = CIRCLE_OFFSET;
					a.end = LV_VER_RES - CIRCLE_SIZE - CIRCLE_OFFSET;
					a.fp = (lv_anim_fp_t)lv_obj_set_y;
					a.end_cb = NULL;
					a.time = 200;
					lv_anim_create(&a);
#else
					lv_obj_set_pos(circ_area, LV_HOR_RES - CIRCLE_SIZE - CIRCLE_OFFSET, LV_VER_RES - CIRCLE_SIZE - CIRCLE_OFFSET);
#endif
					state = TP_CAL_STATE_WAIT_BOTTOM_RIGHT;
			} else {
					usprintf(buf, "Click the circle in\n"
									"upper left-hand corner\n"
									" %u Left", touch_nb);
			}
			lv_label_set_text(label_main, buf);
			lv_obj_set_pos(label_main, (LV_HOR_RES - lv_obj_get_width(label_main)) / 2,
										 (LV_VER_RES - lv_obj_get_height(label_main)) / 2);

	} else if(state == TP_CAL_STATE_WAIT_BOTTOM_RIGHT) {
        char buf[64];
        touch_nb--;
        lv_indev_t * indev = lv_indev_get_act();
        lv_indev_get_point(indev, &avr[touch_nb]);

        if(!touch_nb) {
            touch_nb = TOUCH_NUMBER;
            get_avr_value(&point[1]);
            usprintf(buf, "x: %d\ny: %d", point[1].x, point[1].y);
            lv_obj_t * label_coord = lv_label_create(lv_scr_act(), NULL);
            lv_label_set_text(label_coord, buf);
            lv_obj_set_pos(label_coord, LV_HOR_RES - lv_obj_get_width(label_coord), LV_VER_RES - lv_obj_get_height(label_main));
            usprintf(buf, "Click the screen\n"
                    "to leave calibration");
            lv_obj_del(circ_area);
            state = TP_CAL_STATE_WAIT_LEAVE;
        } else {
            usprintf(buf, "Click the circle in\n"
                    "lower right-hand corner\n"
                    " %u Left", touch_nb);
        }
        lv_label_set_text(label_main, buf);
        lv_obj_set_pos(label_main, (LV_HOR_RES - lv_obj_get_width(label_main)) / 2,
                       (LV_VER_RES - lv_obj_get_height(label_main)) / 2);
	} else if(state == TP_CAL_STATE_WAIT_LEAVE) {
			lv_scr_load(prev_scr);

			/*
			 * TODO Process 'p' points here to calibrate the touch pad
			 * Offset will be: (CIRCLE_SIZE/2 + CIRCLE_OFFSET)
			 */
      A1 = (1000 * (LV_HOR_RES - 2*(CIRCLE_SIZE/2 + CIRCLE_OFFSET)))/ ( point[1].x - point[0].x);
      B1 = (1000 * (CIRCLE_SIZE/2 + CIRCLE_OFFSET)) - A1 * point[0].x;

      A2 = (1000 * (LV_VER_RES - 2*(CIRCLE_SIZE/2 + CIRCLE_OFFSET)))/ ( point[1].y - point[0].y);
      B2 = (1000 * (CIRCLE_SIZE/2 + CIRCLE_OFFSET)) - A2 * point[0].y;
		
      lv_indev_t *indev = lv_indev_get_act();
      indev->driver.read = gui_input_get_calib;
			state = TP_CAL_STATE_READY;
			GUI_OverAll_Init();
	} else if(state == TP_CAL_STATE_READY) {
	}
	return LV_RES_OK;
}

