#include "NabtonClient.h"

namespace nabton
{
    NabtonClient::NabtonClient(const char* deviceName, ClientType clientType) : wifiManager_(NULL)
    {
        deviceName_ = String(deviceName);
        clientType_ = clientType;
    }

    bool NabtonClient::sendNabtonDataPackage(int appId, int mId, int nSid, int* sid ,double* data, bool ack)
    {

        String url = String("/index.php") + String("?appid=") + String(appId)
                     + String("&mid=") + String(mId) + String("&nsid=") + String(nSid);

        for (int i=0; i < nSid; i++)
        {
            url = url + String("&sid") + String(i+1) +String("=")+ String(sid[i])
                      + String("&dat") + String(i+1) +String("=")+ String(data[i],6);
        }

        url = url + String("&name=")+deviceName_;

        if (clientType_ == ClientType::WIFI_TYPE)
        {
            url = url + String("&ip=")+WIFI_NABTON->getOwnIPAddress();
            WLOG_INFO << "URL: " << url.c_str();
            String urlResponse = WIFI_NABTON->sendGETPackage(url,ack);
            WLOG_INFO << "Response:" << urlResponse.c_str();
        }
        else if (clientType_ == ClientType::SERIAL_TYPE)
        {
            SERIAL_NABTON->sendPackage(url);
        }
        else
        {
            ULOG_ERROR << "Please define a valied Client type in configuration file";
        }

        return true;
    }

    bool NabtonClient::updateStatus()
    {
        bool result = WIFI_NABTON->updateWifiConnection();
        return result;
    }


}
