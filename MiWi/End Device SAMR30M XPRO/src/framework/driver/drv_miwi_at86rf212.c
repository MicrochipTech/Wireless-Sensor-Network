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
#include <string.h>
#include <stdbool.h>
#include <trx_access.h>
#include "config/symbol.h"
#include "config/miwi_config.h"
#include "config/config_atrf86.h"
#include "config/miwi_config_p2p.h"
#include "sio2host.h"
#include "delay.h"
#include "security/security.h"
#include "framework/driver/drv_miwi_at86rf212.h"

#ifdef ENABLE_SECURITY
uint8_t mySecurityKey[16] = { SECURITY_KEY_00, SECURITY_KEY_01, SECURITY_KEY_02, SECURITY_KEY_03, SECURITY_KEY_04,
    SECURITY_KEY_05, SECURITY_KEY_06, SECURITY_KEY_07, SECURITY_KEY_08, SECURITY_KEY_09, SECURITY_KEY_10, SECURITY_KEY_11,
    SECURITY_KEY_12, SECURITY_KEY_13, SECURITY_KEY_14, SECURITY_KEY_15 };
uint8_t final_mic_value[16], CBC_mic[16], CTR_mic[16];
uint8_t InitVect_in[16];

const uint8_t myKeySequenceNumber = KEY_SEQUENCE_NUMBER; // The sequence number of security key. Used to identify the security key

API_UINT32_UNION OutgoingFrameCounter;
#endif

/*- Types ------------------------------------------------------------------*/
typedef enum {
    PHY_STATE_INITIAL,
    PHY_STATE_IDLE,
    PHY_STATE_SLEEP,
    PHY_STATE_TX_WAIT_END,
} PhyState_t;

/*- Prototypes -------------------------------------------------------------*/
static void phyWriteRegister(uint8_t reg, uint8_t value);
static uint8_t phyReadRegister(uint8_t reg);
static void phyWaitState(uint8_t state);
static void phyTrxSetState(uint8_t state);
static void phySetRxState(void);

API_UINT16_UNION MAC_PANID;
API_UINT16_UNION myNetworkAddress;
volatile RADIO_STATUS trx_status;
uint8_t IEEESeqNum;
MACINIT_PARAM MACInitParams;
volatile uint16_t failureCounter = 0;
uint8_t BankIndex = 0xFF;
// Mic Size is 4 bytes.
uint8_t calculated_mic_values[AES_BLOCKSIZE / 4];
uint8_t received_mic_values[AES_BLOCKSIZE / 4];

uint8_t Received_RSSI_Val;
uint8_t Received_LQI_Val;

/*- Variables --------------------------------------------------------------*/
static PhyState_t phyState = PHY_STATE_INITIAL;
static uint8_t phyRxBuffer[FRAME_BUFFER];
static bool phyRxState;
uint8_t MACCurrentChannel;

#define MIPS 8000000 / 40000000
#define FAILURE_COUNTER ((uint16_t)0x20 * MIPS)

struct
    {
    uint8_t PayloadLen;
    uint8_t Payload[RX_PACKET_SIZE];
} RxBuffer[BANK_SIZE];

/*************************************************************************/ /**
*****************************************************************************/
// Trigger to Transmit Packet
void PHY_DataReq(uint8_t* data)
{
    phyTrxSetState(TRX_CMD_TX_ARET_ON);

    phyReadRegister(RG_IRQ_STATUS);

    /* size of the buffer is sent as first byte of the data
	 * and data starts from second byte.
	 */
    data[1] += 2; // 2
    trx_frame_write(&data[1], (data[1] - 1) /* length value*/);
    phyState = PHY_STATE_TX_WAIT_END;

    TRX_SLP_TR_HIGH();
    TRX_TRIG_DELAY();
    TRX_SLP_TR_LOW();
}

/*************************************************************************/ /**
*****************************************************************************/
// Random Number Generator
uint16_t PHY_RandomReq(void)
{
    uint16_t rnd = 0;
    uint8_t rndValue;

    phyTrxSetState(TRX_CMD_RX_ON);

    for (uint8_t i = 0; i < 16; i += 2) {
        delay_cycles_us(1);
        rndValue = (phyReadRegister(RG_PHY_RSSI) >> RND_VALUE) & 3;
        rnd |= rndValue << i;
    }
    phySetRxState();

    return rnd;
}

/*************************************************************************/ /**
*****************************************************************************/
// Radio Initialization
void PHY_Init(void)
{
    trx_spi_init();
    PhyReset();
    phyRxState = false;

    phyWriteRegister(RG_IRQ_MASK, 0xFF);
    do {
        phyWriteRegister(RG_TRX_STATE, TRX_CMD_TRX_OFF);
    } while (TRX_STATUS_TRX_OFF != (phyReadRegister(RG_TRX_STATUS) & TRX_STATUS_MASK));

    phyWriteRegister(RG_TRX_CTRL_1,
        (1 << TX_AUTO_CRC_ON) | (3 << SPI_CMD_MODE) | (1 << IRQ_MASK_MODE));

    phyWriteRegister(RG_TRX_CTRL_2,
        (1 << RX_SAFE_MODE) | (1 << OQPSK_SCRAM_EN));
	phyWriteRegister(0x05, 0xE1);
}

/*************************************************************************/ /**
*****************************************************************************/
// Set Radio to Receive State
void PHY_SetRxState(bool rx)
{
    phyRxState = rx;
    phySetRxState();
}

/*************************************************************************/ /**
*****************************************************************************/
// Set Channel
void PHY_SetChannel(uint8_t channel)
{
    uint8_t reg;

    reg = phyReadRegister(PHY_CC_CCA_REG) & ~0x1f;
    phyWriteRegister(PHY_CC_CCA_REG, reg | channel);
}

/*************************************************************************/ /**
*****************************************************************************/
// Set Pan ID
void PHY_SetPanId(uint16_t panId)
{
    uint8_t* d = (uint8_t*)&panId;
    phyWriteRegister(RG_PAN_ID_0, d[0]);
    phyWriteRegister(RG_PAN_ID_1, d[1]);
}

