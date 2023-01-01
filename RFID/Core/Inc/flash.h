/*
 * flash.h
 *
 *  Created on: Dec 5, 2022
 *      Author: hoang
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#include "stm32f1xx_hal.h"
#include "stdint.h"
#include "string.h"

void Flash_Erase(uint32_t address);
void Flash_Write_2Byte(uint32_t address , uint16_t data);
void Flash_Write_4Byte(uint32_t address , uint32_t data);
void Flash_Write_Array(uint32_t address , uint8_t *data , uint16_t length);

uint8_t  Flash_Read_Byte(uint32_t address);
uint16_t Flash_Read_2Byte(uint32_t address);
uint32_t Flash_Read_4Byte(uint32_t address);
uint64_t Flash_Read_8Byte(uint32_t address);
void  Flash_Read_Array(uint32_t address, uint8_t *data , uint16_t length);

#endif /* INC_FLASH_H_ */
