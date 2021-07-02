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
float count = 0 ;
uint16_t yi = 0 ;
float y = 0 ;
uint16_t Ca0 = 0 ;
uint16_t Ca1 = 0 ;
float a0 = 0 ;
uint16_t a1 = 0 ;
float CodeX = 0 ;

     switch (Ch)
     {
     case 1:
         switch (m)
         {
         case m12:
        	 yi = floor(((float)((float)(((abs(MIN_VOLT_MODE_12)+abs(MAX_VOLT_MODE_12))/STEP_CALIBRATE)*abs(MIN_VOLT_MODE_12))/(float)(abs(MIN_VOLT_MODE_12)+abs(MAX_VOLT_MODE_12)))/(float)(abs(MIN_VOLT_MODE_12)))*volt+((float)(((abs(MIN_VOLT_MODE_12)+abs(MAX_VOLT_MODE_12))/STEP_CALIBRATE)*abs(MIN_VOLT_MODE_12))/(float)(abs(MIN_VOLT_MODE_12)+abs(MAX_VOLT_MODE_12)))); // искомый индекс в массиве!!! найти минимальное значение от него 26.5-->26 через floor
         	 Ca0 =(uint16_t *) calibTable->dacValA_m12[yi];
        	 Ca1 = calibTable->dacValA_m12[yi+1];
        	 y = ((float)((float)(((abs(MIN_VOLT_MODE_12)+abs(MAX_VOLT_MODE_12))/STEP_CALIBRATE)*abs(MIN_VOLT_MODE_12))/(float)(abs(MIN_VOLT_MODE_12)+abs(MAX_VOLT_MODE_12)))/(float)(abs(MIN_VOLT_MODE_12)))*volt+((float)(((abs(MIN_VOLT_MODE_12)+abs(MAX_VOLT_MODE_12))/STEP_CALIBRATE)*abs(MIN_VOLT_MODE_12))/(float)(abs(MIN_VOLT_MODE_12)+abs(MAX_VOLT_MODE_12)));
        	 a0 = (yi* STEP_CALIBRATE) - abs( MIN_VOLT_MODE_12);// FIXME: !!!!!не правильные значение 2080 вместо 2400 TODO: не правльно переводит значения!27*200 = 5400
        	 a1 = ((yi+1) * STEP_CALIBRATE) -abs( MIN_VOLT_MODE_12);// TODO: не правльно переводит значения!28*200 = 5600

        	 CodeX = ((((float)(Ca1-Ca0))/((float)a1-a0)))*(volt-a0)+Ca0;
        	 return CodeX; //2400 -> 2662


             break;
         case m27:

        	 yi = floor(((float)((float)(((abs(MIN_VOLT_MODE_12)+abs(MAX_VOLT_MODE_12))/STEP_CALIBRATE)*abs(MIN_VOLT_MODE_12))/(float)(abs(MIN_VOLT_MODE_12)+abs(MAX_VOLT_MODE_12)))/(float)(abs(MIN_VOLT_MODE_12)))*volt+((float)(((abs(MIN_VOLT_MODE_12)+abs(MAX_VOLT_MODE_12))/STEP_CALIBRATE)*abs(MIN_VOLT_MODE_12))/(float)(abs(MIN_VOLT_MODE_12)+abs(MAX_VOLT_MODE_12)))); // искомый индекс в массиве!!! найти минимальное значение от него 26.5-->26 через floor
         	 Ca0 =(uint16_t *) calibTable->dacValA_m27[yi];
        	 Ca1 = calibTable->dacValA_m27[yi+1];
        	 y = ((float)((float)(((abs(MIN_VOLT_MODE_12)+abs(MAX_VOLT_MODE_12))/STEP_CALIBRATE)*abs(MIN_VOLT_MODE_12))/(float)(abs(MIN_VOLT_MODE_12)+abs(MAX_VOLT_MODE_12)))/(float)(abs(MIN_VOLT_MODE_12)))*volt+((float)(((abs(MIN_VOLT_MODE_12)+abs(MAX_VOLT_MODE_12))/STEP_CALIBRATE)*abs(MIN_VOLT_MODE_12))/(float)(abs(MIN_VOLT_MODE_12)+abs(MAX_VOLT_MODE_12)));
        	 a0 = (yi* STEP_CALIBRATE) - abs( MIN_VOLT_MODE_12);// FIXME: !!!!!не правильные значение 2080 вместо 2400 TODO: не правльно переводит значения!27*200 = 5400
        	 a1 = ((yi+1) * STEP_CALIBRATE) -abs( MIN_VOLT_MODE_12);// TODO: не правльно переводит значения!28*200 = 5600

        	 CodeX = ((((float)(Ca1-Ca0))/((float)a1-a0)))*(volt-a0)+Ca0;
        	 return CodeX; //2400 -> 2662
             break;
         }
         break;
     case 2:
         switch (m)
         {
         case m12:
        	 yi = floor(((float)((float)(((abs(MIN_VOLT_MODE_12)+abs(MAX_VOLT_MODE_12))/STEP_CALIBRATE)*abs(MIN_VOLT_MODE_12))/(float)(abs(MIN_VOLT_MODE_12)+abs(MAX_VOLT_MODE_12)))/(float)(abs(MIN_VOLT_MODE_12)))*volt+((float)(((abs(MIN_VOLT_MODE_12)+abs(MAX_VOLT_MODE_12))/STEP_CALIBRATE)*abs(MIN_VOLT_MODE_12))/(float)(abs(MIN_VOLT_MODE_12)+abs(MAX_VOLT_MODE_12)))); // искомый индекс в массиве!!! найти минимальное значение от него 26.5-->26 через floor
			 Ca0 =(uint16_t *) calibTable->dacValB_m12[yi];
			 Ca1 = calibTable->dacValB_m12[yi+1];
			 y = ((float)((float)(((abs(MIN_VOLT_MODE_12)+abs(MAX_VOLT_MODE_12))/STEP_CALIBRATE)*abs(MIN_VOLT_MODE_12))/(float)(abs(MIN_VOLT_MODE_12)+abs(MAX_VOLT_MODE_12)))/(float)(abs(MIN_VOLT_MODE_12)))*volt+((float)(((abs(MIN_VOLT_MODE_12)+abs(MAX_VOLT_MODE_12))/STEP_CALIBRATE)*abs(MIN_VOLT_MODE_12))/(float)(abs(MIN_VOLT_MODE_12)+abs(MAX_VOLT_MODE_12)));
			 a0 = (yi* STEP_CALIBRATE) - abs( MIN_VOLT_MODE_12);// FIXME: !!!!!не правильные значение 2080 вместо 2400 TODO: не правльно переводит значения!27*200 = 5400
			 a1 = ((yi+1) * STEP_CALIBRATE) -abs( MIN_VOLT_MODE_12);// TODO: не правльно переводит значения!28*200 = 5600

			 CodeX = ((((float)(Ca1-Ca0))/((float)a1-a0)))*(volt-a0)+Ca0;
			 return CodeX; //2400 -> 2662
             break;
         case m27:


        	 yi = floor(((float)((float)(((abs(MIN_VOLT_MODE_12)+abs(MAX_VOLT_MODE_12))/STEP_CALIBRATE)*abs(MIN_VOLT_MODE_12))/(float)(abs(MIN_VOLT_MODE_12)+abs(MAX_VOLT_MODE_12)))/(float)(abs(MIN_VOLT_MODE_12)))*volt+((float)(((abs(MIN_VOLT_MODE_12)+abs(MAX_VOLT_MODE_12))/STEP_CALIBRATE)*abs(MIN_VOLT_MODE_12))/(float)(abs(MIN_VOLT_MODE_12)+abs(MAX_VOLT_MODE_12)))); // искомый индекс в массиве!!! найти минимальное значение от него 26.5-->26 через floor
         	 Ca0 =(uint16_t *) calibTable->dacValB_m27[yi];
        	 Ca1 = calibTable->dacValB_m27[yi+1];
        	 y = ((float)((float)(((abs(MIN_VOLT_MODE_12)+abs(MAX_VOLT_MODE_12))/STEP_CALIBRATE)*abs(MIN_VOLT_MODE_12))/(float)(abs(MIN_VOLT_MODE_12)+abs(MAX_VOLT_MODE_12)))/(float)(abs(MIN_VOLT_MODE_12)))*volt+((float)(((abs(MIN_VOLT_MODE_12)+abs(MAX_VOLT_MODE_12))/STEP_CALIBRATE)*abs(MIN_VOLT_MODE_12))/(float)(abs(MIN_VOLT_MODE_12)+abs(MAX_VOLT_MODE_12)));
        	 a0 = (yi* STEP_CALIBRATE) - abs( MIN_VOLT_MODE_12);// FIXME: !!!!!не правильные значение 2080 вместо 2400 TODO: не правльно переводит значения!27*200 = 5400
        	 a1 = ((yi+1) * STEP_CALIBRATE) -abs( MIN_VOLT_MODE_12);// TODO: не правльно переводит значения!28*200 = 5600

        	 CodeX = ((((float)(Ca1-Ca0))/((float)a1-a0)))*(volt-a0)+Ca0;
        	 return CodeX; //2400 -> 2662

             break;
         }

         break;
     }
     return CodeX;
 }


