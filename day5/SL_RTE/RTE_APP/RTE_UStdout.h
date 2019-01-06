#ifndef __UARTSTDIO_H__
#define __UARTSTDIO_H__
#include "RTE_Include.h"
//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif
typedef struct
{
	void (*PutsFunc)(const char *,uint16_t);
}RTE_Stdout_t;
extern void RTE_Reg_Puts(void (*PutsFunc)(const char *,uint16_t));
extern void RTE_Printf(const char *pcString, ...);
extern void RTE_Vprintf(const char *pcString, va_list vaArgP);
#if RTE_USE_OS == 1
extern osMutexId_t MutexIDStdio; 
#endif
//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __UARTSTDIO_H__
