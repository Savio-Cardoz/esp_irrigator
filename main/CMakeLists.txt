idf_component_register( SRCS "lvgl_demo_ui.c" "display.c" "main.cpp"
                        INCLUDE_DIRS .
                        REQUIRES app_wifi app_clock ds3231 app_common)
