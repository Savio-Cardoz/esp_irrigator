#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "epaper.h"
#include "epd1in54_V2.h"

#define COLORED     0
#define UNCOLORED   1

class Display {
private:
    Epd* epd;
    Paint* painter;
    unsigned char* frameBuffer;
    bool dispNeedRefresh;

public:
    Display();
    void displayOutline();
    void displaySystemStatus();
    void displayRefresh();
    void updateEnvData(float humidity, float temperature);
};

#endif