#ifndef __THREAD_GUI_H
#define __THREAD_GUI_H
#include "RTE_Include.h"
extern osThreadId_t ThreadIDGUI;
extern const osThreadAttr_t GUIThreadControl;
extern __NO_RETURN void ThreadTaskGUI (void *argument);
#endif