/*************************************************************************/ /**
*****************************************************************************/
// Set Short Address
void PHY_SetShortAddr(uint16_t addr)
{
    uint8_t* d = (uint8_t*)&addr;

    phyWriteRegister(RG_SHORT_ADDR_0, d[0]);
    phyWriteRegister(RG_SHORT_ADDR_1, d[1]);
    phyWriteRegister(RG_CSMA_SEED_0, d[0] + d[1]);
}

/*************************************************************************/ /**
*****************************************************************************/
// Set Transmit Power
void PHY_SetTxPower(uint8_t txPower)
{
    uint8_t reg;
    reg = phyReadRegister(RG_PHY_TX_PWR) & ~0x0f;
    phyWriteRegister(RG_PHY_TX_PWR, reg | txPower);
}

/*************************************************************************/ /**
*****************************************************************************/
// Radio Sleep
void PHY_Sleep(void)
{
    phyTrxSetState(TRX_CMD_TRX_OFF);
    TRX_SLP_TR_HIGH();
    phyState = PHY_STATE_SLEEP;
}

void change_antenna(uint8_t ant_ctrl)
{
	/* do the configurations if diversity has to be disabled */
	trx_bit_write(SR_ANT_DIV_EN, ANT_DIV_DISABLE);

	trx_bit_write(SR_ANT_EXT_SW_EN, ANT_EXT_SW_ENABLE);
	if (ant_ctrl == ANT_CTRL_1) {
		/* Enable A1/X2 */
		trx_bit_write(SR_ANT_CTRL, ANT_CTRL_1);
		} else if (ant_ctrl == ANT_CTRL_2) {
		/* Enable A2/X3 */
		trx_bit_write(SR_ANT_CTRL, ANT_CTRL_2);
	}
}

/*************************************************************************/ /**
*****************************************************************************/
// Radio Wake Up
void PHY_Wakeup(void)
{
    TRX_SLP_TR_LOW();
    phySetRxState();
    phyState = PHY_STATE_IDLE;
}

/*************************************************************************/ /**
*****************************************************************************/
// Encrypt Block
void PHY_EncryptReq(uint8_t* text, uint8_t* key)
{
    sal_aes_setup(key, AES_MODE_ECB, AES_DIR_ENCRYPT);
#if (SAL_TYPE == AT86RF2xx)
    sal_aes_wrrd(text, NULL);
#else
    sal_aes_exec(text);
#endif
    sal_aes_read(text);
}

void PHY_EncryptReqCBC(uint8_t* text, uint8_t* key)
{
    sal_aes_setup(key, AES_MODE_CBC, AES_DIR_ENCRYPT);
#if (SAL_TYPE == AT86RF2xx)
    sal_aes_wrrd(text, NULL);
#else
    sal_aes_exec(text);
#endif
    sal_aes_read(text);
}

/*************************************************************************/ /**
*****************************************************************************/
// Decrypt Block
void PHY_DecryptReq(uint8_t* text, uint8_t* key)
{
    sal_aes_setup(key, AES_MODE_ECB, AES_DIR_DECRYPT);
    sal_aes_wrrd(text, NULL);
    sal_aes_read(text);
}

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

bool MiMAC_SetAltAddress(uint8_t* Address, uint8_t* PANID)
{
    myNetworkAddress.v[0] = Address[0];
    myNetworkAddress.v[1] = Address[1];
    MAC_PANID.v[0] = PANID[0];
    MAC_PANID.v[1] = PANID[1];
    PHY_SetShortAddr(APP_ADDR);
    PHY_SetPanId(MY_PAN_ID);
    return true;
}

#if defined(ENABLE_SECURITY)
void mic_generator(uint8_t* Payloadinfo, uint8_t len, uint8_t frame_control, API_UINT32_UNION FrameCounter, uint8_t* SourceIEEEAddress)
{
    uint8_t i, j, iterations, copy_packet[FRAME_BUFFER], cbc_header_with_padding[AES_BLOCKSIZE];
    // Calculating No of blocks in the packet (1 block = 16 bytes of data)
    iterations = len / AES_BLOCKSIZE;
    if (len % AES_BLOCKSIZE != 0)
        iterations++;
    InitVect_in[0] = frame_control; //0x49
    cbc_header_with_padding[0] = 0x00; // l(a)
    cbc_header_with_padding[1] = 0x0d; // l(a)Header Length
    cbc_header_with_padding[2] = frame_control; //frame_control;
    for (i = 0; i < 8; i++) {
        InitVect_in[i + 1] = SourceIEEEAddress[i];
        cbc_header_with_padding[i + 7] = SourceIEEEAddress[i];
    }
    if (MY_ADDRESS_LENGTH < 8) {
        for (i = MY_ADDRESS_LENGTH; i < 8; i++) {
            InitVect_in[i + 1] = 0; // padding if source address is less than 8 bytes
            cbc_header_with_padding[i + 7] = 0; //
        }
    }
    for (i = 0; i < 4; i++) {
        InitVect_in[i + 9] = FrameCounter.v[i];
        cbc_header_with_padding[i + 3] = FrameCounter.v[i];
    }
    //cbc_header_with_padding[14] = 0x00;  // Padding
    cbc_header_with_padding[15] = 0x00; // Padding
    InitVect_in[13] = SECURITY_LEVEL; //Security Mode // 0x04 --> CCM-32
    InitVect_in[14] = 0x00; // 15 th byte padded to zero
    InitVect_in[15] = len; // For MIC calculation All preceding blocks will be incremented by 1
    PHY_EncryptReq(&InitVect_in[0], mySecurityKey);
    for (i = 0; i < AES_BLOCKSIZE; i++) {
        copy_packet[i] = cbc_header_with_padding[i];
    }
    // Copy the Payload and Do the padding
    for (i = 16; i < len + AES_BLOCKSIZE; i++) {
        copy_packet[i] = Payloadinfo[i - AES_BLOCKSIZE]; // Copying the Payload
    }
    for (i = len + 16; i < (iterations * AES_BLOCKSIZE) + AES_BLOCKSIZE; i++) {
        copy_packet[i] = 0; // Padding
    }
    for (i = 0; i < iterations + 1; i++) {
        for (j = 0; j < AES_BLOCKSIZE; j++) {
            InitVect_in[j] = InitVect_in[j] ^ copy_packet[j + (i * 16)];
        }
        PHY_EncryptReq(&InitVect_in[0], mySecurityKey);
    }
    for (i = 0; i < AES_BLOCKSIZE; i++) {
        CBC_mic[i] = InitVect_in[i];
    }
}

