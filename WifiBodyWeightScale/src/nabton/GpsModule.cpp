#include "GpsModule.h"
#include "Logger.h"

namespace nabton
{

    bool GpsModule::getGpsData(GpsModule::GpsData& gpsResult)
    {
        bool result = false;

        //TODO Implement a way to brake while loop if GPS fails
        while (true)
        {
            delay(5);
            if (gps_.encode(Serial.read()))
            {
                unsigned long fix_age;
                float flat, flon;
                gps_.f_get_position(&flat, &flon, &fix_age);
                WLOG_INFO << flat << "," << flon;
                gpsResult.latitude = flat;
                gpsResult.longitude = flon;

                result = true;
                break;
            }
        }

        return result;
    }

}
