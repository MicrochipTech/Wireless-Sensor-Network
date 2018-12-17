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
#ifndef __CONFIG_APP_H_
#define __CONFIG_APP_H_

/*********************************************************************/
// USE_MAC_ADDRESS will enable the USER to use the Unique MAC Address from
// MAC_EEPROM on board as its LongAddress
/*********************************************************************/
#define USE_MAC_ADDRESS
/*********************************************************************/
// ENABLE_CONSOLE will enable the print out on the hyper terminal
// this definition is very helpful in the debugging process
/*********************************************************************/
#define ENABLE_CONSOLE


//------------------------------------------------------------------------
// Definition of Protocol Stack. ONLY ONE PROTOCOL STACK CAN BE CHOSEN
//------------------------------------------------------------------------
    /*********************************************************************/
    // PROTOCOL_P2P enables the application to use MiWi P2P stack. This
    // definition cannot be defined with PROTOCOL_STAR.
    /*********************************************************************/
    #define PROTOCOL_P2P

    /*********************************************************************/
    // PROTOCOL_STAR enables the application to use MiWi Star networking
    // stack. This definition cannot be defined with PROTOCOL_P2P.
    /*********************************************************************/
    //#define PROTOCOL_STAR      

// End Device (ED)
#define ED
#define NodeID 32

/*********************************************************************/
// MY_PAN_ID defines the PAN identifier. Use 0xFFFF if prefer a
// random PAN ID.
/*********************************************************************/
//------------------------------------------------------------------------
// Definition of RF Transceiver. ONLY ONE TRANSCEIVER CAN BE CHOSEN
//------------------------------------------------------------------------
#define MY_PAN_ID		0x1111
    /*********************************************************************/
    // Definition of AT86RF212B enables the application to use Microchip
    // AT86RF212B Sub GHz IEEE 802.15.4 compliant RF transceiver. Only one
    // RF transceiver can be defined.
    /*********************************************************************/
    #define RF212B
    
    /*********************************************************************/
    // Definition of MRF89XA enables the application to use Microchip
    // MRF89XA subGHz proprietary RF transceiver
    /*********************************************************************/
    //#define MRF89XA
	
	/*********************************************************************/
	// Definition of AT86RF233 enables the application to use Microchip
	// AT86RF233 2.4 GHz IEEE 802.15.4 compliant RF transceiver. Only one
	// RF transceiver can be defined.
	/*********************************************************************/
	//#define AT86RF233


/*********************************************************************/
// ENABLE_NETWORK_FREEZER enables the network freezer feature, which
// stores critical network information into non-volatile memory, so
// that the protocol stack can recover from power loss gracefully.
// The network infor can be saved in data EPROM of MCU, external 
// EEPROM or programming space, if enhanced flash is used in MCU.
// Network freezer feature needs definition of NVM kind to be 
// used, which is specified in HardwareProfile.h
// Network Freezer is not supported as part of this release
// Contact Microchip for Support
/*********************************************************************/

//#define ENABLE_NETWORK_FREEZER


#if defined(PROTOCOL_STAR)
    // if defined the END Device will be considered Active forever 
    // in the network , irrespective of the link status fails.

    //#define MAKE_ENDDEVICE_PERMANENT

    // Used only in  case of Pan Co , PAN Co should share the End device
    // Connection Information. If enabled PAN CO will periodically share
    // its connection table with Peer End devices
    // Time to broadcast connection table is user configurable 
    // change time by changing SHARE_PEER_DEVICE_INFO_TIMEOUT value

    #define ENABLE_PERIODIC_CONNECTIONTABLE_SHARE

    // Link status only used by END Devices in Star Network
    // Link status will confirm Pan CO that the device sending
    // link status is active in network.

    #define ENABLE_LINK_STATUS

            
#endif

/*********************************************************************/
// MY_ADDRESS_LENGTH defines the size of wireless node permanent 
// address in byte. This definition is not valid for IEEE 802.15.4
// compliant RF transceivers.
/*********************************************************************/
#define MY_ADDRESS_LENGTH       8

/*********************************************************************/
// EUI_x defines the xth byte of permanent address for the wireless
// node
/*********************************************************************/
#define EUI_7 0x11
#define EUI_6 0x66
#define EUI_5 0x55
#define EUI_4 0x44
#define EUI_3 0x33
#define EUI_2 0x22
#define EUI_1 0x11
#define EUI_0 0xFF

/*********************************************************************/
// TX_BUFFER_SIZE defines the maximum size of application payload
// which is to be transmitted
/*********************************************************************/
#define TX_BUFFER_SIZE 40

/*********************************************************************/
// RX_BUFFER_SIZE defines the maximum size of application payload
// which is to be received
/*********************************************************************/
#define RX_BUFFER_SIZE 40



/*********************************************************************/
// ADDITIONAL_NODE_ID_SIZE defines the size of additional payload
// will be attached to the P2P Connection Request. Additional payload 
// is the information that the devices what to share with their peers
// on the P2P connection. The additional payload will be defined by 
// the application and defined in main.c
/*********************************************************************/
#define ADDITIONAL_NODE_ID_SIZE   0


/*********************************************************************/
// P2P_CONNECTION_SIZE defines the maximum P2P connections that this 
// device allowes at the same time. 
/*********************************************************************/
#define CONNECTION_SIZE             10


