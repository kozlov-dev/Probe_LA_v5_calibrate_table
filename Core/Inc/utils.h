#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus
 extern "C" {
#endif

/******************************************************************************/
/* Подключение заголовочных файлов используемых модулей */
#define DWT_CONTROL *(volatile unsigned long *)0xE0001000
#define SCB_DEMCR   *(volatile unsigned long *)0xE000EDFC

/******************************************************************************/
/* inline func */
__STATIC_INLINE void DWT_Init(void)
{
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // разрешаем использовать счётчик
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;   // запускаем счётчик
}

__STATIC_INLINE void delay_us(uint32_t us)
{
	uint32_t us_count_tic =  us * (SystemCoreClock / 1000000U);
	DWT->CYCCNT = 0U;
	while(DWT->CYCCNT < us_count_tic);
}
//--------------------------------------------------------------------------
// SWO

//#include "stdio.h"
//
//int _write(int32_t file, uint8_t *ptr, int32_t len)
//{
//	for (int i = 0; i < len; i++)
//	{
//		ITM_SendChar(*ptr++);
//	}
//	return len;
//}


#ifdef __cplusplus
}
#endif
#endif //UTILS_H
