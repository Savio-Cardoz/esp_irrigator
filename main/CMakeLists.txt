set(SOURCES main.cpp portSupervisor.cpp vault.cpp sntp.cpp mqtt.c)
            
idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS .  "include"
                    REQUIRES app_wifi dht20 display non_volatile_storage mqtt spiffs json http_server sampleComponent dht22)  

spiffs_create_partition_image(storage ../spiffs_image FLASH_IN_PROJECT)
