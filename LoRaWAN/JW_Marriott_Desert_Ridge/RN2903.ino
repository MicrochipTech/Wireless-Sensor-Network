
void getEEPROM_RN2903(uint16_t addr, uint8_t *table, uint8_t lenght)
{
    int tick = 0, i;
    uint8_t index = 0;
    uint8_t myBuffer[64], myBuffer1[64], *ptr;
    uint8_t value[64] = {0};
    long key;
    uint8_t count;

    memset(myBuffer, 0, sizeof (myBuffer));
    memset(myBuffer1, 0, sizeof (myBuffer));

    //Flush input Buffer
    do
    {
        if (loraSerial.available())
        {
            value[0] = loraSerial.read();
            debugSerial.print((const char *) value);
            tick = 0;
        }
        tick++;
        delay(1);
    } while (tick < 10);

    ptr = myBuffer1;

    debugSerial.println("sys get nvm:");
    //loraSerial.print("mac get appeui\r\n");
    for (uint8_t index = 0; index < lenght; index++)
    {
        tick = 0;
        sprintf((char*) myBuffer, "sys get nvm %x\r\n", addr++);

        debugSerial.print((const char *) myBuffer);

        count = 0;
        loraSerial.print((char*) myBuffer);
        do
        {
            if (loraSerial.available())
            {
                tick = 0;
                uint8_t temp = loraSerial.read();
                if (temp == '\r' || temp == '\n')
                    continue;
                *ptr = temp;
                sprintf((char *) value, "%c", *ptr++);
                debugSerial.print((const char *) value);
                count++;
            }
            tick++;
            delay(1);
        } while (tick < 10);
        if (count == 1)
        {
            if (*(ptr - 1) == '0')
            {
                *ptr = '0';
                sprintf((char *) value, "%c", *ptr++);
                debugSerial.print((const char *) value);
            } else
            {
                *ptr = *(ptr - 1);
                *(ptr - 1) = '0';
                ptr++;
            }
        }
    }
    debugSerial.print((const char *) myBuffer1);
    debugSerial.println("");


    //Copy ASCII table to "table" and convert in HEX
    ptr = myBuffer1;
    for (i = 0; i < lenght; i++)
    {
        key = (*ptr <= '9') ? (*ptr - '0') : (*ptr - 'A' + 10);
        ptr++;
        key <<= 4;
        key |= (*ptr <= '9') ? (*ptr - '0') : (*ptr - 'A' + 10);
        table[i] = (uint8_t) key;
        //sprintf((char *) myBuffer, "myKey = %lx", key);
        //debugSerial.println((const char *) myBuffer);
        ptr++;
    }
    sprintf((char*) myBuffer, "state%i", myState);
    debugSerial.println((const char *) myBuffer);

    //debugSerial.println("Here2");
    debugSerial.print("appEUI = ");
    for (i = 0; i < lenght; i++)
    {
        sprintf((char *) myBuffer, "%02x", table[i]);
        debugSerial.print((const char *) myBuffer);
        debugSerial.print(":");

    }
    debugSerial.println("");


}

