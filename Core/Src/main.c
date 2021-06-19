/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/*
--------------------------------------------------------------------------
BUG	: В цикле не прерывно идет установка цапов! Формируя цифровой шум. DONE: убарно из цикла
BUG	: Ошибка в работе цап. При переключение в m27 цап принимает значения, но не устанавливет их! DONE: добавлена в библеотеку цап двоойна отправка команды, проблема устранена.
BUG : set 4v-> set 5v -> M27 -> set 4v -> M12 -> set 6v (err:DAC не установил значения, но в ядре значения имеются!!!) DONE: изза ошибки работы ЦАП --> добалена функция отправки значений при устанвоке реле
BUG : Доработать алгоритм установки реле! м.б. добавить в функции реле установку ЦАП по обоим каналам? DONE: Добавлено.
DONE: Добавить серийный номер в щуп
DONE: Добавить калибровочную таблицу в проект через t.py формируется - необходимо заполнять значениями в logic_calibration_table.с
DONE: получить значение V , Поиск по значению напряжения V значение в калибровочной таблице VDAC
--------------------------------------------------------------------------

--> NOW
--------------------------------------------------------------------------
FIXME:Запись в память не работает
TODO: Реализовать прием калибровочной таблицы. 0xOA:Прием калибровочной таблицы [0x0A][1-4][offset][count][data]
TODO: Запись во флеш принятой новой калибровочной таблицы. [0x0D] data: 1B (0x00 - успешно; 0x01 - ошибка при записи)
FIXME:Отправлять длину массива кратно 32b. не работает CRC --> HardFault, 
TODO: Реализовать отправку CRC 1-4 таблицы [0x0B][1-4][CRC(1-4)]
TODO: Установка цап реализованно только для канала A и режима m12. Нужно переписать с учетом режима работы. режим работы определяет какую таблицу использовать.	 
--------------------------------------------------------------------------
TODO: данная реализация плохо отрабатывает! TODO: Нужно переделать на EXTI+TIM
TODO: Проверить первое состоянеи первоначальное состояние реле 27V | RelayState =|1:m12|0:m27|
--------------------------------------------------------------------------
FUTURE: Сформировать калиброчную таблицу через функцию. При остусвтие значения в таблице произвести интерполяцию
--------------------------------------------------------------------------
*/



// Тестовые сборки
#define DEBUG_SWO 			1
#define TEST_UID 			1

#define TEST_DAC 			1
#define TEST_READ_BTN 		1 	//TODO: данная реализация плохо отрабатывает! TODO: Нужно переделать на EXTI+TIM
#define TEST_TIM_CAPTURE 	1
#define TEST_ADC 			1
#define TEST_USB 			1
#define USB_RESET           1
#define TEST_RELAY 			1
#define DWT_INIT 			1

#define TEST_FLASH_TABLE 	1	//FIXME: Запись в память не работает

//--------------------------------------------------------------------------


/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "DAC_AD5322.h"
#include "logic_calibration_table.h"
#include "usbd_cdc_if.h"
#include "string.h"                 // для функции strlen()
#include "stdbool.h"
#include "flash.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */



/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

CRC_HandleTypeDef hcrc;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

/* USER CODE BEGIN PV */
//--------------------------------------------------------------------------
#if USB_RESET
void USB_Reset(void);
#endif /* USB_RESET */
	//--------------------------------------------------------------------------
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_CRC_Init(void);
/* USER CODE BEGIN PFP */
//**************************************************************************
#if DEBUG_SWO
int _write(int32_t file, uint8_t *ptr, int32_t len)
{
	for (int i = 0; i < len; i++)
	{
		ITM_SendChar(*ptr++);
	}
	return len;
}

#endif /* DEBUG_SWO */
//**************************************************************************
#if TEST_UID
// Чтение UID контроллера
//#define UID_BASE 0x1FFFF7E8
uint16_t *idBase0 = (uint16_t *)(UID_BASE);
uint16_t *idBase1 = (uint16_t *)(UID_BASE + 0x02);
uint32_t *idBase2 = (uint32_t *)(UID_BASE + 0x04);
uint32_t *idBase3 = (uint32_t *)(UID_BASE + 0x08);
char buffer[64] = {
	0,
};
#endif /* TEST_UID */
//**************************************************************************
#if TEST_FLASH_TABLE/*
FLASH_EraseInitTypeDef EraseInitStruct = {  // структура для очистки флеша
        .TypeErase      = FLASH_TYPEERASE_PAGES,
        .PageAddress    = FLASH_TABLE_START_ADDR,
        .NbPages        = 1
};*/

