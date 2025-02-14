#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_spiffs.h"
#include "cJSON.h"
#include "esp_vfs.h"
#include <ctime>
#include <chrono>
#include "http_server.h"

static const char *REST_TAG = "esp-rest";

#define SCRATCH_BUFSIZE (10240)

#define REST_CHECK(a, str, goto_tag, ...)                                              \
    do                                                                                 \
    {                                                                                  \
        if (!(a))                                                                      \
        {                                                                              \
            ESP_LOGE(REST_TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            goto goto_tag;                                                             \
        }                                                                              \
    } while (0)

#define FILE_PATH_MAX (ESP_VFS_PATH_MAX + 128)

typedef struct rest_server_context
{
    char base_path[ESP_VFS_PATH_MAX + 1];
    char scratch[SCRATCH_BUFSIZE];
} rest_server_context_t;

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
    auto response_bytes = getFileContents("icon.ico");
    response = httpd_resp_send(req, response_bytes.data(), response_bytes.size());
    return response;
}

static esp_err_t getBootstrapMinCss(httpd_req_t *req)
{
    int response;
    auto response_bytes = getFileContents("bootstrap.min.css");
    response = httpd_resp_send(req, response_bytes.data(), response_bytes.size());
    return response;
}

static esp_err_t getJqueryMinJs(httpd_req_t *req)
{
    int response;
    auto response_bytes = getFileContents("jquery.min.js");
    response = httpd_resp_send(req, response_bytes.data(), response_bytes.size());
    return response;
}

static esp_err_t getBootstrapBundleMinJs(httpd_req_t *req)
{
    int response;
    auto response_bytes = getFileContents("bootstrap.bundle.min.js");
    response = httpd_resp_send(req, response_bytes.data(), response_bytes.size());
    return response;
}

static esp_err_t getSystemTime(httpd_req_t *req)
{
    int response;
    std::time_t today_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    char timeString[21];
    std::strftime(timeString, 20, "%FT%TZ", std::gmtime(&today_time));
    response = httpd_resp_send(req, timeString, 20);
    return response;
}

static esp_err_t updateConfigHandler(httpd_req_t *req)
{
    int total_len = req->content_len;
    int cur_len = 0;
    char *buf = ((rest_server_context_t *)(req->user_ctx))->scratch;
    int received = 0;
    if (total_len >= SCRATCH_BUFSIZE)
    {
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    }
    while (cur_len < total_len)
    {
        received = httpd_req_recv(req, buf + cur_len, total_len);
        if (received <= 0)
        {
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
            return ESP_FAIL;
        }
        cur_len += received;
    }
    buf[total_len] = '\0';

    http_server &serverInstance = http_server::getInstance();
    serverInstance.notifyObserver(buf);

    httpd_resp_sendstr(req, "Post control value successfully");
    return ESP_OK;
}

static httpd_handle_t setup_websocket_server(void)
{
    httpd_handle_t server;

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    rest_server_context_t *rest_context = static_cast<rest_server_context_t *>(calloc(1, sizeof(rest_server_context_t)));
    strlcpy(rest_context->base_path, "/www", sizeof(rest_context->base_path));

    httpd_uri_t uri_get = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = get_req_handler,
        .user_ctx = NULL};

    httpd_uri_t uri_toggle_output_button = {
        .uri = "/config",
        .method = HTTP_POST,
        .handler = updateConfigHandler,
        .user_ctx = rest_context};

    httpd_uri_t uri_get_ico = {
        .uri = "/favicon.ico",
        .method = HTTP_GET,
        .handler = get_ico_handler,
        .user_ctx = NULL};

    httpd_uri_t uri_system_time = {
        .uri = "/systemTime",
        .method = HTTP_GET,
        .handler = getSystemTime,
        .user_ctx = NULL};

    httpd_uri_t jquery_min_js = {
        .uri = "/jquery.min.js",
        .method = HTTP_GET,
        .handler = getJqueryMinJs,
        .user_ctx = NULL};

    httpd_uri_t bootstrapBundleMinJs = {
        .uri = "/bootstrap.bundle.min.js",
        .method = HTTP_GET,
        .handler = getBootstrapBundleMinJs,
        .user_ctx = NULL};

    httpd_uri_t bootstrapMinCss = {
        .uri = "/bootstrap.min.css",
        .method = HTTP_GET,
        .handler = getBootstrapMinCss,
        .user_ctx = NULL};

    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_toggle_output_button);
        httpd_register_uri_handler(server, &uri_get_ico);
        httpd_register_uri_handler(server, &uri_system_time);
        httpd_register_uri_handler(server, &jquery_min_js);
        httpd_register_uri_handler(server, &bootstrapBundleMinJs);
        httpd_register_uri_handler(server, &bootstrapMinCss);
    }

    return server;
}

void http_server::init()
{
    Storage spiffsStorage("/spiffs");
    _server = setup_websocket_server();
}

void http_server::registerConfigParser(std::function<void(const char *buffer)> const &function)
{
    this->cb_configParser = function;
}