/************************************************************************************
 * Function:
 *      bool DataEncrypt( uint8_t *key, uint8_t *Payload, uint8_t *PayloadLen,
 *                        API_UINT32_UNION FrameCounter, uint8_t FrameControl )
 *
 * Summary:
 *      This function decrypt received secured frame
 *
 * Description:
 *      This is the function to encrypt the transmitting packet. All parameters are
 *      input information used in the encryption process. After encryption is
 *      performed successfully, the result will be put into the buffer that is
 *      pointed by input parameter "Payload" and the parameter "PayloadLen" will
 *      also be updated.
 *
 * PreCondition:
 *      Transceiver initialization has been done.
 *
 * Parameters:
 *      uint8_t * Key          - Security Key used to decrypt packet
 *      uint8_t * Payload      - Pointer to the the input plain payload and output
 *                            encrypted payload
 *      uint8_t * PayloadLen   - Pointer to the length of input plain payload and
 *                            output encrypted payload
 *      uint8_t * DestIEEEAddress      - The IEEE address of the packet target
 *      API_UINT32_UNION FrameCounter      - Frame counter of the transmitting packet
 *      uint8_t FrameControl   - The frame control byte of the transmitting packet
 *
 * Returns:
 *      A boolean to indicates if encryption is successful.
 *
 * Example:
 *      <code>
 *      DataEncrypt(key, payload, &payloadLen, FrameCounter, FrameControl);
 *      </code>
 *
 * Remarks:
 *      None
 *
 *****************************************************************************************/

bool DataEncrypt(uint8_t* Payloadinfo, uint8_t* Payload_len, API_UINT32_UNION FrameCounter,
    uint8_t FrameControl)
{
    uint8_t i, iterations, block[AES_BLOCKSIZE], j, CTR_Nonce_and_Counter[AES_BLOCKSIZE];
    // Calculating No of blocks in the packet (1 block = 16 bytes of data)
    iterations = *Payload_len / AES_BLOCKSIZE;
    if (*Payload_len % AES_BLOCKSIZE != 0)
        iterations++;

    mic_generator(&Payloadinfo[0], *Payload_len, FrameControl, FrameCounter, MACInitParams.PAddress);

    for (i = *Payload_len; i < iterations * AES_BLOCKSIZE; i++) {
        Payloadinfo[i] = 0; // Padding
    }
    CTR_Nonce_and_Counter[0] = 0x01; // L
    for (i = 0; i < MY_ADDRESS_LENGTH; i++) {
        CTR_Nonce_and_Counter[i + 1] = MACInitParams.PAddress[i];
    }
    if (MY_ADDRESS_LENGTH < 8) {
        for (i = MY_ADDRESS_LENGTH; i < 8; i++) {
            CTR_Nonce_and_Counter[i + 1] = 0; // padding if address is less than 8 bytes
        }
    }
    // FrameCounter
    for (i = 0; i < 4; i++) {
        CTR_Nonce_and_Counter[i + 9] = FrameCounter.v[i];
    }
    CTR_Nonce_and_Counter[13] = SECURITY_LEVEL; //Security Mode // 0x04 --> CCM-32
    CTR_Nonce_and_Counter[14] = 0x00; // 15 th byte padded to zero
    CTR_Nonce_and_Counter[15] = 0x00; // For MIC calculation All preceding blocks will be incremented by 1

    for (i = 0; i < iterations + 1; i++) {
        for (j = 0; j < AES_BLOCKSIZE; j++) {
            block[j] = CTR_Nonce_and_Counter[j];
        }
        PHY_EncryptReq(&block[0], mySecurityKey);
        for (j = 0; j < AES_BLOCKSIZE; j++) {
            if (CTR_Nonce_and_Counter[15] == 0) {
                final_mic_value[j] = block[j] ^ CBC_mic[j]; // CTR_MIC XOR CBC_MIC gives final MIC values
            }
            else {
                Payloadinfo[j + (i - 1) * AES_BLOCKSIZE] = block[j] ^ Payloadinfo[j + (i - 1) * AES_BLOCKSIZE];
            }
        }
        CTR_Nonce_and_Counter[15]++; // Increment Counter for next opration
    }
    return true;
}
// Validates the Received mic with the mic computed from data packet decryption.
bool validate_mic(void)
{
    if (final_mic_value[0] != received_mic_values[0] || final_mic_value[1] != received_mic_values[1] || final_mic_value[2] != received_mic_values[2] || final_mic_value[3] != received_mic_values[3]) {
        return false;
    }
    else {
        return true;
    }
}

/************************************************************************************
 * Function:
 *      bool DataDecrypt( uint8_t *key, uint8_t *Payload, uint8_t *PayloadLen,
 *                        uint8_t *SourceIEEEAddress, API_UINT32_UNION FrameCounter,
 *                        uint8_t FrameControl )
 *
 * Summary:
 *      This function decrypt received secured frame
 *
 * Description:
 *      This is the function to decrypt the secured packet. All parameters are
 *      input information used in the decryption process. After decryption is
 *      performed successfully, the result will be put into the buffer that is
 *      pointed by input parameter "Payload" and the parameter "PayloadLen" will
 *      also be updated.
 *
 * PreCondition:
 *      Transceiver initialization has been done.
 *
 * Parameters:
 *      uint8_t * Key          - Security Key used to decrypt packet
 *      uint8_t * Payload      - Pointer to the the input secured payload and output
 *                            decrypted payload
 *      uint8_t * PayloadLen   - Pointer to the length of input secured payload and
 *                            output decrypted payload
 *      uint8_t * SourceIEEEAddress    - The IEEE address of the package originator
 *      API_UINT32_UNION FrameCounter      - Frame counter of the received packet
 *      uint8_t FrameControl   - The frame control byte of the received packet
 *
 * Returns:
 *      A boolean to indicates if decryption is successful.
 *
 * Example:
 *      <code>
 *      DataDecrypt(key, payload, &payloadLen, SourceIEEEAddr, FrameCounter, FrameControl);
 *      </code>
 *
 * Remarks:
 *      None
 *
 *****************************************************************************************/
