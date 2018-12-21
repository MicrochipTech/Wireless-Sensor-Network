/*********************************************************************
 *                                                                    
 * Software License Agreement                                         
 *                                                                    
 * Copyright © 2016-2017 Microchip Technology Inc.  All rights reserved.
 *
 * Microchip licenses to you the right to use, modify, copy and distribute 
 * Software only when embedded on a Microchip microcontroller or digital 
 * signal controller and used with a Microchip radio frequency transceiver, 
 * which are integrated into your product or third party product (pursuant 
 * to the terms in the accompanying license agreement).   
 *
 * You should refer to the license agreement accompanying this Software for 
 * additional information regarding your rights and obligations.
 *
 * SOFTWARE AND DOCUMENTATION ARE PROVIDED ?AS IS? WITHOUT WARRANTY OF ANY 
 * KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A 
 * PARTICULAR PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE 
 * LIABLE OR OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, 
 * CONTRIBUTION, BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY 
 * DIRECT OR INDIRECT DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO 
 * ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, 
 * LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF SUBSTITUTE GOODS, 
 * TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT 
 * NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.             
 *                                                                    
 *********************************************************************/
/**
 * \file
 *
 * \brief Board configuration
 *
 * Copyright (c) 2016-2017 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

/*
 * Copyright (c) 2016-2017 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */
#ifndef  _CONSOLE_H_
#define  _CONSOLE_H_

/************************ HEADERS **********************************/
#include "config/miwi_config.h"
#include "config/config_atrf86.h"
#include "config/miwi_config_p2p.h"


#define BAUD_RATE 19200


/*********************************************************************/
// ENABLE_CONSOLE will enable the print out on the hyper terminal
// this definition is very helpful in the debugging process
// Defined in miwi_config.h
/*********************************************************************/

#if defined(ENABLE_CONSOLE)

//DEFINITIONS




#define INPUT_PIN   1
#define OUPUT_PIN   0


//FUNCTION PROTOTYPES
/*********************************************************************
* Function:         void CONSOLE_Initialize(void)
*
* PreCondition:     none
*
* Input:	    none
*
* Output:	    none
*
* Side Effects:	    UART is configured
*
* Overview:         This function will configure the UART for use at
*                   in 8 bits, 1 stop, no flowcontrol mode
*
* Note:             None
********************************************************************/
 void CONSOLE_Initialize(void);


/*********************************************************************
* Function:         void CONSOLE_Put(uint8_t c)
*
* PreCondition:     none
*
* Input:            c - character to be printed
*
* Output:           none
*
* Side Effects:	    c is printed to the console
*
* Overview:	    This function will print the inputed character
*
* Note:		    Do not power down the microcontroller until
*                   the transmission is complete or the last
*                   transmission of the string can be corrupted.
********************************************************************/
 void CONSOLE_Put(uint8_t c);



/*********************************************************************
* Function:         void CONSOLE_PutString(ROM char* str)
*
* PreCondition:     none
*
* Input:            str - String that needs to be printed
*
* Output:           none
*
* Side Effects:	    str is printed to the console
*
* Overview:         This function will print the inputed ROM string
*
* Note:             Do not power down the microcontroller until
*                   the transmission is complete or the last
*                   transmission of the string can be corrupted.
********************************************************************/
void CONSOLE_PutString(char* str);

/*********************************************************************
* Function:         uint8_t CONSOLE_Get(void)
*
* PreCondition:     none
*
* Input:            none
*
* Output:           one byte received by UART
*
* Side Effects:	    none
*
* Overview:         This function will receive one byte from UART
*
* Note:             Do not power down the microcontroller until
*                   the transmission is complete or the last
*                   transmission of the string can be corrupted.
********************************************************************/
uint8_t CONSOLE_Get(void);



/*********************************************************************
* Function:         void CONSOLE_PrintHex(uint8_t toPrint)
*
* PreCondition:     none
*
* Input:            toPrint - character to be printed
*
* Output:           none
*
* Side Effects:	    toPrint is printed to the console
*
* Overview:         This function will print the inputed char to
*                   the console in hexidecimal form
*
* Note:             Do not power down the microcontroller until
*                   the transmission is complete or the last
*                   transmission of the string can be corrupted.
********************************************************************/
void CONSOLE_PrintHex(uint8_t);

/*********************************************************************
* Function:         void CONSOLE_PrintDec(uint8_t toPrint)
*
* PreCondition:     none
*
* Input:		    toPrint - character to be printed. Range is 0-99
*
* Output:		    none
*
* Side Effects:	    toPrint is printed to the console in decimal
*
*
* Overview:		    This function will print the inputed uint8_t to
*                   the console in decimal form
*
* Note:			    Do not power down the microcontroller until
*                   the transmission is complete or the last
*                   transmission of the string can be corrupted.
********************************************************************/
void CONSOLE_PrintDec(uint8_t);


//Function Prototype
// Comments: Prints the received message
void print_rx_message(void);

//If console is not enabled
#else

#define CONSOLE_Initialize()
#define CONSOLE_IsPutReady()                 1
#define CONSOLE_IsGetReady()                 1
#define CONSOLE_Put(c)
#define CONSOLE_PutString(str)
#define CONSOLE_Get()                        'a'
#define CONSOLE_PrintHex(a)
#define CONSOLE_PrintDec(a)
void DemoOutput_HandleMessage(void)

#endif

#define Printf(x) CONSOLE_PutString((char*)x)

#endif


