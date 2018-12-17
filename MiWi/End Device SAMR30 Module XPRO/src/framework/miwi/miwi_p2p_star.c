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

/************************ HEADERS **********************************/
#include "config/symbol.h"
#include "asf.h"
#include "config/miwi_config.h"
#include "config/config_atrf86.h"
#include "config/miwi_config_p2p.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "framework/driver/drv_miwi_at86rf.h"
#include "framework/miwi/miwi_p2p_star.h"
//#include "framework/miwi/miwi_nvm.h"
#include "framework/miwi/miwi_api.h"



//Global Variables
// In p2p total no of devices , In star used only by PAN COR to calculate no of END Devices
uint8_t conn_size ;
// Used by END_DEVICE to store the index value shared by PAN Co on join
uint8_t MyindexinPC;  
#if defined(PROTOCOL_STAR)
    uint8_t myConnectionIndex_in_PanCo;
    MIWI_TICK tick1,tick2 , tick3 , tick4 ;
    bool FW_Stat;
    bool lost_connection = false;
#endif

uint8_t temp_bit;


/************************ VARIABLES ********************************/
uint8_t myLongAddress[8];

// Evaluate Total No of Peer Connection on a Node
uint8_t Total_Connections(void)
{
    uint8_t count=0 , i;
    for (i=0;i<CONNECTION_SIZE;i++)
    {
    #if defined(PROTOCOL_STAR)
        if (ConnectionTable[i].Address[0] != 0x00 || ConnectionTable[i].Address[1] != 0x00 || ConnectionTable[i].Address[2] != 0x00)
        {
            count++;
        }
    #else
        if (ConnectionTable[i].status.bits.isValid)
        {
            count++;
        }
    #endif      
    }
    return count;
}


#if defined(PROTOCOL_STAR)
    // Static Variables
    // used by PAN COR to generate SW_LAYER ACK
    bool rec_ack ;
    
    // Global Variables
    // used by END_DEVICES to store total no of end_devices in network
    uint8_t end_nodes = 0; 
    END_DEVICES_Unique_Short_Address  END_DEVICES_Short_Address[CONNECTION_SIZE];
    enum DeviceRole role; // Role of Device in Star Network

    
    #if defined(ENABLE_LINK_STATUS)
            uint8_t link_stat[CONNECTION_SIZE] ;
            uint8_t permanent_connections[CONNECTION_SIZE] ;
    #endif

    // All connections (FFD || RFD)are stored in Connection Table of PAN CO
    // Each Connection is identified by its index no.
    // In case of Data TX , EDx --> PAN CO --> EDy
    // PAN CO will forward the data to EDy , In order to know 
    uint8_t Find_Index (uint8_t *DestAddr)
    {
        uint8_t i;
        uint8_t return_val;
        for (i=0;i<conn_size;i++)
        {
            if (ConnectionTable[i].status.bits.isValid)
            {
                if (DestAddr[1] == ConnectionTable[i].Address[0] && DestAddr[2] == ConnectionTable[i].Address[1] && DestAddr[3] == ConnectionTable[i].Address[2] )
                {
                    return_val = i;
                    break;
                }
                
            }
        }
        if (i==conn_size)
            {
                return_val = 0xff;
            }
        return return_val;
    }
     
    
    void handle_lost_connection(void)
    {
        uint8_t i ;
        bool stat = false;
        if (role == EndDevice)
        {
            for (i=0;i<end_nodes+1;i++)
            {
                if (myLongAddress[0] == END_DEVICES_Short_Address[i].Address[0] && myLongAddress[1] == END_DEVICES_Short_Address[i].Address[1])
                {
                    stat = true;
                }
            }
            if (!stat)
            {
                lost_connection = true;
            }
        }
        
    }
     
    // Function to store the Connection Table Information which is Broadcasted by PAN CO
    // Used by END_DEVICES (FFD || RFD) only
    void store_connection_tb(uint8_t *payload)
    { 
        uint8_t i , j ;
        for (i=4; i < RX_BUFFER_SIZE; i++)
        {
            if (i % 4 == 0 && (i+3) < RX_BUFFER_SIZE)
            {
                j = payload[i+3];
                END_DEVICES_Short_Address[j].connection_slot = j;
                END_DEVICES_Short_Address[j].Address[0] = payload[i];
                END_DEVICES_Short_Address[j].Address[1] = payload[i+1];
                END_DEVICES_Short_Address[j].Address[2] = payload[i+2];
                // END Device is Active but is being wrongly considered as Inactive by PAN CO
                
            }
            
            
        }
        handle_lost_connection();
        
      
    }
    
    


    /// This function will only be called by Pan co ordinator to forward the packet to the destination end device
    bool Forward_Packet_PANCr_to_EDy(uint8_t *fw_payload ,uint8_t index , uint8_t *EDxAddress)
    {
        uint8_t i;
        bool status;
        MiApp_FlushTx();
        MiApp_WriteData(EDxAddress[0]);    // Unique address of EDy (DEST ED) 
        MiApp_WriteData(EDxAddress[1]);    // Unique address of EDy (DEST ED) 
        MiApp_WriteData(EDxAddress[2]);    // Unique address of EDy (DEST ED) 
        for(i = 3; i < TX_BUFFER_SIZE; i++)
        {
            MiApp_WriteData(fw_payload[i]);
        }
        #if defined(ENABLE_SECURITY)
                status = MiApp_UnicastConnection (index,true) ;
        #else
                status = MiApp_UnicastConnection (index,false) ;
        #endif
        
        return status;
                    
    }

#endif

#if defined(ENABLE_ED_SCAN) || defined(ENABLE_ACTIVE_SCAN) || defined(ENABLE_FREQUENCY_AGILITY)
    // Scan Duration formula for P2P Connection:
    //  60 * (2 ^ n + 1) symbols, where one symbol equals 16us
    #define SCAN_DURATION_0 SYMBOLS_TO_TICKS(120)
    #define SCAN_DURATION_1 SYMBOLS_TO_TICKS(180)
    #define SCAN_DURATION_2 SYMBOLS_TO_TICKS(300)
    #define SCAN_DURATION_3 SYMBOLS_TO_TICKS(540)
    #define SCAN_DURATION_4 SYMBOLS_TO_TICKS(1020)
    #define SCAN_DURATION_5 SYMBOLS_TO_TICKS(1980)
    #define SCAN_DURATION_6 SYMBOLS_TO_TICKS(3900)
    #define SCAN_DURATION_7 SYMBOLS_TO_TICKS(7740)
    #define SCAN_DURATION_8 SYMBOLS_TO_TICKS(15420)
    #define SCAN_DURATION_9 SYMBOLS_TO_TICKS(30780)
    #define SCAN_DURATION_10 SYMBOLS_TO_TICKS(61500)
    #define SCAN_DURATION_11 SYMBOLS_TO_TICKS(122940)
    #define SCAN_DURATION_12 SYMBOLS_TO_TICKS(245820)
    #define SCAN_DURATION_13 SYMBOLS_TO_TICKS(491580)
    #define SCAN_DURATION_14 SYMBOLS_TO_TICKS(983100)
    const  uint32_t ScanTime[15] = {SCAN_DURATION_0,SCAN_DURATION_1,SCAN_DURATION_2,SCAN_DURATION_3,
        SCAN_DURATION_4,SCAN_DURATION_5,SCAN_DURATION_6,SCAN_DURATION_7,SCAN_DURATION_8,SCAN_DURATION_9,
        SCAN_DURATION_10, SCAN_DURATION_11, SCAN_DURATION_12, SCAN_DURATION_13,SCAN_DURATION_14
    };
#endif



uint8_t TxBuffer[TX_BUFFER_SIZE];


#ifdef ENABLE_INDIRECT_MESSAGE
   
        INDIRECT_MESSAGE indirectMessages[INDIRECT_MESSAGE_SIZE];   // structure to store the indirect messages
                                                                    // for nodes with radio off duing idle time
#endif


    CONNECTION_ENTRY    ConnectionTable[CONNECTION_SIZE];


#if defined(IEEE_802_15_4)
    API_UINT16_UNION        myPANID;                    // the PAN Identifier for the device
#endif
uint8_t            currentChannel = 0;             // current operating channel for the device
uint8_t            TxData;
uint8_t            ConnMode = DISABLE_ALL_CONN;
uint8_t            P2PCapacityInfo;
RECEIVED_MESSAGE  rxMessage;                    // structure to store information for the received packet
uint8_t            LatestConnection;
volatile P2P_STATUS P2PStatus;
extern uint8_t     AdditionalNodeID[];             // the additional information regarding the device
                                                // that would like to share with the peer on the 
                                                // other side of P2P connection. This information 
                                                // is applicaiton specific. 
#if defined(ENABLE_ACTIVE_SCAN)
    uint8_t    ActiveScanResultIndex;
    ACTIVE_SCAN_RESULT ActiveScanResults[ACTIVE_SCAN_RESULT_SIZE];  // The results for active scan, including
                                                                    // the PAN identifier, signal strength and 
                                                                    // operating channel
#endif

#ifdef ENABLE_SLEEP
    MIWI_TICK DataRequestTimer;
#endif

MAC_RECEIVED_PACKET MACRxPacket;

#if defined(ENABLE_SECURITY)
    API_UINT32_UNION IncomingFrameCounter[CONNECTION_SIZE];  // If authentication is used, IncomingFrameCounter can prevent replay attack
#endif

#if defined(ENABLE_NETWORK_FREEZER)
    MIWI_TICK nvmDelayTick;
#endif


#if defined(ENABLE_TIME_SYNC)
    #if defined(ENABLE_SLEEP)
        uint16_t_VAL WakeupTimes;
        uint16_t_VAL CounterValue;
    #elif defined(ENABLE_INDIRECT_MESSAGE)
        uint8_t TimeSyncSlot = 0;
        MIWI_TICK TimeSyncTick;
        MIWI_TICK TimeSlotTick;
    #endif
#endif

/************************ FUNCTION DEFINITION ********************************/
uint8_t AddConnection(void);

#if defined(IEEE_802_15_4)
    bool SendPacket(INPUT bool Broadcast, INPUT API_UINT16_UNION DestinationPANID,
                    INPUT uint8_t *DestinationAddress, INPUT bool isCommand,
                    INPUT bool SecurityEnabled);
#else
    bool SendPacket(INPUT bool Broadcast,
                    INPUT uint8_t *DestinationAddress, INPUT bool isCommand,
                    INPUT bool SecurityEnabled);
#endif                    

#ifdef ENABLE_FREQUENCY_AGILITY
    void StartChannelHopping(INPUT uint8_t OptimalChannel);
#endif

bool CheckForData(void);

// Function Prototypes
void MacroNop(void);
void P2PTasks(void);
/************************ FUNCTIONS ********************************/

void MacroNop(void)
{
   nop();
}    

/*********************************************************************
 * void P2PTasks( void )
 *
 * Overview:        This function maintains the operation of the stack
 *                  It should be called as often as possible. 
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    The stack receives, handles, buffers, and transmits 
 *                  packets.  It also handles all of the joining 
 * 
 ********************************************************************/
