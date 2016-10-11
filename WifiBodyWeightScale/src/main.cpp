//==============================
// Project: Body weight scale
// Description: Esp8266 that measures the pulse width modulate signal from an
// old body weight scale and sends the results to a web server.
//
// Creation date:2016-10-05
//==============================
#include "../config/configuration.h"
using namespace nabton;

// Number of measurements that are averaged
const int len_data_buffer = 10;
double dataWeight_ [len_data_buffer] = {0};
int volatile index_data = 0;
int numberOfBlinks;

uint8_t WEIGHT_SENT_LED_PIN = 2;
uint8_t INTERRUPT_PIN = 0;
volatile int lastTime = 0;

// Function for converting duration in microseconds to kilograms
double linearApprox(int x)
{
    //These calibration coefficients will differ in your body weight scale!
    double k = 0.0021845724;
    double m = -57.9144449;
    double weightKg = k*x+m;
    return weightKg;
}

// The interrupt service rutine (ISR)
// Called every time an interrupt occurs
void measurePulseWidth()
{
    int t = micros();
    int duration = t - lastTime;
    // Microseconds (100 to 20 ms)
    if (duration < 100000 && duration > 20000)
    {
        WLOG << " Measurement found!  " << duration;
        dataWeight_[index_data] = linearApprox(duration);
        index_data++;
        //Reset counter
        if (index_data == len_data_buffer)
            index_data = 0;
    }
    lastTime = t;
}

float findWeight()
{
    //Calculate sum, mean and variance of the data buffer
    float mean, sd, var, dev, sum = 0.0, sdev = 0.0;
    for(int i = 0; i < len_data_buffer; i++){
        sum = sum + dataWeight_[i];
    }

    mean = sum / len_data_buffer;

    for(int i = 0; i < len_data_buffer; i++){
        dev = (dataWeight_[i] - mean)*(dataWeight_[i] - mean);
        sdev = sdev + dev;
    }

    var = sdev / (len_data_buffer - 1);
    // Measurement is valid if the variance is < 0.1
    if (var < 0.10 && mean > 0)
    {
        WLOG << "var:" << var << " mean:" << mean;
        return mean;
    }
    //Returned if no valid measurement was found
    return -1.0;
}


void setup(void)
{
    //Initialize serial support
    Serial.begin(CONFIG_BAUDRATE);
    delay(100);
    //Initialize wifi support
    WifiManager::getInstance(CONFIG_SSID, CONFIG_SSID_PP, CONFIG_BROKER_IP,
                             CONFIG_BROKER_PORT, CONFIG_HOST, CONFIG_HOST_PORT,
                             CONFIG_LOW_POWER_MODE);
    delay(1000);
    // ########################################################################
    WLOG_DEBUG << "Setup attempt!";

    // ########################################################################
    //    Now you have the basics all set up. Send logs to your computer either
    //    over Serial or WifiManager.
    // ########################################################################
    pinMode(INTERRUPT_PIN, INPUT_PULLUP);
    pinMode(WEIGHT_SENT_LED_PIN, OUTPUT);
    attachInterrupt(INTERRUPT_PIN, measurePulseWidth, CHANGE);

    WLOG_DEBUG << "Setup() done!";
}

void loop(void)
{
    delay(CONFIG_REPORT_FREQUENCY_MILLISECONDS);
    int who = 0;
    //Attempt to compute weight once every second
    float weight = findWeight();

    //If a valid weight was found
    if (weight > 0)
    {
        WLOG_DEBUG << "Valid weight found!";
        // Detach the interrupt while preparing the GET request
        detachInterrupt(INTERRUPT_PIN);
        // empty the data buffer
        for(int i = 0; i < len_data_buffer; i++){
            dataWeight_[i] = 0;
        }

        //Optional - measuring several people. Works if they differ a lot in weight
        if (weight > 70.0 && weight < 90)
        {
            // Its person 1!
            who = 1;
            numberOfBlinks = 1;
        }
        else if (weight > 45.0 && weight < 70)
        {
            // Its person 2!
            who = 2;
            numberOfBlinks = 2;
        }
        else
            return;

        //Construct and send the GET-request
        String request = String("/index.php") + String("?who=") + String(who)
                       + String("&weight=") + String(weight);
        //WIFI_NABTON->sendGETPackage(String data, bool ack)
        String htmlResponse = WIFI_NABTON->sendGETPackage(request, true);

        //Blink the LED indicating that the measurment is done. Number of blinks
        //depends on which person it was
        for(int i = 0 ; i < numberOfBlinks; i++)
        {
            digitalWrite(WEIGHT_SENT_LED_PIN, HIGH);
            delay(500);
            digitalWrite(WEIGHT_SENT_LED_PIN, LOW);
            delay(500);
        }
        //Wait 5 seconds before reattaching the interrupt. (Enough time for the
        //person to get off the scale)
        delay(5000);
        attachInterrupt(INTERRUPT_PIN, measurePulseWidth, CHANGE);
    }
}