void saveEEPROM_RN2903(uint16_t addr, uint8_t *table, uint8_t lenght)
{
    int tick = 0;
    uint8_t index = 0;
    char myBuffer[25];
    uint8_t value[5] = {0, 0, 0, 0, 0};

    //Flush input buffer
    do
    {
        if (loraSerial.available())
        {
            value[0] = loraSerial.read();
            debugSerial.print((const char *) value);
            tick = 0;
        }
        tick++;
        delay(1);
    } while (tick < 10);

    debugSerial.println("sys set nvm:");
    //loraSerial.print("mac get appeui\r\n");
    for (uint8_t index = 0; index < lenght; index++)
    {
        tick = 0;
        sprintf(myBuffer, "sys set nvm %x %x\r\n", addr++, *table++);

        debugSerial.println((const char *) myBuffer);
        loraSerial.print(myBuffer);
        do
        {
            if (loraSerial.available())
            {
                value[0] = loraSerial.read();
                debugSerial.print((const char *) value);
                tick = 0;
            }
            tick++;
            delay(1);
        } while (tick < 10);
    }
}
uint8_t isTimeSetRetries = 0;
void commissionRN2903(void)
{
    int tick = 0;
    char value[256] = {0, 0, 0, 0, 0};
    char myBuffer[256], temp[256];
    uint8_t index = 0;

    isTimeSetRetries = 0;

    debugSerial.println("mac get appeui");
    loraSerial.print("mac get appeui\r\n");
    do
    {
        if (loraSerial.available())
        {
            temp[index] = loraSerial.read();
            sprintf(myBuffer, "%c", temp[index++]);
            debugSerial.print((const char *) myBuffer);
            tick = 0;
        }
        tick++;
        delay(1);
    } while (tick < 10);
//    memcpy(myappEUI, temp, sizeof (myappEUI));
//    debugSerial.println("");
//    debugSerial.print((const char *) myappEUI);
//    debugSerial.println("");


    // == Get the internal Hardware EUI of the module ==
    uint8_t hwEUI[8];
    uint8_t len = LoRa.getHWEUI(hwEUI, sizeof (hwEUI));
    debugSerial.println("");
    if (len > 0)
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

    } else
    {
        debugSerial.println("Failed to get the internal hardware EUI");

    }

    // == Join the network ==
    bool res = false;
    int tentative = 0, i;

    // App Key can be a default key for all devices or can be unique per device
    // Here the key is computed by the help of the Device EUI
    // The 16-Bytes format will start by: FF EE DD CC BB AA 99 88 and 
    // will finish by: the 8 Bytes of the devEUI
    memcpy(appKey, appKeyPrefix, sizeof (appKeyPrefix));
    memcpy(appKey + 8, devEUI, sizeof (devEUI));

    //sprintf (temp,"isTimeset = %x",isTimeSet);
    //debugSerial.println((const char *) temp);

    saveEEPROM_RN2903(0x300, appKey, sizeof (appKey));

    debugSerial.println("");
    debugSerial.print("appKey: ");
    for (i = 0; i < 16; i++)
    {
        sprintf(value, "%02x", appKey[i]);
        debugSerial.print((const char*) value);
    }
    debugSerial.println("");

    debugSerial.print("appEUI: ");
    for (i = 0; i < 8; i++)
    {
        sprintf(value, "%02x", appEUI[i]);
        debugSerial.print((const char*) value);
    }
    debugSerial.println("");

// ********************
// set power index configuration parameters  (KY)
// this needs changes to library file Lora.cpp to support getPwrIdx function
// this code can be removed and board will default to pwridx = 6 by default
    //LoRa.setPwrIdx(5);
    //int pwr;
    //pwr = LoRa.getPwrIdx();
    //sprintf(value,"pwridx = %d",pwr);
    //debugSerial.println((const char*) value);
//*********************


    do
    {
        setRgbColor(0x00, 0x00, 0xFF);
        debugSerial.println("Try to join the network through one of the activation method");


        res = LoRa.joinOTAALoRaNetwork(subBand, devEUI, appEUI, appKey, false, 1);
        debugSerial.println(res ? "OTAA Join Accepted." : "OTAA Join Failed! Trying again. Waiting 1 seconds.");

        if (!res)
        {
            setRgbColor(0xFF, 0x00, 0x00);
            tentative++;
            delay(1000);
        }
        if (tentative == 3)
        {
          //uint8_t broadcast[16] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88}; //NOT IMPORTANT SINCE WE ARE USING devEUI programmed in RNxxxx module

            while (1)
            {
                pulseRGB(RED, 2);
                delay(250);
                pulseRGB(GREEN, 2);
                delay(250);
                pulseRGB(BLUE, 2);
                delay(250);
            }
        }
    } while (res == 0);

    //debugSerial.println("Sleeping for 5 seconds before starting.");
    //sleep(5);

    pulseRGB(BLUE, 5);
    attachInterrupt(BUTTON, ISR_Button, LOW); // FALLING does not wake it up from Sleep //creates an interrupt source on external pin

    //isTimeSet = 0;


}


bool once = false;

