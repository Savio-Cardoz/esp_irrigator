/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "display.h"

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "lvgl.h"

void app_main(void)
{
    int counter = 0;
    display_init();

    lv_obj_t *timer_label = lv_label_create(lv_scr_act());
    lv_obj_align(timer_label, LV_ALIGN_TOP_LEFT, 0, 0);

    while (1)
    {
        lv_label_set_text_fmt(timer_label, "%d", counter);
        // raise the task priority of LVGL and/or reduce the handler period can improve the performance
        // The task running lv_timer_handler should have lower priority than that running `lv_tick_inc`
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(66000 * 1));
        counter++;
    }
}
