#include "rak_wifi_os_port.h"
//��������ӿ�
const osThreadAttr_t RAKWifiThreadControl = {
  .stack_size =  1024, 
	.priority = osPriorityHigh,
};
void* rw_creat_task(RW_OS_TASK_PTR p_task)
{	
  osThreadId_t p_tcb;
  p_tcb = osThreadNew(p_task, NULL, &RAKWifiThreadControl); 
  return p_tcb;
}
//ɾ������ӿ�
int rw_del_task(void* p_tcb)
{
  osThreadTerminate(p_tcb);
  return RW_OS_OK;
}
//����һ��������
static osMutexAttr_t MutexIDRAKAttr = {
	NULL,     // human readable mutex name
	osMutexRecursive,//osMutexRecursive  ,    					// attr_bits
	NULL,                // memory for control block   
	0U                   // size for control block
};
//����һ��������
void* rw_creat_mutex(void)
{
  osMutexId_t p_mutex;
  p_mutex = osMutexNew(&MutexIDRAKAttr);
  return (void *)p_mutex;	
}
//�ͷ�һ��������
int rw_del_mutex(void* p_mutex)
{
  osMutexDelete(p_mutex);
  return RW_OS_OK;
}
//��ȡ��
int rw_lock_mutex(void* p_mutex, uint32_t timeout)
{
  if (timeout ==0) {  //wait forever
    timeout = osWaitForever;
  }
  osMutexAcquire(p_mutex,timeout);
  return RW_OS_OK;
}
//����
int rw_unlock_mutex(void* p_mutex)
{
  osMutexRelease(p_mutex);
  return RW_OS_OK;
}
//����һ���ź���
void* rw_creat_sem(void)
{
  osSemaphoreId_t p_sem;
  p_sem = osSemaphoreNew(1,1,NULL);
  return p_sem;
}
//ɾ��һ���ź���
int rw_del_sem(void* p_sem)
{
  osSemaphoreDelete(p_sem);
  return RW_OS_OK;
}
//����һ���ź���
int rw_post_sem(void* p_sem)
{
  osSemaphoreRelease(p_sem);
  return RW_OS_OK;
}
//�ȴ�һ���ź���
int rw_pend_sem(void* p_sem, uint32_t timeout)
{
   int32_t oserr;
    
   if (timeout ==0) {  //wait forever
      timeout = osWaitForever;
    }
    oserr =osSemaphoreAcquire(p_sem,timeout);  
    if (oserr == osOK) {
        return RW_OS_OK;     
    } else if (oserr == osErrorTimeout||oserr == osError) {
        return RW_OS_TIME_OUT;
    }
    return RW_OS_ERROR;
}
void rw_enter_critical(void)
{
	
}
void rw_exit_critical(void)
{
	
}
//����һ���ź���
int rw_post_drv_sem(void* p_sem)
{
  osSemaphoreRelease(p_sem);
  return RW_OS_OK;
}


