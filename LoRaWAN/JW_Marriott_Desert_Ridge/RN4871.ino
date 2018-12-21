


void initBLE_RN4871(void)
{
    uint8_t i = 0;
    unsigned char myBuffer[256];

     // Initialize the BLE hardware
    rn487xBle.hwInit();


    // Finalize the init. process
    while (!rn487xBle.enterCommandMode())
    {
        pulseRGB(RED, 1);
        rn487xBle.hwInit();
    }

    if (rn487xBle.swInit())
    {
        setRgbColor(0, 255, 0);
        debugSerial.println("Init. procedure done!");
    } else
    {
        setRgbColor(255, 0, 0);
        debugSerial.println("Init. procedure failed!");
        while (1);
    }
    rn487xBle.factoryReset();
    
    // Re-enter command mode
    while (!rn487xBle.enterCommandMode())
    {
        rn487xBle.hwInit();
    }
    //rn487xBle.enterCommandMode();

    // Stop advertising before starting the demo
    rn487xBle.stopAdvertising();
    // Set the advertising output power (range: min = 5, max = 0)
    rn487xBle.setAdvPower(0);
    // Set the serialized device name
    rn487xBle.setDevName(myDeviceName);
    rn487xBle.clearAllServices();
    rn487xBle.reboot();
    while (!rn487xBle.enterCommandMode())
    { 
        pulseRGB(RED, 1);
        rn487xBle.hwInit();
    }
    // Start adv. MLDP 
    
    rn487xBle.setDefaultServices(0xc0);
    rn487xBle.reboot();


    //Flush LoRa module input Buffer
 //   uint8_t tick;
 //    do
 //    {
 //       if (loraSerial.available())
 //        {
 //           uint8_t value = loraSerial.read();
 //            debugSerial.print((const char *) &value);
 //            tick = 0;
 //        }
 //        tick++;
 //        delay(1);
 //    } while (tick < 10);


    //debugSerial.println("You can now establish a connection from the Microchip SmartDiscovery App");
    //debugSerial.print("with the starter kit: ");
    //debugSerial.println(rn487xBle.getDeviceName());
    //rn487xBle.flush();

}