void P2PTasks(void)
{
    uint8_t i;

            


    #if defined(PROTOCOL_STAR) 
        tick2.Val = MiWi_TickGet();
        tick3.Val = MiWi_TickGet();
        #if defined(ENABLE_LINK_STATUS)
            if (role == PanCoordinator)
            {
                if( MiWi_TickGetDiff(tick3, tick4) > (FIND_INACTIVE_DEVICE_TIMEOUT))
                {
                    Find_InActiveDevices ();
                    tick4 = tick3;
                } 

            }
            else
            {
                if( MiWi_TickGetDiff(tick3, tick1) > (LINK_STATUS_TIMEOUT) )
                {
                        send_link_status ();
                        tick1 = tick3;
                }
            }
        #endif

        #if defined(ENABLE_PERIODIC_CONNECTIONTABLE_SHARE)
            if( MiWi_TickGetDiff(tick2, tick1) > (SHARE_PEER_DEVICE_INFO_TIMEOUT) && role == PanCoordinator)
            {
                MiApp_BroadcastConnectionTable ();
                tick1 = tick2;
            }
        #endif
    #endif

    
    #ifdef ENABLE_INDIRECT_MESSAGE
        // check indirect message periodically. If an indirect message is not acquired within
        // time of INDIRECT_MESSAGE_TIMEOUT
		MIWI_TICK   tmpTick;
        for(i = 0; i < INDIRECT_MESSAGE_SIZE; i++)
        {
            if( indirectMessages[i].flags.bits.isValid )
            {
                tmpTick.Val = MiWi_TickGet();
                if( MiWi_TickGetDiff(tmpTick, indirectMessages[i].TickStart) > INDIRECT_MESSAGE_TIMEOUT )
                {
                    indirectMessages[i].flags.Val = 0x00;   
                    Printf("\r\nIndirect message expired");
                }    
            }    
        }
    #endif
    
    #ifdef ENABLE_SLEEP
        // check if a response for Data Request has been received with in 
        // time of RFD_DATA_WAIT, defined in P2P.h. Expire the Data Request
        // to let device goes to sleep, if no response is received. Save
        // battery power even if something wrong with associated device
        if( P2PStatus.bits.DataRequesting )
        {
            tmpTick.Val = MiWi_TickGet();
            if( MiWi_TickGetDiff(tmpTick, DataRequestTimer) > RFD_DATA_WAIT )
            {
                Printf("Data Request Expired\r\n");
                P2PStatus.bits.DataRequesting = 0;
                #if defined(ENABLE_TIME_SYNC)
                    WakeupTimes.Val = RFD_WAKEUP_INTERVAL / 16;
                    CounterValue.Val = 0xFFFF - ((uint16_t)4000*(RFD_WAKEUP_INTERVAL % 16));
                #endif
            }
        }
    #endif

    #if defined(ENABLE_NETWORK_FREEZER)
        if( P2PStatus.bits.SaveConnection )
        {
            tmpTick.Val = MiWi_TickGet();
            if( MiWi_TickGetDiff(tmpTick, nvmDelayTick) > (ONE_SECOND) )
            {
                P2PStatus.bits.SaveConnection = 0;
                nvmPutConnectionTable(ConnectionTable);  
                //Printf("\r\nSave Connection\r\n");   
            }
        }
    #endif

    #if defined(ENABLE_TIME_SYNC) && !defined(ENABLE_SLEEP) && defined(ENABLE_INDIRECT_MESSAGE)
        tmpTick = MiWi_TickGet();
        if( MiWi_TickGetDiff(tmpTick, TimeSyncTick) > ((ONE_SECOND) * RFD_WAKEUP_INTERVAL) )
        {
            TimeSyncTick.Val += ((uint32_t)(ONE_SECOND) * RFD_WAKEUP_INTERVAL);
            if( TimeSyncTick.Val > tmpTick.Val )
            {
                TimeSyncTick.Val = tmpTick.Val;
            }    
            TimeSyncSlot = 0;
        }    
    #endif

  // Check if transceiver receive any message.
    if( P2PStatus.bits.RxHasUserData == 0 && MiMAC_ReceivedPacket() )
    {
        rxMessage.flags.Val = 0;
        //rxMessage.flags.bits.broadcast = MACRxPacket.flags.bits.broadcast;
        
            temp_bit = MACRxPacket.flags.bits.broadcast;
            rxMessage.flags.bits.broadcast = temp_bit;

        rxMessage.flags.bits.secEn = MACRxPacket.flags.bits.secEn;
        rxMessage.flags.bits.command = (MACRxPacket.flags.bits.packetType == PACKET_TYPE_COMMAND) ? 1:0;
        rxMessage.flags.bits.srcPrsnt = MACRxPacket.flags.bits.sourcePrsnt;
        if( MACRxPacket.flags.bits.sourcePrsnt )
        {
            rxMessage.SourceAddress = MACRxPacket.SourceAddress;
        }
        #if defined(IEEE_802_15_4) && !defined(TARGET_SMALL)
            rxMessage.SourcePANID.Val = MACRxPacket.SourcePANID.Val;
        #endif

        rxMessage.PayloadSize = MACRxPacket.PayloadLen;
        rxMessage.Payload = MACRxPacket.Payload;
        
        
            
               
        
        /************************/
      
        #ifndef TARGET_SMALL
            rxMessage.PacketLQI = MACRxPacket.LQIValue;
            rxMessage.PacketRSSI = MACRxPacket.RSSIValue;
        #endif

        if( rxMessage.flags.bits.command )
        {
            // if comes here, we know it is a command frame
            switch( rxMessage.Payload[0] )
            {
                #if defined(ENABLE_HAND_SHAKE)
				#if defined(R) || defined(PC) 
                    case CMD_P2P_CONNECTION_REQUEST:
                        {
                            #if defined(PROTOCOL_STAR)
                                if(role == PanCoordinator)   // Important if implementing a star network
                                {    
                            #endif
                                  
                            // if a device goes to sleep, it can only have one
                            // connection, as the result, it cannot accept new
                            // connection request
                            #ifdef ENABLE_SLEEP
                                MiMAC_DiscardPacket();
                                break;
                            #else
                                
                                uint8_t status = STATUS_SUCCESS;
                                
                                // if channel does not math, it may be a 
                                // sub-harmonics signal, ignore the request
                                if( currentChannel != rxMessage.Payload[1] )
                                {
                                    MiMAC_DiscardPacket();
                                    break;
                                }
                                
                                // if new connection is not allowed, ignore 
                                // the request
                                if( ConnMode == DISABLE_ALL_CONN )
                                { 
                                    MiMAC_DiscardPacket();
                                    break;
                                }
                                
                                #if !defined(TARGET_SMALL) && defined(IEEE_802_15_4)
                                    // if PANID does not match, ignore the request
                                    if( rxMessage.SourcePANID.Val != 0xFFFF &&
                                        rxMessage.SourcePANID.Val != myPANID.Val &&
                                        rxMessage.PayloadSize > 2)
                                    {
                                        status = STATUS_NOT_SAME_PAN;
                                    }
                                    else
                                #endif
                                {
                                    // request accepted, try to add the requesting
                                    // device into P2P Connection Entry
                                    status = AddConnection();
                                }
                                #if defined(PROTOCOL_STAR) && defined(ENABLE_LINK_STATUS) 
                                    uint8_t p;
                                    if (rxMessage.Payload[3] == 0xAA)
                                    {
 
                                        for (p = 0 ;p <CONNECTION_SIZE;p++)
                                        {
                                            if (isSameAddress(rxMessage.SourceAddress, ConnectionTable[p].Address) )
                                            {
                                               permanent_connections[p] = 0xFF;

                                            }

                                        }
                                    }
                                #endif
							
                                if( (ConnMode == ENABLE_PREV_CONN) && (status != STATUS_EXISTS && status != STATUS_ACTIVE_SCAN) )
                                {
                                    status = STATUS_NOT_PERMITTED;
                                }

                                if( (status == STATUS_SUCCESS || status == STATUS_EXISTS ) &&
                                    MiApp_CB_AllowConnection(LatestConnection) == false )
                                {
                                    ConnectionTable[LatestConnection].status.Val = 0;
                                    status = STATUS_NOT_PERMITTED;
                                }
                                    
                                
                                // prepare the P2P_CONNECTION_RESPONSE command
                                MiApp_FlushTx();
                                MiApp_WriteData(CMD_P2P_CONNECTION_RESPONSE);
                                MiApp_WriteData(status);
                                MiApp_WriteData(MyindexinPC);
								#if defined (R)
									MiApp_WriteData(0xA1); // I'm a Router
								#endif
								#if defined (PC)
									MiApp_WriteData(0xB1); // I'm the PC
								#endif
                                if( status == STATUS_SUCCESS ||
                                    status == STATUS_EXISTS )
                                {
                                    MiApp_WriteData(P2PCapacityInfo);
                                    #if ADDITIONAL_NODE_ID_SIZE > 0
                                        for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
                                        {
                                            MiApp_WriteData(AdditionalNodeID[i]);
                                        }
                                    #endif
                                }
                                
                                MiMAC_DiscardPacket();
                                
                                // unicast the response to the requesting device
                                #ifdef TARGET_SMALL
                                    #if defined(IEEE_802_15_4)
                                        SendPacket(false, myPANID, rxMessage.SourceAddress, true, rxMessage.flags.bits.secEn);
                                    #else
                                        SendPacket(false, rxMessage.SourceAddress, true, rxMessage.flags.bits.secEn);
                                    #endif
                                #else
                                        
                                    #if defined(IEEE_802_15_4)
                                        SendPacket(false, rxMessage.SourcePANID, rxMessage.SourceAddress, true, rxMessage.flags.bits.secEn);
                                    #else
                                        SendPacket(false, rxMessage.SourceAddress, true, rxMessage.flags.bits.secEn);
                                    #endif 
                                #endif

                                #if defined(PROTOCOL_STAR)
                                    //MiApp_BroadcastConnectionTable ();  // Broad Cast the CT on join
                                #endif
                                #if defined(ENABLE_NETWORK_FREEZER)
                                    if( status == STATUS_SUCCESS )
                                    {
                                        nvmPutConnectionTableIndex(&(ConnectionTable[LatestConnection]), LatestConnection);
                                    }
                                #endif
      
                                     
                            #endif  // end of ENABLE_SLEEP
                            #if defined(PROTOCOL_STAR)
                                    }     // Important if Implementing a Star Network
                            #endif
							
							#if defined(PROTOCOL_STAR)
                              else
                              {
                                  MiMAC_DiscardPacket();
                              }
							#endif
                              
                        }
                        break; 
				#endif
                    case CMD_P2P_ACTIVE_SCAN_REQUEST:
                        {
                            if(ConnMode > ENABLE_ACTIVE_SCAN_RSP)
                            {
                                MiMAC_DiscardPacket();
                                break;
                            }
                            if( currentChannel != rxMessage.Payload[1] )
                            {
                                MiMAC_DiscardPacket();
                                break;
                            }
                            
                            MiApp_FlushTx();
                            MiApp_WriteData(CMD_P2P_ACTIVE_SCAN_RESPONSE);
                            MiApp_WriteData(P2PCapacityInfo);
                            #if ADDITIONAL_NODE_ID_SIZE > 0
                                for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
                                {
                                    MiApp_WriteData(AdditionalNodeID[i]);
                                }
                            #endif
                            MiMAC_DiscardPacket();
                            
                            // unicast the response to the requesting device
                            #ifdef TARGET_SMALL
                                #if defined(IEEE_802_15_4)
                                    SendPacket(false, myPANID, rxMessage.SourceAddress, true, rxMessage.flags.bits.secEn);
                                #else
                                    SendPacket(false, rxMessage.SourceAddress, true, rxMessage.flags.bits.secEn);
                                #endif
                            #else
                                #if defined(IEEE_802_15_4)
                                    SendPacket(false, rxMessage.SourcePANID, rxMessage.SourceAddress, true, rxMessage.flags.bits.secEn);
                                #else
                                    SendPacket(false, rxMessage.SourceAddress, true, rxMessage.flags.bits.secEn);
                                #endif
                            #endif
                        }
                        break;
                    
                        
                        
                    
                    
                    #ifndef TARGET_SMALL    
                    case CMD_P2P_CONNECTION_REMOVAL_REQUEST:
                        {         
                            MiApp_FlushTx();
                            MiApp_WriteData(CMD_P2P_CONNECTION_REMOVAL_RESPONSE);

                            for(i = 0; i < CONNECTION_SIZE; i++)
                            {
                                // if the record is valid
                                if( ConnectionTable[i].status.bits.isValid )
                                {
                                    // if the record is the same as the requesting device
                                    if( isSameAddress(rxMessage.SourceAddress, ConnectionTable[i].Address) )
                                    {
                                        // find the record. disable the record and
                                        // set status to be SUCCESS
                                        ConnectionTable[i].status.Val = 0;
                                        #if defined(ENABLE_NETWORK_FREEZER)
                                            nvmPutConnectionTableIndex(&(ConnectionTable[i]), i);
                                        #endif
                                        MiApp_WriteData(STATUS_SUCCESS);

                                        break;
                                    }
                                } 
                            }

                            MiMAC_DiscardPacket();

                            if( i == CONNECTION_SIZE ) 
                            {
                                // not found, the requesting device is not my peer
                                MiApp_WriteData(STATUS_ENTRY_NOT_EXIST);
                            }
                            #ifdef TARGET_SMALL
                                #if defined(IEEE_802_15_4)
                                    SendPacket(false, myPANID, rxMessage.SourceAddress, true, rxMessage.flags.bits.secEn);
                                #else
                                    SendPacket(false, rxMessage.SourceAddress, true, rxMessage.flags.bits.secEn);
                                #endif
                            #else
                                #if defined(IEEE_802_15_4)
                                    SendPacket(false, rxMessage.SourcePANID, rxMessage.SourceAddress, true, rxMessage.flags.bits.secEn);
                                #else
                                    SendPacket(false, rxMessage.SourceAddress, true, rxMessage.flags.bits.secEn);
                                #endif
                            #endif

                        }
                            break;
                    #endif
                    
                    case CMD_P2P_CONNECTION_RESPONSE:
                        {
                            switch( rxMessage.Payload[1] )
                            {              
                                case STATUS_SUCCESS:
                                case STATUS_EXISTS:
                                    #if defined(IEEE_802_15_4)
                                        if( myPANID.Val == 0xFFFF )
                                        {
                                            myPANID.Val = rxMessage.SourcePANID.Val;
                                            {
                                                uint16_t tmp = 0xFFFF;
                                                MiMAC_SetAltAddress((uint8_t *)&tmp, (uint8_t *)&myPANID.Val);
                                            }    
                                            #if defined(ENABLE_NETWORK_FREEZER)
                                                nvmPutMyPANID(myPANID.v);
                                            #endif
                                        }
                                    #endif
                                    AddConnection();
                                    #if defined(PROTOCOL_STAR)
                                        myConnectionIndex_in_PanCo = rxMessage.Payload[2];
										#if defined (ENABLE_NETWORK_FREEZER)
											nvmPutMyIndex(&myConnectionIndex_in_PanCo);
										#endif
                                    #endif
                                    
                                    #if defined(ENABLE_NETWORK_FREEZER)
                                        P2PStatus.bits.SaveConnection = 1;
                                        nvmDelayTick.Val = MiWi_TickGet();
                                    #endif
                                    break;
                                default:
                                    break;
                            }                        
                        }
                        MiMAC_DiscardPacket();
                        break; 
                    
                    
                    case CMD_P2P_ACTIVE_SCAN_RESPONSE:
                        {
                            if( P2PStatus.bits.Resync )
                            {
                                P2PStatus.bits.Resync = 0;   
                            }
                            #ifdef ENABLE_ACTIVE_SCAN 
                                else   
                                {
                                    i = 0;
                                    for(; i < ActiveScanResultIndex; i++)
                                    {
                                        if( (ActiveScanResults[i].Channel == currentChannel) &&
                                        #if defined(IEEE_802_15_4)
                                            (ActiveScanResults[i].PANID.Val == rxMessage.SourcePANID.Val) &&
                                        #endif
                                            isSameAddress(ActiveScanResults[i].Address, rxMessage.SourceAddress)
                                        )
                                        {
                                            break;
                                        }
                                    }
                                    if( i == ActiveScanResultIndex && (i < ACTIVE_SCAN_RESULT_SIZE))
                                    {
                                        ActiveScanResults[ActiveScanResultIndex].Channel = currentChannel;
                                        ActiveScanResults[ActiveScanResultIndex].RSSIValue = rxMessage.PacketRSSI;
                                        ActiveScanResults[ActiveScanResultIndex].LQIValue = rxMessage.PacketLQI;
                                        #if defined(IEEE_802_15_4)
                                            ActiveScanResults[ActiveScanResultIndex].PANID.Val = rxMessage.SourcePANID.Val;
                                        #endif
                                        for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                                        {
                                            ActiveScanResults[ActiveScanResultIndex].Address[i] = rxMessage.SourceAddress[i];
                                        }
                                        ActiveScanResults[ActiveScanResultIndex].Capability.Val = rxMessage.Payload[1];
                                        #if ADDITIONAL_NODE_ID_SIZE > 0
                                            for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
                                            {
                                                ActiveScanResults[ActiveScanResultIndex].PeerInfo[i] = rxMessage.Payload[2+i];
                                            }
                                        #endif
                                        ActiveScanResultIndex++;
                                    }
                                }
                            #endif

                            MiMAC_DiscardPacket(); 
                        }
                        break;                
                    
                    #ifndef TARGET_SMALL
                    case CMD_P2P_CONNECTION_REMOVAL_RESPONSE:
                        {
                            if( rxMessage.Payload[1] == STATUS_SUCCESS )
                            {
                                for(i = 0; i < CONNECTION_SIZE; i++)
                                {
                                    // if the record is valid
                                    if( ConnectionTable[i].status.bits.isValid )
                                    {
                                        // if the record address is the same as the requesting device
                                        if( isSameAddress(rxMessage.SourceAddress, ConnectionTable[i].Address) )
                                        {
                                            // invalidate the record
                                            ConnectionTable[i].status.Val = 0;
                                            #if defined(ENABLE_NETWORK_FREEZER)
                                                nvmPutConnectionTableIndex(&(ConnectionTable[i]), i);
                                            #endif
                                            break;
                                        }
                                    } 
                                }
                            }
                        }
                        MiMAC_DiscardPacket();
                        break;
                    #endif
                #endif
                
                #ifdef ENABLE_INDIRECT_MESSAGE
                    case CMD_DATA_REQUEST:
                    case CMD_MAC_DATA_REQUEST: 
                        {
                            bool isCommand = false;
                            MIWI_TICK tmpW;
                            
                            MiApp_FlushTx();
                            
                            #if defined(ENABLE_TIME_SYNC) && !defined(ENABLE_SLEEP)
                                MiApp_WriteData(CMD_TIME_SYNC_DATA_PACKET);
                                isCommand = true;
                                tmpTick.Val = MiWi_TickGet();
                                //tmpW.Val = (((ONE_SECOND) * RFD_WAKEUP_INTERVAL) - (tmpTick.Val - TimeSyncTick.Val) + ( TimeSlotTick.Val * TimeSyncSlot ) ) / (ONE_SECOND * 16);
                                //tmpW.Val = (((ONE_SECOND) * RFD_WAKEUP_INTERVAL) - (tmpTick.Val - TimeSyncTick.Val) + ( TimeSlotTick.Val * TimeSyncSlot ) ) / SYMBOLS_TO_TICKS((uint32_t)0xFFFF * MICRO_SECOND_PER_COUNTER_TICK / 16);
                                tmpW.Val = (((ONE_SECOND) * RFD_WAKEUP_INTERVAL) - (tmpTick.Val - TimeSyncTick.Val) + ( TimeSlotTick.Val * TimeSyncSlot ) ) / SYMBOLS_TO_TICKS((uint32_t)0xFFFF * MICRO_SECOND_PER_COUNTER_TICK / 16);
                                MiApp_WriteData(tmpW.v[0]);
                                MiApp_WriteData(tmpW.v[1]);
                                //tmpW.Val = 0xFFFF - (uint16_t)((TICKS_TO_SYMBOLS((((ONE_SECOND) * RFD_WAKEUP_INTERVAL) - (tmpTick.Val - TimeSyncTick.Val)  + ( TimeSlotTick.Val * TimeSyncSlot ) + TimeSlotTick.Val/2 - (ONE_SECOND * tmpW.Val * 16) )) * 16 / 250));
                                //tmpW.Val = 0xFFFF - (uint16_t)((TICKS_TO_SYMBOLS((((ONE_SECOND) * RFD_WAKEUP_INTERVAL) - (tmpTick.Val - TimeSyncTick.Val)  + ( TimeSlotTick.Val * TimeSyncSlot ) + TimeSlotTick.Val/2 - ((uint32_t)0xFFFF * tmpW.Val) )) * 16 / MICRO_SECOND_PER_COUNTER_TICK));
                                tmpW.Val = 0xFFFF - (uint16_t)((TICKS_TO_SYMBOLS((((ONE_SECOND) * RFD_WAKEUP_INTERVAL) - (tmpTick.Val - TimeSyncTick.Val)  + ( TimeSlotTick.Val * TimeSyncSlot ) + TimeSlotTick.Val/2 - SYMBOLS_TO_TICKS((uint32_t)0xFFFF * MICRO_SECOND_PER_COUNTER_TICK / 16 * tmpW.Val) )) * 16 / MICRO_SECOND_PER_COUNTER_TICK));
                                if( TimeSyncSlot < TIME_SYNC_SLOTS )
                                {
                                    TimeSyncSlot++;
                                }    
                                MiApp_WriteData(tmpW.v[0]);
                                MiApp_WriteData(tmpW.v[1]);

                            #endif
                            
                            for(i = 0; i < INDIRECT_MESSAGE_SIZE; i++)
                            {
                                if( indirectMessages[i].flags.bits.isValid )
                                {    
                                    uint8_t j;

                                    #ifdef ENABLE_BROADCAST
                                        if( indirectMessages[i].flags.bits.isBroadcast )
                                        {
                                            for(j = 0; j < CONNECTION_SIZE; j++)
                                            {
                                                if( indirectMessages[i].DestAddress.DestIndex[j] != 0xFF &&
                                                    isSameAddress(ConnectionTable[indirectMessages[i].DestAddress.DestIndex[j]].Address, rxMessage.SourceAddress) )
                                                {
                                                    indirectMessages[i].DestAddress.DestIndex[j] = 0xFF;
                                                    for(j = 0; j < indirectMessages[i].PayLoadSize; j++)
                                                    {
                                                        MiApp_WriteData(indirectMessages[i].PayLoad[j]);
                                                    }
                                                    #if defined(ENABLE_TIME_SYNC)
                                                        if( indirectMessages[i].flags.bits.isCommand )
                                                        {
                                                            TxBuffer[0] = CMD_TIME_SYNC_COMMAND_PACKET;
                                                        } 
                                                    #endif   
                                                    #if defined(IEEE_802_15_4)
                                                        SendPacket(false, indirectMessages[i].DestPANID, rxMessage.SourceAddress, isCommand, indirectMessages[i].flags.bits.isSecured);
                                                    #else
                                                        SendPacket(false, rxMessage.SourceAddress, isCommand, indirectMessages[i].flags.bits.isSecured);
                                                    #endif 
                                                    //goto DiscardPacketHere;
                                                    goto END_OF_SENDING_INDIRECT_MESSAGE;
                                                }
                                            }         
                                        }
                                        else 
                                    #endif
                                    if( isSameAddress(indirectMessages[i].DestAddress.DestLongAddress, rxMessage.SourceAddress) )
                                    {                          
                                        for(j = 0; j < indirectMessages[i].PayLoadSize; j++)
                                        {
                                            MiApp_WriteData(indirectMessages[i].PayLoad[j]);
                                        }
                                        #if defined(ENABLE_TIME_SYNC)
                                            if( indirectMessages[i].flags.bits.isCommand )
                                            {
                                                TxBuffer[0] = CMD_TIME_SYNC_COMMAND_PACKET;
                                            } 
                                        #endif 
                                        #if defined(IEEE_802_15_4)
                                            SendPacket(false, indirectMessages[i].DestPANID, indirectMessages[i].DestAddress.DestLongAddress, isCommand, (bool)indirectMessages[i].flags.bits.isSecured);
                                        #else
                                            SendPacket(false, indirectMessages[i].DestAddress.DestLongAddress, isCommand, (bool)indirectMessages[i].flags.bits.isSecured);
                                        #endif
                                        indirectMessages[i].flags.Val = 0;   
                                        goto END_OF_SENDING_INDIRECT_MESSAGE;    
                                    }    
                                }
                            }
                           
                            if( i == INDIRECT_MESSAGE_SIZE )
                            {
                                #ifdef TARGET_SMALL
                                    #if defined(IEEE_802_15_4)
                                        SendPacket(false, myPANID, rxMessage.SourceAddress, isCommand, false);
                                    #else
                                        SendPacket(false, rxMessage.SourceAddress, isCommand, false);
                                    #endif
                                #else
                                    #if defined(IEEE_802_15_4)
                                        SendPacket(false, rxMessage.SourcePANID, rxMessage.SourceAddress, isCommand, false);
                                    #else
                                        SendPacket(false, rxMessage.SourceAddress, isCommand, false);
                                    #endif
                                #endif
                            }
                            
END_OF_SENDING_INDIRECT_MESSAGE:
                            #if defined(ENABLE_ENHANCED_DATA_REQUEST)
                                if( MACRxPacket.PayloadLen > 1 )
                                {
                                    rxMessage.Payload = &(MACRxPacket.Payload[1]);
                                    rxMessage.PayloadSize--;
                                    P2PStatus.bits.RxHasUserData = 1;
                                }
                                else    
                            #endif                        
                            MiMAC_DiscardPacket();
                        }    
                        break;
                #endif
                
                
                #if defined(ENABLE_TIME_SYNC) && defined(ENABLE_SLEEP)
                    case CMD_TIME_SYNC_DATA_PACKET:
                    case CMD_TIME_SYNC_COMMAND_PACKET:
                        {
                            WakeupTimes.v[0] = rxMessage.Payload[1];
                            WakeupTimes.v[1] = rxMessage.Payload[2];
                            CounterValue.v[0] = rxMessage.Payload[3];
                            CounterValue.v[1] = rxMessage.Payload[4];

                            if( rxMessage.PayloadSize > 5 )
                            {
                                if( rxMessage.Payload[0] == CMD_TIME_SYNC_DATA_PACKET )
                                {
                                    rxMessage.flags.bits.command = 0;
                                }    
                                rxMessage.PayloadSize -= 5;
                                rxMessage.Payload = &(rxMessage.Payload[5]);
                                P2PStatus.bits.RxHasUserData = 1;
                            }  
                            else
                            {
                                P2PStatus.bits.DataRequesting = 0;
                                MiMAC_DiscardPacket();
                            }      
                        }
                        break;    
                #endif
                
                
                     
                #if defined(ENABLE_FREQUENCY_AGILITY) 
                    case CMD_CHANNEL_HOPPING:
                        if( rxMessage.Payload[1] != currentChannel )
                        {
                            MiMAC_DiscardPacket();
                            break;
                        }
                        StartChannelHopping(rxMessage.Payload[2]);
                        #if defined(EIGHT_BIT_WIRELESS_BOARD)
                            Printf("\r\nHopping Channel to ");
                        #endif
                        
                        MiMAC_DiscardPacket();
                        break;
                    
                #endif

                #if defined (PROTOCOL_STAR)
                    case CMD_DATA_TO_ENDDEV_SUCCESS:
                        {
                            if (role == EndDevice)
                            {
                                rec_ack = true;
                                MiMAC_DiscardPacket();
                            }
                            else
                            {
                                MiMAC_DiscardPacket();
                            }
                        }
                        break;
                    case CMD_SHARE_CONNECTION_TABLE:
                        {
                          if (role == EndDevice)
                            {
                                // END_devices FFD|| RFD process this Packet
                                end_nodes = rxMessage.Payload[1]-1; 
                                store_connection_tb(rxMessage.Payload);
                                MiMAC_DiscardPacket();
                            }
                          else
                              MiMAC_DiscardPacket();
                        }
                        break;

                    case CMD_FORWRD_PACKET:
                        {
                            if (role == PanCoordinator)
                            {
                                // PAN CO handles the Packet Forwards 
                                uint8_t final_index; // EDy Index which is the destination address
                                final_index = Find_Index(rxMessage.Payload); 
                                if (final_index != 0xFF)
                                {
                                    FW_Stat = Forward_Packet_PANCr_to_EDy (rxMessage.Payload , final_index , rxMessage.SourceAddress);
                                }

                                        MiMAC_DiscardPacket();
                           }
                            
                            else
                                MiMAC_DiscardPacket();          
                        }
                        break;
                    #if defined(ENABLE_LINK_STATUS)
                        case CMD_IAM_ALIVE:
                        {
                            if (role == PanCoordinator)
                            {
                                // PAN CP processes this packet to qualify it as alive , increments the link stat
                                uint8_t p;
                                for (p=0  ; p < CONNECTION_SIZE ; p++)
                                {
                                        if (ConnectionTable[p].Address[0] == rxMessage.SourceAddress[0] && ConnectionTable[p].Address[1] == rxMessage.SourceAddress[1]
                                                && ConnectionTable[p].Address[2] == rxMessage.SourceAddress[2])
                                        {
                                            link_stat[p]++;
                                            break;
                                        }
                                }
                                MiMAC_DiscardPacket();
                            }
                            else
                            {
                                MiMAC_DiscardPacket();
                            }

                        }
                        break;
                    #endif
                #endif
                
                        
                default:
                    // let upper application layer to handle undefined command frame
                    P2PStatus.bits.RxHasUserData = 1;
                    break;
            }
        }
        else
        {
            P2PStatus.bits.RxHasUserData = 1;
        }

        #ifdef ENABLE_SLEEP
            if( P2PStatus.bits.DataRequesting && P2PStatus.bits.RxHasUserData )
            {
                P2PStatus.bits.DataRequesting = 0;
            }
        #endif       
        if( rxMessage.PayloadSize == 0  || P2PStatus.bits.SearchConnection || P2PStatus.bits.Resync )
        {
            P2PStatus.bits.RxHasUserData = 0;
            MiMAC_DiscardPacket();
        }   
            
    }   
}