bool DataDecrypt(uint8_t* Payload, uint8_t* PayloadLen, uint8_t* SourceIEEEAddress,
    API_UINT32_UNION FrameCounter, uint8_t FrameControl)
{
    uint8_t i, iterations, block[AES_BLOCKSIZE], j, CTR_Nonce_and_Counter[AES_BLOCKSIZE];
    // Calculating No of blocks in the packet (1 block = 16 bytes of data)
    iterations = *PayloadLen / 16;
    if (*PayloadLen % AES_BLOCKSIZE != 0)
        iterations++;

    for (i = *PayloadLen; i < iterations * AES_BLOCKSIZE; i++) {
        Payload[i] = 0; // Padding
    }
    CTR_Nonce_and_Counter[0] = 0x01; // L
    for (i = 0; i < MY_ADDRESS_LENGTH; i++) {
        CTR_Nonce_and_Counter[i + 1] = SourceIEEEAddress[i];
    }
    if (MY_ADDRESS_LENGTH < 8) {
        for (i = MY_ADDRESS_LENGTH; i < 8; i++) {
            CTR_Nonce_and_Counter[i + 1] = 0; // padding if source address is less than 8 bytes
        }
    }
    for (i = 0; i < 4; i++) {
        CTR_Nonce_and_Counter[i + 9] = FrameCounter.v[i];
    }
    CTR_Nonce_and_Counter[13] = SECURITY_LEVEL; //Security Mode
    CTR_Nonce_and_Counter[14] = 0x00; // 15 th byte padded to zero
    CTR_Nonce_and_Counter[15] = 0x00; // For MIC calculation All preceding blocks will be incremented by 1

    for (i = 0; i < iterations + 1; i++) {
        for (j = 0; j < 16; j++) {
            block[j] = CTR_Nonce_and_Counter[j];
        }
        PHY_EncryptReq(&block[0], mySecurityKey);
        for (j = 0; j < 16; j++) {
            if (CTR_Nonce_and_Counter[15] == 0) {
                CTR_mic[j] = block[j]; // CTR_MIC XOR CBC_MIC gives final MIC values
            }
            else {
                Payload[j + (i - 1) * 16] = block[j] ^ Payload[j + (i - 1) * 16];
            }
        }
        CTR_Nonce_and_Counter[15]++; // Increment Counter for next opration
    }
    *PayloadLen = *PayloadLen - 4;
    mic_generator(&Payload[0], *PayloadLen, FrameControl, FrameCounter, SourceIEEEAddress);
    for (i = 0; i < 16; i++) {
        final_mic_value[i] = CTR_mic[i] ^ CBC_mic[i];
    }
    return validate_mic();
}
#endif

#if defined ENABLE_SLEEP
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
bool MiMAC_PowerState(INPUT uint8_t PowerState)
{
    switch (PowerState) {
    case POWER_STATE_DEEP_SLEEP: {
        //;clear the WAKE pin in order to allow the device to go to sleep
        PHY_Sleep();
    } break;

    case POWER_STATE_OPERATE: {
#if 1
        // use wake pin to wake up the radio
        // enable the radio to wake up quicker

        uint8_t results;
        MIWI_TICK t1, t2;

        //wake up the device
        SLP_TR_LOW();

        t1.Val = MiWi_TickGet();

        while (1) {
            t2.Val = MiWi_TickGet();
            t2.Val = MiWi_TickGetDiff(t2, t1);

            // if timeout, assume the device has waken up
            if (t2.Val > HUNDRED_MILI_SECOND) {
                PHY_Init();
                MiMAC_SetChannel(MACCurrentChannel, 0);
                MiMAC_SetAltAddress(myNetworkAddress.v, MAC_PANID.v);
                break;
            }
        }

        while (1) {
            t2.Val = MiWi_TickGet();
            t2.Val = MiWi_TickGetDiff(t2, t1);

            // if timeout, assume the device has waken up
            if (t2.Val > HUNDRED_MILI_SECOND) {
                PHY_Init();
                MiMAC_SetChannel(MACCurrentChannel, 0);
                MiMAC_SetAltAddress(myNetworkAddress.v, MAC_PANID.v);
                break;
            }
        }

#else

        // use reset to wake up the radio is more
        // reliable
        PHY_Init();
        MiMAC_SetAltAddress(myNetworkAddress.v, MAC_PANID.v);

#endif

    } break;

    default:
        return false;
    }
    return true;
}
#endif

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
bool MiMAC_SetChannel(uint8_t channel, uint8_t offsetFreq)
{
	if (channel < 0x00 || channel > 0x0A) {
		return false;
	}
	MACCurrentChannel = channel;
	PHY_SetChannel(channel);
	return true;
}


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
bool MiMAC_Init(MACINIT_PARAM initValue)
{
    uint8_t i;

    PHY_Init();
    MACInitParams = initValue;
    uint16_t x = PHY_RandomReq();
    // Set RF mode
    PHY_SetRxState(true);
    IEEESeqNum = x & 0xff;

    MACCurrentChannel = 11;

    // Set Node Address
    PHY_SetIEEEAddr(myLongAddress);

    trx_status.Val = 0;

    for (i = 0; i < BANK_SIZE; i++) {
        RxBuffer[i].PayloadLen = 0;
    }
#ifdef ENABLE_SECURITY
#if defined(ENABLE_NETWORK_FREEZER)
    if (initValue.actionFlags.bits.NetworkFreezer) {
        nvmGetOutFrameCounter(OutgoingFrameCounter.v);
        OutgoingFrameCounter.Val += FRAME_COUNTER_UPDATE_INTERVAL;
        nvmPutOutFrameCounter(OutgoingFrameCounter.v);
    }
    else {
        OutgoingFrameCounter.Val = 0;
        nvmPutOutFrameCounter(OutgoingFrameCounter.v);
        OutgoingFrameCounter.Val = 1;
    }
#else
    OutgoingFrameCounter.Val = 1;
#endif
#endif

    return true;
}

