/*
 * usb_handler.h
 *
 *  Created on: Jun 21, 2021
 *      Author: Shein
 */

#ifndef INC_USB_HANDLER_H_
#define INC_USB_HANDLER_H_

HAL_StatusTypeDef   usb_rx_handler(usb_rx_data_type *usb);



/*  Command system
    0x01 - включение реле                           data: 1B (0x00 - выключить; 0x01 - включить)    answer: 0x01 + 1B status
    0x02 - ЦАП канал А                              data: 2B (значение)                             answer: 0x02 + 1B status
    0x03 - ЦАП канал B                              data: 2B (значение)                             answer: 0x03 + 1B status
    0x04 - АЦП запрос значения                      data: 0B                                        answer: 0x04 + 2B value
    0x05 - запрос состояния (Relay, DA, DB)         data: 0B                                        answer: 0x05 + 1B состиояние реле + 2B значение ЦАП канал А + 2B значение ЦАП канал B
    0x06 - запрос состояния кнопок (Run, Up, Down)  data: 0B                                        answer: 0x06 + 1B состояние кнопки Run + 1B состояние кнопки Up + 1B состояние кнопки Down
    0x07 - запрос ID устройства
    ("SN+WW+YY+NNN")                                    data: 0B                                        answer: 0x07 + 9B ID ("SN+WW+YY+NNN") SN1121001- 11 неделя-21год - 001 порядковый номер изготовления
    0х08 - запрос измеренной длительности           data: 0B (0x00 - сработал; 0x01 - не сработал)  answer: 0x08 + 1B status
    0х09 - запрос измеренной длительности           data: 0B (0x00 - сработал; 0x01 - не сработал)  answer: 0x08 + 1B status
    --------------------------------------------------------------------------
    0х0C - Прием длины калибровочной таблицы [0x0C][Длина][???]             0x00 - сработал; 0x01 - не сработал)    answer: 0x09 + 1B status
    0х0B - Отправка CRC 1-4 таблицы [0x0B][1-4][CRC(1-4)]   answer:
    0х0A - Прием калибровочной таблицы [0x0A][1-4][offset][count][data]     answer: [0x0A]+[1-4]+[offset]+[count]+[status] (0x00 - сработал; 0x01 - не сработал)
            u8 Buf[0] = 0x0A
            u8 Buf[1] = 0x00-0x03
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
                Buf[count] = data[n]    u8

    0х0D - Запись во флеш калибровочной таблицы [0x0D] data: 1B (0x00 - успешно; 0x01 - ошибка при записи)  answer: 0x0D + 1B status
    0x0E - Прием значения шага калибровочной таблицы
    --------------------------------------------------------------------------
    status
    0x00 - успешно
    0x01 - ошибка
    --------------------------------------------------------------------------
    Калибровка
    На вход щупа подается семетричная пила с частотой 1кГц с оффестом установленным на ип.
    после компарирования сигнала МК измеряет длительность импульса
    т.к. 1 кГц соотвествет длительности в 500 мс то это означает что мы попапали в полуку офсета. */
/*---------------------------------------------------------------------------*/

#endif /* INC_USB_HANDLER_H_ */
