#ifndef LOGGER_H
#define LOGGER_H

#define ULOG Logger(0)
#define ULOG_DEBUG Logger(0)   << "[DEBUG] "
#define ULOG_INFO Logger(0)    << "[INFO] "
#define ULOG_WARNING Logger(0) << "[WARNING] "
#define ULOG_ERROR Logger(0)   << "[ERROR] "

#define WLOG nabton::Logger(1)
#define WLOG_DEBUG nabton::Logger(1)   << "[DEBUG] "
#define WLOG_INFO nabton::Logger(1)    << "[INFO] "
#define WLOG_WARNING nabton::Logger(1) << "[WARNING] "
#define WLOG_ERROR nabton::Logger(1)   << "[ERROR] "

#include "WifiManager.h"

namespace nabton
{
    class Logger
    {
    public:
         enum LogType
         {
             LOGTYPE_SERIAL,
             LOGTYPE_WIFI,
             LOGTYPE_FILE
         };
         Logger(int type);
        ~Logger();
        Logger& operator<<(const char* msg);
        Logger& operator<<(int msg);
        Logger& operator<<(double msg);
        Logger& operator<<(float msg);
        Logger& operator<<(bool msg);
        Logger& operator<<(String msg);
    private:
        bool logPossible_;
        LogType defaultLogType_;
        String logMessage_;
    };


}


#endif
