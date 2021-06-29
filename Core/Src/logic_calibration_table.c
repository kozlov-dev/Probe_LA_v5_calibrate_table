/*******************************************************************************
 * @File: logic_calibration_table.c
 * @Author: Kozlov-dev, A.
 * @Project: Probe_LA.c * @Microcontroller: STM32F103C8T
 * @Device: Probe_LA
 * @Date: 07.04.2021
 * @Purpose: Calibration table volt2dac
 *******************************************************************************/
#include <logic_calibration_table.h>
#include <stdlib.h>
#include <math.h>

void crete_calibration_table(Table_t *calibTable)
{ //de

    calibTable->dacValA_m12[0] = 0x159;
    calibTable->dacValA_m12[1] = 0x182;
    calibTable->dacValA_m12[2] = 0x225;
    calibTable->dacValA_m12[3] = 0x309;
    calibTable->dacValA_m12[4] = 0x3eb;
    calibTable->dacValA_m12[5] = 0x4cd;
    calibTable->dacValA_m12[6] = 0x5b0;
    calibTable->dacValA_m12[7] = 0x693;
    calibTable->dacValA_m12[8] = 0x777;
    calibTable->dacValA_m12[9] = 0x861;
    calibTable->dacValA_m12[10] = 0x93c;
    calibTable->dacValA_m12[11] = 0xa20;
    calibTable->dacValA_m12[12] = 0xb04;
    calibTable->dacValA_m12[13] = 0xbe5;
    calibTable->dacValA_m12[14] = 0xcc7;
    calibTable->dacValA_m12[15] = 0xda9;
    calibTable->dacValA_m12[16] = 0xe62;
    calibTable->dacValA_m12[17] = 0xe96;
    calibTable->dacValA_m12[18] = 0xeac;

    calibTable->dacValB_m12[0] = 0x145;
    calibTable->dacValB_m12[1] = 0x16e;
    calibTable->dacValB_m12[2] = 0x212;
    calibTable->dacValB_m12[3] = 0x2f0;
    calibTable->dacValB_m12[4] = 0x3d2;
    calibTable->dacValB_m12[5] = 0x4b4;
    calibTable->dacValB_m12[6] = 0x597;
    calibTable->dacValB_m12[7] = 0x67a;
    calibTable->dacValB_m12[8] = 0x75e;
    calibTable->dacValB_m12[9] = 0x84a;
    calibTable->dacValB_m12[10] = 0x923;
    calibTable->dacValB_m12[11] = 0xa07;
    calibTable->dacValB_m12[12] = 0xae8;
    calibTable->dacValB_m12[13] = 0xbcc;
    calibTable->dacValB_m12[14] = 0xcae;
    calibTable->dacValB_m12[15] = 0xd90;
    calibTable->dacValB_m12[16] = 0xe49;
    calibTable->dacValB_m12[17] = 0xe7b;
    calibTable->dacValB_m12[18] = 0xe92;

    calibTable->dacValA_m27[0] = 0x0;
    calibTable->dacValA_m27[1] = 0x1000;

    calibTable->dacValB_m27[0] = 0x0;
    calibTable->dacValB_m27[1] = 0x1000;
}

//--------------------------------------------------------------------------
// Описание: 
// Функция floor() возвращает наибольшее целое число (представленное как double), которое не больше, чем num. Например, если дано 1.02, то функция floor() возвращает 1.0. Если задано —1.02, то функция floor() возвращает —2.0.

// Функция floorl() представляет собой версию этой функции для переменных типа long double.
// floor()
// Пример: 
// Следующий фрагмент кода выводит на экран «10»:
// printf("%f", floor(10.9));