/************************************************************************************
 * Function:
 *      bool MiMAC_SendPacket(  MAC_TRANS_PARAM transParam,
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
bool MiMAC_SendPacket(MAC_TRANS_PARAM transParam,
    uint8_t* MACPayload,
    uint8_t MACPayloadLen)
{
    uint8_t headerLength;
    uint8_t loc = 0;
    uint8_t i = 0;
    uint8_t packet[FRAME_BUFFER];
    uint8_t frameControl = 0;
#ifndef TARGET_SMALL
    bool IntraPAN;
#endif

    if (transParam.flags.bits.broadcast) {
        transParam.altDestAddr = true;
    }

    if (transParam.flags.bits.secEn) {
        transParam.altSrcAddr = false;
    }

// wait for the previous transmission finish
#if !defined(VERIFY_TRANSMIT)
    MIWI_TICK t1, t2;
    t1.Val = MiWi_TickGet();
    while (trx_status.bits.TX_BUSY) {
        t2.Val = MiWi_TickGet();
        t2.Val = MiWi_TickGetDiff(t2, t1);
        if (t2.Val > TWENTY_MILI_SECOND) // 20 ms
        {
            PHY_Init();
            MiMAC_SetChannel(MACCurrentChannel, 0);
            MiMAC_SetAltAddress(myNetworkAddress.v, MAC_PANID.v);
            trx_status.bits.TX_BUSY = 0;
        }
    }
#endif

    // set the frame control in variable i
    if (transParam.flags.bits.packetType == PACKET_TYPE_COMMAND) {
        frameControl = 0x03;
    }
    else if (transParam.flags.bits.packetType == PACKET_TYPE_DATA) {
        frameControl = 0x01;
    }

// decide the header length for different addressing mode
#ifndef TARGET_SMALL
    if ((transParam.DestPANID.Val == MAC_PANID.Val) && (MAC_PANID.Val != 0xFFFF)) // this is intraPAN
#endif
    {
        headerLength = 5;
        frameControl |= 0x40;
#ifndef TARGET_SMALL
        IntraPAN = true;
#endif
    }
#ifndef TARGET_SMALL
    else {
        headerLength = 7;
        IntraPAN = false;
    }
#endif

    if (transParam.altDestAddr) {
        headerLength += 2;
    }
    else {
        headerLength += 8;
    }

    if (transParam.altSrcAddr) {
        headerLength += 2;
    }
    else {
        headerLength += 8;
    }

    if (transParam.flags.bits.ackReq && transParam.flags.bits.broadcast == false) {
        frameControl |= 0x20;
    }

    // use PACKET_TYPE_RESERVE to represent beacon. Fixed format for beacon packet
    if (transParam.flags.bits.packetType == PACKET_TYPE_RESERVE) {
        frameControl = 0x00;
        headerLength = 7;
#if !defined(TARGET_SMALL)
        IntraPAN = false;
#endif
        transParam.altSrcAddr = true;
        transParam.flags.bits.ackReq = false;
    }

#ifdef ENABLE_SECURITY
    if (transParam.flags.bits.secEn) {
        frameControl |= 0x08;
        //DataEncrypt(MACPayload, &MACPayloadLen , 0 , 0 , 0);
        DataEncrypt(MACPayload, &MACPayloadLen, OutgoingFrameCounter, frameControl);
    }
#endif

    // set header length
    packet[loc++] = headerLength;
// set packet length
#ifdef ENABLE_SECURITY
    if (transParam.flags.bits.secEn) {
        // 4 byte mic and 5 byte outgoing frame counter
        packet[loc++] = MACPayloadLen + headerLength + 9;
    }
    else
#endif
    {
        packet[loc++] = MACPayloadLen + headerLength;
    }

    // set frame control LSB
    packet[loc++] = frameControl;

    // set frame control MSB
    if (transParam.flags.bits.packetType == PACKET_TYPE_RESERVE) {
        packet[loc++] = 0x80;
        // sequence number
        packet[loc++] = IEEESeqNum++;
    }
    else {
        if (transParam.altDestAddr && transParam.altSrcAddr) {
            packet[loc++] = 0x88;
        }
        else if (transParam.altDestAddr && transParam.altSrcAddr == 0) {
            packet[loc++] = 0xC8;
        }
        else if (transParam.altDestAddr == 0 && transParam.altSrcAddr == 1) {
            packet[loc++] = 0x8C;
        }
        else {
            packet[loc++] = 0xCC;
        }

        // sequence number
        packet[loc++] = IEEESeqNum++;

        // destination PANID
        packet[loc++] = transParam.DestPANID.v[0];
        packet[loc++] = transParam.DestPANID.v[1];

        // destination address
        if (transParam.flags.bits.broadcast) {
            packet[loc++] = 0xFF;
            packet[loc++] = 0xFF;
        }
        else {
            if (transParam.altDestAddr) {
                packet[loc++] = transParam.DestAddress[0];
                packet[loc++] = transParam.DestAddress[1];
            }
            else {
                for (i = 0; i < 8; i++) {
                    packet[loc++] = transParam.DestAddress[i];
                }
            }
        }
    }

#ifndef TARGET_SMALL
    // source PANID if necessary
    if (IntraPAN == false) {
        packet[loc++] = MAC_PANID.v[0];
        packet[loc++] = MAC_PANID.v[1];
    }
#endif

    // source address
    if (transParam.altSrcAddr) {
        packet[loc++] = myNetworkAddress.v[0];
        packet[loc++] = myNetworkAddress.v[1];
    }
    else {
        for (i = 0; i < 8; i++) {
            packet[loc++] = MACInitParams.PAddress[i];
        }
    }
#ifdef ENABLE_SECURITY
    if (transParam.flags.bits.secEn) {
        // fill the additional security aux header
        for (i = 0; i < 4; i++) {
            packet[loc++] = OutgoingFrameCounter.v[i];
        }
        OutgoingFrameCounter.Val++;

#if defined(ENABLE_NETWORK_FREEZER)
        if ((OutgoingFrameCounter.v[0] == 0) && ((OutgoingFrameCounter.v[1] & 0x03) == 0)) {
            nvmPutOutFrameCounter(OutgoingFrameCounter.v);
        }
#endif
        //copy myKeySequenceNumber
        packet[loc++] = myKeySequenceNumber;
    }
#endif

#ifndef ENABLE_SECURITY
    // write the payload
    for (i = 0; i < MACPayloadLen; i++) {
        packet[loc++] = MACPayload[i];
    }
#else
    // write the payload
    for (i = 0; i < MACPayloadLen; i++) {
        packet[loc++] = MACPayload[i];
    }
    // MIC added
    packet[loc++] = final_mic_value[0];
    packet[loc++] = final_mic_value[1];
    packet[loc++] = final_mic_value[2];
    packet[loc++] = final_mic_value[3];
#endif

    trx_status.bits.TX_BUSY = 1;

    // set the trigger value
    if (transParam.flags.bits.ackReq && transParam.flags.bits.broadcast == false) {
        i = 0x05;
#ifndef TARGET_SMALL
        trx_status.bits.TX_PENDING_ACK = 1;
#endif
    }
    else {
        i = 0x01;
#ifndef TARGET_SMALL
        trx_status.bits.TX_PENDING_ACK = 0;
#endif
    }
    // Now Trigger the Transmission of packet
    PHY_DataReq(packet);

    return !trx_status.bits.TX_FAIL;
}

/*************************************************************************/ /**
*****************************************************************************/
// Energy Detection
uint8_t PHY_EdReq(void)
{
    uint8_t ed;

    phyTrxSetState(TRX_CMD_RX_ON);
    phyWriteRegister(RG_PHY_ED_LEVEL, 0);

    while (0 == (phyReadRegister(RG_IRQ_STATUS) & (1 << CCA_ED_DONE))) {
    }

    ed = (uint8_t)phyReadRegister(RG_PHY_ED_LEVEL);

    phySetRxState();

    return ed + PHY_RSSI_BASE_VAL;
}