union NVRAM DevNVRAM;
bool changeTableFlag = false;   // TODO тестовый флаг для записи в while
//--------------------------------------------------------------------------
/*
void writeTableInFlash()        // FIXME:Запись в память не работает
{
	volatile uint32_t l_Address = FLASH_TABLE_START_ADDR;
	uint32_t l_Error = 0;
	uint32_t l_Index = 0;
	//Читаем и сравниваем
	while (l_Address < FLASH_TABLE_STOP_ADDR) {
		if ( DevNVRAM.data32[l_Index] != *(uint32_t *)l_Address )
			l_Error++;
		l_Index = l_Index + 1;
		l_Address = l_Address + 4;
	}
	// конфигурация изменилась сохраняем
	printf("Ошибка чтения таблицы :%lu", l_Error);
	if (l_Error > 0) {
		// конфигурация изменилась сохраняем
		// Готовим к записи в память
		HAL_FLASH_Unlock();
		// Очищаем страницу памяти
		if ( HAL_FLASHEx_Erase(&EraseInitStruct, &l_Error) != HAL_OK )
		{
		    printf("ERASE error");
		}
		//Пишем данные в память
		l_Address = FLASH_TABLE_START_ADDR;
		l_Error = 0x00;
		l_Index = 0x00;
		DevNVRAM.sector.NWrite = DevNVRAM.sector.NWrite + 1;
		DevNVRAM.sector.CheckSum = 0; //HAL_CRC_Calculate(&hcrc, &DevNVRAM.calibration_table, (sizeof(DevNVRAM.calibration_table)/4));
		while (l_Address < FLASH_TABLE_STOP_ADDR) {
			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, l_Address,
			        DevNVRAM.data32[l_Index]) != HAL_OK)
			    l_Error++;
			l_Address = l_Address + 4;
			l_Index = l_Index + 1;
			HAL_Delay(10);
		}
		HAL_FLASH_Lock();
	}
//	HAL_Delay(100);
}
*/
//--------------------------------------------------------------------------
//FIXME:Отправлять длину массива кратно 32b. не работает CRC --> HardFault, 
uint32_t getCRC_table_a_m12()	
{
	//	uint16_t len_ = sizeof(aqrr)/(sizeof(uint32_t)*2);
//	uint32_t crc = HAL_CRC_Calculate(&hcrc, (uint32_t*)DevNVRAM.calibration_table.dacValA_m12, sizeof(DevNVRAM.calibration_table.dacValA_m12) / (sizeof(uint32_t) * 2));
    uint32_t crc = HAL_CRC_Calculate(&hcrc, 0x00, 1);
//    for (uint8_t i = 0; i < MAX_VAL_M12; ++i)
//	{
//	    crc = HAL_CRC_Accumulate(&hcrc, DevNVRAM.calibration_table.dacValA_m12[i], 1);
//	}
//	HAL_Delay(1);
	return crc;
}
uint32_t getCRC_table_b_m12()
{
	//	uint16_t len_ = sizeof(aqrr)/(sizeof(uint32_t)*2);
	return HAL_CRC_Calculate(&hcrc, (uint32_t*)DevNVRAM.calibration_table.dacValB_m12, sizeof(DevNVRAM.calibration_table.dacValB_m12) / (sizeof(uint32_t) * 2));
}
uint32_t getCRC_table_a_m27()
{
	//	uint16_t len_ = sizeof(aqrr)/(sizeof(uint32_t)*2);
	return HAL_CRC_Calculate(&hcrc, (uint32_t*)DevNVRAM.calibration_table.dacValA_m27, sizeof(DevNVRAM.calibration_table.dacValA_m27) / (sizeof(uint32_t) * 2));
}
uint32_t getCRC_table_b_m27()
{
	//	uint16_t len_ = sizeof(aqrr)/(sizeof(uint32_t)*2);
	return HAL_CRC_Calculate(&hcrc, (uint32_t*)DevNVRAM.calibration_table.dacValB_m27, sizeof(DevNVRAM.calibration_table.dacValB_m27) / (sizeof(uint32_t) * 2));
}
//--------------------------------------------------------------------------
#endif /* TEST_FLASH_TABLE */
//**************************************************************************
#if TEST_DAC
uint16_t VDAC_A = 0;
uint16_t VDAC_B = 0;
// Старая реализация. для приема dgt значений цап.
void SetDacA(uint16_t da) {
	VDAC_A = da;
	DAC_AD5322_Ch2(&hspi1, VDAC_A);
}
void SetDacB(uint16_t db) {
	VDAC_B = db;
	DAC_AD5322_Ch2(&hspi1, VDAC_B);
}
//void SetAllDAC() {
//	DAC_AD5322_Ch1Ch2(&hspi1,VDAC_A,VDAC_B);
//}
//--------------------------------------------------------------------------
// Новая реализация. для приема значений в напряжениях, с поиском по структуре DevNVRAM выгруженной из памяти.
//TODO: Установка цап реализованно только для канала A и режима m12. Нужно переписать с учетом режима работы. режим работы определяет какую таблицу использовать.
//void SetDacA(int16_t da)
//{
//	VDAC_A = volt2dgt(&(DevNVRAM.calibration_table), da);
//	DAC_AD5322_Ch1(&hspi1, VDAC_A);
//}
//void SetDacB(int16_t db) //BUG: Не работает. Установка цап реализованно только для канала A и режима m12. Нужно переписать с учетом режима работы. режим работы определяет какую таблицу использовать.
//{
//	VDAC_B = volt2dgt(&(DevNVRAM.calibration_table), db);
//	DAC_AD5322_Ch2(&hspi1, VDAC_B);
//}
void SetAllDAC()
{
	DAC_AD5322_Ch1Ch2(&hspi1, VDAC_A, VDAC_B);
}
uint16_t GetDacA()
{
	return VDAC_A;
}
uint16_t GetDacB()
{
	return VDAC_B;
}
#endif /* TEST_DAC */
//**************************************************************************
#if TEST_READ_BTN
#define KEY_LONG_DELAY 1000
//PB12
uint8_t short_state1 = 0;
uint8_t long_state1 = 0;
uint32_t time_key1 = 0;

uint8_t btn1_long_rd = 0;
uint8_t btn1_short_rd = 0;

//PB13
uint8_t short_state2 = 0;
uint8_t long_state2 = 0;
uint32_t time_key2 = 0;

uint8_t btn2_long_rd = 0;
uint8_t btn2_short_rd = 0;

//PB14
uint8_t short_state3 = 0;
uint8_t long_state3 = 0;
uint32_t time_key3 = 0;