uint32_t addr1 =  0x0080A00C;

bool MiApp_ProtocolInit(bool bNetworkFreezer)
{
	uint8_t i;
	
	MACINIT_PARAM initValue;

	
	#if defined(ENABLE_NVM)
	#if defined(ENABLE_NVM_MAC)
	if( MY_ADDRESS_LENGTH > 6 )
	{
		for(i = 0; i < 3; i++)
		{
			EEPROM_Read(&(myLongAddress[MY_ADDRESS_LENGTH-1-i]), EEPROM_MAC_ADDR+i, 1);
		}
		myLongAddress[4] = 0xFF;
		if( MY_ADDRESS_LENGTH > 7 )
		{
			myLongAddress[3] = 0xFE;
		}
		for(i = 0; i < 3; i++)
		{
			EEPROM_Read(&(myLongAddress[2-i]), EEPROM_MAC_ADDR+3+i, 1);
		}
	}
	else
	{
		for(i = 0; i < MY_ADDRESS_LENGTH; i++)
		{
			EEPROM_Read(&(myLongAddress[MY_ADDRESS_LENGTH-1-i]), EEPROM_MAC_ADDR+i, 1);
		}
	}
	#endif
	
	#endif
	
	#if defined(ENABLE_NETWORK_FREEZER)
	NVMInit();
	#endif
	
	//clear all status bits
	P2PStatus.Val = 0;
	
	#if defined (USE_MAC_ADDRESS)
	// Each SAM MCU has a unique address stored in Flash Memory
	uint32_t val_1, val_2, val_3, val_4;
	uint32_t *ptr1 = (uint32_t *)0x0080A00C;
	val_1 = *ptr1;
	uint32_t *ptr = (uint32_t *)0x0080A040;
	val_2 = *ptr;
	ptr++;
	val_3 = *ptr;
	ptr++;
	val_4 = *ptr;
	myLongAddress[0] = (val_1 & 0xFF) + ((val_1>>8) & 0xFF);
	myLongAddress[1] = ((val_1>>16) & 0xFF) + ((val_1)>>24 & 0xFF);
	myLongAddress[2] = (val_2 & 0xFF) + ((val_2>>8) & 0xFF);
	myLongAddress[3] = ((val_2>>16) & 0xFF) + ((val_2)>>24 & 0xFF);
	myLongAddress[4] = (val_3 & 0xFF) + ((val_3>>8) & 0xFF);
	myLongAddress[5] = ((val_3>>16) & 0xFF) + ((val_3)>>24 & 0xFF);
	myLongAddress[6] = (val_4 & 0xFF) + ((val_4>>8) & 0xFF);
	myLongAddress[7] = ((val_4>>16) & 0xFF) + ((val_4)>>24 & 0xFF);
	#else
	myLongAddress[0] = EUI_0;
	myLongAddress[1] = EUI_1;
	myLongAddress[2] = EUI_2;
	myLongAddress[3] = EUI_3;
	myLongAddress[4] = EUI_4;
	myLongAddress[5] = EUI_5;
	myLongAddress[6] = EUI_6;
	myLongAddress[7] = EUI_7;
	#endif

	
	if (!bNetworkFreezer)
	{
		for(i = 0; i < CONNECTION_SIZE; i++)
		{
			ConnectionTable[i].status.Val = 0;
			ConnectionTable[i].Address[0] = 0x00;
			ConnectionTable[i].Address[1] = 0x00;
			ConnectionTable[i].Address[2] = 0x00;
			#if defined(ENABLE_LINK_STATUS)
			link_stat[i] = 0x00;
			permanent_connections[i] = 0x00;
			#endif
		}
	}
	
	TxData = 0;

	#ifdef ENABLE_INDIRECT_MESSAGE
	for(i = 0; i < INDIRECT_MESSAGE_SIZE; i++)
	{
		indirectMessages[i].flags.Val = 0;
	}
	#endif
	
	#if defined(ENABLE_SECURITY)
	for(i = 0; i < CONNECTION_SIZE; i++)
	{
		IncomingFrameCounter[i].Val = 0;
	}
	#endif
	

	#if defined(IEEE_802_15_4)
	myPANID.Val = MY_PAN_ID;
	#endif

	
	initValue.PAddress = myLongAddress;
	initValue.actionFlags.bits.CCAEnable = 1;
	initValue.actionFlags.bits.PAddrLength = MY_ADDRESS_LENGTH;
	initValue.actionFlags.bits.NetworkFreezer = bNetworkFreezer;
	initValue.actionFlags.bits.RepeaterMode = 0;

	MiMAC_Init(initValue);
	

	
	#if defined(IEEE_802_15_4)
	{
		uint16_t tmp = 0xFFFF;
		MiMAC_SetAltAddress((uint8_t *)&tmp, (uint8_t *)&myPANID.Val);
	}
	#endif
	
	MiApp_SetChannel(currentChannel);

	#if defined(ENABLE_TIME_SYNC)
	#if defined(ENABLE_SLEEP)
	WakeupTimes.Val = 0;
	CounterValue.Val = 61535;   // (0xFFFF - 4000) one second
	#elif defined(ENABLE_INDIRECT_MESSAGE)
	TimeSlotTick.Val = ((ONE_SECOND) * RFD_WAKEUP_INTERVAL) / TIME_SYNC_SLOTS;
	#endif
	#endif

	P2PCapacityInfo = 0;
	#if !defined(ENABLE_SLEEP)
	P2PCapacityInfo |= 0x01;
	#endif
	#if defined(ENABLE_SECURITY)
	P2PCapacityInfo |= 0x08;
	#endif
	P2PCapacityInfo |= (ConnMode << 4);
	ENABLE_TRX_IRQ();
	return true;
}

