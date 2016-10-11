#include "../WifiManager.h"
#include "../Logger.h"

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
            ConfigManager config;
            config.broker_ip = config_broker_ip;
            config.ssid = config_ssid;
            config.ssid_pp = config_ssid_pp;
            config.broker_port = config_broker_port;
            config.low_power_mode = config_low_power_mode;
            config.host = config_host;
            config.host_port = config_host_port;

            instance_ = new WifiManager(ConfigManager(config));
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

    WifiManager::WifiManager(ConfigManager config): wifiStatus_(WL_DISCONNECTED), config_(config)
    {
        ULOG_ERROR << "Wifi: Stub called. Wifi not supported";
    }

    WifiManager::~WifiManager()
    {
        ULOG_INFO << "WifiManager destructor called";
    }

    bool WifiManager::sendUdpPackage(String message)
    {
        ULOG_ERROR << "Wifi: Stub called sendUdpPackage. Wifi not supported";
        return false;
    }

    String WifiManager::sendGETPackage(String data)
    {
        ULOG_ERROR << "Wifi: Stub called sendGETPackage. Wifi not supported";
        return "Failed";
    }

    wl_status_t WifiManager::getStatus()
    {
        ULOG_ERROR << "Wifi: Stub called getStatus. Wifi not supported";
        return WL_DISCONNECTED;
    }

    String WifiManager::getOwnIPAddress()
    {
        ULOG_ERROR << "Wifi: Stub called getOwnIPAddress. Wifi not supported";
        String ipString = "not.a.real.ip";
        return ipString;
    }

    bool WifiManager::isLowPowerMode()
    {
        return config_.low_power_mode;
    }


}