void loraWAN(void)
{
    bool charprint = 1;
    //Run LoRaWAN state
    while (goToSleep) //Only RTCC and BUTTON ISR will get us out of here
    {
        if (!onlySAMSleep)  //Lora is already in SLEEP
          LoRa.sleep();
        //delay(1000);
        if (!debugSerial)
        { // If USB debug is connected,  do NOT go in SLEEP, just wait for isISR_RTCC
            //rn487xBle.hwSleep();
            //de_initLed();
            pulseRGB(BLUE, 2);
            PORT->Group[0].DIRSET.reg = 0b00111100000011000000000000001111;
            PORT->Group[0].OUTSET.reg = 0b00111100000011000000000000001111;
            
            PORT->Group[1].DIRSET.reg = 0b00111111111111000001111111011111;
            PORT->Group[1].OUTSET.reg = 0b00111111111111000001111111011111;

            USB->DEVICE.CTRLA.reg = 0x01;  //SWRST
            
            samSleep();
            //initLed();

            while (!getButton()); // if woke up because of button,  wait until release
        } else
        { //Only reach here when in DEBUG mode with Terminal connected to Mote
            debugSerial.print(".");
            delay(1000);
            pulseRGB(RED, 2);
            setRgbColor(0x00, 0x00, 0x00); //turn off led
            return;
        }
    }
    goToSleep = true;

    if (isISR_RTCC || isISR_Button)
    {
        isISR_RTCC = false;

        LoRa.wakeUp();

        debugSerial.println("Awake");
        delay(200);
        //Flush receive buffer
        uint8_t ms_cur = 0;
        do
        {
            if (loraSerial.available() > 0)
            {
                uint8_t incomingByte = loraSerial.read();
                debugSerial.write(incomingByte);
            }
            ++ms_cur;
            delay(1);

        } while (ms_cur < 100);

        //debugSerial.println("Awake");

    }

    bool pushButton = false;
    char load[50];
    char load1[50];
    byte sbuf[25]; 
    float tempf;
    float vbatt;
    int i;
    int b;
    if (isISR_Button == true)
    {
        isISR_Button = false;
        LoRa.setDataRate(1);
        sprintf(load, "COMMISSIONING at %02d:%02d", rtc.getHours(), rtc.getMinutes());

        //sprintf(load, "Temp=%.2f BandGap=%.2f at %02d:%02d", getTemperature(), BandGap(), rtc.getHours(), rtc.getMinutes());
        //debugSerial.println(load);
    } else
    {
#ifdef Vbatt_MON_Enable
      if ((isTimeSet)||(isTimeSetRetries >= 5)){ // proceed with vbatt or temp only if time set or beyond time set retry limit
          tempf = getTemperature();
          tempf = tempf*1.8 + 32;
          vbatt = getVbatt();
          
          i = int(tempf*100);
          debugSerial.println(i);
          sbuf[0] = highByte(i);
          sbuf[1] = lowByte(i);
          debugSerial.println(sbuf[0]);
          debugSerial.println(sbuf[1]);
          
          i = int(vbatt * 100);
          sbuf[2] = highByte(i);
          sbuf[3] = lowByte(i);
         
          sbuf[4] = rtc.getHours();
          sbuf[5] = rtc.getMinutes();
          debugSerial.println(tempf);
          debugSerial.println(vbatt);
          debugSerial.println(sbuf[4]);
          debugSerial.println(sbuf[5]);
          
          sprintf(load, "temp = %d.%d battery = %d.%d  hrs = %d  xmin = %d",sbuf[0],sbuf[1],sbuf[2],sbuf[3],sbuf[4],sbuf[5]);
          debugSerial.println(load);

          for (i = 0; i < 7; i++)
          {
            load[i] = sbuf[i];
          }
          charprint = 0;  // set flag for hex printing
#endif
      }
/*
      // previous non json method to add vbatt message
      if ((isTimeSet)||(isTimeSetRetries >= 5)){ // proceed with vbatt or temp only if time set or beyond time set retry limit
          debugSerial.print("Vbatt_Cycle_Cnt = ");debugSerial.println(Vbatt_Cycle_Cnt);
          if (Vbatt_Cycle_Cnt == 0)
          {
              sprintf(load, "Vbatt=%.2f at %02d:%02d", getVbatt(), rtc.getHours(), rtc.getMinutes());
          }
          else
              sprintf(load, "Temp=%.2f at %02d:%02d", getTemperature(), rtc.getHours(), rtc.getMinutes());
          Vbatt_Cycle_Cnt++;
          Vbatt_Cycle_Cnt = Vbatt_Cycle_Cnt%2 ;
      #else
           sprintf(load, "Temp=%.2f at %02d:%02d", getTemperature(), rtc.getHours(), rtc.getMinutes());
           //debugSerial.println(load);
      #endif
      }
*/
    }
    //char Location[20];
    //memset(Location,0,sizeof(Location));
    if ((!isTimeSet)&&(isTimeSetRetries < 5))
    {
        if(!once &&(strlen((char *)Location)>0)){
          once = true;
          //getEEPROM_RN2903(0x320,(uint8_t *)Location,sizeof(Location));
          debugSerial.print("My location is :");
          debugSerial.println((const char *)Location);
          sprintf(load, "LOC=%s",Location);        
        }else
        {
            sprintf(load, "Time?");
            isTimeSetRetries++;
            //isTimeSet = true;
        }

        goToSleep = false;        //Don't go to sleep until the time is updated
//        delay(1000);
    }


    debugSerial.println("Try to transmit an uplink message ...");
    turnBlueLedOn();
    delay(50);
    turnBlueLedOff();


    // Uplink
    debugSerial.print("load >");
    if (charprint == 1){
      debugSerial.println(load);
    }
    else
    {
      sprintf(load1,"%X %X %X %X %X %X",load[0],load[1],load[2],load[3], load[4],load[5]);
      debugSerial.println(load1);
    }
    
    switch (LoRa.send(LORA_PORT, (const uint8_t*) load, strlen(load))) {
        case NoError:
            Done = true;
            debugSerial.println("Successful transmission.");
            pulseRGB(BLUE, 3);
            break;
        case NoResponse:
            debugSerial.println("There was no response from the device.");
            pulseRGB(RED, 3);
            break;
        case Timeout:
            debugSerial.println("Connection timed-out. Check your serial connection to the device! Sleeping for 20sec.");
            pulseRGB(RED, 6);
            break;
        case PayloadSizeError:
            debugSerial.println("The size of the payload is greater than allowed. Transmission failed!");
            pulseRGB(RED, 9);
            break;
        case InternalError:
            debugSerial.println("Oh No! This shouldn't happen. Something is really wrong! Try restarting the device!\r\nThe program will now halt.");

            while (1)
            {
                pulseRGB(RED, 1);
            };
            break;
        case Busy:
            debugSerial.println("The device is busy. Sleeping for 10 extra seconds.");
            delay(10000);
            break;
        case NetworkFatalError:
            debugSerial.println("There is a non-recoverable error with the network connection. You should re-connect.\r\nThe program will now halt.");
            setRgbColor(0xFF, 0x00, 0x00);
            while (1)
            {
            };
            break;
        case NotConnected:
            debugSerial.println("The device is not connected to the network. Please connect to the network before attempting to send data.\r\nThe program will now halt.");
            setRgbColor(0xFF, 0x00, 0x00);
            while (1)
            {
            };
            break;
        case NoAcknowledgment:
            debugSerial.println("There was no acknowledgment sent back!");
            setRgbColor(0xFF, 0x00, 0x00);
            break;
        default:
            break;
    }

    //return RGB to original value
    setRgbColor(r, g, b);



    // Downlink
    int frameReceivedSize = LoRa.receive(frameReceived, sizeof (frameReceived));
    if (frameReceivedSize > 0)
    {
        debugSerial.print("Downlink received : ");

        debugSerial.print((char *) frameReceived);
        if (strstr((const char*) frameReceived, "Color"))
        {
            sscanf((const char*) frameReceived, "%s %d %d %d", load, &r, &g, &b);
            //sprintf(load1, "%s %d %d %d", load, &r, &g, &b);
            //debugSerial.print((char *) load1);
            setRgbColor(r, g, b);
        }
        if (strstr((const char*) frameReceived, "Time") && !isTimeSet)
        {
            isTimeSet = true;
            sscanf((const char*) frameReceived, "%s %d %d %d", load, (unsigned int) &hours, (unsigned int) &minutes, (unsigned int) &seconds);
            sprintf(load1, "%s %d %d %d", load, &hours, &minutes, &seconds);
            debugSerial.print((char *) load1);
            rtc.setTime(hours, minutes, seconds); //set time
            for (uint8_t i = 0; i < 10; i++)
            {
                delay(50);
                turnBlueLedOn();
                delay(50);
                turnBlueLedOff();
            }
        }
        debugSerial.print(" ");
        debugSerial.println("");
    }
    if (Done)
    {
        Done = false;
        //debugSerial.println("Sleep for 60 seconds");
        //sleep(10);
        debugSerial.println("Push the button to send a message over the LoRa Network ...");
    }

}