uint8_t btn3_long_rd = 0;
uint8_t btn3_short_rd = 0;
//--------------------------------------------------------------------------
uint8_t GetBtnRunState()
{
	// 0x00 - не нажата; 0x01 - короткое нажатие; 0x02 - длительное нажатие
	if (btn1_short_rd == 0x00 && btn1_long_rd == 0x00)
	{
		btn1_long_rd = 0;
		btn1_short_rd = 0;
		return 0x00;
	}
	if (btn1_short_rd == 0x01 && btn1_long_rd == 0x00)
	{
		btn1_long_rd = 0;
		btn1_short_rd = 0;
		return 0x01;
	}
	if (btn1_short_rd == 0x00 && btn1_long_rd == 0x01)
	{
		btn1_long_rd = 0;
		btn1_short_rd = 0;
		return 0x02;
	}

	btn1_long_rd = 0;
	btn1_short_rd = 0;

	//	if (short_state1 == 0x00 && long_state1 == 0x00)	return 0x00;
	//	if (short_state1 == 0x01 && long_state1 == 0x00)	return 0x01;
	//	if (short_state1 == 0x00 && long_state1 == 0x01)	return 0x02;
	return 0x00;
}
//--------------------------------------------------------------------------
uint8_t GetBtnUpState()
{
	// 0x00 - не нажата; 0x01 - короткое нажатие; 0x02 - длительное нажатие
	if (btn2_short_rd == 0x00 && btn2_long_rd == 0x00)
	{
		btn2_long_rd = 0;
		btn2_short_rd = 0;
		return 0x00;
	}
	if (btn2_short_rd == 0x01 && btn2_long_rd == 0x00)
	{
		btn2_long_rd = 0;
		btn2_short_rd = 0;
		return 0x01;
	}
	if (btn2_short_rd == 0x00 && btn2_long_rd == 0x01)
	{
		btn2_long_rd = 0;
		btn2_short_rd = 0;
		return 0x02;
	}

	btn2_long_rd = 0;
	btn2_short_rd = 0;
	//	// 0x00 - не нажата; 0x01 - короткое нажатие; 0x02 - длительное нажатие
	//	if (short_state2 == 0x00 && long_state2 == 0x00)	return 0x00;
	//	if (short_state2 == 0x01 && long_state2 == 0x00)	return 0x01;
	//	if (short_state2 == 0x00 && long_state2 == 0x01)	return 0x02;
	return 0x00;
}
//--------------------------------------------------------------------------
uint8_t GetBtnDownState()
{
	// 0x00 - не нажата; 0x01 - короткое нажатие; 0x02 - длительное нажатие
	if (btn3_short_rd == 0x00 && btn3_long_rd == 0x00)
	{
		btn3_long_rd = 0;
		btn3_short_rd = 0;
		return 0x00;
	}
	if (btn3_short_rd == 0x01 && btn3_long_rd == 0x00)
	{
		btn3_long_rd = 0;
		btn3_short_rd = 0;
		return 0x01;
	}
	if (btn3_short_rd == 0x00 && btn3_long_rd == 0x01)
	{
		btn3_long_rd = 0;
		btn3_short_rd = 0;
		return 0x02;
	}

	btn3_long_rd = 0;
	btn3_short_rd = 0;
	//	if (short_state3 == 0x00 && long_state3 == 0x00)	return 0x00;
	//	if (short_state3 == 0x01 && long_state3 == 0x00)	return 0x01;
	//	if (short_state3 == 0x00 && long_state3 == 0x01)	return 0x02;
	return 0x00;
}
//--------------------------------------------------------------------------

#endif /* TEST_READ_BTN */
//**************************************************************************
#if TEST_TIM_CAPTURE
volatile uint8_t timWork = 0;
volatile uint8_t count_overflowTIM3 = 0;
volatile uint8_t count_overflowTIM4 = 0;

volatile uint16_t g_vTIM3_PB4 = 0;
volatile uint16_t g_vTIM4_PB6 = 0;

// char trans2_str[64] = {
// 	0,
// };
//--------------------------------------------------------------------------
void EnableTIM3_PB4()
{
	timWork = 1;
}
uint16_t GetTIM3()
{
	return g_vTIM3_PB4;
}
void resValTIM3_PB4()
{
	g_vTIM3_PB4 = 0;
}
//--------------------------------------------------------------------------
void EnableTIM4_PB6()
{
	timWork = 0;
}
uint16_t GetTIM4()
{
	return g_vTIM4_PB6;
}
void resValTIM4_PB6()
{
	g_vTIM4_PB6 = 0;
}
//--------------------------------------------------------------------------
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	uint16_t periodTIM3, pulseWidthTIM3, periodTIM4, pulseWidthTIM4;

	if (timWork)
	{
		if (htim->Instance == TIM3)
		{
			if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
			{
				periodTIM3 = HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_1);
				pulseWidthTIM3 = HAL_TIM_ReadCapturedValue(&htim3,
														   TIM_CHANNEL_2);

				TIM3->CNT = 0;

				int16_t deltaTIM3 = (int16_t)periodTIM3 - (int16_t)pulseWidthTIM3;
				deltaTIM3 = (deltaTIM3 < 0) ? (-1 * deltaTIM3) : deltaTIM3;
				g_vTIM3_PB4 = deltaTIM3;
			}
		}
	}
	else
	{
		if (htim->Instance == TIM4)
		{
			if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
			{
				periodTIM4 = HAL_TIM_ReadCapturedValue(&htim4, TIM_CHANNEL_1);
				pulseWidthTIM4 = HAL_TIM_ReadCapturedValue(&htim4,
														   TIM_CHANNEL_2);

				TIM4->CNT = 0;

				int16_t deltaTIM4 = (int16_t)periodTIM4 - (int16_t)pulseWidthTIM4;
				deltaTIM4 = (deltaTIM4 < 0) ? (-1 * deltaTIM4) : deltaTIM4;
				g_vTIM4_PB6 = deltaTIM4;
			}
		}
	}
}

#endif /* TEST_TIM_CAPTURE */
//**************************************************************************

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//**************************************************************************
#if TEST_ADC
volatile uint16_t g_VADC = 0;

