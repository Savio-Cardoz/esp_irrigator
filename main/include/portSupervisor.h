#pragma once

#include <vector>
#include <time.h>
#include <sys/time.h>
#include "esp_log.h"
#include "port.h"

namespace PortSupervisor {
    enum Result {
        OK,
        ERR,
        OK_PORTLIST_NEEDS_SAVING
    };

    /*! \class Port
        \brief Port Object class definition

        This class defines attributes like port number, current state of port, 
        required amount of flow and current flow, interval and duration of port 
        activation, pointers to functions that activate and deactivate the actual
        port.
    */
    class Port {
    public:
        uint8_t portNumber_u8;
        uint32_t switchTime_u32;
        portState_t portState_e;
        uint16_t flowLitresRequired_u16;
        uint16_t flowLitresCurrent_u16;
        uint32_t interval;
        uint32_t duration;
        void (*openPort)(portMap_t number);
        void (*closePort)(portMap_t number);
        bool b_portEnabled;
        Port(uint8_t, uint16_t, uint32_t, uint32_t, void (*)(portMap_t), void (*)(portMap_t));
        void portSwitchState();
        void setSwitchTime(uint32_t swTime);
        uint32_t getSwitchTime();
    };

    class Supervisor {
        public:
            std::vector<Port> portList;
        
            /**
             * Add Port object to the system port list
             * @param portNum Should be an enum.
             * @param reqFlow uint16_t number indicating litres to be allowed to flow through.
             * @param interval uint32_t number indicating seconds of interval between consecutive port open events.
             * @param duration uint32_t number indicating seconds of duration for which port should remain open.
             * @param enablePort function to open port.
             * @param disablePort function to disable port.
             * @returns None 
            */
            void addPort(uint8_t portNum, uint16_t reqFlow, uint32_t interval, uint32_t duration, void (*enablePort)(portMap_t), void (*disablePort)(portMap_t));

            /*
            */
            PortSupervisor::Result runPortCheck();

            /*
            */
            uint32_t getNextPortTriggerTime();
    };


}
