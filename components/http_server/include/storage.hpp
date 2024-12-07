#ifndef __STORAGE_H__
#define __STORAGE_H__

#include <stdio.h>
#include <string>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "mbedtls/md5.h"

static const char *funcName = "example";

class Storage
{
public:
    Storage(std::string path)
    {
        esp_vfs_spiffs_conf_t conf = {
            .base_path = path.c_str(),
            .partition_label = NULL,
            .max_files = 5,
            .format_if_mount_failed = false};

        esp_err_t ret = esp_vfs_spiffs_register(&conf);

        if (ret != ESP_OK)
        {
            if (ret == ESP_FAIL)
            {
                ESP_LOGE(funcName, "Failed to mount or format filesystem");
            }
            else if (ret == ESP_ERR_NOT_FOUND)
            {
                ESP_LOGE(funcName, "Failed to find SPIFFS partition");
            }
            else
            {
                ESP_LOGE(funcName, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
            }
            return;
        }

        size_t total = 0, used = 0;
        ret = esp_spiffs_info(NULL, &total, &used);
        if (ret != ESP_OK)
        {
            ESP_LOGE(funcName, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
        }
        else
        {
            ESP_LOGI(funcName, "Partition size: total: %d, used: %d", total, used);
        }
    }

    // std::string getConfig()
    // {
    //     char data[60];

    //     ESP_LOGI(funcName, "Reading hello.txt");

    //     // Open for reading hello.txt
    //     FILE *f = fopen("/spiffs/hello.txt", "r");
    //     if (f == NULL)
    //     {
    //         ESP_LOGE(funcName, "Failed to open hello.txt");
    //         return "";
    //     }
    //     while (fgets(data, sizeof(data), f) != NULL)
    //         ;
    //     fclose(f);

    //     // Display the read contents from the file
    //     ESP_LOGI(funcName, "Read from hello.txt: %s", data);
    //     return data;
    // }
};

std::string getFileContents(std::string filename);

#endif