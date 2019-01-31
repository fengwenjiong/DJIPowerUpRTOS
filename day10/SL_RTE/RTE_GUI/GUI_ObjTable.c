#include "GUI_Manage.h"
const GUI_str2objTable_t str2objTable[GUI_OBJ_CNT] =
{
    {
        .name = "arc",
        .type = GUI_ARC,
        .design = lv_arc_create,
    },
    {
        .name = "bar",
        .type = GUI_BAR,
        .design = lv_bar_create,
    },
    {
        .name = "btn",
        .type = GUI_BTN,
        .design = lv_btn_create,
    },
    {
        .name = "btnm",
        .type = GUI_BTNM,
        .design = lv_btnm_create,
    },
    {
        .name = "calendar",
        .type = GUI_CALENDAR,
        .design = lv_calendar_create,
    },
    {
        .name = "cb",
        .type = GUI_CB,
        .design = lv_cb_create,
    },
    {
        .name = "chart",
        .type = GUI_CHART,
        .design = lv_chart_create,
    },
    {
        .name = "cont",
        .type = GUI_CONT,
        .design = lv_cont_create,
    },
    {
        .name = "ddlist",
        .type = GUI_DDLIST,
        .design = lv_ddlist_create,
    },
    {
        .name = "gauge",
        .type = GUI_GAUGE,
        .design = lv_gauge_create,
    },
    {
        .name = "img",
        .type = GUI_IMG,
        .design = lv_img_create,
    },
    {
        .name = "imgbtn",
        .type = GUI_IMGBTN,
        .design = lv_imgbtn_create,
    },
    {
        .name = "kb",
        .type = GUI_KB,
        .design = lv_kb_create,
    },
    {
        .name = "label",
        .type = GUI_LABEL,
        .design = lv_label_create,
    },
    {
        .name = "led",
        .type = GUI_LED,
        .design = lv_led_create,
    },
    {
        .name = "line",
        .type = GUI_LINE,
        .design = lv_line_create,
    },
    {
        .name = "list",
        .type = GUI_LIST,
        .design = lv_list_create,
    },
    {
        .name = "lmeter",
        .type = GUI_LMETER,
        .design = lv_lmeter_create,
    },
    {
        .name = "mbox",
        .type = GUI_MBOX,
        .design = lv_mbox_create,
    },
    {
        .name = "page",
        .type = GUI_PAGE,
        .design = lv_page_create,
    },
    {
        .name = "preload",
        .type = GUI_PRELOAD,
        .design = lv_preload_create,
    },
    {
        .name = "roller",
        .type = GUI_ROLLER,
        .design = lv_roller_create,
    },
    {
        .name = "slider",
        .type = GUI_SLIDER,
        .design = lv_slider_create,
    },
    {
        .name = "sw",
        .type = GUI_SW,
        .design = lv_sw_create,
    },
    {
        .name = "ta",
        .type = GUI_TA,
        .design = lv_ta_create,
    },
    {
        .name = "tabview",
        .type = GUI_TABVIEW,
        .design = lv_tabview_create,
    },
    {
        .name = "win",
        .type = GUI_WIN,
        .design = lv_win_create,
    },
};

