/**
 * @file lvgl.h
 * Include all LittleV GL related headers
 */

#ifndef LVGL_H
#define LVGL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "RTE_Include.h"
#define lv_log_add(level, file, line, dsc) {;}
#define LV_LOG_TRACE(dsc) {;}
#define LV_LOG_INFO(dsc) {;}
#define LV_LOG_WARN(dsc) {;}
#define LV_LOG_ERROR(dsc) {;}

#define lv_tick_get RTE_RoundRobin_GetTick
#define lv_tick_elaps RTE_RoundRobin_TickElaps
#define lv_mem_alloc(size) RTE_MEM_Alloc(MEM_EX,size)
#define lv_mem_free(data) RTE_MEM_Free(MEM_EX,data)
#define lv_mem_assert RTE_AssertParam
#define lv_mem_realloc(data,size) RTE_MEM_Realloc(MEM_EX,data,size)
#define lv_mem_get_size RTE_MEM_GetDataSize
/*Test misc. module version*/

#include "lv_misc/lv_anim.h"
#include "lv_misc/lv_area.h"
#include "lv_misc/lv_circ.h"
#include "lv_misc/lv_color.h"
#include "lv_misc/lv_font.h"
#include "lv_misc/lv_ll.h"
#include "lv_misc/lv_math.h"
#include "lv_misc/lv_symbol_def.h"
#include "lv_misc/lv_txt.h"
	
#include "lv_draw/lv_draw.h"
#include "lv_draw/lv_draw_arc.h"
#include "lv_draw/lv_draw_img.h"
#include "lv_draw/lv_draw_label.h"
#include "lv_draw/lv_draw_line.h"
#include "lv_draw/lv_draw_rbasic.h"
#include "lv_draw/lv_draw_rect.h"
#include "lv_draw/lv_draw_triangle.h"
#include "lv_draw/lv_draw_vbasic.h"


#include "lv_hal/lv_hal_disp.h"
#include "lv_hal/lv_hal_indev.h"


#include "lv_core/lv_indev.h"
#include "lv_core/lv_group.h"
#include "lv_core/lv_obj.h"
#include "lv_core/lv_refr.h"
#include "lv_core/lv_style.h"
#include "lv_core/lv_vdb.h"

#include "lv_themes/lv_theme.h"

#include "lv_objx/lv_btn.h"
#include "lv_objx/lv_imgbtn.h"
#include "lv_objx/lv_img.h"
#include "lv_objx/lv_label.h"
#include "lv_objx/lv_line.h"
#include "lv_objx/lv_page.h"
#include "lv_objx/lv_cont.h"
#include "lv_objx/lv_list.h"
#include "lv_objx/lv_chart.h"
#include "lv_objx/lv_cb.h"
#include "lv_objx/lv_bar.h"
#include "lv_objx/lv_slider.h"
#include "lv_objx/lv_led.h"
#include "lv_objx/lv_btnm.h"
#include "lv_objx/lv_kb.h"
#include "lv_objx/lv_ddlist.h"
#include "lv_objx/lv_roller.h"
#include "lv_objx/lv_ta.h"
#include "lv_objx/lv_win.h"
#include "lv_objx/lv_tabview.h"
#include "lv_objx/lv_mbox.h"
#include "lv_objx/lv_gauge.h"
#include "lv_objx/lv_lmeter.h"
#include "lv_objx/lv_sw.h"
#include "lv_objx/lv_kb.h"
#include "lv_objx/lv_arc.h"
#include "lv_objx/lv_preload.h"
#include "lv_objx/lv_calendar.h"

/*********************
 *      DEFINES
 *********************/
/*Current version of LittlevGL*/
#define LVGL_VERSION_MAJOR   5
#define LVGL_VERSION_MINOR   2
#define LVGL_VERSION_PATCH   1
#define LVGL_VERSION_INFO    "dev"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
}
#endif

#endif /*LVGL_H*/
