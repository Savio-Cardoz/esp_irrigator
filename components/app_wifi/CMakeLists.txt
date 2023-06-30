set(srcs "app_wifi.cpp")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "include"
                       REQUIRES "esp_wifi" "nvs_flash" "esp_event" "esp_netif lwip")