uint16_t GetADC()
{
	return g_VADC;
}
//--------------------------------------------------------------------------
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	if (hadc->Instance == ADC1) //check if the interrupt comes from ACD1
	{
		g_VADC = HAL_ADC_GetValue(&hadc1); // глобальная переменна g_VADC вычитывается
	}
}
#endif /* TEST_ADC */
//**************************************************************************
#if USB_RESET
void USB_Reset(void)
{

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	// reset USB DP (D+)
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	// инициализируем пин DP как выход
	GPIO_InitStruct.Pin = GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET); // прижимаем DP к "земле"
	for (uint16_t i = 0; i < 10000; i++)
	{
	}; // немного ждём

	// переинициализируем пин для работы с USB
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	for (uint16_t i = 0; i < 10000; i++)
	{
	}; // немного ждём
}
#endif /* USB_RESET */
//**************************************************************************
#if TEST_RELAY
enum RelState {m12 = 0x01, m27 = 0x00}; //RelayState =|1:m12|0:m27|
bool RelayState = m12; //TODO: проверить первое состояние --> первоначальное состояние реле 27V FIXME: Нужно изменить на m12 и подправить у Йоноса!
#endif /* TEST_RELAY */
//**************************************************************************
#if TEST_USB
void runCommands(uint8_t *Buf, uint32_t *Len) 		// Обработчик USB
/*	command
	0x01 - включение рэле							data: 1B (0x00 - выключить; 0x01 - включить)	answer: 0x01 + 1B status
	0x02 - ЦАП канал А								data: 2B (значение)								answer: 0x02 + 1B status
	0x03 - ЦАП канал B								data: 2B (значение)								answer: 0x03 + 1B status
	0x04 - АЦП запрос значения						data: 0B										answer: 0x04 + 2B value
	0x05 - запрос состояния (Relay, DA, DB)			data: 0B										answer: 0x05 + 1B состиояние рэле + 2B значение ЦАП канал А + 2B значение ЦАП канал B
	0x06 - запрос состояния кнопок (Run, Up, Down)	data: 0B										answer: 0x06 + 1B состояние кнопки Run + 1B состояние кнопки Up + 1B состояние кнопки Down
	0x07 - запрос ID устройства
	("SN+WW+YY+NNN")									data: 0B										answer: 0x07 + 9B ID ("SN+WW+YY+NNN") SN1121001- 11 неделя-21год - 001 порядковый номер изготовления
	0х08 - запрос измеренной длительности			data: 0B (0x00 - сработал; 0x01 - не сработал)	answer: 0x08 + 1B status
	0х09 - запрос измеренной длительности			data: 0B (0x00 - сработал; 0x01 - не сработал)	answer: 0x08 + 1B status
	--------------------------------------------------------------------------
	0х0C - Прием длины калибровочной таблицы [0x0C][Длина][???]				0x00 - сработал; 0x01 - не сработал)	answer: 0x09 + 1B status
	0х0B - Отправка CRC 1-4 таблицы [0x0B][1-4][CRC(1-4)]	answer:
	0х0A - Прием калибровочной таблицы [0x0A][1-4][offset][count][data] 	answer: [0x0A]+[1-4]+[offset]+[count]+[status] (0x00 - сработал; 0x01 - не сработал)
	0х0D - Запись во флеш калибровочной таблицы [0x0D] data: 1B (0x00 - успешно; 0x01 - ошибка при записи)	answer: 0x0D + 1B status
	--------------------------------------------------------------------------
	status
	0x00 - успешно
	0x01 - ошибка
	--------------------------------------------------------------------------
	Калибровка 
	На вход щупа подается семетричная пила с частотой 1кГц с оффестом установленным на ип.
	после компарирования сигнала МК измеряет длительность импульса
	т.к. 1 кГц соотвествет длительности в 500 мс то это означает что мы попапали в полуку офсета.
	--------------------------------------------------------------------------*/
{
	if (*Len < 1)
		return;

	//--------------------------------------------------------------------------

	uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];
	uint16_t tVal16;
	uint8_t cmd = Buf[0];
	//--------------------------------------------------------------------------
	if 		(cmd == 0x01)	// Relay:1 - 12V	[0x01 - 0x01]
	{
		if (*Len >= 2 && (Buf[1] == 0x01 || Buf[1] == 0x00))
		{
			if (Buf[1] == 0x01)
			{
				#if TEST_RELAY
				HAL_GPIO_WritePin(Relay_GPIO_Port, Relay_Pin, GPIO_PIN_SET);
				RelayState = m12;
				printf("RelayState:12V - %d \n", RelayState);
				SetAllDAC();
				UserTxBufferFS[0] = cmd;
				UserTxBufferFS[1] = 0x00; // успешно
				CDC_Transmit_FS(UserTxBufferFS, 2);
				#endif /* TEST_RELAY */
				return;
			}
			else if (Buf[1] == 0x00)
			{
				#if TEST_RELAY
				HAL_GPIO_WritePin(Relay_GPIO_Port, Relay_Pin, GPIO_PIN_RESET);
				RelayState = m27;
				printf("RelayState:27V - %d \n", RelayState);
				SetAllDAC();

				UserTxBufferFS[0] = cmd;
				UserTxBufferFS[1] = 0x00; // успешно
				CDC_Transmit_FS(UserTxBufferFS, 2);
				#endif /* TEST_RELAY */
				return;
			}
		}

		UserTxBufferFS[0] = cmd;
		UserTxBufferFS[1] = 0x01; // ошибка
		CDC_Transmit_FS(UserTxBufferFS, 2);
		return;

		//--------------------------------------------------------------------------
		
	}
	else if (cmd == 0x02)	// DA:4095
	{
		if (*Len >= 3)
		{
			resValTIM3_PB4(); // обнуление переменной для проведения калиброки
			resValTIM4_PB6(); // обнуление переменной для проведения калиброки
			memcpy(&tVal16, Buf + 1, sizeof(tVal16));
			SetDacA(tVal16);
			//			SetDacA(tVal16);

			printf("DacA: %d\n", tVal16);
			UserTxBufferFS[0] = cmd;
			UserTxBufferFS[1] = 0x00; // успешно

			CDC_Transmit_FS(UserTxBufferFS, 2);
			return;
		}

		UserTxBufferFS[0] = cmd;
		UserTxBufferFS[1] = 0x01; // ошибка
		CDC_Transmit_FS(UserTxBufferFS, 2);
		return;
		//--------------------------------------------------------------------------
		
	}
	else if (cmd == 0x03)	// DB:4095
	{
		if (*Len >= 3)
		{
			resValTIM3_PB4(); // обнуление переменной для проведения калиброки
			resValTIM4_PB6(); // обнуление переменной для проведения калиброки
			memcpy(&tVal16, Buf + 1, sizeof(tVal16));
			SetDacB(tVal16);

			UserTxBufferFS[0] = cmd;
			UserTxBufferFS[1] = 0x00; // успешно

			printf("DacB: %d \n", tVal16);

			CDC_Transmit_FS(UserTxBufferFS, 2);
			return;
		}

		UserTxBufferFS[0] = cmd;
		UserTxBufferFS[1] = 0x01; // ошибка
		CDC_Transmit_FS(UserTxBufferFS, 2);
		return;
		//--------------------------------------------------------------------------
		
	}
	else if (cmd == 0x04)	// ADC?
	{
		tVal16 = GetADC();
		UserTxBufferFS[0] = cmd;
		memcpy(UserTxBufferFS + 1, &tVal16, sizeof(tVal16));
		CDC_Transmit_FS(UserTxBufferFS, 3);
		return;
		//--------------------------------------------------------------------------
		
	}
	else if (cmd == 0x05)	// Relay?DA?DB?
	{
		UserTxBufferFS[0] = cmd;
		#if TEST_RELAY
		UserTxBufferFS[1] = RelayState;
		#endif /* TEST_RELAY */

		tVal16 = GetDacA();
		memcpy(UserTxBufferFS + 2, &tVal16, sizeof(tVal16));

		tVal16 = GetDacB();
		memcpy(UserTxBufferFS + 4, &tVal16, sizeof(tVal16));

		CDC_Transmit_FS(UserTxBufferFS, 6);
		return;
		//--------------------------------------------------------------------------
		
	}
	else if (cmd == 0x06)	// Btn?
	{
		UserTxBufferFS[0] = cmd;
		UserTxBufferFS[1] = GetBtnRunState();
		UserTxBufferFS[2] = GetBtnUpState();
		UserTxBufferFS[3] = GetBtnDownState();
		CDC_Transmit_FS(UserTxBufferFS, 4);
		return;
		//--------------------------------------------------------------------------
		

	// ID?
	} else if (cmd == 0x07) {
		char str[] = "prb_v0.3";
		UserTxBufferFS[0] = cmd;
		UserTxBufferFS[1] = strlen(str);
		memcpy(UserTxBufferFS + 2, str, strlen(str));
		CDC_Transmit_FS(UserTxBufferFS, strlen(str) + 2);
		return (USBD_OK);
//	else if (cmd == 0x07)	// ID? норвая реализация с серийным номером
//	{
//		char str[9] = {
//			0,
//		};
//		memcpy(str, "SN", strlen("SN"));
//		itoa(SN_DEFINE, str + 2, 16);
//
//		UserTxBufferFS[0] = cmd;
//		UserTxBufferFS[1] = strlen(str);
//		memcpy(UserTxBufferFS + 2, str, strlen(str));
//		CDC_Transmit_FS(UserTxBufferFS, strlen(str) + 2);
//		return;
	//--------------------------------------------------------------------------
	/* Калибровка 
	На вход щупа подается семетричная пила с частотой 1кГц с оффестом установленным на ип.
	после компарирования сигнала МК измеряет длительность импульса
	т.к. 1 кГц соотвествет длительности в 500 мс то это означает что мы попапали в полуку офсета.*/
	//--------------------------------------------------------------------------
	}
	else if (cmd == 0x08)	// Калибровка TIM inHL?
	{
		EnableTIM3_PB4();
		uint16_t temp = GetTIM3();
		UserTxBufferFS[0] = cmd;
		memcpy(UserTxBufferFS + 1, &temp, sizeof(uint16_t));
		CDC_Transmit_FS(UserTxBufferFS, 1 + sizeof(uint16_t));
		return;
	//--------------------------------------------------------------------------
	}
	else if (cmd == 0x09)	// Калибровка TIM inLL?
	{
		EnableTIM4_PB6();
		uint16_t temp = GetTIM4();
		UserTxBufferFS[0] = cmd;
		memcpy(UserTxBufferFS + 1, &temp, sizeof(uint16_t));
		CDC_Transmit_FS(UserTxBufferFS, 1 + sizeof(uint16_t));
		return;
	//--------------------------------------------------------------------------
	}
	else if (cmd == 0x0A)	// TODO: Прием калибровочной таблицы [0x0A][1-4][offset][count][data] 	answer: [0x0A]+[1-4]+[offset]+[count]+[status] (0x00 - сработал; 0x01 - не сработал)
	{
		//Прием калибровочной таблицы [0x0A][1-4][offset][count][data]
		//FIXME: За одну посылку можно получить максимум 64 byte. Нужно организовать пакетную передачу
		uint16_t tOffset, tCount, tData;
		if (*Len >= 2 && Buf[1] >= 0x00 && Buf[1] <= 0x03)
		{
			if 		(Buf[1] == 0x00)	//А_m12
			{
				/*
				u8 Buf[0] = 0x0A
				u8 Buf[1] = 0x01-0x04
				---------------------
				u16 Buf[2] = offset u8
				    Buf[3] = offset u8
				----------------------
				u16 Buf[4] = count u8
					Buf[5] = count u8
				----------------------
				u16 Buf[6] = data1 u8
					Buf[7] = data1 u8
				----------------------
				...
				u16 Buf[count] = data[n-1]  u8
					Buf[count] = data[n] 	u8
				*/
				memcpy(&tOffset, Buf + 2, sizeof(uint16_t));
				memcpy(&tCount, Buf + 2 + sizeof(uint16_t), sizeof(uint16_t));

				for (uint16_t i = 0; i < tCount; i++)
				{
					memcpy(&tData, Buf + 2 + 2 * sizeof(uint16_t) + i * 2, sizeof(uint16_t));
					DevNVRAM.calibration_table.dacValA_m12[i + tOffset] = tData;
				}

				changeTableFlag = true;


				//answer: [0x0A]+[1-4]+[offset]+[count]+[status] (0x00 - сработал; 0x01 - не сработал)
				/*
				u8 	Buf[0] = 0x0A
				u8	Buf[1] = 0x01-0x04	// Номер таблицы
				---------------------
				u16 Buf[2] = offset u8
				    Buf[3] = offset u8
				----------------------
				u16 Buf[4] = count u8
					Buf[5] = count u8
				----------------------
				u8	Buf[6] = status u8	(0x00 - сработал; 0x01 - не сработал)
				----------------------
				*/
				UserTxBufferFS[0] = cmd;												  //1			->1
				UserTxBufferFS[1] = 0x01;												  //1			->2
				memcpy(UserTxBufferFS + 2, &tOffset, sizeof(uint16_t));					  //2+2			->4
				memcpy(UserTxBufferFS + 2 + sizeof(uint16_t), &tCount, sizeof(uint16_t)); //2+2+2		->6
				UserTxBufferFS[2 + 2 * sizeof(uint16_t) + 1] = 0x00;					  //2+2*2+1		->7

				CDC_Transmit_FS(UserTxBufferFS, sizeof(UserTxBufferFS[2 + 2 * sizeof(uint16_t) + 1]));
				return;
				//--------------------------------------------------------------------------
			}
			//--------------------------------------------------------------------------
			else if (Buf[1] == 0x01)	//B_m12
			{
				/*
				u8 Buf[0] = 0x0A
				u8 Buf[1] = 0x01-0x04
				---------------------
				u16 Buf[2] = offset u8
				    Buf[3] = offset u8
				----------------------
				u16 Buf[4] = count u8
					Buf[5] = count u8
				----------------------
				u16 Buf[6] = data1 u8
					Buf[7] = data1 u8
				----------------------
				...
				u16 Buf[count] = data[n-1]  u8
					Buf[count] = data[n] 	u8
				*/
				memcpy(&tOffset, Buf + 2, sizeof(uint16_t));
				memcpy(&tCount, Buf + 2 + sizeof(uint16_t), sizeof(uint16_t));

				for (uint16_t i = 0; i < tCount; i++)
				{
					memcpy(&tData, Buf + 2 + 2 * sizeof(uint16_t) + i * 2, sizeof(uint16_t));
					DevNVRAM.calibration_table.dacValB_m12[i + tOffset] = tData;
				}

				//answer: [0x0A]+[1-4]+[offset]+[count]+[status] (0x00 - сработал; 0x01 - не сработал)
				/*
				u8 	Buf[0] = 0x0A
				u8	Buf[1] = 0x01-0x04	// Номер таблицы
				---------------------
				u16 Buf[2] = offset u8
				    Buf[3] = offset u8
				----------------------
				u16 Buf[4] = count u8
					Buf[5] = count u8
				----------------------
				u8	Buf[6] = status u8	(0x00 - сработал; 0x01 - не сработал)
				----------------------
				*/
				UserTxBufferFS[0] = cmd;												  //1			->1
				UserTxBufferFS[1] = 0x01;												  //1			->2
				memcpy(UserTxBufferFS + 2, &tOffset, sizeof(uint16_t));					  //2+2		->4
				memcpy(UserTxBufferFS + 2 + sizeof(uint16_t), &tCount, sizeof(uint16_t)); //2+2+2		->6
				UserTxBufferFS[2 + 2 * sizeof(uint16_t) + 1] = 0x00;					  //2+2*2+1	->7

				CDC_Transmit_FS(UserTxBufferFS, sizeof(UserTxBufferFS[2 + 2 * sizeof(uint16_t) + 1]));
				return;
				//--------------------------------------------------------------------------
			}
			//--------------------------------------------------------------------------
			else if (Buf[1] == 0x02)	//A_m27
			{
				/*
				u8 Buf[0] = 0x0A
				u8 Buf[1] = 0x01-0x04
				---------------------
				u16 Buf[2] = offset u8
				    Buf[3] = offset u8
				----------------------
				u16 Buf[4] = count u8
					Buf[5] = count u8
				----------------------
				u16 Buf[6] = data1 u8
					Buf[7] = data1 u8
				----------------------
				...
				u16 Buf[count] = data[n-1]  u8
					Buf[count] = data[n] 	u8
				*/
				memcpy(&tOffset, Buf + 2, sizeof(uint16_t));
				memcpy(&tCount, Buf + 2 + sizeof(uint16_t), sizeof(uint16_t));

				for (uint16_t i = 0; i < tCount; i++)
				{
					memcpy(&tData, Buf + 2 + 2 * sizeof(uint16_t) + i * 2, sizeof(uint16_t));
					DevNVRAM.calibration_table.dacValA_m27[i + tOffset] = tData;
				}

				//answer: [0x0A]+[1-4]+[offset]+[count]+[status] (0x00 - сработал; 0x01 - не сработал)
				/*
				u8 	Buf[0] = 0x0A
				u8	Buf[1] = 0x01-0x04	// Номер таблицы
				---------------------
				u16 Buf[2] = offset u8
				    Buf[3] = offset u8
				----------------------
				u16 Buf[4] = count u8
					Buf[5] = count u8
				----------------------
				u8	Buf[6] = status u8	(0x00 - сработал; 0x01 - не сработал)
				----------------------
				*/
				UserTxBufferFS[0] = cmd;												  //1			->1
				UserTxBufferFS[1] = 0x01;												  //1			->2
				memcpy(UserTxBufferFS + 2, &tOffset, sizeof(uint16_t));					  //2+2		->4
				memcpy(UserTxBufferFS + 2 + sizeof(uint16_t), &tCount, sizeof(uint16_t)); //2+2+2		->6
				UserTxBufferFS[2 + 2 * sizeof(uint16_t) + 1] = 0x00;					  //2+2*2+1	->7

				CDC_Transmit_FS(UserTxBufferFS, sizeof(UserTxBufferFS[2 + 2 * sizeof(uint16_t) + 1]));
				return;
				//--------------------------------------------------------------------------
			}
			//--------------------------------------------------------------------------
			else if (Buf[1] == 0x03)	//B_m27
			{
				/*
				u8 Buf[0] = 0x0A
				u8 Buf[1] = 0x01-0x04
				---------------------
				u16 Buf[2] = offset u8
				    Buf[3] = offset u8
				----------------------
				u16 Buf[4] = count u8
					Buf[5] = count u8
				----------------------
				u16 Buf[6] = data1 u8
					Buf[7] = data1 u8
				----------------------
				...
				u16 Buf[count] = data[n-1]  u8
					Buf[count] = data[n] 	u8
				*/
				memcpy(&tOffset, Buf + 2, sizeof(uint16_t));
				memcpy(&tCount, Buf + 2 + sizeof(uint16_t), sizeof(uint16_t));

				for (uint16_t i = 0; i < tCount; i++)
				{
					memcpy(&tData, Buf + 2 + 2 * sizeof(uint16_t) + i * 2, sizeof(uint16_t));
					DevNVRAM.calibration_table.dacValB_m27[i + tOffset] = tData;
				}

				//answer: [0x0A]+[1-4]+[offset]+[count]+[status] (0x00 - сработал; 0x01 - не сработал)
				/*
				u8 	Buf[0] = 0x0A
				u8	Buf[1] = 0x01-0x04	// Номер таблицы
				---------------------
				u16 Buf[2] = offset u8
				    Buf[3] = offset u8
				----------------------
				u16 Buf[4] = count u8
					Buf[5] = count u8
				----------------------
				u8	Buf[6] = status u8	(0x00 - сработал; 0x01 - не сработал)
				----------------------
				*/
				UserTxBufferFS[0] = cmd;												  //1			->1
				UserTxBufferFS[1] = 0x01;												  //1			->2
				memcpy(UserTxBufferFS + 2, &tOffset, sizeof(uint16_t));					  //2+2		->4
				memcpy(UserTxBufferFS + 2 + sizeof(uint16_t), &tCount, sizeof(uint16_t)); //2+2+2		->6
				UserTxBufferFS[2 + 2 * sizeof(uint16_t) + 1] = 0x00;					  //2+2*2+1	->7

				CDC_Transmit_FS(UserTxBufferFS, sizeof(UserTxBufferFS[2 + 2 * sizeof(uint16_t) + 1]));
				return;
				//--------------------------------------------------------------------------
			}
			//--------------------------------------------------------------------------
		}
		UserTxBufferFS[0] = cmd;
		UserTxBufferFS[1] = 0x01; // ошибка
		CDC_Transmit_FS(UserTxBufferFS, 2);
		return;
		//--------------------------------------------------------------------------
	}
	else if (cmd == 0x0B)	// TODO: Отправка CRC 1-4 таблицы [0x0B][1-4][CRC(1-4)]	answer:
	{
		return;
	//--------------------------------------------------------------------------
	}
	else if (cmd == 0x0C)	// TODO: Прием длины калибровочной таблицы [0x0C][Длина][???]
	{
		return;
	//--------------------------------------------------------------------------
	}
	else if (cmd == 0x0D)	// TODO: Запись во флеш калибровочной таблицы [0x0D] data: 1B (0x00 - успешно; 0x01 - ошибка при записи)	answer: 0x0D + 1B status
	{
		if (*Len >= 2 && (Buf[1] == 0x02))
		{
			//TODO: Функция записи фо флеш. FIXME: не работает запись фо флеш!
			 changeTableFlag = true;
			// writeTableInFlash();
			printf("changeTableFlag = true!");
			UserTxBufferFS[0] = cmd;
			UserTxBufferFS[1] = 0x00; // успешно
			CDC_Transmit_FS(UserTxBufferFS, 2);
			return;
		}
 		
		//--------------------------------------------------------------------------
		UserTxBufferFS[0] = cmd;
		UserTxBufferFS[1] = 0x01; // ошибка
		CDC_Transmit_FS(UserTxBufferFS, 2);
		return;
		//--------------------------------------------------------------------------
	}
