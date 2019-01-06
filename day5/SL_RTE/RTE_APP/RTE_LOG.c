/**
  ******************************************************************************
  * @file    RTE_LOG.h
  * @author  Shan Lei ->>lurenjia.tech ->>https://github.com/sudashannon
  * @brief   RTE日志模块，以不同的方式输出程序运行情况。
  * @version V1.0 2018/11/02 第一版
	* @History V1.0 创建，移植自lvgl
  ******************************************************************************
  */
#include "RTE_LOG.h"
#if RTE_USE_LOG == 1
#define LOG_DEBUG_STR "[LOG]"
/**
 * Add a log
 * @param level the level of log. (From `lv_log_level_t` enum)
 * @param file name of the file when the log added
 * @param line line number in the source code where the log added
 * @param dsc description of the log
 */
void RTE_LOG_ADD(RTE_LOG_Level_t level, const char * file, uint32_t line, const char * dsc)
{
	if(level >= _RTE_LOG_LEVEL_NUM) return;      /*Invalid level*/
	static const char * log_level[] = {"Trace", "Info", "Warn", "Error"};
	RTE_Printf("%10s    %s: %s \t(%s #%d)\r\n", LOG_DEBUG_STR,log_level[level], dsc,  file, line);
}
#endif
