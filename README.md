Проект щупа логического анализатора.
![3D](https://github.com/kozlov-dev/Probe_LA_v5_calibrate_table/blob/main/3D.jpeg "3D")
STM32F103C8T

UPDATE:



**DONE**: Добавить калибровочную таблицу в проект через t.py формируется - необходимо заполнять значениями в logic_calibration_table.с

**DONE**: получить значение V , Поиск по значению напряжения V значение в калибровочной таблице VDAC

--> NOW

**FIXME:**Запись в память не работает

**TODO:** Реализовать прием калибровочной таблицы. 0xOA:Прием калибровочной таблицы 0x0Aoffset[data]

**TODO:** Запись во флеш принятой новой калибровочной таблицы. [0x0D] data: 1B (0x00 - успешно; 0x01 - ошибка при записи)

**FIXME:**Отправлять длину массива кратно 32b. не работает CRC --> HardFault, 

**TODO:** Реализовать отправку CRC 1-4 таблицы 0x0B[CRC(1-4)]

**TODO:** Установка цап реализовано только для канала A и режима m12. Нужно переписать с учетом режима работы. режим работы определяет какую таблицу использовать.	 

**TODO:** Добавить управлением переполюсовкой платы калибровки. добавить пин PA15 + команду в USB

**TODO:** данная реализация плохо отрабатывает! TODO: Нужно переделать на EXTI+TIM

**TODO:** Проверить первое состояниt первоначальное состояние реле 27V | RelayState =|1:m12|0:m27|

**FUTURE:** Сделать возможность изменения длительности измерения. сейчас только 1кГц, нужно с настраиваемой частотой

**FUTURE:** Сформировать калиброчную таблицу через функцию. При отсутствие значения в таблице произвести интерполяцию


:white_check_mark:**BUG**	: ~~В цикле не прерывно идет установка цапов! Формируя цифровой шум.~~ **DONE**: убрано из цикла

:white_check_mark:**BUG**	: ~~Ошибка в работе цап. При переключение в m27 цап принимает значения, но не устанавливает их!~~ **DONE**: добавлена в библиотеку цап двойная отправка команды, проблема устранена.

:white_check_mark:**BUG** : ~~set 4v-> set 5v -> M27 -> set 4v -> M12 -> set 6v (err:DAC не установил значения, но в ядре значения имеются!!!)~~ **DONE**: из-за ошибки работы ЦАП --> добелена функция отправки значений при установке реле

:white_check_mark:**BUG** : ~~Доработать алгоритм установки реле! м.б. добавить в функции реле установку ЦАП по обоим каналам?~~ **DONE**: Добавлено.
:white_check_mark:**DONE**: Добавить серийный номер в щуп
