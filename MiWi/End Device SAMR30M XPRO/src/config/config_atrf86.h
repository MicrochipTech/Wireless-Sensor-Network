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

#ifndef __CONFIG_AT86RF212B_H

    #define __CONFIG_AT86RF212B_H
    
    /*********************************************************************/
    // TURBO_MODE enables AT86RF212B transceiver to perform the communication
    // in proprietary modulation, which is not compliant to IEEE 802.15.4
    // specification. The data rate at turbo mode is up to 625Kbps.
    /*********************************************************************/
    //#define TURBO_MODE
    
    
    /*********************************************************************/
    // VERIFY_TRANSMIT configures the AT86RF212B transceiver to transmit 
    // data in a block procedure, which ensures finish transmission before
    // continue other task. This block procedure ensures the delivery state
    // of transmitting known to the upper protocol layer, thus may be 
    // necessary to detect transmission failure. However, this block procedure
    // slightly lowers the throughput
    /*********************************************************************/
    #define VERIFY_TRANSMIT
    
    
    /*********************************************************************/
    // SECURITY_KEY_xx defines xxth byte of security key used in the
    // block cipher
    /*********************************************************************/
    #define SECURITY_KEY_00 0x00
    #define SECURITY_KEY_01 0x01
    #define SECURITY_KEY_02 0x02
    #define SECURITY_KEY_03 0x03
    #define SECURITY_KEY_04 0x04
    #define SECURITY_KEY_05 0x05
    #define SECURITY_KEY_06 0x06
    #define SECURITY_KEY_07 0x07
    #define SECURITY_KEY_08 0x08
    #define SECURITY_KEY_09 0x09
    #define SECURITY_KEY_10 0x0a
    #define SECURITY_KEY_11 0x0b
    #define SECURITY_KEY_12 0x0c
    #define SECURITY_KEY_13 0x0d
    #define SECURITY_KEY_14 0x0e
    #define SECURITY_KEY_15 0x0f
    
    
    /*********************************************************************/
    // KEY_SEQUENCE_NUMBER defines the sequence number that is used to
    // identify the key. Different key should have different sequence
    // number, if multiple security keys are used in the application.
    /*********************************************************************/
    #define KEY_SEQUENCE_NUMBER 0x00
    
    
    /*********************************************************************/
    // SECURITY_LEVEL defines the security mode used in the application
    /*********************************************************************/
    #define SECURITY_LEVEL SEC_LEVEL_CCM_32


    /*********************************************************************/
    // FRAME_COUNTER_UPDATE_INTERVAL defines the NVM update interval for
    // frame counter, when security is enabled. The same interval will be
    // added to the frame counter read from NVM when Network Freezer
    // feature is enabled.
    /*********************************************************************/ 
    #define FRAME_COUNTER_UPDATE_INTERVAL 1024
    
    /*********************************************************************/
    // BANK_SIZE defines the number of packet can be received and stored
    // to wait for handling in MiMAC layer.
    /*********************************************************************/
    #define BANK_SIZE           20
	



    #if defined(AT86RF212B) || defined(AT86RF233)
        #define UNDEFINED_LOCATION  0x00
        #define UNITED_STATES       0x01
        #define CANADA              0x02
        #define EUROPE              0x03
        #define APPLICATION_SITE    UNITED_STATES
    #endif
	
    
#endif