#ifdef ENABLE_SLEEP
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
    uint8_t MiApp_TransceiverPowerState(INPUT uint8_t Mode)
    {
        //uint8_t status;
        
        switch(Mode)
        {
            case POWER_STATE_SLEEP:
                {
                    #if defined(ENABLE_NETWORK_FREEZER)
                        if( P2PStatus.bits.SaveConnection )
                        {
                            nvmPutConnectionTable(ConnectionTable);
                            P2PStatus.bits.SaveConnection = 0;
                        }
                    #endif
                    if( MiMAC_PowerState(POWER_STATE_DEEP_SLEEP) )
                    {
                        P2PStatus.bits.Sleeping = 1;
                        return SUCCESS;
                    }
                    return ERR_TRX_FAIL;
                }
                
            case POWER_STATE_WAKEUP:
                {
                    if( MiMAC_PowerState(POWER_STATE_OPERATE) )
                    {
                        P2PStatus.bits.Sleeping = 0;
                        return SUCCESS;
                    }
                    return ERR_TRX_FAIL;
                }
               
            case POWER_STATE_WAKEUP_DR:
                {
                    if( false == MiMAC_PowerState(POWER_STATE_OPERATE) )
                    {
                        return ERR_TRX_FAIL;
                    }
                    P2PStatus.bits.Sleeping = 0;
                    if( CheckForData() == false )
                    {
                        return ERR_TX_FAIL;
                    }
                    while( P2PStatus.bits.DataRequesting ) 
                    {
                        P2PTasks();
                    }
                    return SUCCESS;
                }
                
             default:
                break;

        }
        
        return ERR_INVALID_INPUT;    
    }

     
     /*********************************************************************
     * BOOL CheckForData(void)
     *
     * Overview:        This function sends out a Data Request to the peer
     *                  device of the first P2P connection. 
     *
     * PreCondition:    Transceiver is initialized and fully waken up
     *
     * Input:           None
     *
     * Output:          None
     *
     * Side Effects:    The P2P stack is waiting for the response from
     *                  the peer device. A data request timer has been
     *                  started. In case there is no response from the
     *                  peer device, the data request will time-out itself
     *
     ********************************************************************/
     bool CheckForData(void)
     {
        uint8_t tmpTxData = TxData;
        uint8_t firstuint8_t = TxBuffer[0];
        
        MiApp_FlushTx();
        MiApp_WriteData(CMD_MAC_DATA_REQUEST);

        #if defined(ENABLE_ENHANCED_DATA_REQUEST)
            if( tmpTxData > 0 )
            {
                uint8_t i;
                
                for( i = tmpTxData; i > 1; i--)
                {
                    TxBuffer[i] = TxBuffer[i-1];
                }
                TxBuffer[1] = firstuint8_t;
                TxData = tmpTxData + 1;  
            }    
        #endif
                    
        #if defined(IEEE_802_15_4)
            #if defined(ENABLE_ENHANCED_DATA_REQUEST)
                if( SendPacket(false, myPANID, ConnectionTable[0].Address, true, P2PStatus.bits.Enhanced_DR_SecEn) )
            #else
                if( SendPacket(false, myPANID, ConnectionTable[0].Address, true, false) )
            #endif
        #else
            #if defined(ENABLE_ENHANCED_DATA_REQUEST)
                if( SendPacket(false, ConnectionTable[0].Address, true, P2PStatus.bits.Enhanced_DR_SecEn) )
            #else
                if( SendPacket(false, ConnectionTable[0].Address, true, false) )
            #endif
        #endif
        {
            P2PStatus.bits.DataRequesting = 1; 
            #if defined(ENABLE_ENHANCED_DATA_REQUEST) 
                P2PStatus.bits.Enhanced_DR_SecEn = 0;
            #endif
            DataRequestTimer.Val = MiWi_TickGet();
            TxBuffer[0] = firstuint8_t;
            TxData = tmpTxData;
            #if defined(ENABLE_TIME_SYNC)
				set_timer_count_value(0);
            #endif
            return true;
        }
        TxBuffer[0] = firstuint8_t;
        TxData = tmpTxData;
        #if defined(ENABLE_ENHANCED_DATA_REQUEST)
            P2PStatus.bits.Enhanced_DR_SecEn = 0;
        #endif
        return false;
     }
 #endif
 

