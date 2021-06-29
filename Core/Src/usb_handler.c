/*
 * usb_handler.c
 *
 *  Created on: Jun 21, 2021
 *      Author: Shein
 */

/*-USER FILES----------------------------------------------------------------*/
#include "main.h"
#include "logic_calibration_table.h"
#include "flash.h"
#include "usb_handler.h"
#include "crc.h"

/*-STM LIBRARY FILES---------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "usbd_cdc_if.h"

/*-STANDART C FILES----------------------------------------------------------*/
#include "stdbool.h"
#include "stdlib.h"

/*-EXTERNAL VARIABLES--------------------------------------------------------*/

extern bool RelayState;
extern bool changeTableFlag;
extern union NVRAM DevNVRAM;

/*---------------------------------------------------------------------------*/

/*
 * @brief   USB package handler
 * @note    Len <= 64
 * @retval  HAL Status
 */
HAL_StatusTypeDef usb_rx_handler(usb_rx_data_type *usb)
{
    if ( usb->is_received != true ||
         usb->is_handled  != false )
    {
        /* Если пакет ещё не принят или уже обработан, то
         * ошибка - обрабатывать либо ещё, либо уже нечего. */
        return HAL_ERROR;
    }
    usb->is_received = false;

    /*-MAIN HANDLER CODE-----------------------------------------------------*/

    if ( usb->len < 1 || usb->len > 64 ) {
        /* Если размер пакета не соответствует нормальному - ошибка */
        return HAL_ERROR;
    }

    /* Буффер для формирования пакета для отправки.
     * Во всех командах пакет отправки не превышает 64 байт, следовательно
     * такого размера будет достаточно. */
    uint8_t     usb_tx_buff[64];
    /* 16-ти битовая переменная для установки значения ЦАП */
    uint16_t    tVal16;
    /* Переменная, содержащая текущую команду, сделана для удобства */
    uint8_t     cmd = usb->buff[0];

    switch (cmd) {
        /* Команда включения реле */
        case 0x01 :
            if ( usb->len >= 2 && (usb->buff[1] == 0x01 || usb->buff[1] == 0x00) ) {

                HAL_GPIO_WritePin(Relay_GPIO_Port, Relay_Pin, GPIO_PIN_SET);

                if ( usb->buff[1] == 0x01 ) {
                    RelayState = m12;
                    printf("RelayState:12V - %d \n", RelayState);
                }
                else if (usb->buff[1] == 0x00) {
                    RelayState = m27;
                    printf("RelayState:27V - %d \n", RelayState);
                }
                SetAllDAC();
                usb_tx_buff[1] = 0x00; // успешно
            }
            else {
                usb_tx_buff[1] = 0x01; // ошибка
            }

            usb_tx_buff[0] = cmd;
            CDC_Transmit_FS(usb_tx_buff, 2);
            break;

        /* Команда калибровки ЦАП А */
        case 0x02 :
            if (usb->len >= 3) {
                resValTIM3_PB4(); // обнуление переменной для проведения калибровки
                resValTIM4_PB6(); // обнуление переменной для проведения калибровки
                memcpy(&tVal16, usb->buff + 1, sizeof(tVal16));
                SetDacA(tVal16);
                printf("DacA: %d\n", tVal16);

                usb_tx_buff[1] = 0x00; // успешно
            }
            else {
                usb_tx_buff[1] = 0x01; // ошибка
            }
            usb_tx_buff[0] = cmd;
            CDC_Transmit_FS(usb_tx_buff, 2);
            break;

        /* Команда калибровки ЦАП В */
        case 0x03 :
            if (usb->len >= 3) {
                resValTIM3_PB4(); // обнуление переменной для проведения калиброки
                resValTIM4_PB6(); // обнуление переменной для проведения калиброки
                memcpy(&tVal16, usb->buff + 1, sizeof(tVal16));
                SetDacB(tVal16);
                printf("DacB: %d \n", tVal16);

                usb_tx_buff[1] = 0x00; // успешно
            }
            else {
                usb_tx_buff[1] = 0x01; // ошибка
            }
            usb_tx_buff[0] = cmd;
            CDC_Transmit_FS(usb_tx_buff, 2);
            break;

        /* Команда запроса значения АЦП */
        case 0x04 :
            tVal16 = GetADC();
            usb_tx_buff[0] = cmd;
            memcpy(usb_tx_buff + 1, &tVal16, sizeof(tVal16));
            CDC_Transmit_FS(usb_tx_buff, 3);
            break;

        /* Команда запроса состояния ЦАПов */
        case 0x05 :
            usb_tx_buff[0] = cmd;
            usb_tx_buff[1] = RelayState;

            tVal16 = GetDacA();
            memcpy(usb_tx_buff + 2, &tVal16, sizeof(tVal16));

            tVal16 = GetDacB();
            memcpy(usb_tx_buff + 4, &tVal16, sizeof(tVal16));

            CDC_Transmit_FS(usb_tx_buff, 6);
            break;

        /* Команда запроса состояния кнопок */
        case 0x06 :
            usb_tx_buff[0] = cmd;
            usb_tx_buff[1] = GetBtnRunState();
            usb_tx_buff[2] = GetBtnUpState();
            usb_tx_buff[3] = GetBtnDownState();
            CDC_Transmit_FS(usb_tx_buff, 4);
            break;

        /* Команда запроса идентификатора устройства */
        case 0x07 :
        {
            /*
            char str[9] = {
                0,0,0,0,0,0,0,0,0
            };
            memcpy(str, "SN", strlen("SN"));
            itoa(SN_DEFINE, str + 2, 16);
            */

            char str[] = {"prb_v0.3"};

            usb_tx_buff[0] = cmd;
            usb_tx_buff[1] = strlen(str);
            memcpy(usb_tx_buff + 2, str, strlen(str));
            CDC_Transmit_FS(usb_tx_buff, strlen(str) + 2);
            break;
        }

        /* Команда запроса измеренной длительности */
        case 0x08 :
        {
            EnableTIM3_PB4();
            uint16_t temp = GetTIM3();
            usb_tx_buff[0] = cmd;
            memcpy(usb_tx_buff + 1, &temp, sizeof(uint16_t));
            CDC_Transmit_FS(usb_tx_buff, 1 + sizeof(uint16_t));
            break;
        }

        /* Команда запроса измеренной длительности */
        case 0x09 :
        {
            EnableTIM4_PB6();
            uint16_t temp = GetTIM4();
            usb_tx_buff[0] = cmd;
            memcpy(usb_tx_buff + 1, &temp, sizeof(uint16_t));
            CDC_Transmit_FS(usb_tx_buff, 1 + sizeof(uint16_t));
            break;
        }

        /* Команда приема калибровочной таблицы */
        case 0x0A :
        {
            /* Константа для обозначения максимального количества передаваемых значений.
             * Определяется максимальным количеством байт, передаваемых по USB за раз.
             * (64(всего) - 6(команда))/2(так как числа 2-х байтовые) = 29*/
            const uint8_t usb_max_calib_value = 29;
            /* Значение для ответа на команду, 0х00 - успешное выполнение */
            usb_tx_buff[6] = 0x00;
            /* В буффер для отправки ответа заносим номер таблицы */
            usb_tx_buff[1] = usb->buff[1];


            /* DEBUG
            static uint8_t deb_num = 0;
            ++deb_num;
            if (deb_num == 4) {
                deb_num = 0;
            }
            ---------*/


            /* dataStartNumber  - номер ячейки, с которой начинается запись
             * dataEndNumber    - номер последней ячейки, в которую должны записываться данные
             * dataOffset       - разница между ними */
            uint16_t dataStartNumber, dataEndNumber, dataOffset;

            memcpy(&dataStartNumber, &usb->buff[2], sizeof(uint16_t));
            memcpy(&dataOffset, &usb->buff[4], sizeof(uint16_t));
            dataEndNumber = dataStartNumber + dataOffset;

            switch (usb->buff[1]) {
                case 0x00 :
                {
                    if (dataStartNumber >= MAX_VAL_M12 ||
                        dataEndNumber > MAX_VAL_M12    ||
                        dataOffset > usb_max_calib_value) {
                        usb_tx_buff[6] = 0x01;   /* Произошла ошибка - возвращаем 0х01 */
                        break;
                    }

                    uint8_t i_usb    = 6;
                    uint16_t i_calib = dataStartNumber;

                    while ( (i_usb < 64) && (i_calib < dataEndNumber) ) {

                        memcpy( &DevNVRAM.calibration_table.dacValA_m12[i_calib],
                                &usb->buff[i_usb],
                                sizeof(uint16_t) );

                        i_usb   += 2;
                        i_calib += 1;
                    }
                    break;

                }
                case 0x01 :
                {
                    if (dataStartNumber >= MAX_VAL_M12 ||
                         dataEndNumber > MAX_VAL_M12    ||
                         dataOffset > usb_max_calib_value) {
                         usb_tx_buff[6] = 0x01;   /* Произошла ошибка - возвращаем 0х01 */
                         break;
                     }

                     uint8_t i_usb    = 6;
                     uint16_t i_calib = dataStartNumber;

                     while ( (i_usb < 64) && (i_calib < dataEndNumber) ) {

                         memcpy( &DevNVRAM.calibration_table.dacValB_m12[i_calib],
                                 &usb->buff[i_usb],
                                 sizeof(uint16_t) );

                         i_usb   += 2;
                         i_calib += 1;
                     }
                     break;

                }
                case 0x02 :
                {
                    if (dataStartNumber >= MAX_VAL_M12 ||
                         dataEndNumber > MAX_VAL_M12    ||
                         dataOffset > usb_max_calib_value) {
                         usb_tx_buff[6] = 0x01;   /* Произошла ошибка - возвращаем 0х01 */
                         break;
                     }

                     uint8_t i_usb    = 6;
                     uint16_t i_calib = dataStartNumber;

                     while ( (i_usb < 64) && (i_calib < dataEndNumber) ) {

                         memcpy( &DevNVRAM.calibration_table.dacValA_m27[i_calib],
                                 &usb->buff[i_usb],
                                 sizeof(uint16_t) );

                         i_usb   += 2;
                         i_calib += 1;
                     }
                     break;

                }
                case 0x03 :
                {
                    if (dataStartNumber >= MAX_VAL_M12 ||
                         dataEndNumber > MAX_VAL_M12    ||
                         dataOffset > usb_max_calib_value) {
                         usb_tx_buff[6] = 0x01;   /* Произошла ошибка - возвращаем 0х01 */
                         break;
                     }

                     uint8_t i_usb    = 6;
                     uint16_t i_calib = dataStartNumber;

                     while ( (i_usb < 64) && (i_calib < dataEndNumber) ) {

                         memcpy( &DevNVRAM.calibration_table.dacValB_m27[i_calib],
                                 &usb->buff[i_usb],
                                 sizeof(uint16_t) );

                         i_usb   += 2;
                         i_calib += 1;
                     }
                     break;

                }
                default :
                    usb_tx_buff[6] = 0x01;   /* Произошла ошибка - возвращаем 0х01 */
                    break;
            };

            usb_tx_buff[0] = cmd;
            memcpy(usb_tx_buff + 2, &dataStartNumber, sizeof(uint16_t));
            memcpy(usb_tx_buff + 4, &dataOffset, sizeof(uint16_t));
            usb_tx_buff[2 + 2 * sizeof(uint16_t) + 1] = 0x00;
            CDC_Transmit_FS(usb_tx_buff, 7);
        }
            break;

        /* Команда запроса CRC 1-4 таблицы */
        case 0x0B :
            break;

        /* Команда приема длины калибровочной таблицы */
        case 0x0C :
            break;

        /* Команда записи во флеш калибровочной таблицы */
        case 0x0D :
        {
            if (usb->len >= 2 && (usb->buff[1] == 0x02))
            {
                changeTableFlag = true;
                if ( flash_write_calibTable() != HAL_OK ) {
                    break;
                }
                usb_tx_buff[1] = 0x00; // успешно
            }
            else {
                usb_tx_buff[1] = 0x01; // ошибка
            }
            usb_tx_buff[0] = cmd;
            CDC_Transmit_FS(usb_tx_buff, 2);
            break;
        }

        /* Команда приема шага калибровки */
        case 0x0E :
            break;

        /* По умолчанию, если прочитанная команда не соответствует ни одной команде
         * в протоколе, отправляем в ответ просто 0, как сигнал ошибки. */
        default:
            CDC_Transmit_FS(0, 1);
    };

    /*-END MAIN HANDLER CODE-------------------------------------------------*/

    usb->is_handled = true;

    return HAL_OK;
}

/*---------------------------------------------------------------------------*/
