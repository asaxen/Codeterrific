#include "WifiManager.h"
#include "Logger.h"


namespace nabton
{
    WifiManager* WifiManager::instance_ = NULL;

    int DEFAULT_GPIO_PIN_WIFI = 13;

    WifiManager* WifiManager::getInstance(const char* config_ssid,
                                          const char* config_ssid_pp,
                                          const char* config_broker_ip,
                                          const int   config_broker_port,
                                          const char* config_host,
                                          const int config_host_port,
                                          bool config_low_power_mode)
    {
        if (!instance_)
        {
            ConfigStruct config;
            config.broker_ip = config_broker_ip;
            config.ssid = config_ssid;
            config.ssid_pp = config_ssid_pp;
            config.broker_port = config_broker_port;
            config.low_power_mode = config_low_power_mode;
            config.host = config_host;
            config.host_port = config_host_port;

            instance_ = new WifiManager(ConfigStruct(config));
        }
        return instance_;
    }

    WifiManager* WifiManager::getInstance()
    {
        if (instance_)
        {
            return instance_;
        }
        else
        {
            return NULL;
        }

    }

    WifiManager::WifiManager(ConfigStruct config): wifiStatus_(WL_DISCONNECTED), config_(config)
    {
        if (!config_.low_power_mode)
            pinMode(DEFAULT_GPIO_PIN_WIFI, OUTPUT);

        wifiMulti_.addAP(config_.ssid, config_.ssid_pp);
        for(int i=0; i < 10; i++)
        {
            if (updateWifiConnection())
                return;
            delay(1000);
        }
        ULOG_ERROR << "Wifi: Failed to connect to: " << config_.ssid;
    }

    WifiManager::~WifiManager()
    {
        ULOG_INFO << "WifiManager destructor called";
    }

    bool WifiManager::updateWifiConnection()
    {
        wifiStatus_ = wifiMulti_.run();
        if (wifiStatus_ == WL_CONNECTED)
        {
            ULOG_INFO << "Wifi: Connected to: " << config_.ssid;
            return true;
        }
        else
            return false;
    }

    bool WifiManager::sendUdpPackage(String message)
    {
        if (getStatus() == WL_CONNECTED)
        {
            if (wifiUdp_.beginPacket(config_.broker_ip, config_.broker_port))
            {
               wifiUdp_.write(message.c_str(), message.length());
               wifiUdp_.endPacket();
               return true;
            }
        }
        return false;
    }

    String WifiManager::sendGETPackage(String data, bool ack)
    {
        String response = "failed";
        if (getStatus() == WL_CONNECTED)
        {
            if (!config_.low_power_mode)
                digitalWrite(DEFAULT_GPIO_PIN_WIFI,HIGH);

            WiFiClient client;
            if (!client.connect(config_.host, config_.host_port))
            {
                ULOG_DEBUG << "Connection to host:" << config_.host << " failed";
                return response;
            }
            // This will send the request to the server
            client.print(String("GET ") + data + " HTTP/1.1\r\n" +
                       "Host: " + String(config_.host) + "\r\n" +
                       "Connection: close\r\n\r\n");

            if (ack)
            {
              delay(1000);

              while(client.available()){
                  response = client.readStringUntil('\r');
              }
            }
            else
            {
              response = "Nack";
            }

            if (!config_.low_power_mode)
                digitalWrite(DEFAULT_GPIO_PIN_WIFI,LOW);
        }

        return response;
    }

    wl_status_t WifiManager::getStatus()
    {
        return WiFi.status();
    }

    String WifiManager::getOwnIPAddress()
    {
        String ipString = addressToString(WiFi.localIP());
        return ipString;
    }

    bool WifiManager::isLowPowerMode()
    {
        return config_.low_power_mode;
    }

    String WifiManager::addressToString(IPAddress address)
    {
        return  String(address[0]) + "." +
                String(address[1]) + "." +
                String(address[2]) + "." +
                String(address[3]);
    }

}
