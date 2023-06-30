/*
 * non_volatile_storage.c
 *
 *  Created on: 07-May-2021
 *      Author: Shivang
 */
#include "esp_log.h"
#include "esp_err.h"
#include "non_volatile_storage.h"

static const char *TAG = "non_volatile_storage";

void nvs_erase()
{
	nvs_handle_t my_handle;
	nvs_open(NON_VOLATILE_STORAGE, NVS_READWRITE, &my_handle);
	nvs_erase_all(my_handle);
	nvs_commit(my_handle);
	nvs_close(my_handle);
}

esp_err_t nvs_read_u8(const char* key, uint8_t* data)
{
	nvs_handle_t my_handle;
	nvs_open(NON_VOLATILE_STORAGE, NVS_READONLY, &my_handle);
	esp_err_t err = nvs_get_u8(my_handle, key, data);
	nvs_close(my_handle);
	return err;
}
void nvs_write_u8(const char* key, uint8_t data)
{
	nvs_handle_t my_handle;
	nvs_open(NON_VOLATILE_STORAGE, NVS_READWRITE, &my_handle);
	nvs_set_u8(my_handle, key, data);
	nvs_commit(my_handle);
	nvs_close(my_handle);
}

esp_err_t nvs_read_i8(const char* key, int8_t* data)
{
	nvs_handle_t my_handle;
	nvs_open(NON_VOLATILE_STORAGE, NVS_READONLY, &my_handle);
	esp_err_t err = nvs_get_i8(my_handle, key, data);
	nvs_close(my_handle);
	return err;
}
void nvs_write_i8(const char* key, int8_t data)
{
	nvs_handle_t my_handle;
	nvs_open(NON_VOLATILE_STORAGE, NVS_READWRITE, &my_handle);
	nvs_set_i8(my_handle, key, data);
	nvs_commit(my_handle);
	nvs_close(my_handle);
}

esp_err_t nvs_read_u16(const char* key, uint16_t* data)
{
	nvs_handle_t my_handle;
	nvs_open(NON_VOLATILE_STORAGE, NVS_READONLY, &my_handle);
	esp_err_t err = nvs_get_u16(my_handle, key, data);
	nvs_close(my_handle);
	return err;
}
void nvs_write_u16(const char* key, uint16_t data)
{
	nvs_handle_t my_handle;
	nvs_open(NON_VOLATILE_STORAGE, NVS_READWRITE, &my_handle);
	nvs_set_u16(my_handle, key, data);
	nvs_commit(my_handle);
	nvs_close(my_handle);
}

esp_err_t nvs_read_i16(const char* key, int16_t* data)
{
	nvs_handle_t my_handle;
	nvs_open(NON_VOLATILE_STORAGE, NVS_READONLY, &my_handle);
	esp_err_t err = nvs_get_i16(my_handle, key, data);
	nvs_close(my_handle);
	return err;
}
void nvs_write_i16(const char* key, int16_t data)
{
	nvs_handle_t my_handle;
	nvs_open(NON_VOLATILE_STORAGE, NVS_READWRITE, &my_handle);
	nvs_set_i16(my_handle, key, data);
	nvs_commit(my_handle);
	nvs_close(my_handle);
}

esp_err_t nvs_read_u32(const char* key, uint32_t* data)
{
	nvs_handle_t my_handle;
	nvs_open(NON_VOLATILE_STORAGE, NVS_READONLY, &my_handle);
	esp_err_t err = nvs_get_u32(my_handle, key, data);
	nvs_close(my_handle);
	return err;
}
void nvs_write_u32(const char* key, uint32_t data)
{
	nvs_handle_t my_handle;
	nvs_open(NON_VOLATILE_STORAGE, NVS_READWRITE, &my_handle);
	nvs_set_u32(my_handle, key, data);
	nvs_commit(my_handle);
	nvs_close(my_handle);
}

