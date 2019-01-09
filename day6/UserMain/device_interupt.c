#include "stm32h7xx_hal.h"
#include "stm32h7xx.h"
#include "stm32h7xx_it.h"
#include "RTE_Include.h"
/**
* @brief This function handles Non maskable interrupt.
*/
void NMI_Handler(void)
{

}

/**
* @brief This function handles Hard fault interrupt.
*/
void HardFault_Handler(void)
{
  while (1)
  {

  }
}

/**
* @brief This function handles Memory management fault.
*/
void MemManage_Handler(void)
{
  while (1)
  {

  }
}

/**
* @brief This function handles Pre-fetch fault, memory access fault.
*/
void BusFault_Handler(void)
{
  while (1)
  {

  }
}

/**
* @brief This function handles Undefined instruction or illegal state.
*/
void UsageFault_Handler(void)
{
  while (1)
  {

  }

}
#if RTE_USE_OS == 0
/**
* @brief This function handles System service call via SWI instruction.
*/
void SVC_Handler(void)
{

}
#endif
/**
* @brief This function handles Debug monitor.
*/
void DebugMon_Handler(void)
{

}
#if RTE_USE_OS == 0
/**
* @brief This function handles Pendable request for system service.
*/
void PendSV_Handler(void)
{

}
#endif
