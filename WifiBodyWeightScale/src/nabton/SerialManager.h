/*
 * SerialManager module for nabton
 *
 *
 */

#ifndef SERIAL_MANAGER_H
#define SERIAL_MANAGER_H

#include <HardwareSerial.h>

#define SERIAL_NABTON SerialManager::getInstance()

namespace nabton
{
    class SerialManager
    {
        struct ConfigManager
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
        static SerialManager* getInstance();
        //static SerialManager* getInstance();
        static SerialManager* instance_;
        bool sendPackage(String message);


    private:
        SerialManager();
       ~SerialManager();

        //ConfigManager config_;
    };


}


#endif
