#pragma once

#include <stdint.h>
#include "esp_event.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "esp_system.h"

#define CONFIG_BROKER_URL "mqtt://test.mosquitto.org"

typedef struct {
    esp_mqtt_client_handle_t client;

}mqttClientContext;

void mqttClientContextInit(mqttClientContext* clientContext);
void mqttEventHandler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
esp_err_t mqttEventHandlerCallback(esp_mqtt_event_handle_t event);


#ifdef __cplusplus
extern "C" {
#endif

void mqttStart(void);

#ifdef __cplusplus
}
#endif