#include "SerialManager.h"
#include "Logger.h"


namespace nabton
{
    SerialManager* SerialManager::instance_ = NULL;
    int DEFAULT_GPIO_PIN_SERIAL = 13;

    SerialManager* SerialManager::getInstance()
    {
        if (instance_)
        {
            instance_ = new SerialManager();
            return instance_;
        }
        else
        {
            return NULL;
        }
    }

    SerialManager::SerialManager()
    {
        pinMode(DEFAULT_GPIO_PIN_SERIAL, OUTPUT);
        //ULOG_ERROR << "Wifi: Failed to connect to: " << config_.ssid;
    }

    SerialManager::~SerialManager()
    {
        //ULOG_INFO << "WifiManager destructor called";
    }

    bool SerialManager::sendPackage(String message)
    {
        digitalWrite(DEFAULT_GPIO_PIN_SERIAL, HIGH);
        // Create checksum
        message = message + String(": ")+message.length();
        Serial.write(message.c_str());

        digitalWrite(DEFAULT_GPIO_PIN_SERIAL, LOW);
        return true;
    }

}
