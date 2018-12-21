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

#ifndef __MCHP_MAC_H
    #define __MCHP_MAC_H

    #include "config/symbol.h"
	#include "config/miwi_config.h"
	#include "config/config_atrf86.h"
	#include "config/miwi_config_p2p.h"

	#if defined(PROTOCOL_P2P) || defined (PROTOCOL_STAR)
        #include "framework/miwi/miwi_p2p_star.h"
    #endif
	
	 
	#if defined(RF212B)
	#include "framework/driver/drv_miwi_at86rf212.h"
	#endif
    
    #define CHANNEL_ASSESSMENT_CARRIER_SENSE    0x00
    #define CHANNEL_ASSESSMENT_ENERGY_DETECT    0x01
    
    #define POWER_STATE_DEEP_SLEEP              0x00
    #define POWER_STATE_OPERATE                 0xFF


    #define PACKET_TYPE_MASK        0x03
    #define BROADCAST_MASK          0x04
    #define SECURITY_MASK           0x08
    #define REPEAT_MASK             0x10
    #define ACK_MASK                0x20
    #define DSTPRSNT_MASK           0x40
    #define SRCPRSNT_MASK           0x80
    
    #define PACKET_TYPE_DATA        0x00
    #define PACKET_TYPE_COMMAND     0x01
    #define PACKET_TYPE_ACK         0x02
    #define PACKET_TYPE_RESERVE     0x03


    // DOM-IGNORE-BEGIN
    /*********************************************************************
     Overview: Data types for drivers. This will facilitate easy
               access smaller chunks of larger data types when sending
               or receiving data (for example byte sized send/receive
               over parallel 8-bit interface.
    *********************************************************************/
    // DOM-IGNORE-END
    

    typedef union _DRIVER_UINT8_UNION_
    {
        uint8_t Val;
        struct
        {
        uint8_t b0:1;
        uint8_t b1:1;
        uint8_t b2:1;
        uint8_t b3:1;
        uint8_t b4:1;
        uint8_t b5:1;
        uint8_t b6:1;
        uint8_t b7:1;
        } bits;
        
    } DRIVER_UINT8_UNION;



	/*- Types ------------------------------------------------------------------*/
	typedef struct PHY_DataInd_t {
		uint8_t *data; // Pointer to Data
		uint8_t size;   // Size of the frame
		uint8_t lqi;    // Link Quality Index
		int8_t rssi;    // RSSI
	} PHY_DataInd_t;


    /***************************************************************************
     * Parameters to Transmit a Packet
     *  
     *      This structure contains configurations to transmit a packet
     **************************************************************************/
    typedef struct
    {
        union
        {
            uint8_t    Val;
            struct
            {
                uint8_t        packetType      : 2;        // type of packet. Possible types are
                                                        // * PACKET_TYPE_DATA - Data type
                                                        // * PACKET_TYPE_COMMAND -  Command type
                                                        // * PACKET_TYPE_ACK -  Acknowledgement type
                                                        // * PACKET_TYPE_RESERVE - Reserved type
                uint8_t        broadcast       : 1;        // 1: broadcast, 0: unicast
                uint8_t        secEn           : 1;        // 1: secure the MAC payload, 0: send plain text
                uint8_t        repeat          : 1;        // 1: allow repeaters to forward the message, 0: send message directly
                uint8_t        ackReq          : 1;        // 1: acknowledgement required, 0: no acknowldgement
                uint8_t        destPrsnt       : 1;        // 1: destination address in the packet, 0: destination address not in the packet
                uint8_t        sourcePrsnt     : 1;        // 1: source address in the packet, 0: source address not in the packet
            } bits;
        } flags;
        
        uint8_t        *DestAddress;           // destination address
        #if defined(IEEE_802_15_4)
            bool                        altDestAddr;        // use the alternative network address as destination in the packet
            bool                        altSrcAddr;         // use the alternative network address as source in the packet
            API_UINT16_UNION         DestPANID;          // PAN identifier of the destination
        #endif

    } MAC_TRANS_PARAM;
    
    
    /***************************************************************************
     * Content of the Received Message
     *  
     *      This structure contains all information of the received message
     **************************************************************************/
    typedef struct
    {
        union
        {
            uint8_t        Val;
            struct
            {
                uint8_t    packetType      :2;             // type of packet. Possible types are
                                                        // * PACKET_TYPE_DATA - Data type
                                                        // * PACKET_TYPE_COMMAND -  Command type
                                                        // * PACKET_TYPE_ACK -  Acknowledgement type
                                                        // * PACKET_TYPE_RESERVE - Reserved type
                uint8_t    broadcast       :1;             // 1: broadcast, 0: unicast
                uint8_t    secEn           :1;             // 1: secure the MAC payload, 0: send plain text
                uint8_t    repeat          :1;             // 1: allow repeaters to forward the message, 0: send message directly
                uint8_t    ackReq          :1;             // 1: acknowledgement required, 0: no acknowldgement
                uint8_t    destPrsnt       :1;             // 1: destination address in the packet, 0: destination address not in the packet
                uint8_t    sourcePrsnt     :1;             // 1: source address in the packet, 0: source address not in the packet
            } bits;
        } flags;
            
        uint8_t *      SourceAddress;                      // Address of the Sender
        uint8_t *      Payload;                            // Pointer to the payload
        uint8_t        PayloadLen;                         // Payload size
        uint8_t        RSSIValue;                          // RSSI value for the received packet
        uint8_t        LQIValue;                           // LQI value for the received packet
        #if defined(IEEE_802_15_4)
            bool                    altSourceAddress;               // Source address is the alternative network address
            API_UINT16_UNION     SourcePANID;                    // PAN ID of the sender
        #endif
    } MAC_RECEIVED_PACKET;
        
    /***************************************************************************
     * Initialization Parameters for MAC
     *  
     *      This structure contains parameters in the initialization
     **************************************************************************/
    typedef struct
    {
        union
        {
            uint8_t        Val;
            struct
            {
                uint8_t    RepeaterMode    :1;             // 1: Allow to act as repeater, 0: disable repeater function
                uint8_t    CCAEnable       :1;             // 1: Enable Clear Channel Assessment, 0: Disable CCA
                uint8_t    NetworkFreezer  :1;
                uint8_t    PAddrLength     :4;             // Length of the permanent address, range from 1 to 8.
            } bits;   
        } actionFlags;
        
        uint8_t *PAddress;                                 // Permenet transceiver address
        
    } MACINIT_PARAM;
    
    extern MAC_RECEIVED_PACKET  MACRxPacket;
    
    /************************************************************************************
     * Function:
     *      bool MiMAC_SetChannel(uint8_t channel, uint8_t offsetFreq)
     *
     * Summary:
     *      This function set the operating channel for the RF transceiver
     *
     * Description:        
     *      This is the primary MiMAC interface for the protocol layer to 
     *      set the operating frequency of the RF transceiver. Valid channel
     *      number are from 0 to 31. For different frequency band, data rate
     *      and other RF settings, some channels from 0 to 31 might be
     *      unavailable. Paramater offsetFreq is used to fine tune the center
     *      frequency across the frequency band. For transceivers that follow
     *      strict definition of channels, this parameter may be discarded.
     *      The center frequency is calculated as 
     *      (LowestFrequency + Channel * ChannelGap + offsetFreq)
     *
     * PreCondition:    
     *      Hardware initialization on MCU has been done. 
     *
     * Parameters: 
     *      uint8_t channel -  Channel number. Range from 0 to 31. Not all channels
     *                      are available under all conditions.
     *      uint8_t offsetFreq -   Offset frequency used to fine tune the center
     *                          frequency. May not apply to all RF transceivers
     *
     * Returns: 
     *      A boolean to indicates if channel setting is successful.
     *
     * Example:
     *      <code>
     *      // Set center frequency to be exactly channel 12
     *      MiMAC_SetChannel(12, 0);
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/       
    bool MiMAC_SetChannel(uint8_t channel, uint8_t offsetFreq);
    
    
    /************************************************************************************
     * Function:
     *      bool MiMAC_SetPower(uint8_t outputPower)
     *
     * Summary:
     *      This function set the output power for the RF transceiver
     *
     * Description:        
     *      This is the primary MiMAC interface for the protocol layer to 
     *      set the output power for the RF transceiver. Whether the RF
     *      transceiver can adjust output power depends on the hardware
     *      implementation.
     *
     * PreCondition:    
     *      MiMAC initialization has been done. 
     *
     * Parameters: 
     *      uint8_t outputPower -  RF transceiver output power.
     *
     * Returns: 
     *      A boolean to indicates if setting output power is successful.
     *
     * Example:
     *      <code>
     *      // Set output power to be 0dBm
     *      MiMAC_SetPower(TX_POWER_0_DB);
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/  
    bool MiMAC_SetPower(uint8_t outputPower);
    
    
    /************************************************************************************
     * Function:
     *      bool MiMAC_SetAltAddress(uint8_t *Address, uint8_t *PANID)
     *
     * Summary:
     *      This function set the alternative network address and PAN identifier if
     *      applicable
     *
     * Description:        
     *      This is the primary MiMAC interface for the protocol layer to 
     *      set alternative network address and/or PAN identifier. This function
     *      call applies to only IEEE 802.15.4 compliant RF transceivers. In case
     *      alternative network address is not supported, this function will return
     *      FALSE.
     *
     * PreCondition:    
     *      MiMAC initialization has been done. 
     *
     * Parameters: 
     *      uint8_t * Address -    The alternative network address of the host device.
     *      uint8_t * PANID -      The PAN identifier of the host device
     *
     * Returns: 
     *      A boolean to indicates if setting alternative network address is successful.
     *
     * Example:
     *      <code>
     *      uint16_t NetworkAddress = 0x0000;
     *      uint16_t PANID = 0x1234;
     *      MiMAC_SetAltAddress(&NetworkAddress, &PANID);
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/ 
    bool MiMAC_SetAltAddress(uint8_t *Address, uint8_t *PANID);
    
    /************************************************************************************
     * Function:
     *      bool MiMAC_Init(MACINIT_PARAM initValue)
     *
     * Summary:
     *      This function initialize MiMAC layer
     *
     * Description:        
     *      This is the primary MiMAC interface for the protocol layer to 
     *      initialize the MiMAC layer. The initialization parameter is 
     *      assigned in the format of structure MACINIT_PARAM.
     *
     * PreCondition:    
     *      MCU initialization has been done. 
     *
     * Parameters: 
     *      MACINIT_PARAM initValue -   Initialization value for MiMAC layer
     *
     * Returns: 
     *      A boolean to indicates if initialization is successful.
     *
     * Example:
     *      <code>
     *      MiMAC_Init(initParameter);
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/ 
    bool MiMAC_Init(MACINIT_PARAM initValue);
    
    /************************************************************************************
     * Function:
     *      uint8_t MiMAC_ChannelAssessment(uint8_t AssessmentMode)
     *
     * Summary:
     *      This function perform the noise detection on current operating channel
     *
     * Description:        
     *      This is the primary MiMAC interface for the protocol layer to 
     *      perform the noise detection scan. Not all assessment modes are supported
     *      for all RF transceivers.
     *
     * PreCondition:    
     *      MiMAC initialization has been done.  
     *
     * Parameters: 
     *      uint8_t AssessmentMode -   The mode to perform noise assessment. The possible
     *                              assessment modes are
     *                              * CHANNEL_ASSESSMENT_CARRIER_SENSE Carrier sense detection mode
     *                              * CHANNEL_ASSESSMENT_ENERGY_DETECT Energy detection mode
     *
     * Returns: 
     *      A byte to indicate the noise level at current channel.
     *
     * Example:
     *      <code>
     *      NoiseLevel = MiMAC_ChannelAssessment(CHANNEL_ASSESSMENT_CARRIER_SENSE);
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/ 
    uint8_t MiMAC_ChannelAssessment(uint8_t AssessmentMode);
    
    
    /************************************************************************************
     * Function:
     *      void MiMAC_DiscardPacket(void)
     *
     * Summary:
     *      This function discard the current packet received from the RF transceiver
     *
     * Description:        
     *      This is the primary MiMAC interface for the protocol layer to 
     *      discard the current packet received from the RF transceiver.
     *
     * PreCondition:    
     *      MiMAC initialization has been done. 
     *
     * Parameters: 
     *      None
     *
     * Returns: 
     *      None
     *
     * Example:
     *      <code>
     *      if( TRUE == MiMAC_ReceivedPacket() )
     *      {
     *          // handle the raw data from RF transceiver
     * 
     *          // discard the current packet
     *          MiMAC_DiscardPacket();
     *      }
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/ 
    void MiMAC_DiscardPacket(void);
    
    
    /************************************************************************************
     * Function:
     *      bool MiMAC_ReceivedPacket(void)
     *
     * Summary:
     *      This function check if a new packet has been received by the RF transceiver
     *
     * Description:        
     *      This is the primary MiMAC interface for the protocol layer to 
     *      check if a packet has been received by the RF transceiver. When a packet has
     *      been received, all information will be stored in the global variable 
     *      MACRxPacket in the format of MAC_RECEIVED_PACKET;
     *
     * PreCondition:    
     *      MiMAC initialization has been done. 
     *
     * Parameters: 
     *      None
     *
     * Returns: 
     *      A boolean to indicate if a packet has been received by the RF transceiver.
     *
     * Example:
     *      <code>
     *      if( true == MiMAC_ReceivedPacket() )
     *      {
     *          // handle the raw data from RF transceiver
     * 
     *          // discard the current packet
     *          MiMAC_DiscardPacket();
     *      }
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/ 
    bool MiMAC_ReceivedPacket(void);
    
    
    /************************************************************************************
     * Function:
     *      BOOL MiMAC_SendPacket(  MAC_TRANS_PARAM transParam, 
     *                              uint8_t *MACPayload, uint8_t MACPayloadLen)
     *
     * Summary:
     *      This function transmit a packet
     *
     * Description:        
     *      This is the primary MiMAC interface for the protocol layer to 
     *      send a packet. Input parameter transParam configure the way
     *      to transmit the packet.
     *
     * PreCondition:    
     *      MiMAC initialization has been done. 
     *
     * Parameters: 
     *      MAC_TRANS_PARAM transParam -    The struture to configure the transmission way
     *      uint8_t * MACPaylaod -             Pointer to the buffer of MAC payload
     *      uint8_t MACPayloadLen -            The size of the MAC payload
     *
     * Returns: 
     *      A boolean to indicate if a packet has been received by the RF transceiver.
     *
     * Example:
     *      <code>
     *      MiMAC_SendPacket(transParam, MACPayload, MACPayloadLen);
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/ 
    bool MiMAC_SendPacket(MAC_TRANS_PARAM transParam, uint8_t *MACPayload, uint8_t MACPayloadLen);
    
    
    /************************************************************************************
     * Function:
     *      bool MiMAC_PowerState(uint8_t PowerState)
     *
     * Summary:
     *      This function puts the RF transceiver into sleep or wake it up
     *
     * Description:        
     *      This is the primary MiMAC interface for the protocol layer to 
     *      set different power state for the RF transceiver. There are minimal 
     *      power states defined as deep sleep and operating mode. Additional
     *      power states can be defined for individual RF transceiver depends
     *      on hardware design.
     *
     * PreCondition:    
     *      MiMAC initialization has been done. 
     *
     * Parameters: 
     *      uint8_t PowerState -   The power state of the RF transceiver to be set to.
     *                          The minimum definitions for all RF transceivers are
     *                          * POWER_STATE_DEEP_SLEEP RF transceiver deep sleep mode.
     *                          * POWER_STATE_OPERATE RF transceiver operating mode.
     * Returns: 
     *      A boolean to indicate if chaning power state of RF transceiver is successful.
     *
     * Example:
     *      <code>
     *      // Put RF transceiver into sleep
     *      MiMAC_PowerState(POWER_STATE_DEEP_SLEEP);
     *      // Put MCU to sleep
     *      Sleep();
     *      // Wake up the MCU by WDT, external interrupt or any other means
     *
     *      // Wake up the RF transceiver
     *      MiMAC_PowerState(POWER_STATE_OPERATE); 
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/ 
    bool MiMAC_PowerState(uint8_t PowerState);
    
    #if defined(IEEE_802_15_4)
        #undef MY_ADDRESS_LENGTH
        #define MY_ADDRESS_LENGTH       8 
    #endif
    
    #if defined(ENABLE_SECURITY)
        extern API_UINT32_UNION IncomingFrameCounter[CONNECTION_SIZE];
    #endif
    
    void UserInterruptHandler(void);

#endif

