#include "mqtt.h"

static const char *TAG = "MQTT_EXAMPLE";

static void log_error_if_nonzero(const char * message, int error_code)
{
    if (error_code != 0) {
        // ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
        //printf("Last error %s: 0x%x", message, error_code);
    }
}

esp_err_t mqttEventHandlerCallback(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            // ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            //printf("MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_publish(client, "/Cardoz/qos1", "data_3", 0, 1, 0);
            // ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            //printf("sent publish successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, "/Cardoz/qos0", 0);
            // ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
            //printf("sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, "/Cardoz/qos1", 1);
            // ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
            //printf("sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_unsubscribe(client, "/Cardoz/qos1");
            // ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
            //printf("sent unsubscribe successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_DISCONNECTED:
            // ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            //printf("MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            // ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            //printf("MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            msg_id = esp_mqtt_client_publish(client, "/Cardoz/qos0", "data", 0, 0, 0);
            // ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            //printf("sent publish successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            // ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            //printf("MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            // ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            //printf("MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            // ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            //printf("MQTT_EVENT_DATA");
            //printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            //printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            // ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            //printf("MQTT_EVENT_ERROR");
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
                log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
                log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
                ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
            }
            break;
        default:
            // ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            //printf("Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}

void mqttEventHandler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    // ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    //printf("Event dispatched from event loop base=%s, event_id=%ld", base, event_id);
    mqttEventHandlerCallback(event_data);
}

void mqttStart(void) {
    esp_mqtt_client_config_t mqtt_cfg;
    mqtt_cfg.broker.address.uri = CONFIG_BROKER_URL;

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqttEventHandler, client);
    esp_mqtt_client_start(client);
}