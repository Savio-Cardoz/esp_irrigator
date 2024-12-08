set(srcs "epdif.cpp" "epdpaint.cpp" "font8.c" "font12.c" "font16.c" "font20.c" "font24.c" "fontArial26.c" "aller_bld.c" segoe12.c segoe25.c segoe20.c segoe30.c icons.c)

if(CONFIG_EPD_1_54_V2)
    list(APPEND srcs "epd1in54_V2.cpp")
elseif(CONFIG_EPD_2_13_D)
    list(APPEND srcs "epd2in13d.cpp")
elseif(CONFIG_EPD_2_7)
    list(APPEND srcs "epd2in7.cpp")
endif()

idf_component_register(SRCS "icons.c" "${srcs}"
                       INCLUDE_DIRS "include"
                       REQUIRES esp_common driver pinmap)