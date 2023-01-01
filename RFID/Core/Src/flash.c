/*
 * flash.c
 *
 *  Created on: Dec 5, 2022
 *      Author: hoang
 */


#include <flash.h>

void Flash_Erase(uint32_t address){
	HAL_FLASH_Unlock();
	FLASH_EraseInitTypeDef	EraseInit;
	EraseInit.Banks  = 1;
	EraseInit.NbPages = 1;
	EraseInit.PageAddress = address;
	EraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
	uint32_t pageerr;
	HAL_FLASHEx_Erase(&EraseInit ,&pageerr );
	HAL_FLASH_Lock();
}
void Flash_Write_2Byte(uint32_t address , uint16_t data){
	HAL_FLASH_Unlock();
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address, data);
	HAL_FLASH_Lock();
}
void Flash_Write_4Byte(uint32_t address , uint32_t data){
	HAL_FLASH_Unlock();
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, data);
	HAL_FLASH_Lock();
}
void Flash_Write_Array(uint32_t address , uint8_t *data , uint16_t length){
	HAL_FLASH_Unlock();
	for( uint16_t i = 0 ; i< length ; i = i+2){
		Flash_Write_2Byte(address +i,*(uint16_t *)(data +i) );
	}
	HAL_FLASH_Lock();
}

uint8_t  Flash_Read_Byte(uint32_t address){
	return *(__IO uint8_t *)(address);
}
uint16_t Flash_Read_2Byte(uint32_t address){
	return *(__IO uint16_t *)(address);
}
uint32_t Flash_Read_4Byte(uint32_t address){
	return *(__IO uint32_t *)(address);
}
uint64_t Flash_Read_8Byte(uint32_t address){
	return *(__IO uint64_t *)(address);
}
void  Flash_Read_Array(uint32_t address, uint8_t *data , uint16_t length){
	for(uint16_t i = 0; i< length ; i++){
		*(data +i) = *(__IO uint8_t *)(address+i);
	}
}