#ifdef ENABLE_INDIRECT_MESSAGE
    
    /*********************************************************************
     * BOOL IndirectPacket(BOOL Broadcast, 
     *                     uint16_t_VAL DestinationPANID,
     *                     uint8_t *DestinationAddress,
     *                     BOOL isCommand, 
     *                     BOOL SecurityEnabled)
     *
     * Overview:        This function store the indirect message for node
     *                  that turns off radio when idle     
     *
     * PreCondition:    None
     *
     * Input:           Broadcast           - Boolean to indicate if the indirect
     *                                        message a broadcast message
     *                  DestinationPANID    - The PAN Identifier of the 
     *                                        destination node
     *                  DestinationAddress  - The pointer to the destination
     *                                        long address
     *                  isCommand           - The boolean to indicate if the packet
     *                                        is command
     *                  SecurityEnabled     - The boolean to indicate if the 
     *                                        packet needs encryption
     *
     * Output:          boolean to indicate if operation successful
     *
     * Side Effects:    An indirect message stored and waiting to deliever
     *                  to sleeping device. An indirect message timer
     *                  has started to expire the indirect message in case
     *                  RFD does not acquire data in predefined interval
     *
     ********************************************************************/
    #if defined(IEEE_802_15_4)
        bool IndirectPacket(INPUT bool Broadcast, 
                            API_UINT16_UNION  DestinationPANID,
                            INPUT uint8_t *DestinationAddress,
                            INPUT bool isCommand, 
                            INPUT bool SecurityEnabled)
    #else
        bool IndirectPacket(INPUT bool Broadcast, 
                            INPUT uint8_t *DestinationAddress,
                            INPUT bool isCommand, 
                            INPUT bool SecurityEnabled)
    #endif                            
    { 
        uint8_t i;
        
        #ifndef ENABLE_BROADCAST
            if( Broadcast )
            {
                return false;
            }
        #endif

        // loop through the available indirect message buffer and locate
        // the empty message slot
        for(i = 0; i < INDIRECT_MESSAGE_SIZE; i++)
        {
            if( indirectMessages[i].flags.bits.isValid == 0 )
            {
                uint8_t j;
                
                // store the message
                indirectMessages[i].flags.bits.isValid          = true;
                indirectMessages[i].flags.bits.isBroadcast      = Broadcast;
                indirectMessages[i].flags.bits.isCommand        = isCommand;
                indirectMessages[i].flags.bits.isSecured        = SecurityEnabled;
                #if defined(IEEE_802_15_4)
                    indirectMessages[i].DestPANID.Val           = DestinationPANID.Val;
                #endif
                if( DestinationAddress != NULL )
                {
                    for(j = 0; j < MY_ADDRESS_LENGTH; j++)
                    {
                        indirectMessages[i].DestAddress.DestLongAddress[j] = DestinationAddress[j];
                    }
                }
                #ifdef ENABLE_BROADCAST
                    else
                    {
                        uint8_t k = 0;
    
                        for(j = 0; j < CONNECTION_SIZE; j++)
                        {
                            //if( (ConnectionTable[j].PeerInfo[0] & 0x83) == 0x82 )
                            if( ConnectionTable[j].status.bits.isValid &&
                                ConnectionTable[j].status.bits.RXOnWhenIdle == 0 )
                            {
                                indirectMessages[i].DestAddress.DestIndex[k++] = j;
                            }
                        }
                        for(; k < CONNECTION_SIZE; k++)
                        {
                            indirectMessages[i].DestAddress.DestIndex[k] = 0xFF;
                        }
                    }
                #endif
                
                indirectMessages[i].PayLoadSize = TxData;
                for(j = 0; j < TxData; j++)
                {
                    indirectMessages[i].PayLoad[j] = TxBuffer[j];
                }
                indirectMessages[i].TickStart.Val = MiWi_TickGet();
                return true;
            }
        }
        return false;
    }
#endif


/*********************************************************************
 * BOOL SendPacket(BOOL Broadcast, 
 *                 uint16_t_VAL DestinationPANID,
 *                 uint8_t *DestinationAddress,
 *                 BOOL isCommand, 
 *                 BOOL SecurityEnabled)
 *
 * Overview:        This function sends the packet  
 *
 * PreCondition:    Transceiver is initialized
 *
 * Input:     
 *          BOOL        Broadcast           If packet to send needs to be broadcast
 *          uint16_t_VAL    DestinationPANID    Destination PAN Identifier
 *          uint8_t *      DestinationAddress  Pointer to destination long address
 *          BOOL        isCommand           If packet to send is a command packet
 *          BOOL        SecurityEnabled     If packet to send needs encryption
 *                  
 * Output: 
 *          BOOL                            If operation successful
 *
 * Side Effects:    Transceiver is triggered to transmit a packet
 *
 ********************************************************************/
#if defined(IEEE_802_15_4)
    bool SendPacket(INPUT bool Broadcast,
                    API_UINT16_UNION DestinationPANID,
                    INPUT uint8_t *DestinationAddress,
                    INPUT bool isCommand,
                    INPUT bool SecurityEnabled)
#else
    bool SendPacket(INPUT bool Broadcast,
                    INPUT uint8_t *DestinationAddress,
                    INPUT bool isCommand,
                    INPUT bool SecurityEnabled)