esp_err_t nvs_read_i32(const char* key, int32_t* data)
{
	nvs_handle_t my_handle;
	nvs_open(NON_VOLATILE_STORAGE, NVS_READONLY, &my_handle);
	esp_err_t err = nvs_get_i32(my_handle, key, data);
	nvs_close(my_handle);
	return err;
}
void nvs_write_i32(const char* key, int32_t data)
{
	nvs_handle_t my_handle;
	nvs_open(NON_VOLATILE_STORAGE, NVS_READWRITE, &my_handle);
	nvs_set_i32(my_handle, key, data);
	nvs_commit(my_handle);
	nvs_close(my_handle);
}

esp_err_t nvs_read_u64(const char* key, uint64_t* data)
{
	nvs_handle_t my_handle;
	nvs_open(NON_VOLATILE_STORAGE, NVS_READONLY, &my_handle);
	esp_err_t err = nvs_get_u64(my_handle, key, data);
	nvs_close(my_handle);
	return err;
}
void nvs_write_u64(const char* key, uint64_t data)
{
	nvs_handle_t my_handle;
	nvs_open(NON_VOLATILE_STORAGE, NVS_READWRITE, &my_handle);
	nvs_set_u64(my_handle, key, data);
	nvs_commit(my_handle);
	nvs_close(my_handle);
}

esp_err_t nvs_read_i64(const char* key, int64_t* data)
{
	nvs_handle_t my_handle;
	nvs_open(NON_VOLATILE_STORAGE, NVS_READONLY, &my_handle);
	esp_err_t err = nvs_get_i64(my_handle, key, data);
	nvs_close(my_handle);
	return err;
}
void nvs_write_i64(const char* key, int64_t data)
{
	nvs_handle_t my_handle;
	nvs_open(NON_VOLATILE_STORAGE, NVS_READWRITE, &my_handle);
	nvs_set_i64(my_handle, key, data);
	nvs_commit(my_handle);
	nvs_close(my_handle);
}

esp_err_t nvs_read_float(const char* key, float* data)
{
	nvs_handle_t my_handle;
	nvs_open(NON_VOLATILE_STORAGE, NVS_READONLY, &my_handle);
	size_t len = sizeof(float);
	esp_err_t err = nvs_get_blob(my_handle, key, data, &len);
	nvs_close(my_handle);
	return err;
}
void nvs_write_float(const char* key, float* data)
{
	nvs_handle_t my_handle;
	nvs_open(NON_VOLATILE_STORAGE, NVS_READWRITE, &my_handle);
	nvs_set_blob(my_handle, key, data, sizeof(float));
	nvs_commit(my_handle);
	nvs_close(my_handle);
}

esp_err_t nvs_read_double(const char* key, double* data)
{
	nvs_handle_t my_handle;
	nvs_open(NON_VOLATILE_STORAGE, NVS_READONLY, &my_handle);
	size_t len = sizeof(double);
	esp_err_t err = nvs_get_blob(my_handle, key, data, &len);
	nvs_close(my_handle);
	return err;
}
void nvs_write_double(const char* key, double* data)
{
	nvs_handle_t my_handle;
	nvs_open(NON_VOLATILE_STORAGE, NVS_READWRITE, &my_handle);
	nvs_set_blob(my_handle, key, data, sizeof(double));
	nvs_commit(my_handle);
	nvs_close(my_handle);
}

esp_err_t nvs_read_string(const char* key, char* data, size_t len) {
	nvs_handle_t handler;
	esp_err_t err;

	nvs_open(NON_VOLATILE_STORAGE, NVS_READONLY, &handler);
	
	size_t length = 0;
    err = nvs_get_blob(handler, key, NULL, &length);        // Read the length of the ssid string in NVS
    if ( (err == ESP_ERR_NVS_NOT_FOUND) || (err != ESP_OK) ){
		nvs_close(handler);
        return err;
    }
    
    if ( len < length )		// Checking if the length of the string in NVS (length) can be accomodated in the size of the data buffer
		return ESP_FAIL;

    err = nvs_get_blob(handler, key, data, &length);
	nvs_close(handler);
	return err;
}

void nvs_write_string(const char* key, char* data, size_t len)
{
	nvs_handle_t my_handle;
	nvs_open(NON_VOLATILE_STORAGE, NVS_READWRITE, &my_handle);
	nvs_set_blob( my_handle, key, data, len);
	nvs_commit(my_handle);
	nvs_close(my_handle);
}
