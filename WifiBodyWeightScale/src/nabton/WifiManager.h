/*
 * WifiManager module for nabton
 *
 * Note that by using this module GPIO 13 will be dedicated for signaling GET
 * request unless LOW_POWER_MODE is configured to true
 */

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#ifdef D_ESP8266
    #include "ESP8266WiFiMulti.h"
    #include "WiFiUdp.h"
#endif
#include <Arduino.h>

#ifndef D_ESP8266
    enum wl_status_t
    {
        WL_CONNECTED = 1,
        WL_DISCONNECTED =2
    };
#endif


#define WIFI_NABTON WifiManager::getInstance()

namespace nabton
{
    class WifiManager
    {
        struct ConfigStruct
        {
            const char* ssid;
            const char* ssid_pp;
            const char* broker_ip;
            const char* host;
            int host_port;
            int broker_port;
            bool low_power_mode;
        };

    public:
        static WifiManager* getInstance(const char* ssid,
                                        const char* ssid_pp,
                                        const char* broker_ip,
                                        int broker_port,
                                        const char* host,
                                        int host_port,
                                        bool low_power_mode);
        static WifiManager* getInstance();
        static WifiManager* instance_;
        wl_status_t getStatus();
        bool sendUdpPackage(String message);
        String sendGETPackage(String data, bool ack);
        String getOwnIPAddress();
        bool isLowPowerMode();
        bool updateWifiConnection();
    private:
        WifiManager(ConfigStruct config);
       ~WifiManager();
        #ifdef D_ESP8266
            String addressToString(IPAddress address);
            ESP8266WiFiMulti wifiMulti_;
            WiFiUDP wifiUdp_;
        #endif
        wl_status_t wifiStatus_;
        ConfigStruct config_;

    };


}


#endif