#endif                                        
{ 
    MAC_TRANS_PARAM tParam;
    bool status;

    tParam.flags.Val = 0;
    tParam.flags.bits.packetType = (isCommand) ? PACKET_TYPE_COMMAND : PACKET_TYPE_DATA;
    tParam.flags.bits.ackReq = (Broadcast) ? 0 : 1;
    tParam.flags.bits.broadcast = Broadcast;
    tParam.flags.bits.secEn = SecurityEnabled;
    #if defined(IEEE_802_15_4)
        tParam.altSrcAddr = 0;
        tParam.altDestAddr = (Broadcast) ? true : false;
    #endif
    
    #if defined(INFER_DEST_ADDRESS)
        tParam.flags.bits.destPrsnt = 0;
    #else
        tParam.flags.bits.destPrsnt = (Broadcast) ? 0:1;
    #endif
    
    #if defined(SOURCE_ADDRESS_ABSENT)
        if( tParam.flags.bits.packetType == PACKET_TYPE_COMMAND )
        {
            tParam.flags.bits.sourcePrsnt = 1;
        }
        else
        {
            tParam.flags.bits.sourcePrsnt = 0;
        }
    #else
        tParam.flags.bits.sourcePrsnt = 1;
    #endif
    
    tParam.DestAddress = DestinationAddress;

    #if defined(IEEE_802_15_4)
        tParam.DestPANID.Val = DestinationPANID.Val;
    #endif

    status = MiMAC_SendPacket(tParam, TxBuffer, TxData); 
    TxData = 0;
    
    return status;
}

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
 *      BOOL SecEn -    The boolean indicates if the application payload needs to be
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
bool MiApp_BroadcastPacket( INPUT bool SecEn )
{
    #ifdef ENABLE_INDIRECT_MESSAGE
        uint8_t i;
        
        for(i = 0; i < CONNECTION_SIZE; i++)
        {
            if( ConnectionTable[i].status.bits.isValid && ConnectionTable[i].status.bits.RXOnWhenIdle == 0 )
            {
                #if defined(IEEE_802_15_4)
                    IndirectPacket(true, myPANID, NULL, false, SecEn);
                #else
                    IndirectPacket(true, NULL, false, SecEn);
                #endif
                break;
            }     
        }
    #endif
    
    #if defined(ENABLE_ENHANCED_DATA_REQUEST) && defined(ENABLE_SLEEPING)
        if( P2PStatus.bits.Sleeping )
        {
            P2PStatus.bits.Enhanced_DR_SecEn = SecEn;
            return true;
        }    
    #endif
    
    #if defined(IEEE_802_15_4)
        return SendPacket(true, myPANID, NULL, false, SecEn);
    #else
        return SendPacket(true, NULL, false, SecEn);
    #endif
}

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
 *      BOOL SecEn -    The boolean indicates if the application payload needs to be
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
bool MiApp_UnicastConnection( INPUT uint8_t ConnectionIndex,
                        INPUT bool SecEn)
{
    if( ConnectionTable[ConnectionIndex].status.bits.isValid )
    {
        #ifdef ENABLE_INDIRECT_MESSAGE
            // check if RX on when idle
            //if( (ConnectionTable[ConnectionIndex].PeerInfo[0] & 0x03) == 0x02 )
            if( ConnectionTable[ConnectionIndex].status.bits.RXOnWhenIdle == 0 )
            {
                #if defined(IEEE_802_15_4)
                    return IndirectPacket(false, myPANID, ConnectionTable[ConnectionIndex].Address, false, SecEn);
                #else
                    // Define the additional step to assign the address to
                    // bypass compiler bug in C18 v3.38.
                    uint8_t address[MY_ADDRESS_LENGTH];
                    uint8_t i;

                    for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                    {
                        address[i] = ConnectionTable[ConnectionIndex].Address[i];  
                    }
                    
                    //return IndirectPacket(false, ConnectionTable[ConnectionIndex].Address, false, SecEn);
                    return IndirectPacket(false, address, false, SecEn);
                #endif
            }
        #endif
        
        #if defined(ENABLE_ENHANCED_DATA_REQUEST) 
            if( P2PStatus.bits.Sleeping )
            {
                P2PStatus.bits.Enhanced_DR_SecEn = SecEn;
                return true;
            }    
        #endif
        
        return MiApp_UnicastAddress(ConnectionTable[ConnectionIndex].Address, true, SecEn);
    }
    return false;
}



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
 *      BOOL PermanentAddr -    The boolean to indicate if the destination address above is a
 *                              permanent address or alternative network address. This parameter
 *                              is only used in a network protocol.         
 *      BOOL SecEn -    The boolean indicates if the application payload needs to be
 *                      secured before transmission.
 *
 * Returns: 
 *      A boolean to indicates if the unicast procedure is succcessful.
 *
 * Example:
 *      <code>
 *      // Secure and then broadcast the message stored in TxBuffer to the permanent address
 *      // specified in the input parameter.
 *      MiApp_UnicastAddress(DestAddress, true, true);
 *      </code>
 *
 * Remarks:    
 *      None
 *
 *****************************************************************************************/    
bool MiApp_UnicastAddress(INPUT uint8_t *DestinationAddress,
                    INPUT bool PermanentAddr,
                    INPUT bool SecEn)
{
    #ifdef ENABLE_INDIRECT_MESSAGE
        uint8_t i;
        
        for(i = 0; i < CONNECTION_SIZE; i++)
        {
            // check if RX on when idle
            if( ConnectionTable[i].status.bits.isValid && (ConnectionTable[i].status.bits.RXOnWhenIdle == 0) &&
                isSameAddress(DestinationAddress, ConnectionTable[i].Address) )
            {
                #if defined(IEEE_802_15_4)
                    return IndirectPacket(false, myPANID, DestinationAddress, false, SecEn);
                #else
                    return IndirectPacket(false, DestinationAddress, false, SecEn);
                #endif
            }
        }
    #endif
    
    #if defined(ENABLE_ENHANCED_DATA_REQUEST) && defined(ENABLE_SLEEPING)
        if( P2PStatus.bits.Sleeping )
        {
            P2PStatus.bits.Enhanced_DR_SecEn = SecEn;
            return true;
        }    
    #endif
    
    #if defined(IEEE_802_15_4)
        return SendPacket(false, myPANID, DestinationAddress, false, SecEn);
    #else
        return SendPacket(false, DestinationAddress, false, SecEn);
    #endif
}

#if defined(PROTOCOL_STAR)
    /************************************************************************************
    * Function:
    *      void MiApp_BroadcastConnectionTable(void)
    *
    * Summary:
    *      This function is  used for command type packet TX (Only used by PAN CO)
    *
    * Description:        
    *      This function is used by only PAN CO in a Star network and is a cmd
    *      type packet. PAN CO in Star Network , holds the responsibility to Share 
    *      peer end devices connection table.
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
    void MiApp_BroadcastConnectionTable(void)
    {
        uint8_t i,j , k , count;
        // Based on Connection Size in Network braodcast the connection details Multiple Times 
        // so that all the END_DEVICES in Star Network Receive the packet 
        uint8_t broadcast_count = 0;

        if ((conn_size  * 4 ) + 4 < TX_BUFFER_SIZE)
        {
            broadcast_count = 1;
        }
        else
        {
            broadcast_count = ((conn_size * 4) + 4 )/ TX_BUFFER_SIZE;
            if ((conn_size *4) + 4 % TX_BUFFER_SIZE != 0)
            {
                broadcast_count = broadcast_count + ((conn_size *4) + 4 )% TX_BUFFER_SIZE;
            }
            
        }
        
        for (i = 0 ; i < broadcast_count ; i++)
        {
            MiApp_FlushTx();
            MiApp_WriteData(CMD_SHARE_CONNECTION_TABLE);
            MiApp_WriteData(conn_size); // No of end devices in network
            MiApp_WriteData(((TX_BUFFER_SIZE-4)/4)*i);
            MiApp_WriteData(((TX_BUFFER_SIZE-4)/4)*(i+1));
            count = 4;
            for (j= ((TX_BUFFER_SIZE-4)/4)*i ;j<((TX_BUFFER_SIZE-4)/4)*(i+1);j++)
            {
                if (j < conn_size)
                {
                    if (ConnectionTable[j].status.bits.isValid)
                    {

                        MiApp_WriteData(ConnectionTable[j].Address[0]);
                        MiApp_WriteData(ConnectionTable[j].Address[1]);
                        MiApp_WriteData(ConnectionTable[j].Address[2]);
                        MiApp_WriteData(j);

                    }

                    else
                    {

                        MiApp_WriteData(0xff);
                        MiApp_WriteData(0xff);
                        MiApp_WriteData(0xff);
                        MiApp_WriteData(j);             
                    }
                    count = count + 4;
                }
            }
            // Fill the remaining buffer with garbage value
            for (k=count;k<TX_BUFFER_SIZE;k++)
            {
                MiApp_WriteData(0x11);   // Garbage Value 
            }



            #if defined(IEEE_802_15_4)

                SendPacket(true, myPANID, NULL, true, false);
            #else
                SendPacket(true, NULL, true, false);
            #endif

 
        }
    }

    bool MiApp_UnicastStar  (uint8_t EndDevice_ConnectionIndex , bool SecEn)
    {
        rec_ack = false;
		bool status;
		uint8_t copy_txdata[TX_BUFFER_SIZE] , i , datalen;
		datalen = TxData;
		for (i=0;i<datalen;i++)
		{
			copy_txdata[i] = TxBuffer[i];
		}
		MiApp_FlushTx();
		MiApp_WriteData(CMD_FORWRD_PACKET);
		MiApp_WriteData(END_DEVICES_Short_Address[EndDevice_ConnectionIndex].Address[0]);
		MiApp_WriteData(END_DEVICES_Short_Address[EndDevice_ConnectionIndex].Address[1]);
		MiApp_WriteData(END_DEVICES_Short_Address[EndDevice_ConnectionIndex].Address[2]);
		for (i=0;i<datalen;i++)
		{
			MiApp_WriteData(copy_txdata[i]);
		}
        #if defined(IEEE_802_15_4)
            status = SendPacket(false, myPANID, ConnectionTable[0].Address, true, SecEn);
        #else
            status = SendPacket(false,  ConnectionTable[0].Address, true, SecEn);
        #endif


        return status; 
    }
    
    void MiApp_leave_network( void)
    {
        MiApp_FlushTx();
        MiApp_WriteData(CMD_P2P_CONNECTION_REMOVAL_REQUEST); 
        // Pan Co is @ index 0 of connection table of END_Device
        // In a Star Network
        #if defined(IEEE_802_15_4)
            SendPacket(false, myPANID, ConnectionTable[0].Address, true, false);
        #else
            SendPacket(false,  ConnectionTable[0].Address, true, false);
        #endif
        
    }
 

    #if defined(ENABLE_LINK_STATUS)

        
        void send_link_status (void)
        {
            bool send_status;
            MiApp_FlushTx();
            MiApp_WriteData(CMD_IAM_ALIVE); 
            // Pan Co is @ index 0 of connection table of END_Device
            // In a Star Network
            #if defined(IEEE_802_15_4)
                send_status = SendPacket(false, myPANID, ConnectionTable[0].Address, true, false);
            #else
                send_status = SendPacket(false,  ConnectionTable[0].Address, true, false);
            #endif
                if(!send_status)
                {
                    lost_connection = true;
                }
        }
        
        
        void Find_InActiveDevices(void)
        {
            uint8_t i;
            for (i=0;i<CONNECTION_SIZE;i++)
            {
                if (ConnectionTable[i].status.bits.isValid)
                {
                    if (link_stat[i] == 0 && permanent_connections[i] != 0xFF)
                    {
                        MiApp_RemoveConnection(i);
                    }
                    else
                    {
                        link_stat[i] = 0; 
                    }
                }
            }
        }


    #endif
#endif



/*********************************************************************
 * BOOL    isSameAddress(uint8_t *Address1, uint8_t *Address2)
 *
 * Overview:        This function compares two long addresses and returns
 *                  the boolean to indicate if they are the same
 *
 * PreCondition:    
 *
 * Input:  
 *          Address1    - Pointer to the first long address to be compared
 *          Address2    - Pointer to the second long address to be compared
 *                  
 * Output: 
 *          If the two address are the same
 *
 * Side Effects:    
 *
 ********************************************************************/
bool    isSameAddress(INPUT uint8_t *Address1, INPUT uint8_t *Address2)
{
    uint8_t i;
    
    for(i = 0; i < MY_ADDRESS_LENGTH; i++)
    {
        if( Address1[i] != Address2[i] )
        {
            return false;
        }
    }
    return true;
}

