#ifndef RESTCLIENT_H
#define RESTCLIENT_H

#include <ArduinoJson.h>
#include "Client.h"

class RESTClient
{
  public:
    RESTClient(Client* client, int port);

    int connect(const char *host);
    int httpPOST(const char *host, const char *endpoint, String headers, JsonObject& data, String& resp);
    int httpPUT();
    int httpGET(const char *host, const char *endpoint, String headers, String& resp);
    int httpDELETE();

  private:
    Client* _client;
    int _port;

    void close();
    String getHttpCode(String resp);
};

#endif
