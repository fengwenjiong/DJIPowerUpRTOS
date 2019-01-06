#ifndef __THREAD_SYSTEM_H
#define __THREAD_SYSTEM_H
#include "RTE_Include.h"
extern osThreadId_t ThreadIDSYS;
extern const osThreadAttr_t SystemThreadControl;
extern __NO_RETURN void ThreadTaskSYS (void *argument);
#endif
