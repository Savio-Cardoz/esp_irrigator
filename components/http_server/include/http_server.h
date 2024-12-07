#ifndef _HTTP_SERVER_H
#define _HTTP_SERVER_H

#include <esp_http_server.h>
#include "storage.hpp"

class http_server
{
private:
    httpd_handle_t _server;

public:
    void init();
};

#endif