bool commisionRN4871(void)
{
    static unsigned int i;
    static char value;
    static uint8_t myBuffer[256], test[256], response[256];
    ;
    static uint8_t *myBufferPtr = myBuffer, *ptr = test, tick;
    char* offset;

    static long long int key;
    //debugSerial.print("BLE Loop");


    if (bleSerial.available()) //value = rn487xBle.isDataAvailable())//rn487xBle.isNotification("0072",30))
    {
        //debugSerial.print(".");
        tick = 0;
        value = Serial1.read();

        switch (value) {

            case '\r':
            case '\n':
            {
                if (offset = strstr((const char *) myBuffer, "EUI:"))
                {
                    sscanf((const char *) offset, "EUI:%s", test);
                    //debugSerial.print((const char *) test);
                    //debugSerial.println("Here1");
                    ptr = test;

                    for (i = 0; i < 8; i++)
                    {
                        key = (*ptr <= '9') ? (*ptr - '0') : (*ptr - 'A' + 10);
                        ptr++;
                        key <<= 4;
                        key |= (*ptr <= '9') ? (*ptr - '0') : (*ptr - 'A' + 10);
                        appEUI[i] = key;
                        //sprintf((char *) response, "myKey = %lx", key);
                        //debugSerial.println((const char *) response);
                        ptr++;
                    }
                    //debugSerial.println("Here2");
                    debugSerial.print("appEUI = ");
                    for (i = 0; i < sizeof (appEUI); i++)
                    {
                        sprintf((char *) response, "%02x", appEUI[i]);
                        debugSerial.print((const char *) response);
                        debugSerial.print(":");

                    }
                    debugSerial.println("");


                }
                if (offset = strstr((const char *) myBuffer, "KEY:"))
                {
                    sscanf((const char *) offset, "KEY:%s", test);
                    //debugSerial.println((const char *)test);
                    ptr = test;

                    for (i = 0; i < 8; i++)
                    {
                        key = (*ptr <= '9') ? (*ptr - '0') : (*ptr - 'A' + 10);
                        ptr++;
                        key <<= 4;
                        key |= (*ptr <= '9') ? (*ptr - '0') : (*ptr - 'A' + 10);
                        appKeyPrefix[i] = key;
                        //sprintf((char *) response, "myKey = %lx", key);
                        //debugSerial.println((const char *) response);
                        ptr++;
                    }
                    debugSerial.print("appKeyPrefix = ");
                    for (i = 0; i < sizeof (appKeyPrefix); i++)
                    {
                        sprintf((char *) response, "%02x", appKeyPrefix[i]);
                        debugSerial.print((const char *) response);
                        debugSerial.print(":");

                    }
                    debugSerial.println("");


                }
                if (offset = strstr((const char *) myBuffer, "LOC:"))
                {
                    sscanf((const char *) offset, "LOC:%s", test);
                    //debugSerial.print((const char *) test);
                    //debugSerial.println("Here1");
                    ptr = (uint8_t *)offset + strlen("LOC:");

                    //debugSerial.println("Here2");
                    debugSerial.print("Location = ");
                    debugSerial.println((const char *)ptr);
                    saveEEPROM_RN2903(0x320,ptr,strlen((char *)ptr)+1);
                    memcpy ((char *)Location, (char *)ptr,strlen((char *)ptr));

                }
                if (strstr((const char *) myBuffer, "Done commissioning"))
                {
                    debugSerial.println("Done !");
                    bleSerial.print("Done !\r");
                    myState = LORA_COMMISSION;
                }
                uint8_t hwEUI[8];
                uint8_t len = LoRa.getHWEUI(hwEUI, sizeof (hwEUI));
                debugSerial.println("");
                if (0)//len > 0)
                {
                    debugSerial.print("Internal Hardware EUI = ");
                    char c[2];
                    // put in form for displaying in hex format
                    for (uint8_t i = 0; i < len; i++)
                    {
                        //sprintf(c, "%02X", hwEUI[i]);
                        //debugSerial.print(c);
                    }
                    debugSerial.println("");

                    //memcpy((void*) devEUI, (void*) hwEUI, sizeof (hwEUI));

                }

                if (strstr((const char *) myBuffer, "Get Device EUI"))
                {
                    //sprintf((char*) response, "EUI%02x%02x%02x%02x%02x%02x%02x%02x\r",
                    //        hwEUI[0], hwEUI[1], hwEUI[2], hwEUI[3],
                    //        hwEUI[4], hwEUI[5], hwEUI[6], hwEUI[7]);
                    //debugSerial.println((char*) response);
                    //bleSerial.print((char*) response);
                    //myState = LORA_COMMISSION;
                }
                if (strstr((const char *) myBuffer, "Toggle LED"))
                {
                    for (uint8_t i = 0; i < 1; i++)
                    {
                        pulseRGB(RED, 1);
                        //delay(100);
                        pulseRGB(GREEN, 1);
                        //delay(100);
                        pulseRGB(BLUE, 1);
                        //delay(100);
                    } //myState = LORA_COMMISSION;
                }

                myBufferPtr = &myBuffer[0];
                memset(myBuffer, 0, sizeof (myBuffer));
            }
            default:
            {
                debugSerial.print(value);
                *myBufferPtr++ = value;
                //ebugSerial.print("Ici_1");
                if (strstr((const char *) myBuffer, "%STREAM_OPEN%"))
                    bleConnected = true;

                if (strstr((const char *) myBuffer, "%DISCONNECT%"))
                    bleConnected = false;

            }

        }
    } else
    {
        tick++;
    }

    delay(1);
    if (debugSerial.available() > 0)
    {
        value = debugSerial.read();
        bleSerial.print(value);
        //rn487xBle.sendOneByte(value);
    }
    return bleConnected;
}

void checkBLEaSleep(void)
{
    if (rn487xBle.enterCommandMode())
    {
        // Deep sleep,  need reset to wake it up!
        //Serial1.write("A,0020,005E\r\n");
        pulseRGB(RED, 2);
        Serial1.write("A,FFFF,FFFF\r\n");
        delay(5);
        rn487xBle.hwSleep();
    
    }
}

