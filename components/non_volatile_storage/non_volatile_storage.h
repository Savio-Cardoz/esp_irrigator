/*
 * non_volatile_storage.h
 *
 *  Created on: 07-May-2021
 *      Author: Shivang
 */

#ifndef COMPONENTS_NON_VOLATILE_STORAGE_NON_VOLATILE_STORAGE_H_
#define COMPONENTS_NON_VOLATILE_STORAGE_NON_VOLATILE_STORAGE_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "nvs_flash.h"
#include "nvs.h"

#define NON_VOLATILE_STORAGE	"nvs"

void nvs_erase();

esp_err_t nvs_read_u8(const char* key, uint8_t* data);
void nvs_write_u8(const char* key, uint8_t data);

esp_err_t nvs_read_i8(const char* key, int8_t* data);
void nvs_write_i8(const char* key, int8_t data);

esp_err_t nvs_read_u16(const char* key, uint16_t* data);
void nvs_write_u16(const char* key, uint16_t data);

esp_err_t nvs_read_i16(const char* key, int16_t* data);
void nvs_write_i16(const char* key, int16_t data);

esp_err_t nvs_read_u32(const char* key, uint32_t* data);
void nvs_write_u32(const char* key, uint32_t data);

esp_err_t nvs_read_i32(const char* key, int32_t* data);
void nvs_write_i32(const char* key, int32_t data);

esp_err_t nvs_read_u64(const char* key, uint64_t* data);
void nvs_write_u64(const char* key, uint64_t data);

esp_err_t nvs_read_i64(const char* key, int64_t* data);
void nvs_write_i64(const char* key, int64_t data);

esp_err_t nvs_read_float(const char* key, float* data);
void nvs_write_float(const char* key, float* data);

esp_err_t nvs_read_double(const char* key, double* data);
void nvs_write_double(const char* key, double* data);

/** @brief Reads a given length of chars from NVS
 *  @param key  A identifier key as a char array
 *  @param data pointer to a buffer that will hold the chars read from NVS
 *  @param len  size of the data buffer provided
 *  @return esp_err_t ESP_FAIL if the length of the string in NVS is greater than the 
 *          size of buffer (len) given
 */
esp_err_t nvs_read_string(const char* key, char* data, size_t len);


void nvs_write_string(const char* key, char* data, size_t len);


#endif /* COMPONENTS_NON_VOLATILE_STORAGE_NON_VOLATILE_STORAGE_H_ */
