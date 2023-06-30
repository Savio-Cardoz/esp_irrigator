set(SOURCES main.cpp portSupervisor.cpp)
            
idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS .  "include"
                    PRIV_REQUIRES app_wifi dht20 display) 