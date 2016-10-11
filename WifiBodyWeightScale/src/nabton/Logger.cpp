#include "Logger.h"
#include <HardwareSerial.h>


namespace nabton
{
    Logger::Logger(int logType): logPossible_(true), defaultLogType_(Logger::LogType::LOGTYPE_SERIAL)
    {
        defaultLogType_ = (Logger::LogType)logType;
    }

    Logger::~Logger()
    {
        switch(defaultLogType_)
        {
            case Logger::LogType::LOGTYPE_SERIAL:
                Serial.write('\n');
            break;
            case Logger::LogType::LOGTYPE_WIFI:
                if (!WIFI_NABTON->isLowPowerMode())
                    WIFI_NABTON->sendUdpPackage(logMessage_);
                logMessage_ = "";
            break;
        }
    }

    Logger& Logger::operator<<(int msg)
    {
      if (logPossible_)
      {
          switch(defaultLogType_)
          {
              case Logger::LogType::LOGTYPE_SERIAL:
                  Serial.write(String(msg).c_str());
              break;
              case Logger::LogType::LOGTYPE_WIFI:
                  logMessage_ = logMessage_ + String(msg);
              break;
          };
      }
      return *this;  // Return a reference to self.
    }

    Logger& Logger::operator<<(double msg)
    {
      if (logPossible_)
      {
          switch(defaultLogType_)
          {
              case Logger::LogType::LOGTYPE_SERIAL:
                  //Serial.write(msg);
              break;
              case Logger::LogType::LOGTYPE_WIFI:
                  logMessage_ = logMessage_ + String(msg,6);
              break;
          };
      }
      return *this;  // Return a reference to self.
    }

    Logger& Logger::operator<<(float msg)
    {
      if (logPossible_)
      {
          switch(defaultLogType_)
          {
              case Logger::LogType::LOGTYPE_SERIAL:
                  //Serial.write(msg);
              break;
              case Logger::LogType::LOGTYPE_WIFI:
                  logMessage_ = logMessage_ + String(msg,6);
              break;
          };
      }
      return *this;  // Return a reference to self.
    }

    Logger& Logger::operator<<(const char* msg)
    {
        if (logPossible_)
        {
            switch(defaultLogType_)
            {
                case Logger::LogType::LOGTYPE_SERIAL:
                    Serial.write(msg);
                break;
                case Logger::LogType::LOGTYPE_WIFI:
                    logMessage_ = logMessage_ + String(msg);
                break;
            };
        }
        return *this;  // Return a reference to self.
    }

    Logger& Logger::operator<<(String msg)
    {
        if (logPossible_)
        {
            switch(defaultLogType_)
            {
                case Logger::LogType::LOGTYPE_SERIAL:
                    Serial.write(msg.c_str());
                break;
                case Logger::LogType::LOGTYPE_WIFI:
                    logMessage_ = logMessage_ + msg;
                break;
            };
        }
        return *this;  // Return a reference to self.
    }

    Logger& Logger::operator<<(bool msg)
    {
        if (logPossible_)
        {
            String res = "";
            if (msg)
                res = "True";
            else
                res = "False";

            switch(defaultLogType_)
            {
                case Logger::LogType::LOGTYPE_SERIAL:
                    Serial.write(res.c_str());
                break;
                case Logger::LogType::LOGTYPE_WIFI:
                    logMessage_ = logMessage_ + res;
                break;
            };
        }
        return *this;  // Return a reference to self.
    }

}
