#include <LoRa.h>
#include <SodaqExt_RN2483.h>
#include <Sodaq_wdt.h>
#include <StringLiterals.h>
#include <Switchable_Device.h>
#include <Utils.h>
#include "myTypedef.h"

static int _readResolution = 10;
static int _ADCResolution = 10;
static int _writeResolution = 8;

float BandGap() {
    uint32_t valueRead = 0;
    SYSCTRL->VREF.bit.BGOUTEN = 1;
    ADC->INPUTCTRL.bit.MUXPOS = 0x19;
    syncADC1();
    ADC->CTRLA.bit.ENABLE = 0x01; // Enable ADC

    // Start conversion
    syncADC1();
    ADC->SWTRIG.bit.START = 1;
    while (ADC->INTFLAG.bit.RESRDY == 0); // Waiting for conversion to complete
    
    // Clear the Data Ready flag
    ADC->INTFLAG.bit.RESRDY = 1;

    // Start conversion again, since The first conversion after the reference is changed must not be used.
    syncADC1();
    ADC->SWTRIG.bit.START = 1;

    // Store the value
    while (ADC->INTFLAG.bit.RESRDY == 0); // Waiting for conversion to complete
    valueRead = ADC->RESULT.reg;

    syncADC1();
    ADC->CTRLA.bit.ENABLE = 0x00; // Disable ADC
    syncADC1();
    SYSCTRL->VREF.bit.BGOUTEN = 0; //Disable BandGap

    return mapResolution(valueRead, _ADCResolution, _readResolution);
}

static void syncADC1() {
    while (ADC->STATUS.bit.SYNCBUSY == 1)
        ;
}

static inline uint32_t mapResolution(uint32_t value, uint32_t from, uint32_t to) {
    if (from == to) {
        return value;
    }

    if (from > to) {
        return value >> (from - to);
    } else {
        return value << (to - from);
    }
}

int pinPeripheral( uint32_t ulPin, EPioType ulPeripheral )
{
  // Handle the case the pin isn't usable as PIO
  if ( g_APinDescription[ulPin].ulPinType == PIO_NOT_A_PIN )
  {
    return -1 ;
  }

  switch ( ulPeripheral )
  {
    case PIO_DIGITAL:
    case PIO_INPUT:
    case PIO_INPUT_PULLUP:
    case PIO_OUTPUT:
      // Disable peripheral muxing, done in pinMode
//      PORT->Group[g_APinDescription[ulPin].ulPort].PINCFG[g_APinDescription[ulPin].ulPin].bit.PMUXEN = 0 ;

      // Configure pin mode, if requested
      if ( ulPeripheral == PIO_INPUT )
      {
        pinMode( ulPin, INPUT ) ;
      }
      else
      {
        if ( ulPeripheral == PIO_INPUT_PULLUP )
        {
          pinMode( ulPin, INPUT_PULLUP ) ;
        }
        else
        {
          if ( ulPeripheral == PIO_OUTPUT )
          {
            pinMode( ulPin, OUTPUT ) ;
          }
          else
          {
            // PIO_DIGITAL, do we have to do something as all cases are covered?
          }
        }
      }
    break ;

    case PIO_ANALOG:
    case PIO_SERCOM:
    case PIO_SERCOM_ALT:
    case PIO_TIMER:
    case PIO_TIMER_ALT:
    case PIO_EXTINT:
    case PIO_COM:
    case PIO_AC_CLK:
#if 0
      // Is the pio pin in the lower 16 ones?
      // The WRCONFIG register allows update of only 16 pin max out of 32
      if ( g_APinDescription[ulPin].ulPin < 16 )
      {
        PORT->Group[g_APinDescription[ulPin].ulPort].WRCONFIG.reg = PORT_WRCONFIG_WRPMUX | PORT_WRCONFIG_PMUXEN | PORT_WRCONFIG_PMUX( ulPeripheral ) |
                                                                    PORT_WRCONFIG_WRPINCFG |
                                                                    PORT_WRCONFIG_PINMASK( g_APinDescription[ulPin].ulPin ) ;
      }
      else
      {
        PORT->Group[g_APinDescription[ulPin].ulPort].WRCONFIG.reg = PORT_WRCONFIG_HWSEL |
                                                                    PORT_WRCONFIG_WRPMUX | PORT_WRCONFIG_PMUXEN | PORT_WRCONFIG_PMUX( ulPeripheral ) |
                                                                    PORT_WRCONFIG_WRPINCFG |
                                                                    PORT_WRCONFIG_PINMASK( g_APinDescription[ulPin].ulPin - 16 ) ;
      }
#else
      if ( g_APinDescription[ulPin].ulPin & 1 ) // is pin odd?
      {
        uint32_t temp ;

        // Get whole current setup for both odd and even pins and remove odd one
        temp = (PORT->Group[g_APinDescription[ulPin].ulPort].PMUX[g_APinDescription[ulPin].ulPin >> 1].reg) & PORT_PMUX_PMUXE( 0xF ) ;
        // Set new muxing
        PORT->Group[g_APinDescription[ulPin].ulPort].PMUX[g_APinDescription[ulPin].ulPin >> 1].reg = temp|PORT_PMUX_PMUXO( ulPeripheral ) ;
        // Enable port mux
        PORT->Group[g_APinDescription[ulPin].ulPort].PINCFG[g_APinDescription[ulPin].ulPin].reg |= PORT_PINCFG_PMUXEN ;
      }
      else // even pin
      {
        uint32_t temp ;

        temp = (PORT->Group[g_APinDescription[ulPin].ulPort].PMUX[g_APinDescription[ulPin].ulPin >> 1].reg) & PORT_PMUX_PMUXO( 0xF ) ;
        PORT->Group[g_APinDescription[ulPin].ulPort].PMUX[g_APinDescription[ulPin].ulPin >> 1].reg = temp|PORT_PMUX_PMUXE( ulPeripheral ) ;
        PORT->Group[g_APinDescription[ulPin].ulPort].PINCFG[g_APinDescription[ulPin].ulPin].reg |= PORT_PINCFG_PMUXEN ; // Enable port mux
      }
#endif
    break ;

    case PIO_NOT_A_PIN:
      return -1l ;
    break ;
  }

  return 0l ;
}

