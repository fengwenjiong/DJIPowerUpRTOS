#ifndef __GUI_BENCHMARK_H
#define __GUI_BENCHMARK_H
#include "RTE_Include.h"
typedef struct
{
	lv_obj_t * winbenchmark;
	lv_obj_t * result_label;
}GUI_BenchMark_t;
extern GUI_BenchMark_t BenchMark;
extern void GUI_BenchMark_Init(void);



#endif
