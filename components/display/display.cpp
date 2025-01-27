#include <stdio.h>
#include <iostream>
#include "esp_log.h"
#include "display.h"

static const char *TAG = "Display";
extern unsigned char epd_bitmap_wifi[];
extern unsigned char epd_bitmap_noWifi[];
extern icon_desc wifi_connected;
extern icon_desc wifi_notConnected;
extern icon_desc circle;
extern icon_desc outputClosed;
extern icon_desc outputOpen;

/** Temp funtion to print out framebuffer */
void printBuffer(const unsigned char *buffer, unsigned int bytesToPrint)
{
    for (unsigned int i = 0; i < bytesToPrint; i = i + 25)
    {
        for (int j = 0; j < 25; j++)
        {
            printf("%02x ", buffer[i + j]);
        }
        printf("\n");
    }
}

Display::Display()
{
    epd = new Epd;
    frameBuffer = (unsigned char *)malloc(epd->width * epd->height / 8);

    painter = new Paint(frameBuffer, epd->width, epd->height);
    painter->Clear(UNCOLORED);
    epd->LDirInit();
    epd->Clear();
}

void Display::displayOutline()
{
    painter->DrawRectangle(154, 4, 199, 150, COLORED); // System status Data box
    painter->DrawRectangle(1, 156, 199, 199, COLORED); // Environment Data box
    painter->DrawIcon(170, 10, &wifi_notConnected);
    painter->DrawIcon(170, 40, &outputClosed);
    dispNeedRefresh = true;
}

void Display::updateWifiState(bool state)
{
    static bool currState = false;
    if (currState != state)
    {
        currState = state;
        dispNeedRefresh = true;
        if (true == state)
        {
            painter->DrawIcon(170, 10, &wifi_connected);
        }
        else
        {
            painter->DrawIcon(170, 10, &wifi_notConnected);
        }
    }
}

void Display::updateOutputState(bool state)
{
    static bool currState = false;
    if (currState != state)
    {
        currState = state;
        dispNeedRefresh = true;
        if (true == state)
        {
            painter->DrawIcon(170, 40, &outputOpen);
        }
        else
        {
            painter->DrawIcon(170, 40, &outputClosed);
        }
    }
}

void Display::portSwitchTime(uint32_t switchTime_u32)
{
    time_t rawtime = static_cast<unsigned int>(switchTime_u32);
    struct tm ts;
    char buf[80];

    // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
    ts = *localtime(&rawtime);
    strftime(buf, sizeof(buf), "%a %H:%M", &ts);
    painter->DrawStringAt(6, 100, buf, &segoe30, UNCOLORED);
    painter->DrawStringAt(6, 100, buf, &segoe30, COLORED);
    memset(buf, 0, sizeof(buf));
    strftime(buf, sizeof(buf), "%d-%m-%Y", &ts);
    painter->DrawStringAt(6, 123, buf, &segoe30, UNCOLORED);
    painter->DrawStringAt(6, 123, buf, &segoe30, COLORED);
}

void Display::updateEnvData(float humidity, float temperature)
{
    static char datastr[20];
    painter->DrawStringAt(6, 155, datastr, &segoe30, UNCOLORED); // Clear older data in display buffer
    sprintf(datastr, "%0.2f  %0.2f", temperature, humidity);
    painter->DrawStringAt(6, 155, datastr, &segoe30, COLORED);
    dispNeedRefresh = true;
}

void Display::updateFlow(double flow)
{
    static double prevFlow;
    if (prevFlow != flow)
    {
        prevFlow = flow;

        static char datastr[100];
        painter->DrawStringAt(6, 20, datastr, &segoe30, UNCOLORED); // Clear older data in display buffer
        sprintf(datastr, "%0.2f", flow);
        painter->DrawStringAt(6, 20, datastr, &segoe30, COLORED);
        dispNeedRefresh = true;
    }
}

void Display::displayRefresh()
{
    if (dispNeedRefresh)
    {
        ESP_LOGI(TAG, "Display Refresh initiated");
        epd->DisplayPart(frameBuffer);
        dispNeedRefresh = false;
    }
}