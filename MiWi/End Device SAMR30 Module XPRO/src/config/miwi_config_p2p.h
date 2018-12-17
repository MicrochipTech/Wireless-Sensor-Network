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

#ifndef __CONFIGURE_P2P_H

    #define __CONFIGURE_P2P_H
    
    #include "config/miwi_config.h"
    #include "config/symbol.h"
    
        /*********************************************************************/
        // ENABLE_DUMP will enable the stack to be able to print out the 
        // content of the P2P connection entry. It is useful in the debugging
        // process
        /*********************************************************************/
        //#define ENABLE_DUMP
        
        
        /*********************************************************************/
        // RFD_DATA_WAIT is the timeout defined for sleeping device to receive 
        // a message from the associate device after Data Request. After this
        // timeout, the RFD device can continue to operate and then go to 
        // sleep to conserve battery power.
        /*********************************************************************/
        #define RFD_DATA_WAIT                   0x00003FFF
        
        
        /*********************************************************************/
        // CONNECTION_RETRY_TIMES is the maximum time that the wireless node
        // can try to establish a connection. Once the retry times are exhausted
        // control will be return to application layer to decide what to do next
        /*********************************************************************/
        #define CONNECTION_RETRY_TIMES          3
    
    
        /*********************************************************************/
        // CONNECTION_INTERVAL defines the interval in second between two 
        // connection request. 
        /*********************************************************************/
        #define CONNECTION_INTERVAL             2


        /*********************************************************************/
        // FA_BROADCAST_TIME defines the total number of times to broadcast
        // the channel hopping message to the rest of PAN, before the 
        // Frequency Agility initiator jump to the new channel
        /*********************************************************************/
        #define FA_BROADCAST_TIME           0x03
    
    
        /*********************************************************************/
        // RESYNC_TIMES defines the maximum number of times to try resynchronization
        // in all available channels before hand over the control to the application
        // layer
        /*********************************************************************/
        #define RESYNC_TIMES                0x03


        /*********************************************************************/
        // ACTIVE_SCAN_RESULT_SIZE defines the maximum active scan result
        // that the stack can hold. If active scan responses received exceed
        // the definition of ACTIVE_SCAN_RESULT_SIZE, those later active scan
        // responses will be discarded
        /*********************************************************************/
        #define ACTIVE_SCAN_RESULT_SIZE     4
    
    
        /*********************************************************************/
        // INDIRECT_MESSAGE_SIZE defines the maximum number of packets that
        // the device can store for the sleeping device(s)
        /*********************************************************************/
        #define INDIRECT_MESSAGE_SIZE   2
            
            
        /*********************************************************************/
        // INDIRECT_MESSAGE_TIMEOUT defines the timeout interval in seconds
        // for the stored packets for sleeping devices
        /*********************************************************************/
        #define INDIRECT_MESSAGE_TIMEOUT (ONE_SECOND * RFD_WAKEUP_INTERVAL * (INDIRECT_MESSAGE_SIZE + 1))
        
        
        /*********************************************************************/
        // ENABLE_ENHANCED_DATA_REQUEST enables the Enhanced Data Request 
        // feature of P2P stack. It combines the message that is send from
        // the sleeping device with Data Request command upon wakeup, to save
        // 20% - 30% active time for sleeping device, thus prolong the battery
        // life.
        /*********************************************************************/
        //#define ENABLE_ENHANCED_DATA_REQUEST
        
        
        /*********************************************************************/
        // ENABLE_TIME_SYNC enables the Time Synchronizaiton feature of P2P
        // stack. It allows the FFD to coordinate the check-in interval of
        // sleeping device, thus allow one FFD to connect to many sleeping
        // device. Once Time Synchronization feature is enabled, following
        // parameters are also required to be defined:
        //      TIME_SYNC_SLOTS
        //      COUNTER_CRYSTAL_FREQ
        /*********************************************************************/
        //#define ENABLE_TIME_SYNC
        
        
        /*********************************************************************/
        // TIME_SYNC_SLOTS defines the total number of time slot available 
        // within one duty cycle. As a rule, the number of time slot must be
        // equal or higher than the total number of sleeping devices that are
        // connected to the FFD, so that each sleeping device can be assigned
        // to a time slot. The time slot period is calcualted by following 
        // formula:
        //      Time Slot Period = RFD_WAKEUP_INTERVAL / TIME_SYNC_SLOTS
        // The length of time slot period depends on the primary oscillator
        // accuracy on the FFD as well as the 32KHz crystal accuracy on sleeping
        // devices. 
        // The definition of TIME_SYNC_SLOTS is only valid if ENABLE_TIME_SYNC
        // is defined
        /*********************************************************************/
        #define TIME_SYNC_SLOTS         10


        /*********************************************************************/
        // COUNTER_CRYSTAL_FREQ defines the frequency of the crystal that 
        // is connected to the MCU counter to perform timing functionality
        // when MCU is in sleep. 
        /*********************************************************************/
        #define COUNTER_CRYSTAL_FREQ    32768
    
   
#endif