// uint16_t volt2dgt(Table_t *calibTable, int16_t volt){
 uint16_t volt2dgt(Table_t *calibTable, RelState m, uint8_t Ch, int16_t volt)
 {
//------------------------------------------------
int16_t minVolt;
int16_t maxVolt;
uint16_t x0;
uint16_t x1;
float count;
uint16_t y0;
uint16_t y1;
float b;
float a;
int16_t x ;
uint16_t yi;
float y;


uint16_t Ca0;
uint16_t Ca1;

uint16_t a0;
uint16_t a1;
float CodeX ;





int CA;
int Aa;
int AXXX;
//------------------------------------------------



     switch (Ch)
     {
     case 1:
         switch (m)
         {
         case m12:

        	 minVolt = MIN_VOLT_MODE_12;
			 maxVolt = MAX_VOLT_MODE_12;

        	 x0 = abs(MIN_VOLT_MODE_12);
             x1 = abs(MAX_VOLT_MODE_12);

        	 count = (x0+x1)/STEP_CALIBRATE;

        	 y0 = 0;            // начало индекса массива
        	 y1 = count;        // конец индекса массива

        	 b = (count*x0)/(x0+x1);
        	 a = b/x0;

        	 x = volt ;// /Ktr!!!!!!!!!!!!!!
        	 yi = floor(a*x+b); // искомый индекс в массиве!!! найти минимальное значение от него 26.5-->26 через floor

        	  // // TODO:Найти по найденому индексу значение в массиве!  и найти значение n+1

        	 Ca0 =(uint16_t *) calibTable->dacValA_m12[yi];
        	 Ca1 = calibTable->dacValA_m12[yi+1];
        	 y = a*x+b;
        	 a0 = (y * STEP_CALIBRATE) - abs( MIN_VOLT_MODE_12);// FIXME: !!!!!не правильные значение 2080 вместо 2400 TODO: не правльно переводит значения!27*200 = 5400
        	 a1 = ((y+1) * STEP_CALIBRATE) -abs( MIN_VOLT_MODE_12);// TODO: не правльно переводит значения!28*200 = 5600

        	 CA= Ca1-Ca0;
        	 Aa = a1-a0;
        	 AXXX = x-a0;

        	 CodeX = (((CA)/(Aa)))*(AXXX)+Ca0;
        	 return CodeX; //2400 -> 2662












//             // TODO: Нужно ли учитывать Ктр? volt = volt*Ktr
//             count = (abs(MIN_VOLT_MODE_12) + abs(MAX_VOLT_MODE_12)) / STEP_CALIBRATE;
//             y = (floor(
//                 (count * abs(MIN_VOLT_MODE_12)) / (abs(MIN_VOLT_MODE_12) + abs(MAX_VOLT_MODE_12)) / abs(MIN_VOLT_MODE_12) * volt + (count * abs(MIN_VOLT_MODE_12)) / (abs(MIN_VOLT_MODE_12) + abs(MAX_VOLT_MODE_12)))); // искомый индекс в массиве!!! найти минимальное значение от него 26.5-->26 через floor
//             CodeX = (((calibTable->dacValA_m12[y + 1 ] - calibTable->dacValA_m12[y]) / (((y + 1 ) * STEP_CALIBRATE) - abs(MIN_VOLT_MODE_12) - ((y ) * STEP_CALIBRATE) - abs(MIN_VOLT_MODE_12)))) * (volt - ((y ) * STEP_CALIBRATE) - abs(MIN_VOLT_MODE_12)) + calibTable->dacValA_m12[y];
//             return CodeX;

             break;
         case m27:

             // TODO: Нужно ли учитывать Ктр? volt = volt*Ktr
             count = (abs(MIN_VOLT_MODE_27) + abs(MAX_VOLT_MODE_27)) / STEP_CALIBRATE;
             y = (floor(
                 (count * abs(MIN_VOLT_MODE_27)) / (abs(MIN_VOLT_MODE_27) + abs(MAX_VOLT_MODE_27)) / abs(MIN_VOLT_MODE_27) * volt + (count * abs(MIN_VOLT_MODE_27)) / (abs(MIN_VOLT_MODE_27) + abs(MAX_VOLT_MODE_27)))); // искомый индекс в массиве!!! найти минимальное значение от него 26.5-->26 через floor
//             CodeX = (((calibTable->dacValA_m27[y + 1 + 1] - calibTable->dacValA_m27[y]) / (((y + 1 + 1) * STEP_CALIBRATE) - abs(MIN_VOLT_MODE_27) - ((y + 1) * STEP_CALIBRATE) - abs(MIN_VOLT_MODE_27)))) * (volt - ((y + 1) * STEP_CALIBRATE) - abs(MIN_VOLT_MODE_27)) + calibTable->dacValA_m27[y + 1];
             return CodeX;
             break;
         }
         break;
     case 2:
         switch (m)
         {
         case m12:

             // TODO: Нужно ли учитывать Ктр? volt = volt*Ktr
             count = (abs(MIN_VOLT_MODE_12) + abs(MAX_VOLT_MODE_12)) / STEP_CALIBRATE;
             y = (floor(
                 (count * abs(MIN_VOLT_MODE_12)) / (abs(MIN_VOLT_MODE_12) + abs(MAX_VOLT_MODE_12)) / abs(MIN_VOLT_MODE_12) * volt + (count * abs(MIN_VOLT_MODE_12)) / (abs(MIN_VOLT_MODE_12) + abs(MAX_VOLT_MODE_12)))); // искомый индекс в массиве!!! найти минимальное значение от него 26.5-->26 через floor
//             CodeX = (((calibTable->dacValB_m12[y + 1 + 1] - calibTable->dacValB_m12[y]) / (((y + 1 + 1) * STEP_CALIBRATE) - abs(MIN_VOLT_MODE_12) - ((y + 1) * STEP_CALIBRATE) - abs(MIN_VOLT_MODE_12)))) * (volt - ((y + 1) * STEP_CALIBRATE) - abs(MIN_VOLT_MODE_12)) + calibTable->dacValB_m12[y + 1];
             return CodeX;
             break;
         case m27:

             // TODO: Нужно ли учитывать Ктр? volt = volt*Ktr
             count = (abs(MIN_VOLT_MODE_27) + abs(MAX_VOLT_MODE_27)) / STEP_CALIBRATE;
             y = (floor(
                 (count * abs(MIN_VOLT_MODE_27)) / (abs(MIN_VOLT_MODE_27) + abs(MAX_VOLT_MODE_27)) / abs(MIN_VOLT_MODE_27) * volt + (count * abs(MIN_VOLT_MODE_27)) / (abs(MIN_VOLT_MODE_27) + abs(MAX_VOLT_MODE_27)))); // искомый индекс в массиве!!! найти минимальное значение от него 26.5-->26 через floor
//             CodeX = (((calibTable->dacValB_m12[y + 1 + 1] - calibTable->dacValB_m12[y]) / (((y + 1 + 1) * STEP_CALIBRATE) - abs(MIN_VOLT_MODE_27) - ((y + 1) * STEP_CALIBRATE) - abs(MIN_VOLT_MODE_27)))) * (volt - ((y + 1) * STEP_CALIBRATE) - abs(MIN_VOLT_MODE_27)) + calibTable->dacValB_m12[y + 1];
             return CodeX;

             break;
         }

         break;
     }
     return CodeX;
 }