#if defined(ENABLE_HAND_SHAKE)
     
    bool MiApp_StartConnection(uint8_t Mode, uint8_t ScanDuration, uint32_t ChannelMap)
    {
		#if defined(PROTOCOL_STAR)
			role = PanCoordinator;
		#endif
        switch(Mode)
        {
            case START_CONN_DIRECT:
                #if defined(IEEE_802_15_4)
                    #if MY_PAN_ID == 0xFFFF
                        myPANID.v[0] = get_timer_counter_value();
                        myPANID.v[1] = myPANID.v[0]+0x51;
                    #else
                        myPANID.Val = MY_PAN_ID;
                    #endif
                    {
                        uint16_t tmp = 0xFFFF;
                        MiMAC_SetAltAddress((uint8_t *)&tmp, (uint8_t *)&myPANID.Val);
                    }
                #endif
                #if defined(ENABLE_TIME_SYNC) && !defined(ENABLE_SLEEP) && defined(ENABLE_INDIRECT_MESSAGE)
                    TimeSyncTick.Val = MiWi_TickGet();
                #endif
            #if defined(PROTOCOL_STAR)
                tick1.Val = MiWi_TickGet();
                tick4.Val = MiWi_TickGet();
            #endif
                return true;
                
            case START_CONN_ENERGY_SCN:
                #if defined(ENABLE_ED_SCAN)
                {
                    uint8_t channel;
                    uint8_t RSSIValue;
                    
                    #if defined(IEEE_802_15_4)
                        #if MY_PAN_ID == 0xFFFF
                        myPANID.v[0] = get_timer_counter_value();
                        myPANID.v[1] = myPANID.v[0]+0x51;
                        #else
                            myPANID.Val = MY_PAN_ID;
                        #endif
                        {
                            uint16_t tmp = 0xFFFF;
                            MiMAC_SetAltAddress((uint8_t *)&tmp, (uint8_t *)&myPANID.Val);
                        }
                    #endif
                    channel = MiApp_NoiseDetection(ChannelMap, ScanDuration, NOISE_DETECT_ENERGY, &RSSIValue);
                    MiApp_SetChannel(channel);
                    Printf("\r\nStart Wireless Communication on Channel ");
                    PrintDec(channel);
                    Printf("\r\n");
                    #if defined(ENABLE_TIME_SYNC) && !defined(ENABLE_SLEEP) && defined(ENABLE_INDIRECT_MESSAGE)
                        TimeSyncTick.Val = MiWi_TickGet();
                    #endif
                    return true;
                }
                #else
                    return false;
                #endif
                
            case START_CONN_CS_SCN:
                // Carrier sense scan is not supported for current available transceivers
                return false;
            
            default:
                break;
        }    
        return false;
    }
    
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
     *                  * CONN_MODE_DIRECT      Establish a connection without radio range.
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
    uint8_t MiApp_EstablishConnection(INPUT uint8_t ActiveScanIndex, INPUT uint8_t Mode)
    {
        uint8_t    tmpConnectionMode = ConnMode;
        uint8_t    retry = CONNECTION_RETRY_TIMES;
        uint8_t    connectionInterval = 0;
        MIWI_TICK    t1, t2;
        #if defined(PROTOCOL_STAR)
            tick1.Val = MiWi_TickGet();
        #endif
        if( Mode == CONN_MODE_INDIRECT )
        {
            return 0xFF;
        }
       
        t1.Val = MiWi_TickGet();

        t1.Val -= (ONE_SECOND);
        ConnMode = ENABLE_ALL_CONN;
        P2PStatus.bits.SearchConnection = 1;
        while( P2PStatus.bits.SearchConnection )
        {
            t2.Val = MiWi_TickGet();
            if( MiWi_TickGetDiff(t2, t1) > (ONE_SECOND) )
            {   
                t1.Val = t2.Val;

                if( connectionInterval-- > 0 )
                {
                    continue;
                }
                connectionInterval = CONNECTION_INTERVAL-1;
                if( retry-- == 0 )
                {

                    P2PStatus.bits.SearchConnection = 0;
                    return 0xFF;
                }
                MiApp_FlushTx();
                MiApp_WriteData(CMD_P2P_CONNECTION_REQUEST);
                MiApp_WriteData(currentChannel);
                MiApp_WriteData(P2PCapacityInfo);
				#if defined(ED)
					MiApp_WriteData(0xA2);
				#endif
				#if defined(R)
					MiApp_WriteData(0xA1);
				#endif
                #if defined(PROTOCOL_STAR) && defined(MAKE_ENDDEVICE_PERMANENT)
                {
                    MiApp_WriteData(0xAA);
                }
                #endif
                #if ADDITIONAL_NODE_ID_SIZE > 0
                    {
                        uint8_t i;
                        
                        for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
                        {
                            MiApp_WriteData(AdditionalNodeID[i]);
                        }
                    }
                #endif

                #if defined(IEEE_802_15_4)
                    #if defined(ENABLE_ACTIVE_SCAN)
                        if( ActiveScanIndex == 0xFF )
                        {
                            SendPacket(true, myPANID, NULL, true, false);
                        }
                        else
                        {
                            MiApp_SetChannel( ActiveScanResults[ActiveScanIndex].Channel );
                            SendPacket(false, ActiveScanResults[ActiveScanIndex].PANID, ActiveScanResults[ActiveScanIndex].Address, true, false);
                        }
                    #else
                        SendPacket(true, myPANID, NULL, true, false);
                    #endif
                #else
                    #if defined(ENABLE_ACTIVE_SCAN)
                        if( ActiveScanIndex == 0xFF )
                        {
                            SendPacket(true, NULL, true, false);
                        }
                        else
                        {
                            MiApp_SetChannel( ActiveScanResults[ActiveScanIndex].Channel );
                            SendPacket(false, ActiveScanResults[ActiveScanIndex].Address, true, false);
                        }
                    #else
                        SendPacket(true, NULL, true, false);
                    #endif
                #endif
            }
    
            if( MiApp_MessageAvailable())
            {
                MiApp_DiscardMessage();
            }     
            //P2PTasks();
        }
      
        ConnMode = tmpConnectionMode;
        
        #if defined(ENABLE_TIME_SYNC) && !defined(ENABLE_SLEEP) && defined(ENABLE_INDIRECT_MESSAGE)
            TimeSyncTick.Val = MiWi_TickGet();
        #endif
        #if defined(PROTOCOL_STAR)
            if (LatestConnection != 0xFF)
            {
                role = EndDevice;
            }
            else
            {
                role = PanCoordinator;
            }
            #if defined(ENABLE_NETWORK_FREEZER)
                nvmPutMyRole(&role);  // Saving the Role of the device 
            #endif  
        #endif
		#if defined (PROTOCOL_STAR)
			 if (LatestConnection == 0xFF)
			 {
				 role = PanCoordinator;
			 }
			 else
			 {
				 role = EndDevice;
			 }
		#endif
        return LatestConnection;
        
    }


#endif



 
void MiApp_DiscardMessage(void)
{
    P2PStatus.bits.RxHasUserData = 0;
    MiMAC_DiscardPacket();
}


 
bool MiApp_SetChannel(uint8_t channel)
{
    if( MiMAC_SetChannel(channel, 0) )
    {
        currentChannel = channel;
        #if defined(ENABLE_NETWORK_FREEZER)
            nvmPutCurrentChannel(&currentChannel);
        #endif
        return true;
    }
    return false;
}


bool MiApp_MessageAvailable(void)
{ 
    P2PTasks(); 
    
    return P2PStatus.bits.RxHasUserData;
}

#ifdef ENABLE_DUMP
    /*********************************************************************
     * void DumpConnection(uint8_t index)
     *
     * Overview:        This function prints out the content of the connection 
     *                  with the input index of the P2P Connection Entry
     *
     * PreCondition:    
     *
     * Input:  
     *          index   - The index of the P2P Connection Entry to be printed out
     *                  
     * Output:  None
     *
     * Side Effects:    The content of the connection pointed by the index 
     *                  of the P2P Connection Entry will be printed out
     *
     ********************************************************************/
    void DumpConnection(INPUT uint8_t index)
    {
        uint8_t i, j;
        
        if( index > CONNECTION_SIZE )
        {
            Printf("\r\n\r\nMy Address: 0x");
            for(i = 0; i < MY_ADDRESS_LENGTH; i++)
            {
                CONSOLE_PrintHex(myLongAddress[MY_ADDRESS_LENGTH-1-i]);
            }
            #if defined(IEEE_802_15_4)
                Printf("  PANID: 0x");
                CONSOLE_PrintHex(myPANID.v[1]);
                CONSOLE_PrintHex(myPANID.v[0]);
            #endif
            Printf("  Channel: ");
            CONSOLE_PrintDec(currentChannel);
        }
            
        if( index < CONNECTION_SIZE )
        {
            Printf("\r\nConnection     PeerLongAddress     PeerInfo\r\n");  
            if( ConnectionTable[index].status.bits.isValid )
            {
                CONSOLE_PrintHex(index);
                Printf("             ");
                for(i = 0; i < 8; i++)
                {
                    if(i < MY_ADDRESS_LENGTH)
                    {
                        CONSOLE_PrintHex( ConnectionTable[index].Address[MY_ADDRESS_LENGTH-1-i] );
                    }
                    else
                    {
                        Printf("  ");
                    }
                }
                Printf("    ");
                #if ADDITIONAL_NODE_ID_SIZE > 0
                    for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
                    {
                        CONSOLE_PrintHex( ConnectionTable[index].PeerInfo[i] );
                    }
                #endif
                Printf("\r\n");
            }
        }
        else
        {
            Printf("\r\n\r\nConnection     PeerLongAddress     PeerInfo\r\n");  
            for(i = 0; i < CONNECTION_SIZE; i++)
            {
                
                if( ConnectionTable[i].status.bits.isValid )
                {
                    CONSOLE_PrintHex(i);
                    Printf("             ");
                    for(j = 0; j < 8; j++)
                    {
                        if( j < MY_ADDRESS_LENGTH )
                        {
                            CONSOLE_PrintHex( ConnectionTable[i].Address[MY_ADDRESS_LENGTH-1-j] );
                        }
                        else
                        {
                            Printf("  ");
                        }
                    }
                    Printf("    ");
                    #if ADDITIONAL_NODE_ID_SIZE > 0
                        for(j = 0; j < ADDITIONAL_NODE_ID_SIZE; j++)
                        {
                            CONSOLE_PrintHex( ConnectionTable[i].PeerInfo[j] );
                        }
                    #endif
                    Printf("\r\n");
                }  
            }
        }
    }
#endif


#if defined(ENABLE_HAND_SHAKE)
    /*********************************************************************
     * uint8_t AddConnection(void)
     *
     * Overview:        This function create a new P2P connection entry
     *
     * PreCondition:    A P2P Connection Request or Response has been 
     *                  received and stored in rxMessage structure
     *
     * Input:  None
     *                  
     * Output: 
     *          The index of the P2P Connection Entry for the newly added
     *          connection
     *
     * Side Effects:    A new P2P Connection Entry is created. The search 
     *                  connection operation ends if an entry is added 
     *                  successfully
     *
     ********************************************************************/
    uint8_t AddConnection(void)
    {
        uint8_t i;
        uint8_t status = STATUS_SUCCESS;
        uint8_t connectionSlot = 0xFF;
    
        // if no peerinfo attached, this is only an active scan request,
        // so do not save the source device's info
        #ifdef ENABLE_ACTIVE_SCAN
            if( rxMessage.PayloadSize < 3 )
            {
                return STATUS_ACTIVE_SCAN;
            }
        #endif
        
        // loop through all entry and locate an proper slot
        for(i = 0; i < CONNECTION_SIZE; i++)
        {
            // check if the entry is valid
            if( ConnectionTable[i].status.bits.isValid )
            {
                // check if the entry address matches source address of current received packet
                if( isSameAddress(rxMessage.SourceAddress, ConnectionTable[i].Address) )
                {
                    connectionSlot = i;
                    status = STATUS_EXISTS;
                    break;
                }
            }
            else if( connectionSlot == 0xFF )
            {
                // store the first empty slot
                connectionSlot = i;
            }  
        }
            
        if( connectionSlot == 0xFF )
        {
            return STATUS_NOT_ENOUGH_SPACE;
        }
        else 
        {
            if( ConnMode >= ENABLE_PREV_CONN )
            {
                return status;
            }
            MyindexinPC = connectionSlot;    
            // store the source address
            for(i = 0; i < 8; i++)
            {
                ConnectionTable[connectionSlot].Address[i] = rxMessage.SourceAddress[i];
            }
            
            // store the capacity info and validate the entry
            ConnectionTable[connectionSlot].status.bits.isValid = 1;
            ConnectionTable[connectionSlot].status.bits.RXOnWhenIdle = (rxMessage.Payload[2] & 0x01);
            
            // store possible additional connection payload
            #if ADDITIONAL_NODE_ID_SIZE > 0
                for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
                {
                    ConnectionTable[connectionSlot].PeerInfo[i] = rxMessage.Payload[3+i];
                }
            #endif
			ConnectionTable[connectionSlot].flag = rxMessage.Payload[3];
            #ifdef ENABLE_SECURITY
                // if security is enabled, clear the incoming frame control
                IncomingFrameCounter[connectionSlot].Val = 0;
            #endif
            LatestConnection = connectionSlot;
            P2PStatus.bits.SearchConnection = 0;   
        }
        conn_size = Total_Connections();
    #if defined (ENABLE_NETWORK_FREEZER)
        nvmPutMyDC(&conn_size);
    #endif

        return status;
    }
