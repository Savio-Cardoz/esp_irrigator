#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_spiffs.h"

#include "http_server.h"

const char *TAG = "http_server";

static esp_err_t get_req_handler(httpd_req_t *req)
{
    int response;
    auto response_bytes = getFileContents("index.html");
    response = httpd_resp_send(req, response_bytes.data(), response_bytes.size());
    return response;
}

static esp_err_t get_ico_handler(httpd_req_t *req)
{
    int response;
    ESP_LOGI(TAG, "Toggle output request received");
    auto response_bytes = getFileContents("icon.ico");
    response = httpd_resp_send(req, response_bytes.data(), response_bytes.size());
    return response;
}

static esp_err_t toggle_output_handler(httpd_req_t *req)
{
    int response;
    ESP_LOGI(TAG, "Toggle output request received");
    std::string response_string = "received";
    response = httpd_resp_send(req, response_string.c_str(), response_string.length());
    return response;
}

#if 0
static void ws_async_send(void *arg)
{
    httpd_ws_frame_t ws_pkt;
    struct async_resp_arg *resp_arg = arg;
    httpd_handle_t hd = resp_arg->hd;
    int fd = resp_arg->fd;

    led_state = !led_state;
    gpio_set_level(LED_PIN, led_state);

    char buff[4];
    memset(buff, 0, sizeof(buff));
    sprintf(buff, "%d", led_state);

    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.payload = (uint8_t *)buff;
    ws_pkt.len = strlen(buff);
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;

    static size_t max_clients = CONFIG_LWIP_MAX_LISTENING_TCP;
    size_t fds = max_clients;
    int client_fds[max_clients];

    esp_err_t ret = httpd_get_client_list(server, &fds, client_fds);

    if (ret != ESP_OK)
    {
        return;
    }

    for (int i = 0; i < fds; i++)
    {
        int client_info = httpd_ws_get_fd_info(server, client_fds[i]);
        if (client_info == HTTPD_WS_CLIENT_WEBSOCKET)
        {
            httpd_ws_send_frame_async(hd, client_fds[i], &ws_pkt);
        }
    }
    free(resp_arg);
}

static esp_err_t trigger_async_send(httpd_handle_t handle, httpd_req_t *req)
{
    struct async_resp_arg *resp_arg = malloc(sizeof(struct async_resp_arg));
    resp_arg->hd = req->handle;
    resp_arg->fd = httpd_req_to_sockfd(req);
    return httpd_queue_work(handle, ws_async_send, resp_arg);
}

static esp_err_t handle_ws_req(httpd_req_t *req)
{
    if (req->method == HTTP_GET)
    {
        ESP_LOGI(TAG, "Handshake done, the new connection was opened");
        return ESP_OK;
    }

    httpd_ws_frame_t ws_pkt;
    uint8_t *buf = NULL;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;
    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "httpd_ws_recv_frame failed to get frame len with %d", ret);
        return ret;
    }

    if (ws_pkt.len)
    {
        buf = calloc(1, ws_pkt.len + 1);
        if (buf == NULL)
        {
            ESP_LOGE(TAG, "Failed to calloc memory for buf");
            return ESP_ERR_NO_MEM;
        }
        ws_pkt.payload = buf;
        ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d", ret);
            free(buf);
            return ret;
        }
        ESP_LOGI(TAG, "Got packet with message: %s", ws_pkt.payload);
    }

    ESP_LOGI(TAG, "frame len is %d", ws_pkt.len);

    if (ws_pkt.type == HTTPD_WS_TYPE_TEXT &&
        strcmp((char *)ws_pkt.payload, "toggle") == 0)
    {
        free(buf);
        return trigger_async_send(req->handle, req);
    }
    return ESP_OK;
}
#endif

static httpd_handle_t setup_websocket_server(void)
{
    httpd_handle_t server;

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    httpd_uri_t uri_get = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = get_req_handler,
        .user_ctx = NULL};

    httpd_uri_t uri_toggle_output_button = {
        .uri = "/toggle_output",
        .method = HTTP_POST,
        .handler = toggle_output_handler,
        .user_ctx = NULL};

    httpd_uri_t uri_get_ico = {
        .uri = "/favicon.ico",
        .method = HTTP_GET,
        .handler = get_ico_handler,
        .user_ctx = NULL};

    // httpd_uri_t ws = {
    //     .uri = "/ws",
    //     .method = HTTP_GET,
    //     .handler = handle_ws_req,
    //     .user_ctx = NULL,
    //     .is_websocket = true};

    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_toggle_output_button);
        httpd_register_uri_handler(server, &uri_get_ico);
        // httpd_register_uri_handler(server, &ws);
    }

    return server;
}

void http_server::init()
{
    Storage spiffsStorage("/spiffs");
    _server = setup_websocket_server();
}
