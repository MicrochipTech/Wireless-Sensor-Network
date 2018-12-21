/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
 */

#include <LoRa.h>
#include <SodaqExt_RN2483.h>
#include <Sodaq_wdt.h>
#include <StringLiterals.h>
#include <Switchable_Device.h>
#include <Utils.h>
#include "myTypedef.h"
#include <Wire.h>
#include <RTCZero.h>
#include <RN487x_BLE.h>


#define DEBUG

// Remove the following line if battery monitoring is not required
#define Vbatt_MON_Enable
# ifdef Vbatt_MON_Enable
  uint8_t Vbatt_Cycle_Cnt = 1;
# endif

#define debugSerial SerialUSB
#define loraSerial  Serial2
#define I2C  Wire
#define bleSerial Serial1

/* Create an rtc object */
RTCZero rtc;

/* Change these values to set the current initial time */
unsigned int seconds = 12;
unsigned int minutes = 12;
unsigned int hours = 12;

/* Change these values to set the current initial date */
unsigned int day = 17;
unsigned int month = 7;
unsigned int year = 18;

// define wakeup interval in Minutes
#define wakeUpFreq 1 //5
//#define wakeUpFreq 1

// === CUSTOM DEFINITION ===
#define subBand 2
#define LORA_PORT     1
const char* myDeviceName = "SODAQ"; // Custom Device name

// For OTAA provisioning, use the internal IEEE HWEUI provided
// by Microchip module RN2483 module as the devEUI key
// (comment to not use)
//#define USE_INTERNAL_HWEUI

// First, select your method for activation
//#define ABP
#define OTAA

// Then fill your unique keys


uint8_t devEUI[8] = { 0x00, 0x04, 0xA3, 0x0B, 0x00, 0x22, 0x6F, 0xFF };

//uint8_t appEUI[8] = {0x70, 0xB3, 0xD5, 0x7E, 0xF0, 0x00, 0x3E, 0x9F}; 
uint8_t appEUI[8] = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x00, 0x9A, 0x84 }; 


//uint8_t appKeyPrefix[8] = {0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00}; //Explorer0001 Explorer 2
uint8_t appKeyPrefix[8] = {0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88}; //SODAQ0001  JWMARRIOTT
uint8_t appKey[16];
//uint8_t myappEUI[16];

uint8_t Location[20] = {0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00};


// Payload is 'Microchip ExpLoRer' in HEX
const uint8_t myPayload[] = {0x4D, 0x69, 0x63, 0x72, 0x6F, 0x63, 0x68, 0x69, 0x70, 0x20, 0x45, 0x78, 0x70, 0x4C, 0x6F, 0x52, 0x65, 0x72};

// Reception buffer
uint8_t frameReceived[255];

bool isTimeSet = false;

void init_RN2901_RN4781_COMM(void)
{
    // LoRa Init procedure
    LoRa.setDiag(debugSerial); // optional
    LoRa.hwInit();
    loraSerial.begin(LoRa.getDefaultBaudRate());
    LoRa.initLoRaStream(loraSerial);

    // Initialize the BLE hardware
    rn487xBle.hwInit();
    // Open the communication pipe with the BLE module
    bleSerial.begin(rn487xBle.getDefaultBaudRate());

    // Assign the BLE serial port to the BLE library
    rn487xBle.initBleStream(&bleSerial);

   // Set the optional debug stream
    rn487xBle.setDiag(debugSerial);

}
static uint8_t bleConnected = false;
SM_STATE myState = IDLE;

void setup()
{
    while ((!debugSerial) && (millis() < 2000));
    //delay(5000);
    debugSerial.begin(115200);

    init_RN2901_RN4781_COMM();

    debugSerial.println("Microchip Technology ExpLoRer Starter Kit");

    initLed();
    initButton();
    initTemperature();
    initRTCC();

    //Init ATECC508A, long pulse to wake it up
    pinMode((33u), OUTPUT); //SDA
    digitalWrite((33u), LOW);
    delay(100);
    digitalWrite((33u), HIGH);

    //Set CS_FLASH of 25F040 to HIGH,  to minimize power
    #define CS_FLASH (44u)
    pinMode(CS_FLASH, OUTPUT); //PB16,  why 44u ???? but this is the right pin
    digitalWrite(CS_FLASH, HIGH);

    turnBlueLedOn();
    delay(100);
    turnBlueLedOff();
    //debugSerial.println(x);
    
    I2C.begin(); // join i2c bus (address optional for master)

    turnBlueLedOn();
    delay(100);
    turnBlueLedOff();
    byte addr = 0xC0 >>1;
    I2C.beginTransmission(addr); // transmit to device 0xC0
    I2C.write(0x01);              // sends 0x01   
    if ( I2C.endTransmission() == 0)    // stop transmitting
    {
      debugSerial.println("ATECC508A is in SLEEP mode");
      
    }else
      while(1);  //We only end here of ECC508A does not ACK!

   
//USB->DEVICE.CTRLA.bit.SWRST = 0;
//USB->DEVICE.CTRLA.bit.ENABLE = 0;
   
    
   
    return;
/*     
    while (1)
    {
        char buf[100];
        float bandgap = BandGap();
        float Vcc = ( 1.1 * 4096)/bandgap;
        sprintf(buf,"VCC = %f",Vcc);
        debugSerial.println(buf);
        float mVolts = (float) myanalogRead(TEMP_SENSOR) * Vcc / 4096.0;
        
    
    
        float temp = (mVolts - 500.0) / 10.0;
        sprintf(buf,"Badgap = %f Temp = %f",bandgap,(mVolts-.5)*100);
        debugSerial.println(buf);
        delay (2000);   
    }   
*/
}

