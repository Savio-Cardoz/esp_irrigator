# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 11)

add_compile_options(-fexceptions)

# set(EXTRA_COMPONENT_DIRS    "./components/epaper" 
#                             "./components/i2c_comm" 
#                             "./components/dht20" 
#                             "./components/display"
# 			                "./components/app_wifi" )

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(CPPWIFI)