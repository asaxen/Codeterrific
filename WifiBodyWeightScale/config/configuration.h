#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "../src/nabton/NabtonClient.h"

static const int   CONFIG_BAUDRATE = 9600;
static const char* CONFIG_SSID = "myWifiSSID";
static const char* CONFIG_SSID_PP = "myWifiPassword";
// The Ip for your local computer that will receive debug message over WIFI
static const char* CONFIG_BROKER_IP = "192.168.1.198";
static const int   CONFIG_BROKER_PORT = 4444;
static const char* CONFIG_HOST = "myGetRequestServer.com"; //Where to send your GET-request
static const int   CONFIG_HOST_PORT = 80;

//==============================================
//                 Device specific
//==============================================
static const int CONFIG_REPORT_FREQUENCY_MILLISECONDS = 1000;
//In low power mode the WIFI debug messages are not sent
static const bool CONFIG_LOW_POWER_MODE = false;
static const nabton::NabtonClient::ClientType CONFIG_CLIENT_TYPE = nabton::NabtonClient::ClientType::WIFI_TYPE;

#endif
