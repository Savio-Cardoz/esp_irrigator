#pragma once


#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>
#include <sys/time.h>
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_sntp.h"

void time_sync_notification_cb(struct timeval *tv);
void initialize_sntp(void);
void obtain_time(void);

#ifdef __cplusplus
}
#endif