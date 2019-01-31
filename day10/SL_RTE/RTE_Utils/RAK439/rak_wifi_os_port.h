#ifndef __RAW_WIFI_OS_PORT_H
#define __RAW_WIFI_OS_PORT_H
#include "RTE_Include.h"
#include <stdint.h>

#define RW_OS_OK             0
#define RW_OS_ERROR         -1
#define RW_OS_TIME_OUT      -2
typedef  void (*RW_OS_TASK_PTR)(void *p_arg);
void* rw_creat_task(RW_OS_TASK_PTR p_task);
int rw_del_task(void* p_tcb);
void* rw_creat_mutex(void);
int rw_del_mutex(void* p_mutex);
int rw_lock_mutex(void* p_mutex, uint32_t timeout);
int rw_unlock_mutex(void* p_mutex);
void* rw_creat_sem(void);
int rw_del_sem(void* p_sem);
int rw_post_sem(void* p_sem);
int rw_pend_sem(void* p_sem, uint32_t timeout);
#endif