#endif


#ifdef ENABLE_ACTIVE_SCAN
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
     *                          double uint16_t parameter use one bit to represent corresponding
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
    uint8_t MiApp_SearchConnection(INPUT uint8_t ScanDuration, INPUT uint32_t ChannelMap)
    {
        uint8_t i;
        uint32_t channelMask = 0x00000001;
        uint8_t backupChannel = currentChannel;
        MIWI_TICK t1, t2;
        
        for(i = 0; i < ACTIVE_SCAN_RESULT_SIZE; i++)
        {
            ActiveScanResults[i].Channel = 0xFF;
        }
        
        ActiveScanResultIndex = 0;
        i = 0;
        while( i < 32 )
        {
            if( ChannelMap & FULL_CHANNEL_MAP & (channelMask << i) )
            {
                #if defined(IEEE_802_15_4)
                    API_UINT16_UNION tmpPANID;
                #endif

                Printf("\r\nScan Channel ");
                PrintDec(i);
                /* choose appropriate channel */
                MiApp_SetChannel( i );
     
                MiApp_FlushTx();
                MiApp_WriteData(CMD_P2P_ACTIVE_SCAN_REQUEST);
                MiApp_WriteData(currentChannel);
                #if defined(IEEE_802_15_4)
                    tmpPANID.Val = 0xFFFF;
                    SendPacket(true, tmpPANID, NULL, true, false);
                #else
                    SendPacket(true, NULL, true, false);
                #endif
                
                t1.Val = MiWi_TickGet();
                while(1)
                {
                    if( MiApp_MessageAvailable())
                    {
                        MiApp_DiscardMessage();
                    }                     
                    //P2PTasks();
                    t2.Val = MiWi_TickGet();
                    if( MiWi_TickGetDiff(t2, t1) > ((uint32_t)(ScanTime[ScanDuration])) )
                    {
                        // if scan time exceed scan duration, prepare to scan the next channel
                        break;
                    }
                }          
            }  
            i++;
        }
        
        MiApp_SetChannel(backupChannel);
        
        return ActiveScanResultIndex;
    }   

#endif

#ifdef ENABLE_ED_SCAN

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
     *                          double uint16_t parameter use one bit to represent corresponding
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
    uint8_t MiApp_NoiseDetection(INPUT uint32_t ChannelMap, INPUT uint8_t ScanDuration, INPUT uint8_t DetectionMode, OUTPUT uint8_t *RSSIValue)
    {
        uint8_t i;
        uint8_t OptimalChannel;
        uint8_t minRSSI = 0xFF;
        uint32_t channelMask = 0x00000001;
        MIWI_TICK t1, t2;
        
        if( DetectionMode != NOISE_DETECT_ENERGY )
        {
            return 0xFF;
        }
        
        //ConsolePutROMString((ROM char*)"\r\nEnergy Scan Results:");
        i = 0;
        while( i < 32 )
        {
            if( ChannelMap & FULL_CHANNEL_MAP & (channelMask << i) )
            {
                uint8_t RSSIcheck;
                uint8_t maxRSSI = 0;
                uint8_t j, k;

                /* choose appropriate channel */
                MiApp_SetChannel(i);
                
                t1.Val = MiWi_TickGet();
                
                while(1)
                {
                    RSSIcheck = MiMAC_ChannelAssessment(CHANNEL_ASSESSMENT_ENERGY_DETECT);
                    if( RSSIcheck > maxRSSI )
                    {
                        maxRSSI = RSSIcheck;
                    }

                    t2.Val = MiWi_TickGet();
                    if( MiWi_TickGetDiff(t2, t1) > ((uint32_t)(ScanTime[ScanDuration])) )
                    {
                        // if scan time exceed scan duration, prepare to scan the next channel
                        break;
                    }
                } 
                
                Printf("\r\nChannel ");
                PrintDec(i);
                Printf(": ");
                j = maxRSSI/5;
                for(k = 0; k < j; k++)
                {
                    ConsolePut('-');
                }
                Printf(" ");
                PrintChar(maxRSSI);
                
                if( maxRSSI < minRSSI )
                {
                    minRSSI = maxRSSI;
                    OptimalChannel = i;
                    if( RSSIValue )
                    {
                        *RSSIValue = minRSSI;
                    }   
                }              
            }  
            i++;
        }        
       
        return OptimalChannel;
    }
   
#endif


#ifdef ENABLE_FREQUENCY_AGILITY

    /*********************************************************************
     * void StartChannelHopping(uint8_t OptimalChannel)
     *
     * Overview:        This function broadcast the channel hopping command
     *                  and after that, change operating channel to the 
     *                  input optimal channel     
     *
     * PreCondition:    Transceiver has been initialized
     *
     * Input:           OptimalChannel  - The channel to hop to
     *                  
     * Output: 
     *          None
     *
     * Side Effects:    The operating channel for current device will change
     *                  to the specified channel
     *
     ********************************************************************/
    void StartChannelHopping(INPUT uint8_t OptimalChannel)
    {
        uint8_t i;
        MIWI_TICK t1, t2;
        
        for( i = 0; i < FA_BROADCAST_TIME; i++)
        {
            t1.Val = MiWi_TickGet();
            while(1)
            {
                t2.Val = MiWi_TickGet();
                if( MiWi_TickGetDiff(t2, t1) > SCAN_DURATION_9 )
                {
                    MiApp_FlushTx();
                    MiApp_WriteData(CMD_CHANNEL_HOPPING);
                    MiApp_WriteData(currentChannel);
                    MiApp_WriteData(OptimalChannel);
                    #if defined(IEEE_802_15_4)
                        SendPacket(true, myPANID, NULL, true, false);
                    #else
                        SendPacket(true, NULL, true, false);
                    #endif
                    break;
                }
            }
        }
        MiApp_SetChannel(OptimalChannel);  
    }
    
    
    /********************************************************************************************
     * Function:
     *      BOOL MiApp_ResyncConnection(uint8_t ConnectionIndex, uint32_t ChannelMap)
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
     *                          double uint16_t parameter use one bit to represent corresponding
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
    bool MiApp_ResyncConnection(INPUT uint8_t ConnectionIndex, INPUT uint32_t ChannelMap)
    {
        uint8_t i;
        uint8_t j;
        uint8_t backupChannel = currentChannel;
        MIWI_TICK t1, t2;
        
        t1.Val = MiWi_TickGet();
        P2PStatus.bits.Resync = 1;
        for(i = 0; i < RESYNC_TIMES; i++)
        {
            uint32_t ChannelMask = 0x00000001;
            
            j = 0;
            while(P2PStatus.bits.Resync)
            {
                t2.Val = MiWi_TickGet();
                
                if( MiWi_TickGetDiff(t2, t1) > SCAN_DURATION_9 )
                {
                    t1.Val = t2.Val;
                    
                    if( j > 31 )
                    {
                        break;
                    }
                    while( (ChannelMap & FULL_CHANNEL_MAP & (ChannelMask << j)) == 0 )
                    {
                        if( ++j > 31 )
                        {
                            goto GetOutOfLoop;
                        }
                    }
                    
                    Printf("\r\nChecking Channel ");
                    PrintDec(j);
                    MiApp_SetChannel(j);
                    j++;
                    
                    MiApp_FlushTx();
                    MiApp_WriteData(CMD_P2P_ACTIVE_SCAN_REQUEST);
                    MiApp_WriteData(currentChannel);
        
                    #if defined(IEEE_802_15_4)
                        SendPacket(false, myPANID, ConnectionTable[ConnectionIndex].Address, true, false);
                    #else
                        SendPacket(false, ConnectionTable[ConnectionIndex].Address, true, false);
                    #endif
                }
                if( MiApp_MessageAvailable())
                {
                    MiApp_DiscardMessage();
                }        
                //P2PTasks();
            }
            if( P2PStatus.bits.Resync == 0 )
            {
                Printf("\r\nResynchronized Connection to Channel ");
                PrintDec(currentChannel);
                Printf("\r\n");
                return true;
            }
GetOutOfLoop:
            MacroNop();         
        }
        
        MiApp_SetChannel(backupChannel);
        P2PStatus.bits.Resync = 0;
        return false;
    }        

    #ifdef FREQUENCY_AGILITY_STARTER
        /*******************************************************************************************
         * Function:
         *      BOOL MiApp_InitChannelHopping(uint32_t ChannelMap)
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
        bool MiApp_InitChannelHopping( INPUT uint32_t ChannelMap)
        {
            uint8_t RSSIValue;
            uint8_t backupChannel = currentChannel;
            uint8_t backupConnMode = ConnMode;
            uint8_t optimalChannel;
            
            MiApp_ConnectionMode(DISABLE_ALL_CONN);
            optimalChannel = MiApp_NoiseDetection(ChannelMap, 10, NOISE_DETECT_ENERGY, &RSSIValue);
            MiApp_ConnectionMode(backupConnMode);
            
            MiApp_SetChannel(backupChannel);
            if( optimalChannel == backupChannel )
            {
                return false;
            }
            
            Printf("\r\nHopping to Channel ");
            PrintDec(optimalChannel);
            Printf("\r\n");
            StartChannelHopping(optimalChannel);
            return true;
        }
    #endif

#endif



#if !defined(TARGET_SMALL)
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
    void MiApp_RemoveConnection(INPUT uint8_t ConnectionIndex)
    {   
        if( ConnectionIndex == 0xFF )
        {
            uint8_t i;
            for(i = 0; i < CONNECTION_SIZE; i++)
            {
                uint16_t j;
                if( ConnectionTable[i].status.bits.isValid )
                {
                    MiApp_FlushTx();
                    MiApp_WriteData(CMD_P2P_CONNECTION_REMOVAL_REQUEST);   
                    #if defined(IEEE_802_15_4)
                        SendPacket(false, myPANID, ConnectionTable[i].Address, true, false);
                    #else
                        SendPacket(false, ConnectionTable[i].Address, true, false);
                    #endif
                    for(j = 0; j < 0xFFF; j++) {}   // delay
                }
                ConnectionTable[i].status.Val = 0;
                #if defined(ENABLE_NETWORK_FREEZER)
                    nvmPutConnectionTableIndex(&(ConnectionTable[i]), i);
                #endif
            } 
        }
        else if( ConnectionTable[ConnectionIndex].status.bits.isValid )
        {
            uint16_t j;
            
            MiApp_FlushTx();
            MiApp_WriteData(CMD_P2P_CONNECTION_REMOVAL_REQUEST);   
            #if defined(IEEE_802_15_4)
                SendPacket(false, myPANID, ConnectionTable[ConnectionIndex].Address, true, false);
            #else
                SendPacket(false, ConnectionTable[ConnectionIndex].Address, true, false);
            #endif
            for(j = 0; j < 0xFFF; j++) {}   // delay
            ConnectionTable[ConnectionIndex].status.Val = 0; 
            #if defined(ENABLE_NETWORK_FREEZER)
                nvmPutConnectionTableIndex(&(ConnectionTable[ConnectionIndex]), ConnectionIndex);
            #endif
        }
    }
#endif

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
void MiApp_ConnectionMode(INPUT uint8_t Mode)
{
    if( Mode > 3 )
    {
        return;
    }
    ConnMode = Mode;
    P2PCapacityInfo = (P2PCapacityInfo & 0x0F) | (ConnMode << 4);
    
    #if defined(ENABLE_NETWORK_FREEZER)
        nvmPutConnMode(&ConnMode);
    #endif
}