#include <stdio.h>
#include<iostream>
#include "esp_log.h"
#include "display.h"

static const char *TAG = "Display";
extern unsigned char epd_bitmap_wifi[];
extern unsigned char epd_bitmap_noWifi[];
extern icon_desc wifi_connected;
extern icon_desc wifi_notConnected;
extern icon_desc circle;

/** Temp funtion to print out framebuffer */
void printBuffer(const unsigned char* buffer, unsigned int bytesToPrint) {
    for (unsigned int i = 0; i < bytesToPrint; i = i + 25) {
        for (int j = 0; j < 25; j++) {
            printf("%02x ", buffer[i + j]);
        }
        printf("\n");
    }
}

Display::Display() {
    epd = new Epd;
    frameBuffer = (unsigned char*)malloc(epd->width * epd->height / 8);
    
    painter = new Paint(frameBuffer, epd->width, epd->height);
    painter->Clear(UNCOLORED);
    epd->LDirInit();
    epd->Clear();
}

void Display::displayOutline() {
    painter->DrawRectangle(154, 4, 199, 150, COLORED);   // System status Data box
    painter->DrawRectangle(1, 156, 199, 199, COLORED);   // Environment Data box
    painter->DrawIcon(170, 5, &wifi_connected);
    dispNeedRefresh = true;
}

void Display::displaySystemStatus() {
    
}

void Display::updateEnvData(float humidity, float temperature) {
    static char datastr[20];
    painter->DrawStringAt(6, 155, datastr, &segoe30, UNCOLORED);    // Clear older data in display buffer
    sprintf(datastr, "%0.2f  %0.2f", temperature, humidity);
    // painter->DrawFilledRectangle(1, 156, 199, 199, UNCOLORED);
    painter->DrawStringAt(6, 155, datastr, &segoe30, COLORED);
    dispNeedRefresh = true;
}

void Display::displayRefresh() {
    if(dispNeedRefresh) {
        ESP_LOGI(TAG, "Display Refresh initiated");
        epd->DisplayPart(frameBuffer);
        dispNeedRefresh = false;
    }
}