/*************************************************************************/ /**
*****************************************************************************/
static void phyWriteRegister(uint8_t reg, uint8_t value)
{
    trx_reg_write(reg, value);
}

/*************************************************************************/ /**
*****************************************************************************/
static uint8_t phyReadRegister(uint8_t reg)
{
    uint8_t value;

    value = trx_reg_read(reg);

    return value;
}

/*************************************************************************/ /**
*****************************************************************************/
static void phyWaitState(uint8_t state)
{
    while (state != (phyReadRegister(RG_TRX_STATUS) & TRX_STATUS_MASK)) {
    }
}

/*************************************************************************/ /**
*****************************************************************************/
static void phySetRxState(void)
{
    phyTrxSetState(TRX_CMD_TRX_OFF);

    phyReadRegister(RG_IRQ_STATUS);

    if (phyRxState) {
        phyTrxSetState(TRX_CMD_RX_AACK_ON);
    }
}

/*************************************************************************/ /**
*****************************************************************************/
static void phyTrxSetState(uint8_t state)
{
    do {
        phyWriteRegister(RG_TRX_STATE, TRX_CMD_FORCE_TRX_OFF);
    } while (TRX_STATUS_TRX_OFF != (phyReadRegister(RG_TRX_STATUS) & TRX_STATUS_MASK));

    do {
        phyWriteRegister(RG_TRX_STATE,
            state);
    } while (state != (phyReadRegister(RG_TRX_STATUS) & TRX_STATUS_MASK));
}

