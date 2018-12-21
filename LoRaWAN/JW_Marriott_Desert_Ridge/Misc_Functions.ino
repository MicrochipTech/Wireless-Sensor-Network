// --- Temperature sensor

void initTemperature()
{
    pinMode(TEMP_SENSOR, INPUT);
    //Set ADC resolution to 12 bits
    analogReadResolution(12);
}


float getTemperature()
{
    uint8_t Buffer[100], i;
    float mVolts = 0;
    float temp;
    for (i = 0; i < 25; i++)
    {
        float bandgap = BandGap();
        float Vcc = ( 1000 * 1.1 * 4096)/bandgap;
        mVolts += (float) myanalogRead(TEMP_SENSOR) * Vcc / 4096.0;
        //mVolts += (float) myanalogRead(TEMP_SENSOR) * 3280.0 / 4096.0;
    }
    mVolts /= i;
    temp = (mVolts - 500.0) / 10.0;

    return temp;
}

// --- RGB LED and BLUE LED

void initLed()
{
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
}

void de_initLed()
{
    pinMode(LED_BUILTIN, INPUT);
    pinMode(LED_RED, INPUT);
    pinMode(LED_GREEN, INPUT);
    pinMode(LED_BLUE, INPUT);
}
#define COMMON_ANODE  // LED driving

void setRgbColor(uint8_t red, uint8_t green, uint8_t blue)
{
#ifdef COMMON_ANODE
    red = 255 - red;
    green = 255 - green;
    blue = 255 - blue;
#endif
    analogWrite(LED_RED, red);
    analogWrite(LED_GREEN, green);
    analogWrite(LED_BLUE, blue);
}

void turnBlueLedOn()
{
    digitalWrite(LED_BUILTIN, HIGH);
}

void turnBlueLedOff()
{
    digitalWrite(LED_BUILTIN, LOW);
}

void pulseRGB(myledcolor color, int count)
{
    for (; count > 0; count--)
    {
        //sprintf(load, "Count = %d",count);
        //debugSerial.println(load);
        switch (color) {
            case RED:
                setRgbColor(0x80, 0x00, 0x00);
                break;
            case GREEN:
                setRgbColor(0x00, 0x80, 0x00);
                break;
            case BLUE:
                setRgbColor(0x00, 0x00, 0x80);
                break;
            case YELLOW:
                setRgbColor(0x80, 0x80, 0x00);
                break;
            default:
                break;
        }

        delay(50);
        setRgbColor(0x00, 0x00, 0x00);
        delay(50);
    }
}



// --- Push Button

void initButton()
{
    pinMode(BUTTON, INPUT_PULLUP);
}

bool getButton()
{
    return digitalRead(BUTTON);
}

void initRTCC(void)
{
    rtc.begin(); //Start RTC library, this is where the clock source is initialized

    rtc.setTime(hours, minutes, seconds); //set time
    rtc.setDate(day, month, year); //set date

    rtc.setAlarmSeconds(0);
    //following two lines enable alarm, comment both out if you want to do external interrupt
    rtc.enableAlarm(rtc.MATCH_SS); //set alarm
    rtc.attachInterrupt(ISR_RTCC); //creates an interrupt that wakes the SAMD21 which is triggered by a FTC alarm

}

