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

#ifndef __MCHP_API
    #define __MCHP_API

#include "config/symbol.h"
#include "config/miwi_config.h"
#include "config/config_atrf86.h"
#include "config/miwi_config_p2p.h"
    

#if defined(PROTOCOL_P2P) || defined (PROTOCOL_STAR)
    #include "framework/miwi/miwi_p2p_star.h"
#endif
    
    #define INPUT
    #define OUTPUT
    #define IOPUT
	
	//The maximum time to perform scan on single channel. The value is from 5 to 14. The real time to perform scan can
	//be calculated in following formula from IEEE 802.15.4 specification:
    #define SCAN_DURATION 10   
	// Channel Selected can be any one of  11-26 channels 
	#define APP_CHANNEL_SEL 5


   


    /***************************************************************************/
    // Status information of the connected peer information
    //
    //      This structure contains the information regarding the status of 
    //      the connected peer device. 
    /***************************************************************************/
    typedef union __CONNECTION_STATUS
    {
        uint8_t Val;
        struct _CONNECTION_STAUTS_bits
        {
            uint8_t RXOnWhenIdle       :1;     //1 = transceiver always on, 0 = transceiver sleeps when idle
            uint8_t directConnection   :1;     //1 = can talk to this device directly, 0 = must route to this device
            uint8_t longAddressValid   :1;     //1 = long address valid, 0 = long address unknown
            uint8_t shortAddressValid  :1;     //1 = short address valid, 0 = short address unknown
            uint8_t FinishJoin         :1;     //1 = already finish joining procedure, 0 = in the process of join
            uint8_t isFamily           :1;     //1 = family member (parent/child), 0 = not family
            uint8_t filler             :1;
            uint8_t isValid            :1;     //1 = this entry is valid, 0 = this entry is not valid
        } bits;   
    } CONNECTION_STATUS;
 

    /***************************************************************************
     * Peer Device Information in Connection Table
     *  
     *      This structure contains device information about the peer device
     *      of current node. It is the element structure for connection table.
     *      Due to the bank limitation in PIC18 MCU architecture, the size of
     *      CONNECTION_ENTRY must be dividable by 256 in case the array is across
     *      the bank. In this case, the user need to make sure that there is no
     *      problem 
     **************************************************************************/
    typedef struct __CONNECTION_ENTRY
    {
        #if !defined(PROTOCOL_P2P)
            API_UINT16_UNION    PANID;                      // PAN Identifier of the peer device. May not necessary in P2P protocol
            API_UINT16_UNION    AltAddress;                 // Alternative address of the peer device. Not necessary in P2P protocol
        #endif
        uint8_t        Address[MY_ADDRESS_LENGTH];     // Permanent address of peer device
		uint8_t			flag;
        
        CONNECTION_STATUS status;
        
        #if ADDITIONAL_NODE_ID_SIZE > 0
            uint8_t        PeerInfo[ADDITIONAL_NODE_ID_SIZE];  // Additional Node ID information, if defined in application layer
        #endif
    } CONNECTION_ENTRY;

    extern CONNECTION_ENTRY    ConnectionTable[CONNECTION_SIZE];

    extern uint8_t            currentChannel;
    // Source address when sending a packet // can be MAC_Address or EUI based on Users choice 
    extern uint8_t myLongAddress[MY_ADDRESS_LENGTH];

    // the no of connections on a device.
    // In case of Star network conn_size will be shared by pan co to all the end devices. 
    // In case of p2p network every device will have all its on conn_size.
    extern uint8_t conn_size ; 

    /************************************************************************************
     * Function:
     *      void MiApp_ProtocolInit(bool bNetworkFreezer)
     *
     * Summary:
     *      This function initialize the Microchip proprietary wireless protocol
     *
     * Description:        
     *      This is the primary user interface function to initialize the Microchip
     *      proprietary wireless protocol, which is chosen by the application layer. 
     *      Usually, this function must be called after the hardware initialization,
     *      before any other MiApp interface can be called. 
     *
     * PreCondition:    
     *      Hardware initialization has been done.
     *
     * Parameters:           
     *      bool bNetworkFreezer -  Network Freezer is a feature for Microchip proprietary
     *                              protocol. When this feature is enbaled, all critical 
     *                              network information can be saved in nonvotile memory,
     *                              so that after power cycle, the network can be restored 
     *                              to its original state before the power lost without 
     *                              any communication. When Network Freezer feature is enabled
     *                              and the input parameter of bNetworkFreezer is TRUE, the
     *                              initialization process will restore the network paramaters.
     *                  
     * Returns: 
     *      None
     *
     * Example:
     *      <code>
     *      HardwareInit();
     *      MiApp_ProtocolInit();
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *********************************************************************************/
    bool    MiApp_ProtocolInit(bool bNetworkFreezer);
    
    
    /************************************************************************************
     * Function:
     *      bool MiApp_SetChannel(uint8_t Channel)
     *
     * Summary:
     *      This function set the operating channel for the RF transceiver
     *
     * Description:        
     *      This is the primary user interface function to set the operating
     *      channel for the RF transceiver. Valid channels are from 0 to 31.
     *      Depends on the RF transceiver, its frequency band, data rate and 
     *      other settings, not all channels are available. If input channel 
     *      is not available under the current condition, the current operating
     *      channel will not be change, and the return value will be FALSE. 
     *      Otherwise, the return value will be TRUE.
     *
     * PreCondition:    
     *      Protocol initialization has been done.
     *
     * Parameters:           
     *      uint8_t Channel -  The channel to as the future operating channel.
     *                      Channels higher than 31 are invalid. Valid 
     *                      channels between 0-31 depends on a lot of factors
     *                  
     * Returns: 
     *      a boolean to indicate if channel change has been performed successfully
     *
     * Example:
     *      <code>
     *      if( true == MiApp_SetChannel(15) )
     *      {
     *          // channel changes successfully
     *      }
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *********************************************************************************/        
    bool    MiApp_SetChannel(uint8_t Channel);
    
    
    
    #define START_CONN_DIRECT       0x00
    #define START_CONN_ENERGY_SCN   0x01
    #define START_CONN_CS_SCN       0x02
    /************************************************************************************
     * Function:
     *      bool    MiApp_StartConnection(uint8_t Mode, uint8_t ScanDuration, uint32_t ChannelMap)
     *
     * Summary:
     *      This function start a PAN without connected to any other devices
     *
     * Description:        
     *      This is the primary user interface function for the application layer to 
     *      a PAN. Usually, this fucntion is called by the PAN Coordinator who is the
     *      first in the PAN. The PAN Coordinator may start the PAN after a noise scan
     *      if specified in the input mode. 
     *
     * PreCondition:    
     *      Protocol initialization has been done.
     *
     * Parameters:           
     *      uint8_t Mode - Whether to start a PAN after a noise scan. Possible modes are
     *                  * START_CONN_DIRECT Start PAN directly without noise scan
     *                  * START_CONN_ENERGY_SCN Perform an energy scan first, then
     *                                          start the PAN on the channel with least
     *                                          noise.
     *                  * START_CONN_CS_SCN     Perform a carrier-sense scan first,
     *                                          then start the PAN on the channel with
     *                                          least noise.
     *      uint8_t ScanDuration - The maximum time to perform scan on single channel. The
     *                          value is from 5 to 14. The real time to perform scan can
     *                          be calculated in following formula from IEEE 802.15.4 
     *                          specification: 
     *                              960 * (2^ScanDuration + 1) * 10^(-6) second
     *                          ScanDuration is discarded if the connection mode is
     *                          START_CONN_DIRECT.
     *      uint32_t ChannelMap -  The bit map of channels to perform noise scan. The 32-bit
     *                          double word parameter use one bit to represent corresponding
     *                          channels from 0 to 31. For instance, 0x00000003 represent to 
     *                          scan channel 0 and channel 1. ChannelMap is discarded if the 
     *                          connection mode is START_CONN_DIRECT.
     *                  
     * Returns: 
     *      a boolean to indicate if PAN has been started successfully.
     *
     * Example:
     *      <code>
     *      // start the PAN on the least noisy channel after scanning all possible channels.
     *      MiApp_StartConnection(START_CONN_ENERGY_SCN, 10, 0xFFFFFFFF);
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/    
    bool    MiApp_StartConnection( uint8_t Mode, uint8_t ScanDuration, uint32_t ChannelMap);
    
    
    /***************************************************************************
     * Active Scan result 
     *  
     *      This structure contains information from active scan. Application 
     *      layer will depend on this information to decide the way to establish
     *      connections. 
     **************************************************************************/
    typedef struct
    {
        uint8_t		Channel;                        // Operating Channel of the PAN
        uint8_t        Address[MY_ADDRESS_LENGTH];     // Responding device address
        API_UINT16_UNION    PANID;                          // PAN Identifier
        uint8_t       RSSIValue;                      // RSSI value for the response
        uint8_t        LQIValue;                       // LQI value for the response
        union
        {
            uint8_t        Val;
            struct
            {
                uint8_t    Role:		2;              // Role of the responding device in the PAN
                uint8_t    Sleep:		1;              // Whether the responding device goes to sleep when idle
                uint8_t    SecurityEn:	1;              // Whether the responding device is capable of securing the data
                uint8_t    RepeatEn:	1;              // Whether the responding device allow repeat
                uint8_t    AllowJoin:	1;              // Whether the responding device allows other device to join
                uint8_t    Direct:		1;              // Whether the responding device in radio range or through a repeater
                uint8_t    altSrcAddr: 1;              // Whether the Address is alternative network address or permanent address
            } bits;
        } Capability;
        #if ADDITIONAL_NODE_ID_SIZE > 0
            uint8_t        PeerInfo[ADDITIONAL_NODE_ID_SIZE];  // Additional Node ID information, if defined in application layer
        #endif
    } ACTIVE_SCAN_RESULT; 
    
    #ifdef ENABLE_ACTIVE_SCAN
        extern ACTIVE_SCAN_RESULT ActiveScanResults[ACTIVE_SCAN_RESULT_SIZE];
    #endif
    
    
    
    /************************************************************************************
     * Function:
     *      uint8_t    MiApp_SearchConnection(uint8_t ScanDuartion, uint32_t ChannelMap)
     *
     * Summary:
     *      This function perform an active scan to locate operating PANs in the
     *      neighborhood.
     *
     * Description:        
     *      This is the primary user interface function for the application layer to 
     *      perform an active scan. After this function call, all active scan response
     *      will be stored in the global variable ActiveScanResults in the format of 
     *      structure ACTIVE_SCAN_RESULT. The return value indicates the total number
     *      of valid active scan response in the active scan result array.
     *
     * PreCondition:    
     *      Protocol initialization has been done.
     *
     * Parameters:           
     *      uint8_t ScanDuration - The maximum time to perform scan on single channel. The
     *                          value is from 5 to 14. The real time to perform scan can
     *                          be calculated in following formula from IEEE 802.15.4 
     *                          specification 
     *                              960 * (2^ScanDuration + 1) * 10^(-6) second
     *      uint32_t ChannelMap -  The bit map of channels to perform noise scan. The 32-bit
     *                          double word parameter use one bit to represent corresponding
     *                          channels from 0 to 31. For instance, 0x00000003 represent to 
     *                          scan channel 0 and channel 1. 
     *                  
     * Returns: 
     *      The number of valid active scan response stored in the global variable ActiveScanResults.
     *
     * Example:
     *      <code>
     *      // Perform an active scan on all possible channels
     *      NumOfActiveScanResponse = MiApp_SearchConnection(10, 0xFFFFFFFF);
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/
    uint8_t    MiApp_SearchConnection(uint8_t ScanDuration, uint32_t ChannelMap);
    
    #define CONN_MODE_DIRECT        0x00
    #define CONN_MODE_INDIRECT      0x01
    /************************************************************************************
     * Function:
     *      uint8_t    MiApp_EstablishConnection(uint8_t ActiveScanIndex, uint8_t Mode)
     *
     * Summary:
     *      This function establish a connection with one or more nodes in an existing
     *      PAN.
     *
     * Description:        
     *      This is the primary user interface function for the application layer to 
     *      start communication with an existing PAN. For P2P protocol, this function
     *      call can establish one or more connections. For network protocol, this 
     *      function can be used to join the network, or establish a virtual socket
     *      connection with a node out of the radio range. There are multiple ways to
     *      establish connection(s), all depends on the input parameters.
     *
     * PreCondition:    
     *      Protocol initialization has been done. If only to establish connection with
     *      a predefined device, an active scan must be performed before and valid active
     *      scan result has been saved.
     *
     * Parameters:           
     *      uint8_t ActiveScanIndex -  The index of the target device in the ActiveScanResults
     *                              array, if a predefined device is targeted. If the 
     *                              value of ActiveScanIndex is 0xFF, the protocol stack will
     *                              try to establish a connection with any device.
     *      uint8_t Mode - The mode to establish a connection. This parameter is generally valid in
     *                  a network protocol. The possible modes are:
     *                  * CONN_MODE_DIRECT      Establish a connection within radio range.
     *                  * CONN_MODE_INDIRECT    Establish a virtual connection with a device 
     *                                          that may be in or out of the radio range. This
     *                                          mode sometimes is called cluster socket, which
     *                                          is only valid for network protocol. The PAN 
     *                                          Coordinator will be involved to establish a 
     *                                          virtual indirect socket connection.
     *                  
     * Returns: 
     *      The index of the peer device on the connection table.
     *
     * Example:
     *      <code>
     *      // Establish one or more connections with any device
     *      PeerIndex = MiApp_EstablishConnection(0xFF, CONN_MODE_DIRECT);
     *      </code>
     *
     * Remarks:    
     *      If more than one connections have been established through this function call, the
     *      return value points to the index of one of the peer devices.
     *
     *****************************************************************************************/    
    uint8_t    MiApp_EstablishConnection(uint8_t ActiveScanIndex, uint8_t Mode);
    
    /*********************************************************************
     * Function:
     *      void MiApp_RemoveConnection(uint8_t ConnectionIndex)
     *
     * Summary:
     *      This function remove connection(s) in connection table
     *
     * Description:        
     *      This is the primary user interface function to disconnect connection(s).
     *      For a P2P protocol, it simply remove the connection. For a network protocol,
     *      if the device referred by the input parameter is the parent of the device
     *      calling this function, the calling device will get out of network along with
     *      its children. If the device referred by the input parameter is children of
     *      the device calling this function, the target device will get out of network.
     * 
     * PreCondition:    
     *      Transceiver has been initialized. Node has establish
     *      one or more connections
     *
     * Parameters:           
     *      uint8_t ConnectionIndex -  The index of the connection in the
     *                              connection table to be removed
     *                  
     * Returns: 
     *      None
     *
     * Example:
     *      <code>
     *      MiApp_RemoveConnection(0x00);
     *      </code>
     *
     * Remarks:    
     *      None
     *
     ********************************************************************/
    void    MiApp_RemoveConnection(uint8_t ConnectionIndex);
    
    #define ENABLE_ALL_CONN         0x00
    #define ENABLE_PREV_CONN        0x01
    #define ENABLE_ACTIVE_SCAN_RSP  0x02
    #define DISABLE_ALL_CONN        0x03
    /************************************************************************************
     * Function:
     *      void    MiApp_ConnectionMode(uint8_t Mode)
     *
     * Summary:
     *      This function set the current connection mode.
     *
     * Description:        
     *      This is the primary user interface function for the application layer to 
     *      configure the way that the host device accept connection request.
     *
     * PreCondition:    
     *      Protocol initialization has been done. 
     *
     * Parameters:           
     *      uint8_t Mode -     The mode to accept connection request. The privilege for those modes
     *                      decreases gradually as defined. The higher privilege mode has all the 
     *                      rights of the lower privilege modes.
     *                      The possible modes are
     *                      * ENABLE_ALL_CONN       Enable response to all connection request
     *                      * ENABLE_PREV_CONN      Enable response to connection request
     *                                              from device already in the connection
     *                                              table.
     *                      * ENABLE_ACTIVE_SCAN_RSP    Enable response to active scan only
     *                      * DISABLE_ALL_CONN      Disable response to connection request, including
     *                                              an acitve scan request.
     *
     * Returns: 
     *      None
     *
     * Example:
     *      <code>
     *      // Enable all connection request
     *      MiApp_ConnectionMode(ENABLE_ALL_CONN);
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/ 
    void    MiApp_ConnectionMode(uint8_t Mode);
    
    
    /************************************************************************************
     * Function:
     *      MiApp_FlushTx()
     *
     * Description:        
     *      This macro reset the pointer of the TX buffer. This function is usually called 
     *      before filling application payload.
     *
     * PreCondition:    
     *      Protocol initialization has been done. 
     *
     * Parameters:           
     *      None
     *
     * Returns: 
     *      None
     *
     * Example:
     *      <code>
     *      MiApp_FlushTx();
     *      MiApp_WriteData(AppPayload[0]);
     *      MiApp_WriteData(AppPayload[1]);
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/ 
    #define MiApp_FlushTx()   {TxData = PAYLOAD_START;}
    
    /************************************************************************************
     * Function:
     *      MiApp_WriteData()
     *
     * Description:        
     *      This macro writes one byte of application payload to the TX buffer.
     *
     * PreCondition:    
     *      Protocol initialization has been done. MiApp_FlushTx() has been called before
     *      calling the first MiApp_WriteData for the first time.
     *
     * Parameters:           
     *      uint8_t a - One byte of application payload to be written to the TX buffer
     *
     * Returns: 
     *      None
     *
     * Example:
     *      <code>
     *      MiApp_FlushTx();
     *      MiApp_WriteData(AppPayload[0]);
     *      MiApp_WriteData(AppPayload[1]);
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/ 
    #define MiApp_WriteData(a) TxBuffer[TxData++] = a
    
    /************************************************************************************
     * Function:
     *      bool MiApp_BroadcastPacket(bool SecEn )
     *
     * Summary:
     *      This function broadcast a message in the TxBuffer.
     *
     * Description:        
     *      This is the primary user interface function for the application layer to 
     *      broadcast a message. The application payload is filled in the global char
     *      array TxBuffer.
     *
     * PreCondition:    
     *      Protocol initialization has been done. 
     *
     * Parameters:           
     *      bool SecEn -    The boolean indicates if the application payload needs to be
     *                      secured before transmission.
     *
     * Returns: 
     *      A boolean to indicates if the broadcast procedure is succcessful.
     *
     * Example:
     *      <code>
     *      // Secure and then broadcast the message stored in TxBuffer
     *      MiApp_BroadcastPacket(true);
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/ 
    bool MiApp_BroadcastPacket(bool SecEn );
    
    /************************************************************************************
     * Function:
     *      bool MiApp_UnicastConnection(uint8_t ConnectionIndex, bool SecEn)
     *
     * Summary:
     *      This function unicast a message in the TxBuffer to the device with the input 
     *      ConnectionIndex in the connection table. 
     *
     * Description:        
     *      This is one of the primary user interface functions for the application layer to 
     *      unicast a message. The destination device is in the connection table specified by 
     *      the input parameter ConnectionIndex. The application payload is filled in the 
     *      global char array TxBuffer.
     *
     * PreCondition:    
     *      Protocol initialization has been done. The input parameter ConnectionIndex points to
     *      a valid peer device in the connection table.
     *
     * Parameters: 
     *      uint8_t ConnectionIndex -  The index of the destination device in the connection table.
     *      bool SecEn -    The boolean indicates if the application payload needs to be
     *                      secured before transmission.
     *
     * Returns: 
     *      A boolean to indicates if the unicast procedure is succcessful.
     *
     * Example:
     *      <code>
     *      // Secure and then unicast the message stored in TxBuffer to the first device in 
     *      // the connection table
     *      MiApp_UnicastConnection(0, true);
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/     
    bool MiApp_UnicastConnection(uint8_t ConnectionIndex, bool SecEn);

    /************************************************************************************
     * Function:
     *      bool MiApp_UnicastAddress(uint8_t *DestinationAddress, bool PermanentAddr, bool SecEn)
     *
     * Summary:
     *      This function unicast a message in the TxBuffer to the device with DestinationAddress 
     *
     * Description:        
     *      This is one of the primary user interface functions for the application layer to 
     *      unicast a message. The destination device is specified by the input parameter 
     *      DestinationAddress. The application payload is filled in the global char array TxBuffer.
     *
     * PreCondition:    
     *      Protocol initialization has been done. 
     *
     * Parameters: 
     *      uint8_t * DestinationAddress - The destination address of the unicast
     *      bool PermanentAddr -    The boolean to indicate if the destination address above is a
     *                              permanent address or alternative network address. This parameter
     *                              is only used in a network protocol.         
     *      bool SecEn -    The boolean indicates if the application payload needs to be
     *                      secured before transmission.
     *
     * Returns: 
     *      A boolean to indicates if the unicast procedure is succcessful.
     *
     * Example:
     *      <code>
     *      // Secure and then broadcast the message stored in TxBuffer to the permanent address
     *      // specified in the input parameter.
     *      MiApp_UnicastAddress(DestAddress, TRUE, TRUE);
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/      
    bool MiApp_UnicastAddress(uint8_t *DestinationAddress, bool PermanentAddr, bool SecEn);


    #define BROADCAST_TO_ALL            0x01
    #define MULTICAST_TO_COORDINATORS   0x02
    #define MULTICAST_TO_FFDS           0x03
    /***************************************************************************
     * Received Message information
     *  
     *      This structure contains information about the received application
     *      message.
     **************************************************************************/
    typedef struct
    {
        union
        {
            uint8_t Val;
            struct
            {
                uint8_t		broadcast:  2;      // 1: broadcast message
                uint8_t		ackReq:     1;      // 1: sender request acknowledgement in MAC.
                uint8_t		secEn:      1;      // 1: application payload has been secured
                uint8_t		repeat:     1;      // 1: message received through a repeater
                uint8_t		command:    1;      // 1: message is a command frame
                uint8_t        srcPrsnt:   1;      // 1: source address present in the packet
                uint8_t        altSrcAddr: 1;      // 1: source address is alternative network address
            } bits;
        }  flags;

        API_UINT16_UNION    SourcePANID;                // PAN Identifier of the sender
        uint8_t		*SourceAddress;             // pointer to the source address
        uint8_t		*Payload;                   // pointer to the application payload
        uint8_t		PayloadSize;                // application payload length
        uint8_t		PacketRSSI;                 // RSSI value of the receive message
        uint8_t 	PacketLQI;                  // LQI value of the received message

    } RECEIVED_MESSAGE;
    
    
    /************************************************************************************
     * Function:
     *      bool    MiApp_MessageAvailable(void)
     *
     * Summary:
     *      This function return a boolean if a message is available for the application
     *
     * Description:        
     *      This is the primary user interface functions for the application layer to 
     *      call the Microchip proprietary protocol stack to check if a message is available
     *      for the application. The function will call the protocol stack state machine
     *      to keep the stack running. It is expected that this function should be called
     *      periodically in the application. In case a message is available, all information
     *      related to the recevied message will be stored in the global variable RxMessage
     *      in the format of RECEIVED_MESSAGE.
     *
     * PreCondition:    
     *      Protocol initialization has been done. 
     *
     * Parameters: 
     *      None
     *
     * Returns: 
     *      A boolean to indicates if a message is available for application.
     *
     * Example:
     *      <code>
     *      if( true == MiApp_MessageAvailable() )
     *      {
     *          // handle the received message in global variable RxMessage
     *
     *          // discard the received message after processing
     *          MiApp_DiscardMessage();
     *      }
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/      
    bool    MiApp_MessageAvailable(void);


    /************************************************************************************
     * Function:
     *      void    MiApp_DiscardMessage(void)
     *
     * Summary:
     *      This function discard the current message for the application and notify the 
     *      protocol layer that it is ready to receive the next message.
     *
     * Description:        
     *      This is the primary user interface functions for the application layer to 
     *      discard the current active message, release the system resources used and 
     *      ready to receive the next message. It is must be called after finish handling
     *      the message, otherwise, no further message can be received.
     *
     * PreCondition:    
     *      Protocol initialization has been done. A message has been received by the 
     *      application layer.
     *
     * Parameters: 
     *      None
     *
     * Returns: 
     *      None
     *
     * Example:
     *      <code>
     *      if( true == MiApp_MessageAvailable() )
     *      {
     *          // handle the received message in global variable RxMessage
     *
     *          // discard the received message after processing
     *          MiApp_DiscardMessage();
     *      }
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/      
    void    MiApp_DiscardMessage(void);
    
    #define NOISE_DETECT_ENERGY 0x00
    #define NOISE_DETECT_CS     0x01
    /************************************************************************************
     * Function:
     *      uint8_t MiApp_NoiseDetection(  uint32_t ChannelMap, uint8_t ScanDuration,
     *                                  uint8_t DetectionMode, uint8_t *NoiseLevel)
     *
     * Summary:
     *      This function perform a noise scan and returns the channel with least noise
     *
     * Description:        
     *      This is the primary user interface functions for the application layer to 
     *      perform noise detection on multiple channels.
     *
     * PreCondition:    
     *      Protocol initialization has been done. 
     *
     * Parameters: 
     *      uint32_t ChannelMap -  The bit map of channels to perform noise scan. The 32-bit
     *                          double word parameter use one bit to represent corresponding
     *                          channels from 0 to 31. For instance, 0x00000003 represent to 
     *                          scan channel 0 and channel 1. 
     *      uint8_t ScanDuration - The maximum time to perform scan on single channel. The
     *                          value is from 5 to 14. The real time to perform scan can
     *                          be calculated in following formula from IEEE 802.15.4 
     *                          specification 
     *                              960 * (2^ScanDuration + 1) * 10^(-6) second
     *      uint8_t DetectionMode -    The noise detection mode to perform the scan. The two possible
     *                              scan modes are
     *                              * NOISE_DETECT_ENERGY   Energy detection scan mode
     *                              * NOISE_DETECT_CS       Carrier sense detection scan mode
     *      uint8_t *NoiseLevel -  The noise level at the channel with least noise level
     *
     * Returns: 
     *      The channel that has the lowest noise level
     *
     * Example:
     *      <code>
     *      uint8_t NoiseLevel;
     *      OptimalChannel = MiApp_NoiseDetection(0xFFFFFFFF, 10, NOISE_DETECT_ENERGY, &NoiseLevel);
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/  
    uint8_t    MiApp_NoiseDetection(uint32_t ChannelMap, uint8_t ScanDuration, uint8_t DetectionMode, OUTPUT uint8_t *NoiseLevel);
    
    #define POWER_STATE_SLEEP       0x00
    #define POWER_STATE_WAKEUP      0x01
    #define POWER_STATE_WAKEUP_DR   0x02
    /************************************************************************************
     * Function:
     *      uint8_t    MiApp_TransceiverPowerState(uint8_t Mode)
     *
     * Summary:
     *      This function put the RF transceiver into different power state. i.e. Put the 
     *      RF transceiver into sleep or wake it up.
     *
     * Description:        
     *      This is the primary user interface functions for the application layer to 
     *      put RF transceiver into sleep or wake it up. This function is only available
     *      to those wireless nodes that may have to disable the transceiver to save 
     *      battery power.
     *
     * PreCondition:    
     *      Protocol initialization has been done. 
     *
     * Parameters: 
     *      uint8_t Mode - The mode of power state for the RF transceiver to be set. The possible
     *                  power states are following
     *                  * POWER_STATE_SLEEP     The deep sleep mode for RF transceiver
     *                  * POWER_STATE_WAKEUP    Wake up state, or operating state for RF transceiver
     *                  * POWER_STATE_WAKEUP_DR Put device into wakeup mode and then transmit a 
     *                                          data request to the device's associated device
     *
     * Returns: 
     *      The status of the operation. The following are the possible status
     *      * SUCCESS           Operation successful
     *      * ERR_TRX_FAIL      Transceiver fails to go to sleep or wake up
     *      * ERR_TX_FAIL       Transmission of Data Request command failed. Only available if the
     *                          input mode is POWER_STATE_WAKEUP_DR.
     *      * ERR_RX_FAIL       Failed to receive any response to Data Request command. Only available
     *                          if input mode is POWER_STATE_WAKEUP_DR.
     *      * ERR_INVLAID_INPUT Invalid input mode. 
     *
     * Example:
     *      <code>
     *      // put RF transceiver into sleep
     *      MiApp_TransceiverPowerState(POWER_STATE_SLEEP;
     *
     *      // Put the MCU into sleep
     *      Sleep();    
     *
     *      // wakes up the MCU by WDT, external interrupt or any other means
     *      
     *      // make sure that RF transceiver to wake up and send out Data Request
     *      MiApp_TransceiverPowerState(POWER_STATE_WAKEUP_DR);
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/
    uint8_t    MiApp_TransceiverPowerState(uint8_t Mode);
    #define SUCCESS                 0x00
    #define ERR_TRX_FAIL            0x01
    #define ERR_TX_FAIL             0x02
    #define ERR_RX_FAIL             0x03
    #define ERR_INVALID_INPUT       0xFF

    /*******************************************************************************************
     * Function:
     *      bool MiApp_InitChannelHopping(uint32_t ChannelMap)
     *
     * Summary:
     *      
     *      This function tries to start a channel hopping (frequency agility) procedure
     *
     * Description:        
     *      This is the primary user interface function for the application to do energy 
     *      scan to locate the channel with least noise. If the channel is not current 
     *      operating channel, process of channel hopping will be started.
     *
     * PreCondition:    
     *      Transceiver has been initialized
     *
     * Parameters:      
     *      uint32_t ChannelMap -  The bit map of the candicate channels
     *                          which can be hopped to
     *                  
     * Returns: 
     *                  a boolean to indicate if channel hopping is initiated
     *
     * Example:
     *      <code>
     *      // if condition meets, scan all possible channels and hop 
     *      // to the one with least noise
     *      MiApp_InitChannelHopping(0xFFFFFFFF);
     *      </code>
     *
     * Remark:          The operating channel will change to the optimal 
     *                  channel with least noise
     *
     ******************************************************************************************/
    bool MiApp_InitChannelHopping( uint32_t ChannelMap);
    
    /********************************************************************************************
     * Function:
     *      bool MiApp_ResyncConnection(uint8_t ConnectionIndex, uint32_t ChannelMap)
     *
     * Summary:
     *      This function tries to resynchronize the lost connection with 
     *      peers, probably due to channel hopping
     *
     * Description:        
     *      This is the primary user interface function for the application to resynchronize a 
     *      lost connection. For a RFD device that goes to sleep periodically, it may not 
     *      receive the channel hopping command that is sent when it is sleep. The sleeping 
     *      RFD device depends on this function to hop to the channel that the rest of
     *      the PAN has jumped to. This function call is usually triggered by continously 
     *      communication failure with the peers.
     *
     * PreCondition:    
     *      Transceiver has been initialized
     *
     * Parameters:      
     *      uint32_t ChannelMap -  The bit map of channels to perform noise scan. The 32-bit
     *                          double word parameter use one bit to represent corresponding
     *                          channels from 0 to 31. For instance, 0x00000003 represent to 
     *                          scan channel 0 and channel 1. 
     *                  
     * Returns: 
     *                  a boolean to indicate if resynchronization of connection is successful
     *
     * Example:
     *      <code>
     *      // Sleeping RFD device resync with its associated device, usually the first peer
     *      // in the connection table
     *      MiApp_ResyncConnection(0, 0xFFFFFFFF);
     *      </code>
     *
     * Remark:          
     *      If operation is successful, the wireless node will be hopped to the channel that 
     *      the rest of the PAN is operating on.
     *
     *********************************************************************************************/    
    bool MiApp_ResyncConnection(uint8_t ConnectionIndex, uint32_t ChannelMap);

  /********************************************************************************************
     * Function:
     *      uint8_t Total_Connections(void)
     *
     * Summary:
     *      This function will tell the total number of active and non active connections in Network 
     *
     * Description:        
   *        In order to find the no of devices which are connected to a device in p2p and star , we use 
   *        this function , this will give the total no of active and in active devices in Network
     *      By In active -- ? When a device status bit is in valid , that device is considered dead 
   *        in case of star protocol 
     * PreCondition:    
     *      None 
     *
     * Parameters:      
     *      None
     *                  
     * Returns: 
     *                  Total no of connections available in Connection Table 
     *
     *********************************************************************************************/  
        uint8_t Total_Connections(void);

    #if defined (PROTOCOL_STAR) 
        // End devices in Star Network get the total no of devices in Ntwrk 
        // from PAN Co and they save that in variable end_nodes
        extern uint8_t end_nodes ;
        // Role == PAN CO or END Device
        enum DeviceRole { PanCoordinator , EndDevice };
        extern enum DeviceRole role;
        // In case of star network an FFD||RFD End device also saves its index value.
        // Pan Co has connection table where in it store all the FFD || RFD device Information.
        // Each FFD and RFD device is provided a Index by PAN CO on Connection Request
        // this Index is saved by the FFD || RFD end device
        extern  uint8_t  MyindexinPC;

        /***************************************************************************
         * All Peer FFD's && RFD device Information
         * In Star Network PAN CO , broadcasts its Connection Table Information to all its child
         * FFD's && RFD's. This information is gives FFD's && RFD's idea of Peer FFD's || RFD's
         * Presence
         * . It is the element structure.
         * Note : The below structure is used by only end devices(FFD's || RFD's) in a star network
         **************************************************************************/
        typedef struct __END_DEVICES_Unique_Short_Address
        {
            // Short address of peer END device
            uint8_t        Address[3]; 
            //Each end device belongs to a particular connection slot in PAN CO
            uint8_t        connection_slot;    

        } END_DEVICES_Unique_Short_Address;
        extern  END_DEVICES_Unique_Short_Address  END_DEVICES_Short_Address[CONNECTION_SIZE];
        // lost connection is true if a END device is considered In active by PAN CO
        // though the device is active
        extern bool lost_connection;
        //Used by END_DEVICE to store the index value shared by PAN Co on join
        extern uint8_t myConnectionIndex_in_PanCo;
        
        /************************************************************************************
        * Function:
        *      void MiApp_BroadcastConnectionTable(void)
        *
        * Summary:
        *      This function is  a command type packet which is used by Pan Co
        *
        * Description:        
        *      This function is used by only PAN CO in a Star network and is a cmd
        *      type packet. PAN CO in our demo is braodcasting Connection Table details
        *      with every other END_DEVICE (FFD's || RFD's)in the Network, A new peer 
        *     END_DEVICE joins PAN C0 , then Pan Co will again broadcast the new connection 
        *     Table , As its the duty of PAN Co to keep END_DEVICES Updated of the new nodes
        *     added to network   
        *
        * PreCondition:    
        *      Protocol initialization has been done. 
        *
        * Returns: 
        *      None.
        *
        * Remarks:    
        *      None
        *
        *****************************************************************************************/    
       void MiApp_BroadcastConnectionTable();



        #if defined(ENABLE_LINK_STATUS)
            /*********************************************************************
            * Function:        void send_link_status(void)
            *
            * PreCondition:     This function will only be used by End devices(FFD's || RFD's)  
            *
            * Input:             None

            *
            * Output:           None  
            *
            * Side Effects:	    Will Qualify a Device to be active in PAN Co's connection Table 
            *
            * Description :     Function will send a link status , (heart beat ) to PAN CO , which 
            *                         which denotes that the Device is active participant in Star Network
            *                  	    
            **********************************************************************/  
            void send_link_status(void);


            /*********************************************************************
            * Function:        void Find_InActiveDevices(void)
            *
            * PreCondition:     This function is valid for Pan Co  
            *
            * Input:             None

            *
            * Output:           None  
            *
            * Side Effects:	    Inactive devices in Network , will be found 
             *                              
            *
            * Description :     This function will check which devices in the Network have been inactive.
             *                      If a device link stat is 0x00 , it is qualified as inactive. It is the duty of END
             *                      Devices (FFD's || RFD's) to send link status , whenever a END Device , sends 
             *                      link status , link_stat[ED] increments by 1 , denoting its a Active part of the 
             *                      Star Network
             * 
             * Note : If a End device has  requested PAN Co to make it a permanently active device , this function
             *          does not evaluate that particular END_DEVICE's link status.
            **********************************************************************/  
            void Find_InActiveDevices(void);
            
        #endif

        /*********************************************************************
        * Function:        void MiApp_leave_network(void)
        *
        * PreCondition:     This function is valid for End Devices (FFD's && RFD's)
        *
        * Input:            none

        *
        * Output:           None  
        *
        * Side Effects:	    Mandatory Leave 
         *                              
        *
        * Description :     This function will be used by End Devices to leave 
        *                   the Network.The device will be considered inactive 
        *                   by PAN COR Immediately
        * 
        * Note :               The Device can join back in the Ntwk Again , 
        **********************************************************************/   
        void MiApp_leave_network(void);

        /*********************************************************************
        * Function:          bool MiApp_UnicastStar  (uint8_t EndDevice_ConnectionIndex , uint8_t );
        *
        * PreCondition:     This function is valid for End Devices
        *
        * Input:            byte -- Input from APP layer
         *                      Unique index for each END_DEVICE in Star Network 
        *                       This value is stored as magic_no in the END_DEVICE
         *                      *pointer--> User Data Pointer 
         * 
        * Output:               True/ Fale -- Success TX or Fail TX  
        *
        * Side Effects:     None 
         *                              
        *
        * Description :     This function will be used by End Devices to TX data to another 
         *                          END_DEVICE in a STAR Network
         * 
         * Note :              TX happens EDx--> PAN CO--> EDy
         *                          First 4 bytes are occupied by the cmd and DestEndDevice address
         *                          Remaining space is user data
        **********************************************************************/ 
        bool MiApp_UnicastStar (uint8_t EndDevice_ConnectionIndex , bool SecEn);

    #endif    
        

    // Callback functions
    #define MiApp_CB_AllowConnection(handleInConnectionTable) true
    //BOOL MiApp_CB_AllowConnection(uint8_t handleInConnectionTable);
    #define MiApp_CB_RFDAcknowledgement(SourceShortAddress, AcknowledgementSeqNum)
    //void MiApp_CB_RFDAcknowledgement(uint16_t sourceAddr, uint8_t Seq);
    
    extern RECEIVED_MESSAGE rxMessage;
    #if defined(ENABLE_TIME_SYNC)
        extern API_UINT16_UNION CounterValue;
        extern API_UINT16_UNION WakeupTimes;
    #endif
#endif

