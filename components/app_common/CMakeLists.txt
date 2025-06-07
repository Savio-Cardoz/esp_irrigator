set(srcs "app_common.cpp")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "."
                       REQUIRES non_volatile_storage)