/*************************************************************************/ /**
*****************************************************************************/
// Setting the IEEE address
void PHY_SetIEEEAddr(uint8_t* ieee_addr)
{
    uint8_t* ptr_to_reg = ieee_addr;
    for (uint8_t i = 0; i < 8; i++) {
        trx_reg_write((RG_IEEE_ADDR_0 + i), *ptr_to_reg);
        ptr_to_reg++;
    }
}

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
void MiMAC_DiscardPacket(void)
{
    //re-enable the ACKS
    if (BankIndex < BANK_SIZE) {
        RxBuffer[BankIndex].PayloadLen = 0;
    }
}

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
bool MiMAC_ReceivedPacket(void)
{
    uint8_t i;

    //If the stack TX has been busy for a long time then
    //time out the TX because we may have missed the interrupt
    //and don't want to lock up the stack forever
    if (trx_status.bits.TX_BUSY) {
        if (failureCounter >= FAILURE_COUNTER) {
            failureCounter = 0;
            trx_status.bits.TX_BUSY = 0;
        }
        else {
            failureCounter++;
        }
    }

    BankIndex = 0xFF;
    for (i = 0; i < BANK_SIZE; i++) {
        if (RxBuffer[i].PayloadLen > 0) {
            BankIndex = i;
            break;
        }
    }

    if (BankIndex < BANK_SIZE) {
        uint8_t addrMode;
#ifndef TARGET_SMALL
        bool bIntraPAN = true;

        if ((RxBuffer[BankIndex].Payload[0] & 0x40) == 0) {
            bIntraPAN = false;
        }
#endif
        MACRxPacket.flags.Val = 0;
        MACRxPacket.altSourceAddress = false;

        //Determine the start of the MAC payload
        addrMode = RxBuffer[BankIndex].Payload[1] & 0xCC;
        switch (addrMode) {
        case 0xC8: //short dest, long source
            // for P2P only broadcast allows short destination address
            if (RxBuffer[BankIndex].Payload[5] == 0xFF && RxBuffer[BankIndex].Payload[6] == 0xFF) {
                MACRxPacket.flags.bits.broadcast = 1;
            }
            MACRxPacket.flags.bits.sourcePrsnt = 1;

#ifndef TARGET_SMALL
            if (bIntraPAN) // check if it is intraPAN
#endif
            {
#ifndef TARGET_SMALL
                MACRxPacket.SourcePANID.v[0] = RxBuffer[BankIndex].Payload[3];
                MACRxPacket.SourcePANID.v[1] = RxBuffer[BankIndex].Payload[4];
#endif
                MACRxPacket.SourceAddress = &(RxBuffer[BankIndex].Payload[7]);

                MACRxPacket.PayloadLen = RxBuffer[BankIndex].PayloadLen - 19;
                MACRxPacket.Payload = &(RxBuffer[BankIndex].Payload[15]);
            }
#ifndef TARGET_SMALL
            else {
                MACRxPacket.SourcePANID.v[0] = RxBuffer[BankIndex].Payload[7];
                MACRxPacket.SourcePANID.v[1] = RxBuffer[BankIndex].Payload[8];
                MACRxPacket.SourceAddress = &(RxBuffer[BankIndex].Payload[9]);
                MACRxPacket.PayloadLen = RxBuffer[BankIndex].PayloadLen - 21;
                MACRxPacket.Payload = &(RxBuffer[BankIndex].Payload[17]);
            }
#endif

            break;

        case 0xCC: // long dest, long source
            MACRxPacket.flags.bits.sourcePrsnt = 1;
#ifndef TARGET_SMALL
            if (bIntraPAN) // check if it is intraPAN
#endif
            {
//rxFrame.flags.bits.intraPAN = 1;
#ifndef TARGET_SMALL
                MACRxPacket.SourcePANID.v[0] = RxBuffer[BankIndex].Payload[3];
                MACRxPacket.SourcePANID.v[1] = RxBuffer[BankIndex].Payload[4];
#endif
                MACRxPacket.SourceAddress = &(RxBuffer[BankIndex].Payload[13]);
                MACRxPacket.PayloadLen = RxBuffer[BankIndex].PayloadLen - 25;
                MACRxPacket.Payload = &(RxBuffer[BankIndex].Payload[21]);
            }
#ifndef TARGET_SMALL
            else {
                MACRxPacket.SourcePANID.v[0] = RxBuffer[BankIndex].Payload[13];
                MACRxPacket.SourcePANID.v[1] = RxBuffer[BankIndex].Payload[14];
                MACRxPacket.SourceAddress = &(RxBuffer[BankIndex].Payload[15]);
                MACRxPacket.PayloadLen = RxBuffer[BankIndex].PayloadLen - 27;
                MACRxPacket.Payload = &(RxBuffer[BankIndex].Payload[23]);
            }
#endif
            break;

        case 0x80: // short source only. used in beacon
        {
            MACRxPacket.flags.bits.broadcast = 1;
            MACRxPacket.flags.bits.sourcePrsnt = 1;
            MACRxPacket.altSourceAddress = true;
            MACRxPacket.SourcePANID.v[0] = RxBuffer[BankIndex].Payload[3];
            MACRxPacket.SourcePANID.v[1] = RxBuffer[BankIndex].Payload[4];
            MACRxPacket.SourceAddress = &(RxBuffer[BankIndex].Payload[5]);
            MACRxPacket.PayloadLen = RxBuffer[BankIndex].PayloadLen - 11;
            MACRxPacket.Payload = &(RxBuffer[BankIndex].Payload[7]);
        } break;

        case 0x88: // short dest, short source
        {
            if (RxBuffer[BankIndex].Payload[5] == 0xFF && RxBuffer[BankIndex].Payload[6] == 0xFF) {
                MACRxPacket.flags.bits.broadcast = 1;
            }
            MACRxPacket.flags.bits.sourcePrsnt = 1;
            MACRxPacket.altSourceAddress = true;
#ifndef TARGET_SMALL
            if (bIntraPAN == false) {
                MACRxPacket.SourcePANID.v[0] = RxBuffer[BankIndex].Payload[7];
                MACRxPacket.SourcePANID.v[1] = RxBuffer[BankIndex].Payload[8];
                MACRxPacket.SourceAddress = &(RxBuffer[BankIndex].Payload[9]);
                MACRxPacket.PayloadLen = RxBuffer[BankIndex].PayloadLen - 15;
                MACRxPacket.Payload = &(RxBuffer[BankIndex].Payload[11]);
            }
            else
#endif
            {
                MACRxPacket.SourcePANID.v[0] = RxBuffer[BankIndex].Payload[3];
                MACRxPacket.SourcePANID.v[1] = RxBuffer[BankIndex].Payload[4];
                MACRxPacket.SourceAddress = &(RxBuffer[BankIndex].Payload[7]);
                MACRxPacket.PayloadLen = RxBuffer[BankIndex].PayloadLen - 13;
                MACRxPacket.Payload = &(RxBuffer[BankIndex].Payload[9]);
            }
        } break;

        case 0x8C: // long dest, short source
        {
            MACRxPacket.flags.bits.sourcePrsnt = 1;
            MACRxPacket.altSourceAddress = true;
#ifndef TARGET_SMALL
            if (bIntraPAN) // check if it is intraPAN
#endif
            {
#ifndef TARGET_SMALL
                MACRxPacket.SourcePANID.v[0] = RxBuffer[BankIndex].Payload[3];
                MACRxPacket.SourcePANID.v[1] = RxBuffer[BankIndex].Payload[4];
#endif
                MACRxPacket.SourceAddress = &(RxBuffer[BankIndex].Payload[12]);

                MACRxPacket.PayloadLen = RxBuffer[BankIndex].PayloadLen - 19;
                MACRxPacket.Payload = &(RxBuffer[BankIndex].Payload[15]);
            }
#ifndef TARGET_SMALL
            else {
                MACRxPacket.SourcePANID.v[0] = RxBuffer[BankIndex].Payload[12];
                MACRxPacket.SourcePANID.v[1] = RxBuffer[BankIndex].Payload[13];
                MACRxPacket.SourceAddress = &(RxBuffer[BankIndex].Payload[14]);
                MACRxPacket.PayloadLen = RxBuffer[BankIndex].PayloadLen - 21;
                MACRxPacket.Payload = &(RxBuffer[BankIndex].Payload[17]);
            }
#endif
        } break;

        case 0x08: //dest-short, source-none
        {
            if (RxBuffer[BankIndex].Payload[5] == 0xFF && RxBuffer[BankIndex].Payload[6] == 0xFF) {
                MACRxPacket.flags.bits.broadcast = 1;
            }
            MACRxPacket.PayloadLen = RxBuffer[BankIndex].PayloadLen - 10;
            MACRxPacket.Payload = &(RxBuffer[BankIndex].Payload[7]);
        } break;

        // all other addressing mode will not be supported in P2P
        default:
            // not valid addressing mode or no addressing info
            MiMAC_DiscardPacket();
            return false;
        }

#ifdef ENABLE_SECURITY
        if (RxBuffer[BankIndex].Payload[0] & 0x08) {
            API_UINT32_UNION FrameCounter;

            if (addrMode < 0xC0) {
                MiMAC_DiscardPacket();
                return false;
            }

            FrameCounter.v[0] = MACRxPacket.Payload[0];
            FrameCounter.v[1] = MACRxPacket.Payload[1];
            FrameCounter.v[2] = MACRxPacket.Payload[2];
            FrameCounter.v[3] = MACRxPacket.Payload[3];

            for (i = 0; i < CONNECTION_SIZE; i++) {
                if ((ConnectionTable[i].status.bits.isValid) && isSameAddress(ConnectionTable[i].Address, MACRxPacket.SourceAddress)) {
                    break;
                }
            }

            if (i < CONNECTION_SIZE) {
                if (IncomingFrameCounter[i].Val > FrameCounter.Val) {
                    MiMAC_DiscardPacket();
                    return false;
                }
                else {
                    IncomingFrameCounter[i].Val = FrameCounter.Val;
                }
            }

            MACRxPacket.PayloadLen -= 5; // used to 5 for frame counter now -4 also added for MIC integrity

            received_mic_values[0] = MACRxPacket.Payload[MACRxPacket.PayloadLen + 1];
            received_mic_values[1] = MACRxPacket.Payload[MACRxPacket.PayloadLen + 2];
            received_mic_values[2] = MACRxPacket.Payload[MACRxPacket.PayloadLen + 3];
            received_mic_values[3] = MACRxPacket.Payload[MACRxPacket.PayloadLen + 4];
            if (false == DataDecrypt(&(MACRxPacket.Payload[5]), &(MACRxPacket.PayloadLen), MACRxPacket.SourceAddress, FrameCounter, RxBuffer[BankIndex].Payload[0])) {
                MiMAC_DiscardPacket();
                return false;
            }

            // remove the security header from the payload
            MACRxPacket.Payload = &(MACRxPacket.Payload[5]);
            MACRxPacket.flags.bits.secEn = 1;
        }
#else
        if (RxBuffer[BankIndex].Payload[0] & 0x08) {
            MiMAC_DiscardPacket();
            return false;
        }
#endif

        // check the frame type. Only the data and command frame type
        // are supported. Acknowledgement frame type is handled in
        // AT96RF233 transceiver hardware.
        switch (RxBuffer[BankIndex].Payload[0] & 0x07) // check frame type
        {
        case 0x01: // data frame
            MACRxPacket.flags.bits.packetType = PACKET_TYPE_DATA;
            break;
        case 0x03: // command frame
            MACRxPacket.flags.bits.packetType = PACKET_TYPE_COMMAND;
            break;
        case 0x00:
            // use reserved packet type to represent beacon packet
            MACRxPacket.flags.bits.packetType = PACKET_TYPE_RESERVE;
            break;
        default: // not support frame type
            MiMAC_DiscardPacket();
            return false;
        }
#ifndef TARGET_SMALL
        MACRxPacket.LQIValue = Received_LQI_Val;
        MACRxPacket.RSSIValue = Received_RSSI_Val;
#endif

        return true;
    }
    return false;
}