//// TODO: Нужно ли учитывать Ктр? volt = volt*Ktr
//float count = ( abs(MIN_VOLT_MODE_12) + abs(MAX_VOLT_MODE_12) ) / STEP_CALIBRATE;
//uint16_t y = ( floor( (count*abs(MIN_VOLT_MODE_12)) /
//               (abs(MIN_VOLT_MODE_12) + abs(MAX_VOLT_MODE_12)) /
//               abs(MIN_VOLT_MODE_12)*volt
//               +
//               (count*abs(MIN_VOLT_MODE_12)) /
//               (abs(MIN_VOLT_MODE_12) + abs(MAX_VOLT_MODE_12) ) ) ); // искомый индекс в массиве!!! найти минимальное значение от него 26.5-->26 через floor
//uint16_t CodeX = (((calibTable->dacValA_m12[y+1+1]-calibTable->dacValA_m12[y])/(((y+1+1) * STEP_CALIBRATE) -abs( MIN_VOLT_MODE_12)-((y+1) * STEP_CALIBRATE) - abs( MIN_VOLT_MODE_12))))*(volt-((y+1) * STEP_CALIBRATE) - abs( MIN_VOLT_MODE_12))+calibTable->dacValA_m12[y+1];
//return CodeX;
//


    





// printf((CodeX));


