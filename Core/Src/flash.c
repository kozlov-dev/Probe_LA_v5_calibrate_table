/*
 * flash.c
 *
 *  Created on: 17 июн. 2021 г.
 *      Author: Shein
 */

#include "stm32f1xx_hal.h"
#include "logic_calibration_table.h"


/*---------------------------------------------------------------------------*/
extern union NVRAM DevNVRAM;
extern CRC_HandleTypeDef hcrc;

/*---------------------------------------------------------------------------*/
/*
 * @brief   Test function
 */
void flash_fill_calibTable(void)
{
    for (uint16_t i = 0; i < 254; ++i) { 
        DevNVRAM.sector.data[i] = i;
    }
    DevNVRAM.sector.CheckSum = 0xAAAAAAAA;
    DevNVRAM.sector.NWrite = 0;
}

/*---------------------------------------------------------------------------*/
/*
 * @brief   Write calibration table into flash memory
 * @retval  HAL Status
 */
HAL_StatusTypeDef flash_write_calibTable(void)
{
    /* Create some variables */
    volatile uint32_t   addr    = FLASH_TABLE_START_ADDR;
    uint32_t            err     = 0;
    uint32_t            index   = 0;
    uint8_t             status  = HAL_OK;

    /* Compare flash and ram content */
    while ( addr < FLASH_TABLE_STOP_ADDR ) {
        if ( DevNVRAM.data32[index] != *(uint32_t *)addr ) {
            ++err;
        }
        index += 1;
        addr += 4;
    }

    /* If there are differencies -> write new data in flash */
    if (err > 0) {
        /* Unlock flash */
        if ( HAL_FLASH_Unlock() != HAL_OK ) {
            status = HAL_ERROR;
            return status;
        }

        /* Erase flash */
        FLASH_EraseInitTypeDef EraseInitStruct = {
                .TypeErase      = FLASH_TYPEERASE_PAGES,
                .PageAddress    = FLASH_TABLE_START_ADDR,
                .NbPages        = 1
        };
        if ( HAL_FLASHEx_Erase(&EraseInitStruct, &err) != HAL_OK ) {
            status = HAL_ERROR;
            return status;
        }
        if ( err != 0xFFFFFFFF ) {
            status = HAL_ERROR;
            return status;
        }

        /* Reset variables */
        addr    = FLASH_TABLE_START_ADDR;
        err     = 0;
        index   = 0;
        /* Increase number of rewritings */
        DevNVRAM.sector.NWrite += 1;
        /* Calculate calibration table checksum */
        DevNVRAM.sector.CheckSum = HAL_CRC_Calculate( &hcrc,
                                                      (uint32_t*)&DevNVRAM.calibration_table,
                                                      (sizeof(DevNVRAM.calibration_table)/4) );
        /* Write flash */
        while (addr < FLASH_TABLE_STOP_ADDR) {
            if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, DevNVRAM.data32[index]) != HAL_OK) {
                err++;
            }
            index += 1;
            addr += 4;
            /* Wait until flash is busy */
            while ( (FLASH->SR & FLASH_SR_BSY) != 0 )
                ;
        }
        /* Lock flash */
        HAL_FLASH_Lock();
    }
    return status;
}