//-------------------------------------------------------------------------
	
}
#endif /* TEST_USB */
//**************************************************************************
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

//	writeTableInFlash();


  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
//--------------------------------------------------------------------------
#if DWT_INIT
	DWT_Init();
#endif /* DWT_INIT */
	   //--------------------------------------------------------------------------

	//--------------------------------------------------------------------------

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

//--------------------------------------------------------------------------

//-------------------------------------------------------------------------

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USB_DEVICE_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_CRC_Init();
  /* USER CODE BEGIN 2 */
	//**************************************************************************
#if TEST_UID
	sprintf(buffer, "UID %x-%x-%lx-%lx\n", *idBase0, *idBase1, *idBase2, *idBase3);
	printf(buffer);
#endif /* TEST_UID */

	//**************************************************************************
#if TEST_TIM_CAPTURE
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2);
	HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_2);
#endif /* TEST_TIM_CAPTURE */
//--------------------------------------------------------------------------
#if TEST_DAC
	SetAllDAC();
#endif /* TEST_DAC */
	   //**************************************************************************
#if TEST_ADC
	HAL_ADCEx_Calibration_Start(&hadc1);
	HAL_ADC_Start_IT(&hadc1);

#endif /* TEST_ADC */
//**************************************************************************
#if TEST_FLASH_TABLE

	flash_fill_calibTable();
	flash_write_calibTable();
	// Чтение DevNVRAM
    /*volatile uint32_t l_Address = FLASH_TABLE_START_ADDR;
    uint32_t l_Error = 0;
    uint32_t l_Index = 0;*/
    /*
    while (l_Address < FLASH_TABLE_STOP_ADDR)
    {
        DevNVRAM.data32[l_Index] = *(__IO uint32_t *)l_Address;
        l_Index = l_Index + 1;
        l_Address = l_Address + 4;
    }

//--------------------------------------------------------------------------
    // если после чтения майджик кей не найден, то это первый запуск

    if (DevNVRAM.calibration_table.MagicNum != 0)
    {
        // Подготовка
        // Заносим типовые значения
        // TODO: !!!!!Добавить математику расчета калибровочной таблицы!!!!!!!
        memset(DevNVRAM.data32, 0xCA, sizeof(DevNVRAM.data32));

        // ЗАГЛУШКА
        for (uint8_t i = 0; i < MAX_VAL_M12; i++)
        {
            DevNVRAM.calibration_table.dacValA_m12[i] = i;
        }
        for (uint8_t i = 0; i < MAX_VAL_M12; i++)
        {
            DevNVRAM.calibration_table.dacValB_m12[i] = i;
        }
        for (uint8_t i = 0; i < MAX_VAL_M27; i++)
        {
            DevNVRAM.calibration_table.dacValA_m27[i] = i;
        }
        for (uint8_t i = 0; i < MAX_VAL_M27; i++)
        {
            DevNVRAM.calibration_table.dacValB_m12[i] = i;
        }

        crete_calibration_table(&DevNVRAM.calibration_table);

        DevNVRAM.calibration_table.Hardwire = 0x06;
        DevNVRAM.calibration_table.Firmware = 0x05;
        DevNVRAM.calibration_table.SN = 0x1121001; //11 недел	я + год + порядковый номер изготовления
        DevNVRAM.calibration_table.MagicNum = MAGIC_KEY_DEFINE;

        DevNVRAM.sector.NWrite = 0;

        DevNVRAM.sector.CheckSum = 0; //HAL_CRC_Calculate(&hcrc, &DevNVRAM.calibration_table, (sizeof(DevNVRAM.calibration_table)/4));//DONE: нужно отправлять длину кратную 32b! -  по какой то причине в этом проекте не работает CRC!!!

        //--------------------------------------------------------------------------
        //если после чтения майджик кей не найден, то это первый запуск записываем дефолтную таблицу
        l_Address = FLASH_TABLE_START_ADDR;
        l_Error = 0;
        l_Index = 0;

        while (l_Address < FLASH_TABLE_STOP_ADDR)
        {
            if (DevNVRAM.data32[l_Index] != *(__IO uint32_t *)l_Address)
            {
                l_Error++;
            }
            l_Index = l_Index + 1;
            l_Address = l_Address + 4;
        }

        if (l_Error > 0)
        { // конфигурация изменилась сохраняем
            // Готовим к записи в память
            HAL_FLASH_Unlock();
            // Очищаем страницу памяти
            HAL_FLASHEx_Erase(&EraseInitStruct, &l_Error);
            //Пишем данные в память
            l_Address = FLASH_TABLE_START_ADDR;
            l_Error = 0x00;
            l_Index = 0x00;

            while (l_Address < FLASH_TABLE_STOP_ADDR)
            {
                if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, l_Address,
                                      DevNVRAM.data32[l_Index]) != HAL_OK)
                {
                    l_Error++;
                }

                l_Address = l_Address + 4;
                l_Index = l_Index + 1;
                HAL_Delay(10);
            }
            HAL_FLASH_Lock();
        }

    } //если после чтения майджик кей не найден, то это первый запуск записываем дефолтную таблицу
    // TODO: Надо по запросе какая версия калиброчной табцы высылать значения дефолтной таблице...
    //--------------------------------------------------------------------------
*/
#endif /* TEST_FLASH_TABLE */

	uint32_t timme = 0; // для таймера в 10 сек
