#ifndef SD_CARD_MODULE_H
#define SD_CARD_MODULE_H

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

namespace nabton
{
    class SdCard
    {
    public:
        SdCard(const int CS_PIN);
        ~SdCard(){};

        bool openFile(const char* filename);
        bool closeFile();
        bool writeToFile(const char* data);
    private:
        File currentFile_;
        bool sdCardActive_;
    };

}

#endif
