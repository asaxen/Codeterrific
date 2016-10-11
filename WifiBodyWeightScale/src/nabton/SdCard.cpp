#include "SdCard.h"
#include "Logger.h"

namespace nabton
{

    SdCard::SdCard(const int CSPin) : sdCardActive_(false)
    {
        pinMode(CSPin, HIGH);
        if (!SD.begin()) {
            ULOG_ERROR << "Failed to identify sd card";
            return;
        }
        else
        {
            sdCardActive_ = true;
            ULOG_INFO << "SD card found";
        }
    }


    bool SdCard::openFile(const char* filename)
    {
        if (!sdCardActive_)
            return false;

        if (SD.exists(filename))
        {
            currentFile_ = SD.open(filename, FILE_WRITE);
            return true;
        }
        else
        {
            return false;
        }
    }


    bool SdCard::closeFile()
    {
        if (!sdCardActive_)
            return false;

        currentFile_.close();
        return true;
    }


    bool SdCard::writeToFile(const char* data)
    {
        if (!sdCardActive_)
            return false;

        currentFile_.println(data);
    }

}