//**************************************************************************
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
		// Циклически проверяем соотвествует ли информация в памяти массиву настроек?
		if ((HAL_GetTick() - timme) > 10000) // интервал  10сек
		{
#if TEST_FLASH_TABLE
/*
			if (changeTableFlag)
			{
				changeTableFlag = false;
				printf("write");
				// Циклически проверяем соотвествует ли информация в памяти массиву настроек?

				l_Address = FLASH_TABLE_START_ADDR;
				l_Error = 0;
				l_Index = 0;
				//Читаем и сравниваем
				while (l_Address < FLASH_TABLE_STOP_ADDR)
				{
					if (DevNVRAM.data32[l_Index] != *(__IO uint32_t *)l_Address)
					{
						l_Error++;
					}
					l_Index = l_Index + 1;
					l_Address = l_Address + 4;
				}

				if (l_Error > 0)
				{ // конфигурация изменилась сохраняем
					// Готовим к записи в память
					HAL_FLASH_Unlock();
					// Очищаем страницу памяти
					HAL_FLASHEx_Erase(&EraseInitStruct, &l_Error);
					//Пишем данные в память
					l_Address = FLASH_TABLE_START_ADDR;
					l_Error = 0x00;
					l_Index = 0x00;

					DevNVRAM.sector.NWrite = DevNVRAM.sector.NWrite + 1;
					DevNVRAM.sector.CheckSum = 0; //HAL_CRC_Calculate(&hcrc, &DevNVRAM.calibration_table, (sizeof(DevNVRAM.calibration_table)/4));

					while (l_Address < FLASH_TABLE_STOP_ADDR)
					{
						if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, l_Address,
											  DevNVRAM.data32[l_Index]) != HAL_OK)
						{
							l_Error++;
						}

						l_Address = l_Address + 4;
						l_Index = l_Index + 1;
						HAL_Delay(10);
					}
					HAL_FLASH_Lock();
				}
				HAL_Delay(100);
				//--------------------------------------------------------------------------
				printf("flash done");
			}*/
#endif /* TEST_FLASH_TABLE */
			timme = HAL_GetTick();
		}

