#ifndef NABTON_CLIENT_H
#define NABTON_CLIENT_H

#include "Logger.h"
#include "WifiManager.h"
#include "SerialManager.h"
#include <Arduino.h>

namespace nabton
{
    class NabtonClient
    {
    public:
        enum ClientType
        {
            WIFI_TYPE = 1,
            SERIAL_TYPE = 2
        };
        NabtonClient(const char* deviceName, ClientType clientType);
        ~NabtonClient(){};

        bool sendNabtonDataPackage(int appId, int mId, int nSid, int* sid ,double* data, bool ack = true);
        bool updateStatus();
    private:
        String deviceName_;
        ClientType clientType_;
        WifiManager* wifiManager_;
    };
}


#endif
