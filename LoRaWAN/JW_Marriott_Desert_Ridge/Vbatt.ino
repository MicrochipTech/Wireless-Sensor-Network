// Sodaq Explorer  vbatt monitoring
// NOTE!  requires modifications to the Sodaq board package in variant.cpp and variant.h
// to support the pin definitions for VBATT_MEASURE

#include <Arduino.h>


//#define ADC_AREF 3.3f
#define ADC_AREF 3.26f
#define BATVOLT_R1 4.7f   // sodaq explorer R1 voltage divider
#define BATVOLT_R2 10.0f  // sodaq explorer R2 voltage divider

#define Vbatt_samples  20  // number of a/d samples used in average measurement

// VBAT_MEASURE 48    is defined  in variant.h  within the Sodaq Explorer board package


// Configure IO pin to measure battery voltage on Sodaq board
void Vbatt_monitor_init()
{
  // configure pin as input for analog measurement
  pinMode(VBAT_MEASURE, INPUT);
  analogReference(AR_DEFAULT);
  int Var = analogRead(VBAT_MEASURE); // dummy read due to change of analog reference  
}

//Measure Battery voltage of Sodaq board
float getVbatt()
{
    int Avg;  //  int is 32 bit number on SAMD/Sodaq board
    int samples;
    int tempVbatt;
 
    Avg = 0;
    analogReference(AR_DEFAULT);
    tempVbatt = analogRead(VBAT_MEASURE); // dummy read due to recent change of analog reference
    tempVbatt = analogRead(VBAT_MEASURE); // dummy read due to recent change of analog reference
    for (int y = 0; y < Vbatt_samples; y++)
    {
       tempVbatt = analogRead(VBAT_MEASURE);
       //debugSerial.println(tempVbatt);
       Avg = Avg + tempVbatt;
    }
    Avg = Avg/Vbatt_samples;
    debugSerial.print("vbat avg = ");debugSerial.println(Avg);
    float voltage = (uint16_t)((ADC_AREF / 4.095) * (BATVOLT_R1 + BATVOLT_R2) / BATVOLT_R2 * Avg);        
    //debugSerial.print("voltage value : ");debugSerial.println(voltage);
    return (voltage/1000);    // result will be something like  4.30 volts
}

//function to extract decimal part of float
long getDecimal(float val)
{
  int intPart = int(val);
  long decPart = 1000*(val-intPart); //I am multiplying by 1000 assuming that the float values will have a maximum of 3 decimal places. 
                                     //Change to match the number of decimal places you need
  if(decPart>0)return(decPart);           //return the decimal part of float number if it is available 
  else if(decPart<0)return((-1)*decPart); //if negative, multiply by -1
  else if(decPart=0)return(00);           //return 0 if decimal part of float number is not available
}