//**************************************************************************
#if TEST_READ_BTN //TODO: данная реализация плохо отрабатывает! TODO: Нужно переделать на EXTI+TIM

		uint32_t ms = HAL_GetTick();
		uint8_t key1_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12); // подставить свой пин //TODO: Проверить работу BACK key!

		if (key1_state == 0 && !short_state1 && (ms - time_key1) > 50)
		{
			short_state1 = 1;
			long_state1 = 0;
			time_key1 = ms;
		}
		else if (key1_state == 0 && !long_state1 && (ms - time_key1) > KEY_LONG_DELAY)
		{
			long_state1 = 1;
			// действие на длинное нажатие
			btn1_long_rd = 1;
		}
		else if (key1_state == 1 && short_state1 && (ms - time_key1) > 50)
		{
			short_state1 = 0;
			time_key1 = ms;

			if (!long_state1)
			{
				// действие на короткое нажатие
				btn1_short_rd = 1;
			}
		}
		uint8_t key2_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13); // подставить свой пин

		if (key2_state == 0 && !short_state2 && (ms - time_key2) > 50)
		{
			short_state2 = 1;
			long_state2 = 0;
			time_key2 = ms;
		}
		else if (key2_state == 0 && !long_state2 && (ms - time_key2) > KEY_LONG_DELAY)
		{
			long_state2 = 1;

			// действие на длинное нажатие
			btn2_long_rd = 1;
		}
		else if (key2_state == 1 && short_state2 && (ms - time_key2) > 50)
		{
			short_state2 = 0;
			time_key2 = ms;

			if (!long_state2)
			{
				// действие на короткое нажатие
				btn2_short_rd = 1;
			}
		}

		uint8_t key3_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14); // подставить свой пин
		if (key3_state == 0 && !short_state3 && (ms - time_key3) > 50)
		{
			short_state3 = 1;
			long_state3 = 0;
			time_key3 = ms;
		}
		else if (key3_state == 0 && !long_state3 && (ms - time_key3) > KEY_LONG_DELAY)
		{
			long_state3 = 1;
			// действие на длинное нажатие
			btn3_long_rd = 1;
		}
		else if (key3_state == 1 && short_state3 && (ms - time_key3) > 50)
		{
			short_state3 = 0;
			time_key3 = ms;

			if (!long_state3)
			{
				// действие на короткое нажатие
				btn3_short_rd = 1;
			}
		}

#endif	/* TEST_READ_BTN */
		//**************************************************************************

		//--------------------------------------------------------------------------

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC|RCC_PERIPHCLK_USB;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_13CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_16BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 72-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65000-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 72-1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65000-1;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
  if (HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();


  // reset USB DP (D+)
  // инициализируем пин DP как выход
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET); // прижимаем DP к "земле"
  for(uint16_t i = 0; i < 10000; i++) {}; // немного ждём

  // переинициализируем пин для работы с USB
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  for(uint16_t i = 0; i < 10000; i++) {}; // немного ждём


  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, AD5312_LDAC_Pin|AD5312_SYNC_Pin|GPIO_PIN_10, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Relay_GPIO_Port, Relay_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : AD5312_LDAC_Pin Relay_Pin AD5312_SYNC_Pin PA10 */
  GPIO_InitStruct.Pin = AD5312_LDAC_Pin|Relay_Pin|AD5312_SYNC_Pin|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB13 PB14 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

#if USB_RESET
// После перегенерации в Кубе добавить USB_Reset(); в функцию MX_GPIO_Init(void) (после ...CLK_ENABLE(); )
#endif /* USB_RESET */
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	//	 printf("Wrong parameters value: file %s on line %d\r\n", file, line);
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
