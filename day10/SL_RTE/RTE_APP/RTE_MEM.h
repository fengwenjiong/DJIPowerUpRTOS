#ifndef __RTE_MEM_H
#define __RTE_MEM_H
#ifdef __cplusplus
extern "C" {
#endif
	#include "RTE_Config.h"
	#if RTE_USE_MEMMANAGE == 1
	#include <stdint.h>
	#include <string.h>
	#if RTE_USE_OS
	#include "cmsis_os2.h"
	#endif
	#if RTE_MEM_64BIT == 1
	#define MEM_UNIT uint64_t
	#else
	#define MEM_UNIT uint32_t
	#endif
	//--------------------------------------------------------------
	// MEMö��
	//--------------------------------------------------------------
	typedef enum
	{
		MEM_NULL = -1,
		MEM_RTE = 0,
		MEM_DTCM = 1,
		MEM_AXIM = 2,
		MEM_N,
	}RTE_MEM_Name_e;
	//--------------------------------------------------------------
	// MEMͷ�ṹ��
	//--------------------------------------------------------------
	typedef union {
    struct {
        MEM_UNIT used: 1;       //1: if the entry is used
        MEM_UNIT d_size: 31;    //Size off the data (1 means 4 bytes)
    };
    MEM_UNIT header;            //The header (used + d_size)
	} RTE_MEM_Header_t;
	//--------------------------------------------------------------
	// MEM��̬����ṹ��
	//--------------------------------------------------------------
	typedef struct {
		RTE_MEM_Header_t header;
		uint8_t first_data;        /*First data byte in the allocated data (Just for easily create a pointer)*/
	} RTE_MEM_Ent_t;
	//--------------------------------------------------------------
	// ���MEM����̬����ṹ��
	//--------------------------------------------------------------
	typedef struct
	{
		RTE_MEM_Name_e MemName;
	  uint8_t *work_mem;    //Work memory for allocation
		uint32_t totalsize;
#if RTE_USE_OS
		osMutexId_t MutexIDMEM;
#endif
	}RTE_MEM_t;
	//--------------------------------------------------------------
	// MEM��Ϣ�ṹ��
	//--------------------------------------------------------------
	typedef struct
	{
	  uint8_t used_pct;
    uint8_t frag_pct;
    uint32_t total_size;
    uint32_t free_cnt;
    uint32_t free_size;
    uint32_t free_biggest_size;
    uint32_t used_cnt;
	}RTE_MEM_Monitor_t;

	extern void RTE_MEM_Pool(RTE_MEM_Name_e mem_name,void *buf,uint32_t len);
	extern void *RTE_MEM_Alloc(RTE_MEM_Name_e mem_name,uint32_t size);
	extern void *RTE_MEM_Alloc0(RTE_MEM_Name_e mem_name,uint32_t size);
	extern void RTE_MEM_Free(RTE_MEM_Name_e mem_name,const void * data);
	extern void *RTE_MEM_Realloc(RTE_MEM_Name_e mem_name,void *data_p,uint32_t new_size);
	extern void RTE_MEM_Defrag(RTE_MEM_Name_e mem_name);
	extern void RTE_MEM_Monitor(RTE_MEM_Name_e mem_name,RTE_MEM_Monitor_t * mon_p);
	extern uint32_t RTE_MEM_GetDataSize(const void * data);
	extern uint32_t RTE_MEM_MaxFree(RTE_MEM_Name_e mem_name);
	#endif
#ifdef __cplusplus
}
#endif
#endif