unsigned int r, g, b;
uint8_t Done = false;
bool isISR_RTCC = 1, goToSleep = false, isISR_Button, onlySAMSleep = false;

// main application loop
void loop()
{
    int tick;
    int myDelay;
    char test[25];
    switch (myState) {
        case IDLE:
        {
            tick = millis();
            if (getButton() == 0)  
            {
                //Save default AppKey in RN2903 EEPROM
                memcpy(appKey, appKeyPrefix, sizeof (appKeyPrefix));
                memcpy(appKey + 8, devEUI, sizeof (devEUI));
                saveEEPROM_RN2903(0x300, appKey, sizeof (appKey));
                
                //Enable BLE to be ready for BLE commissioning
                pulseRGB(GREEN, 5);

                initBLE_RN4871();

                while (!rn487xBle.enterCommandMode())
                {
                    pulseRGB(RED, 1);
                    rn487xBle.hwInit();
                }
                
                uint8_t hwEUI[8];
                uint8_t len = LoRa.getHWEUI(hwEUI, sizeof (hwEUI));
                if (len >0)
                {//Start advertizing with MAC of RN2903 module
                  char c[6]; 
                 
                  bleSerial.print("&,");

                  for (uint8_t i = 2; i < len; i++)
                  {
                      //sprintf(c, "%02X", hwEUI[i]);
                      //bleSerial.print(c);
                  }

                  bleSerial.println("");

                }else
                {  //Start advertizing with MAC => 55:55:55:55:55:55
                  bleSerial.println("&,555555555555");
                }
                tick = millis();
                rn487xBle.startAdvertising();
                myState = BLE_COMMISSION;
            } else
            {
              //Enable LoRa for commissioning
                getEEPROM_RN2903(0x300, appKeyPrefix, sizeof (appKeyPrefix));
                sprintf(test,"appKeyPrefix = %x",appKeyPrefix);
                debugSerial.println(test);
                pulseRGB(BLUE, 5);
                myState = LORA_COMMISSION;
            }

        }
            break;

        case BLE_COMMISSION:
        {

            turnBlueLedOn();
            bool response = commisionRN4871();
            turnBlueLedOff();

            if ((millis() > (tick + 60000))&&(response == false))
                myState = LORA_COMMISSION;
        }
            break;

        case LORA_COMMISSION:
        {
            // Initialize the BLE hardware
            //rn487xBle.hwInit();
            // Fist, enter into command mode
            while (!rn487xBle.enterCommandMode())
            {
              pulseRGB(RED, 1);
                rn487xBle.hwInit();
            }

            //rn487xBle.setDevName(myDeviceName);
            rn487xBle.reboot();

            rn487xBle.hwSleep();

            commissionRN2903();

            myState = RUNNING;

        }
            break;

        case RUNNING:
        {
            loraWAN();
            checkBLEaSleep();
        }
            break;

        default:
        {
            debugSerial.println("Something Wrong!");

            myState = IDLE;
        }
    }
}

void sleep(uint8_t t)
{
    for (uint8_t i = t; i > 0; i--)
    {
        debugSerial.println(i);
        delay(1000);
        //__WFI();
    }
}

//interrupt service routine (ISR_RTCC), called when interrupt is triggered
//executes after MCU wakes up
bool ledstate = true;

void ISR_RTCC()
{


    uint8_t Buffer[100];
    sprintf((char*) Buffer, "Time %02d:%02d:%02d", rtc.getHours(), rtc.getMinutes(), rtc.getSeconds());
    debugSerial.println((char *) Buffer);

    if ((rtc.getSeconds() % wakeUpFreq)){
        onlySAMSleep = true;  //Only SLEEP the SAMD21
        return;
      
    }
    onlySAMSleep = false;
   
    //Every 5 minutes!
    goToSleep = false;
    debugSerial.println("ISR_RTCC");

    digitalWrite(LED_BUILTIN, ledstate ? HIGH : LOW);
    ledstate ^= 1;
    isISR_RTCC = true;
}
bool shortClick = false;
uint32_t buttonTimeStamp;
void ISR_Button()
{
    uint32_t mydelay;
    goToSleep = false;
    isISR_Button = true;
    debugSerial.println("Button interrupt\r\n");
    if (0)//(mydelay = (millis() - buttonTimeStamp))< 200)
    {
      shortClick = true; 
      pulseRGB(YELLOW, 5);
      
    }
    buttonTimeStamp = millis();
}

void samSleep()
{

    // Set the sleep mode to standby
    debugSerial.println("put SAMD to sleep *standby mode *");
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    shortClick = false;

    
    EIC->WAKEUP.reg |= EIC_INTENSET_EXTINT(1 << 14);
    SYSCTRL->BOD33.reg = 0;
    // SAMD sleep
    __DSB();
    __WFI();
}


