#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "epaper.h"
#include "epd1in54_V2.h"

#define COLORED 0
#define UNCOLORED 1

class Display
{
private:
    Epd *epd;
    Paint *painter;
    unsigned char *frameBuffer;
    bool dispNeedRefresh;

public:
    Display();
    void displayOutline();
    void displaySystemStatus();
    void displayRefresh();
    void portSwitchTime(uint32_t switchTime_u32);
    void updateEnvData(float humidity, float temperature);
    void updateWifiState(bool state);
    void updateOutputState(bool state);
    void updateFlow(double flow);
    void updateDesiredFlow(uint16_t flow);
};

#endif