/*********************************************************************/
// USE_MAC_ADDRESS defines the node address to be unique and the
// stack extracts this address from the flash memory.
/*********************************************************************/
#define USE_MAC_ADDRESS
/*********************************************************************/
// TARGET_SMALL will remove the support of inter PAN communication
// and other minor features to save programming space
/*********************************************************************/
//#define TARGET_SMALL

/*********************************************************************/
// APP_ADDR defines the wireless node short address
/*********************************************************************/
#define APP_ADDR 1




/*********************************************************************/
// ENABLE_HAND_SHAKE enables the protocol stack to hand-shake before 
// communicating with each other. Without a handshake process, RF
// transceivers can only broadcast, or hardcoded the destination address
// to perform unicast.
/*********************************************************************/
#define ENABLE_HAND_SHAKE

/*********************************************************************/
// ENABLE_SLEEP will enable the device to go to sleep and wake up 
// from the sleep
/*********************************************************************/
//#define ENABLE_SLEEP


/*********************************************************************/
// ENABLE_ED_SCAN will enable the device to do an energy detection scan
// to find out the channel with least noise and operate on that channel
/*********************************************************************/
//#define ENABLE_ED_SCAN


/*********************************************************************/
// ENABLE_ACTIVE_SCAN will enable the device to do an active scan to 
// to detect current existing connection. 
/*********************************************************************/
//#define ENABLE_ACTIVE_SCAN


/*********************************************************************/
// ENABLE_SECURITY will enable the device to encrypt and decrypt
// information transferred
/*********************************************************************/
#define ENABLE_SECURITY


/*********************************************************************/
// ENABLE_INDIRECT_MESSAGE will enable the device to store the packets
// for the sleeping devices temporily until they wake up and ask for
// the messages
/*********************************************************************/
//#define ENABLE_INDIRECT_MESSAGE


/*********************************************************************/
// ENABLE_BROADCAST will enable the device to broadcast messages for
// the sleeping devices until they wake up and ask for the messages
/*********************************************************************/
//#define ENABLE_BROADCAST


/*********************************************************************/
// RFD_WAKEUP_INTERVAL defines the wake up interval for RFDs in second.
// This definition is for the FFD devices to calculated various
// timeout. RFD depends on the setting of the watchdog timer to wake 
// up, thus this definition is not used.
/*********************************************************************/
#define RFD_WAKEUP_INTERVAL     8

/*********************************************************************/
// ENABLE_FREQUENCY_AGILITY will enable the device to change operating
// channel to bypass the sudden change of noise
/*********************************************************************/
//#define ENABLE_FREQUENCY_AGILITY


/*********************************************************************/
//AT86RF212 and AT86RF233 have a 128 byte tx/rx frame buffer
/*********************************************************************/
#define FRAME_BUFFER 128


#if !defined(RF212B) && !defined(RF233) 
    #error "One transceiver must be defined for the wireless application"
#endif


#if !defined(PROTOCOL_P2P) && !defined(PROTOCOL_STAR) 
    #error "One Microchip proprietary protocol must be defined for the wireless application."
#endif

#if defined(PROTOCOL_P2P) && defined(PROTOCOL_STAR) 
    #error "Only One Microchip proprietary protocol must be defined for the wireless application."
#endif


#if defined(PROTOCOL_P2P) && defined(ENABLE_LINK_STATUS)  
    #error "Link Status is valid only for MIWI STAR PROTOCOL"
#endif


#if defined(ENABLE_FREQUENCY_AGILITY)
    #define ENABLE_ED_SCAN
#endif

#if MY_ADDRESS_LENGTH > 8
    #error "Maximum address length is 8"
#endif

#if MY_ADDRESS_LENGTH < 2
    #error "Minimum address length is 2"
#endif


#if defined(RF212B) || defined(RF233)

    #define IEEE_802_15_4
    #undef MY_ADDRESS_LENGTH
    #define MY_ADDRESS_LENGTH 8

#endif

#if defined(ENABLE_NETWORK_FREEZER)
    #define ENABLE_NVM
    #define ENABLE_NVM_MAC
#endif


#if defined(PROTOCOL_STAR) && !defined(USE_MAC_ADDRESS)
    #if (EUI_0 == 0x00) && (EUI_1 == 0x00) && (EUI_2 == 0x00)
        #error "Value of zero to EUI_0 is invalid for the demo, use another value"
    #endif
#endif



#if defined(ENABLE_ACTIVE_SCAN) && defined(TARGET_SMALL)
    #error  Target_Small and Enable_Active_Scan cannot be defined together 
#endif

#if defined(ENABLE_INDIRECT_MESSAGE) && !defined(RFD_WAKEUP_INTERVAL)
    #error "RFD Wakeup Interval must be defined if indirect message is enabled"
#endif

#if defined(ENABLE_INDIRECT_MESSAGE) && defined(PROTOCOL_P2P)
    #warning "DEMO SW does not support Sleeping RFD in P2P protocol"
#endif

#if (RX_BUFFER_SIZE > 127)
    #error RX BUFFER SIZE too large. Must be <= 127.
#endif

#if (TX_BUFFER_SIZE > 127)
    #error TX BUFFER SIZE too large. Must be <= 127.
#endif

#if (RX_BUFFER_SIZE < 10)
    #error RX BUFFER SIZE too small. Must be >= 10.
#endif

#if (TX_BUFFER_SIZE < 10)
    #error TX BUFFER SIZE too small. Must be >= 10.
#endif

#if (CONNECTION_SIZE > 0xFE)
    #error NETWORK TABLE SIZE too large.  Must be < 0xFF.
#endif

#endif
