set(SOURCES main.cpp)
            
idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS .  "include"
                     REQUIRES app_wifi app_clock ds3231 app_common)  

# spiffs_create_partition_image(storage ../spiffs_image FLASH_IN_PROJECT)
