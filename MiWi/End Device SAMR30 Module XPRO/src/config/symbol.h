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

#ifndef __SYMBOL_TIME_H_
#define __SYMBOL_TIME_H_

/************************ HEADERS **********************************/

#include "config/miwi_config.h"
#include <stdint.h>
#include "config/timer.h"
/************************ DEFINITIONS ******************************/


#define      ONE_SECOND    31*4//  External freq / prescalar 

#define SYMBOLS_TO_TICKS(a)     (32*ONE_SECOND/1000*(a))/(uint32_t)2000
#define TICKS_TO_SYMBOLS(a)     ((uint32_t)2000*a)/((32*ONE_SECOND/1000)




#define ONE_MILI_SECOND     (ONE_SECOND/1000)
#define HUNDRED_MILI_SECOND (ONE_SECOND/10)
#define FORTY_MILI_SECOND   (ONE_SECOND/25)
#define FIFTY_MILI_SECOND   (ONE_SECOND/20)
#define TWENTY_MILI_SECOND  (ONE_SECOND/50)
#define TEN_MILI_SECOND     (ONE_SECOND/100)
#define FIVE_MILI_SECOND    (ONE_SECOND/200)
#define TWO_MILI_SECOND     (ONE_SECOND/500)
#define ONE_MINUTE          (ONE_SECOND*60)
#define ONE_HOUR            (ONE_MINUTE*60)

#define MiWi_TickGetDiff(a,b) (a.Val - b.Val)

/************************ DATA TYPES *******************************/

#if defined(PROTOCOL_STAR)

    #define SHARE_PEER_DEVICE_INFO_TIMEOUT      ONE_SECOND*10
    #define LINK_STATUS_TIMEOUT                 ONE_SECOND*15 
    // every 5 minutes the stack will evaluate the inactive end nodes 
    #define FIND_INACTIVE_DEVICE_TIMEOUT        ONE_MINUTE*5
    #define END_DEVICES_DISPLAY_TIMEOUT         ONE_SECOND*15

#endif
/******************************************************************
 // Time unit defined based on IEEE 802.15.4 specification.
 // One tick is equal to one symbol time, or 16us. The Tick structure
 // is four bytes in length and is capable of represent time up to
 // about 19 hours.
 *****************************************************************/
typedef union _MIWI_TICK
{
    uint32_t Val;
    struct _MIWI_TICK_bytes
    {
        uint8_t b0;
        uint8_t b1;
        uint8_t b2;
        uint8_t b3;
    } byte;
    uint8_t v[4];
    struct _MIWI_TICK_words
    {
        uint16_t w0;
        uint16_t w1;
    } word;
} MIWI_TICK;

typedef union
{

	uint8_t  v[4];
	uint16_t w[2];
	uint32_t Val;

}API_UINT32_UNION;

typedef union
{

	uint8_t  v[2];
	uint16_t Val;

}API_UINT16_UNION;

void InitSymbolTimer(void);
uint32_t MiWi_TickGet(void);


/************************ VARIABLES ********************************/

extern volatile uint8_t timerExtension1,timerExtension2;
#endif

