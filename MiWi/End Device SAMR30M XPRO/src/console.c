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


#include "sio2host.h"
#include "console.h"
#include "framework/miwi/miwi_api.h"


/************************ VARIABLES ********************************/
unsigned char CharacterArray[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

/*- Implementations --------------------------------------------------------*/

/*************************************************************************//**
*****************************************************************************/

/*********************************************************************
* Function:         void CONSOLE_PutString(char* str)
*
* PreCondition:     none
*
* Input:            str - string that needs to be printed
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
void CONSOLE_PutString(char* str)
{
	uint8_t c;

	while( (c = *str++) )
	sio2host_putchar(c);
}

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
void CONSOLE_PrintHex(uint8_t toPrint)
{
	uint8_t PRINT_VAR;
	PRINT_VAR = toPrint;
	toPrint = (toPrint>>4)&0x0F;
	sio2host_putchar(CharacterArray[toPrint]);
	toPrint = (PRINT_VAR)&0x0F;
	sio2host_putchar(CharacterArray[toPrint]);
	return;
}

/*********************************************************************
* Function:         void CONSOLE_PrintDec(uint8_t toPrint)
*
* PreCondition:     none
*
* Input:            toPrint - character to be printed. Range is 0-99
*
* Output:           none
*
* Side Effects:	    toPrint is printed to the console in decimal
*
*
* Overview:         This function will print the inputed char to
*                   the console in decimal form
*
* Note:             Do not power down the microcontroller until
*                   the transmission is complete or the last
*                   transmission of the string can be corrupted.
********************************************************************/
void CONSOLE_PrintDec(uint8_t toPrint)
{
	if( toPrint >= 100 )
	sio2host_putchar(CharacterArray[toPrint/100]);
	if( toPrint >= 10 )
	sio2host_putchar(CharacterArray[(toPrint%100)/10]);
	sio2host_putchar(CharacterArray[toPrint%10]);
}

void print_rx_message(void)
{
	uint8_t i;

	if( rxMessage.flags.bits.secEn )
	{
	CONSOLE_PutString((char *)"Secured ");
	}

	if( rxMessage.flags.bits.broadcast )
	{
	CONSOLE_PutString((char *)"Broadcast Packet ");
	}
	else
	{
	CONSOLE_PutString((char *)"Unicast Packet ");
	}
	//CONSOLE_PrintHex(rxMessage.PacketRSSI);
	if( rxMessage.flags.bits.srcPrsnt )
	{
	CONSOLE_PutString((char *)" from ");
	if( rxMessage.flags.bits.altSrcAddr )
	{
	CONSOLE_PrintHex( rxMessage.SourceAddress[1]);
	CONSOLE_PrintHex( rxMessage.SourceAddress[0]);
	}
	else
	{
	for(i = 0; i < MY_ADDRESS_LENGTH; i++)
	{
	CONSOLE_PrintHex(rxMessage.SourceAddress[MY_ADDRESS_LENGTH-1-i]);
	}
	}
	}
	CONSOLE_PutString((char *)": ");

	for(i = 0; i < rxMessage.PayloadSize; i++)
	{
		sio2host_putchar(rxMessage.Payload[i]);
		
	}
}