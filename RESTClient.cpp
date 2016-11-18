#include "RESTClient.h"
#include <Arduino.h>

static const int E_OK = 0;
static const int E_NOCONNECTION = -1;

RESTClient::RESTClient(Client* client, int port)
{
  _client = client;
  _port = port;
}

int RESTClient::connect(const char *host)
{
  if(!_client->connect(host, _port))
  {
    return E_NOCONNECTION;
  }

  return E_OK;
}

String RESTClient::getHttpCode(String resp)
{
  int periodIndex = resp.indexOf('.');
  return resp.substring(periodIndex + 3, periodIndex + 6);
}

int RESTClient::httpPOST(const char *host, const char *endpoint, String headers, JsonObject& data, String& resp)
{
  if(connect(host) != E_OK)
  {
    return -1;
    //return "Cant connect";
  }

  char buff[data.measureLength() + 1];
  data.printTo(buff, sizeof(buff));
  String strBuff(buff);
  strBuff += "\n";

  String request = "POST " + (String)endpoint + " HTTP/1.1\r\n" +
    headers +
    "Host: " + (String)host + "\r\n" +
    "Content-Type: application/json\r\n" +
    "Content-Length: " + strBuff.length() + "\r\n\r\n" + strBuff;

  String response = "";
  String chunk = "";
  int limit = 0;
  int sendCount = 0;

  if(_client->connected())
  {
    _client->print(request);

    do
    {
      if (_client->connected())
      {
        chunk = _client->readStringUntil('\n');
        response += chunk;
      }
      limit++;
    } while (chunk.length() > 0 && limit < 100);
  }
  else
  {
    //return "Not connected";
    return -1;
  }
  close();

  if (response.length() > 12)
  {
    String responseCode = getHttpCode(response);
    resp = response.substring(response.indexOf('{'), response.indexOf('}') + 1);

    return responseCode.toInt();
  }

  return -1;
}

int RESTClient::httpGET(const char *host, const char *endpoint, String headers, String& resp)
{
  if(connect(host) != E_OK)
  {
    return -1;
    //return "Cant connect";
  }

  String request = "GET " + (String)endpoint + " HTTP/1.1\r\n" +
    headers +
    "Host: " + (String)host + "\r\n" +
    "Content-Type: application/json\r\n" +
    "Content-Length: 0\r\n\r\n";

  String response = "";
  String chunk = "";
  int limit = 0;
  int sendCount = 0;

  if(_client->connected())
  {
    _client->print(request);

    do
    {
      if (_client->connected())
      {
        chunk = _client->readStringUntil('\n');
        response += chunk;
      }
      limit++;
    } while ((response.length() == 0) || chunk.length() > 0 && limit < 100);
  }
  else
  {
    //return "Not connected";
    return -1;
  }
  close();

  if (response.length() > 12)
  {
    String responseCode = getHttpCode(response);
    resp = response.substring(response.indexOf('{'), response.indexOf('}') + 1);

    return responseCode.toInt();
  }

  return -1;
}

void RESTClient::close(void)
{
  _client->flush();
  _client->stop();
}
