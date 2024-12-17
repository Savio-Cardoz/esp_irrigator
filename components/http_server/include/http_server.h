#ifndef _HTTP_SERVER_H
#define _HTTP_SERVER_H

#include <esp_http_server.h>
#include "storage.hpp"
#include <functional>
#include <mutex>
#include "observerPattern.hpp"

class http_server : public Subject
{
private:
    httpd_handle_t _server;

    static http_server *httpServerInstance;
    static std::mutex mutexHttpServer;
    http_server() = default;

public:
    std::function<void(const char *buffer)> cb_configParser;
    ~http_server() = default;
    void init();
    void registerConfigParser(std::function<void(const char *buffer)> const &function);

    static http_server &getInstance()
    {
        if (!httpServerInstance)
        {
            httpServerInstance = new http_server();
        }
        return *httpServerInstance;
    }

    http_server(http_server const &) = delete;
    void operator=(http_server const &) = delete;
};

#endif