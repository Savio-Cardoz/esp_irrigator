set(SOURCES "http_server.cpp" "storage.cpp")

idf_component_register( SRCS ${SOURCES}
                        INCLUDE_DIRS "include"
                        REQUIRES esp-tls esp_http_server spiffs nvs_flash)