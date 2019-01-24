#ifndef __THREAD_WIFI_H
#define __THREAD_WIFI_H
#include "RTE_Include.h"
extern osThreadId_t ThreadIDWIFI;
extern osEventFlagsId_t EvtIDWIFI;
extern const osThreadAttr_t WIFIThreadControl;
extern __NO_RETURN void ThreadTaskWIFI (void *argument);
#endif
