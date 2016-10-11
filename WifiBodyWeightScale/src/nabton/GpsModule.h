#ifndef GPS_MODULE_H
#define GPS_MODULE_H

#include <Arduino.h>
#include <TinyGPS.h>

namespace nabton
{
    class GpsModule
    {
    public:
         struct GpsData
         {
             double latitude;
             double longitude;
             int timestamp;
         };

        GpsModule(){};
        ~GpsModule(){};
        bool getGpsData(GpsData& gpsResult);

    private:
        String readGpsFromModule();
        bool parseGpsString(String gpsString, GpsData& data);
        TinyGPS gps_;
    };


}


#endif
