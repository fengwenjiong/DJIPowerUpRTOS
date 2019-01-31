/**
  ******************************************************************************
  * @file    RTE_LOG.h
  * @author  Shan Lei ->>lurenjia.tech ->>https://github.com/sudashannon
  * @brief   RTE��־ģ�飬�Բ�ͬ�ķ�ʽ����������������
  * @version V1.1 2019/01/08
	* @History V1.0 ��������ֲ��lvgl
	           V1.1 �޸�ʹ������RTE����
  ******************************************************************************
  */
#include "RTE_LOG.h"
#if RTE_USE_LOG == 1
/**
 * Add a log
 * @param level the level of log. (From `lv_log_level_t` enum)
 * @param file name of the file when the log added
 * @param line line number in the source code where the log added
 * @param dsc description of the log
 */
void RTE_LOG_ADD(RTE_LOG_Level_t level, const char * module ,const char * file, uint32_t line, const char * dsc, ...)
{
	if(level >= _RTE_LOG_LEVEL_NUM) return;      /*Invalid level*/
	static const char * lvl_prefix[] = {"Trace", "Info", "Warn", "Error"};
	RTE_Printf("%10s    <%s>(%s #%d):",module,lvl_prefix[level], file, line);
	va_list vaArgP;
	// Start the varargs processing.
	va_start(vaArgP, dsc);
	RTE_Vprintf(dsc, vaArgP);
	// We're finished with the varargs now.
	va_end(vaArgP);
	RTE_Printf("\r\n");
}
#endif