uint32_t myanalogRead( uint32_t ulPin )
{
  uint32_t valueRead = 0;
  uint8_t delay = 20;

  if ( ulPin < A0 )
  {
    ulPin += A0 ;
  }

  pinPeripheral(ulPin, PIO_ANALOG);

 

  syncADC1();
  ADC->INPUTCTRL.bit.MUXPOS = g_APinDescription[ulPin].ulADCChannelNumber; // Selection for the positive ADC input

  syncADC1();
  ADC->INPUTCTRL.bit.MUXNEG = 0x18; // GND

// Control A
  /*
   * Bit 1 ENABLE: Enable
   *   0: The ADC is disabled.
   *   1: The ADC is enabled.
   * Due to synchronization, there is a delay from writing CTRLA.ENABLE until the peripheral is enabled/disabled. The
   * value written to CTRL.ENABLE will read back immediately and the Synchronization Busy bit in the Status register
   * (STATUS.SYNCBUSY) will be set. STATUS.SYNCBUSY will be cleared when the operation is complete.
   *
   * Before enabling the ADC, the asynchronous clock source must be selected and enabled, and the ADC reference must be
   * configured. The first conversion after the reference is changed must not be used.
   */
  syncADC1();
  ADC->CALIB.reg =
    ADC_CALIB_BIAS_CAL((*(uint32_t *) ADC_FUSES_BIASCAL_ADDR >> ADC_FUSES_BIASCAL_Pos)) |
    ADC_CALIB_LINEARITY_CAL((*(uint64_t *) ADC_FUSES_LINEARITY_0_ADDR >> ADC_FUSES_LINEARITY_0_Pos));

  syncADC1();
  ADC->CTRLA.bit.ENABLE = 0x01;             // Enable ADC
  //syncADC1();
  //ADC->CTRLB.bit.CORREN = 0x01;
  //syncADC1();
  //ADC->AVGCTRL.reg = 0x08;             //256 samples
  
  // Start conversion
  syncADC1();
  ADC->SWTRIG.bit.START = 1;
  while ( ADC->INTFLAG.bit.RESRDY == 0 );   // Waiting for conversion to complete

  // Clear the Data Ready flag
  ADC->INTFLAG.bit.RESRDY = 1;

  // Start conversion again, since The first conversion after the reference is changed must not be used.
  syncADC1();
  ADC->SWTRIG.bit.START = 1;

  // Store the value
  while ( ADC->INTFLAG.bit.RESRDY == 0 );   // Waiting for conversion to complete
  valueRead = ADC->RESULT.reg;

  syncADC1();
  ADC->CTRLA.bit.ENABLE = 0x00;             // Disable ADC
  syncADC1();

  return mapResolution(valueRead, _ADCResolution, _readResolution);
}