/*************************************************************************/ /**
*****************************************************************************/
// Handle Packet Received // ISR calls this routine
void PHY_PacketReceieved(void)
{
    if (PHY_STATE_SLEEP == phyState) {
        return;
    }
    if (phyReadRegister(RG_IRQ_STATUS) & (1 << TRX_END)) {
        if (PHY_STATE_IDLE == phyState) {
            uint8_t size, i, RxBank = 0xFF;

            for (i = 0; i < BANK_SIZE; i++) {
                if (RxBuffer[i].PayloadLen == 0) {
                    RxBank = i;
                    break;
                }
            }
            Received_RSSI_Val = PHY_RSSI_BASE_VAL + (uint8_t)phyReadRegister(RG_PHY_ED_LEVEL);

            trx_frame_read(&size, 1);

            trx_frame_read(phyRxBuffer, size + 2 + 3); // +3 is ED, LQI and RX_STATUS bytes
            RxBuffer[RxBank].PayloadLen = size + 2;
            if (RxBuffer[RxBank].PayloadLen < RX_PACKET_SIZE) {
                //indicate that data is now stored in the buffer
                trx_status.bits.RX_BUFFERED = 1;

                //copy all of the data from the FIFO into the TxBuffer, plus RSSI and LQI
                for (i = 1; i <= size + 2; i++) {
                    RxBuffer[RxBank].Payload[i - 1] = phyRxBuffer[i];
                }
            }
            Received_LQI_Val = phyRxBuffer[i - 2];

            phyWaitState(TRX_STATUS_RX_AACK_ON);
        }
        else if (PHY_STATE_TX_WAIT_END == phyState) {
            uint8_t status
                = (phyReadRegister(RG_TRX_STATE) >> TRAC_STATUS) & 7;
            trx_status.bits.TX_FAIL = 0;
            trx_status.bits.TX_PENDING_ACK = 0;
            if (TRAC_STATUS_SUCCESS == status) {
                status = PHY_STATUS_SUCCESS;
            }
            else if (TRAC_STATUS_CHANNEL_ACCESS_FAILURE == status) {
                status = PHY_STATUS_CHANNEL_ACCESS_FAILURE;
            }
            else if (TRAC_STATUS_NO_ACK == status) {
                status = PHY_STATUS_NO_ACK;
                trx_status.bits.TX_FAIL = 1;
            }
            else {
                status = PHY_STATUS_ERROR;
            }

            phySetRxState();
            phyState = PHY_STATE_IDLE;
        }
    }
}