// uint16_t volt2dgt(union NVRAM *DevNVRAM, int16_t volt){


// // int16_t minVolt = MIN_VOLT_MODE_12;
// // int16_t maxVolt = MAX_VOLT_MODE_12;


// uint16_t x0 = abs(MIN_VOLT_MODE_12);
// uint16_t x1 = abs(MAX_VOLT_MODE_12);

// float count = (x0+x1)/STEP_CALIBRATE;

// uint16_t y0 = 0;            // начало индекса массива
// uint16_t y1 = count;        // конец индекса массива

// float b = (count*x0)/(x0+x1);
// float a = b/x0;

// int16_t x = volt ;// /Ktr!!!!!!!!!!!!!!
// int y = floor(a*x+b); // искомый индекс в массиве!!! найти минимальное значение от него 26.5-->26 через floor

// // TODO:Найти по найденому индексу значение в массиве!  и найти значение n+1

// uint16_t Ca0 =(uint16_t *) DevNVRAM->calibration_table.dacValA_m12[y];
// uint16_t Ca1 = DevNVRAM->calibration_table.dacValA_m12[y+1];

// uint16_t a0 = (y * STEP_CALIBRATE) - abs( MIN_VOLT_MODE_12);// TODO: не правльно переводит значения!27*200 = 5400
// uint16_t a1 = ((y+1) * STEP_CALIBRATE) -abs( MIN_VOLT_MODE_12);// TODO: не правльно переводит значения!28*200 = 5600



// float CodeX = (((Ca1-Ca0)/(a1-a0)))*(x-a0)+Ca0;
// 	return CodeX;
// // printf((CodeX));





































// //
// ////  Интерполяция
// uint16_t interpolation_dacVAL(uint16_t valDAC_0,uint16_t valDAC_1,uint16_t ya,uint16_t yb,uint16_t yc){
// 	uint16_t determined_value = 0;
// 	return determined_value = (yc - yb) * ((valDAC_0 - valDAC_1) / (ya - yb)) + valDAC_1;
// }


// //uint16_t interpolation_dacVAL(uint16_t xa,uint16_t xb,uint16_t ya,uint16_t yb,uint16_t yc){
// //	uint16_t determined_value = 0;
// // 	m = (xa - xb) / (ya - yb);
// //    xc = (yc - yb) * m + xb;
// //
// //    xc = (yc - yb) * (xa - xb) / (ya - yb) + xb;
// //
// //	return determined_value
// //}
// //--------------------------------------------------------------------------
// // Для перевода напряжения (в вольтах) в код ЦАП
// uint16_t vlt2dgt(uint16_t V) {
// 	uint16_t valDAC;
// 	return valDAC = (V/DAC_REF)*DAC_MAX;
// }
// //--------------------------------------------------------------------------
// uint16_t findValDAC(union NVRAM *DevNVRAM, uint16_t Volt) {
// 	// Проверим что таблица записана и контрольная сумма совпадает
// 	if ((DevNVRAM->calibration_table.MagicNum == MAGIC_KEY_DEFINE)
// 			&& ((DevNVRAM->sector.CheckSum == DevNVRAM->sector.CheckSum))) {
// 		//TODO: c начала найти значений которые есть в таблице

// 		for (int i = 0; i < MAX_VAL_M12; ++i) {

// 		if (DevNVRAM->calibration_table.dacValA_m12[i] == vlt2dgt(Volt)) {
// 					return DevNVRAM->calibration_table.dacValA_m12[i];
// 				}
// 				//TODO: Если нет в таблице, то интерпалировать из ближайшего меньшего и сдедующего
// 			else
// 			{

// 				//TODO Калибровочная не верная!!!

// 			}
// 		}
// 	}
// }
