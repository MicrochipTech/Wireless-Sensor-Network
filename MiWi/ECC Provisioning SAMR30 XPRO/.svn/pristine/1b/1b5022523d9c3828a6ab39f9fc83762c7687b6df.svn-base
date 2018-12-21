/**
 * \file
 *
 * \brief IOT spec command set header file.
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
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

#ifndef CMDSET_H_INCLUDED
#define CMDSET_H_INCLUDED

#include "asf.h"

#ifdef __cplusplus
extern "C" {
#endif

// Macro defines to get 32/16/8 bit value from uint8_t pointer
#if 1 // Little-Endian
#define _get_u32(addr) \
		(uint32_t)((*((uint8_t *)addr)) | ((*((uint8_t *)addr + 1)) << 8) | \
		((*((uint8_t *)addr + 2)) << 16) | ((*((uint8_t *)addr + 3)) << 24))
#define _get_u16(addr) \
		(uint16_t)((*((uint8_t *)addr)) | ((*((uint8_t *)addr + 1)) << 8))
#define _get_u8(addr) \
		(uint8_t)(*((uint8_t *)addr))
#else // Big-Endian
#define _get_u32(addr) \
		(uint32_t)(((*((uint8_t *)addr)) << 24) | ((*((uint8_t *)addr + 1)) << 16) | \
		((*((uint8_t *)addr + 2)) << 8) | (*((uint8_t *)addr + 3)))
#define _get_u16(addr) \
		(uint16_t)(((*((uint8_t *)addr)) << 8) | (*((uint8_t *)addr + 1)))
#define _get_u8(addr) \
		(uint8_t)(*((uint8_t *)addr))
#endif // Endian

/** \name Encrypted command length definition. */
/* \{ */
#define ENC_SOF_INDICATER              0x5B /**< Encrypted SOF string. */
#define ENC_SOF_LEN                    1
#define ENC_CMDLNTH_LEN                2
#define ENC_FRAME_HDR_LEN              (ENC_SOF_LEN + ENC_CMDLNTH_LEN)
#define ENC_IV_LEN                     16
#define ENC_PUBKEY_LEN                 64
#define ENC_FRAME_TAIL_LEN             (ENC_IV_LEN + ENC_PUBKEY_LEN)
#define ENC_CRC_LEN                    4
/*@}*/

/** \name Command length definition. */
/* \{ */
// Command header
#define SOF_LEN                        1    /**< Length of SOF. */
#define CMDLNTH_LEN                    2    /**< Length of frame length. */
#define CMDSEQ_LEN                     1    /**< Length of command sequence number. */
#define CMDID_LEN                      1    /**< Length of command ID. */
#define CMDADDR_LEN                    2    /**< Length of target address. */
#define CMDEPPN_LEN                    2    /**< Length of target endpoint/port. */
#define CMDCHKSUM_LEN                  4    /**< Length of frame checksum. */

#define MIN_CMD_LEN                    (SOF_LEN + CMDLNTH_LEN + CMDSEQ_LEN + \
										CMDID_LEN + CMDADDR_LEN + CMDEPPN_LEN)

#define SOF_INDICATER                  0x5A /**< SOF string. */

#define SOF_OFFSET                     0
#define CMD_LEN_OFFSET                 (SOF_LEN) /**< Frame length offset from SOF. */
#define CMD_SEQ_OFFSET                 (SOF_LEN + CMDLNTH_LEN) /**< Command seq# offset from SOF. */
#define CMD_ID_OFFSET                  (SOF_LEN + CMDLNTH_LEN + CMDSEQ_LEN) /**< Command ID offset from SOF. */
#define CMD_ADDR_OFFSET                (SOF_LEN + CMDLNTH_LEN + CMDSEQ_LEN + CMDID_LEN)
#define CMD_EPPN_OFFSET                (SOF_LEN + CMDLNTH_LEN + CMDSEQ_LEN + CMDID_LEN + CMDADDR_LEN)
#define CMD_PARAM_OFFSET               (SOF_LEN + CMDLNTH_LEN + CMDSEQ_LEN + CMDID_LEN + CMDADDR_LEN + CMDEPPN_LEN) /**< Command parameter offset from SOF. */
#define CMD_HDR_LEN                    (CMD_PARAM_OFFSET) /**< Command header length. */
// Command parameter
#define PARAM_CID_LEN                  2    /**< Length of cluster ID. */
#define PARAM_CINDEX_LEN               1    /**< Length of cluster index. */
#define PARAM_ATTRID_LEN               1    /**< Length of attribute ID. */

#define PARAM_CID_OFFSET               0    /**< Cluster ID offset from cmd parameter. */
#define PARAM_CINDEX_OFFSET            (PARAM_CID_LEN) /**< Cluster index offset from cmd parameter. */
#define PARAM_CLST_VALUE_OFFSET        (PARAM_CID_LEN + PARAM_CINDEX_LEN) /**< Cluster value offset from cmd parameter. */

#define PARAM_ATTRID_OFFSET            (PARAM_CID_LEN + PARAM_CINDEX_LEN) /**< Attribute index offset from cmd parameter. */
#define PARAM_ATTR_VALUE_OFFSET        (PARAM_CID_LEN + PARAM_CINDEX_LEN + PARAM_ATTRID_LEN) /**< Attribute value from cmd parameter. */

#define PARAM_CLST_HDR_LEN             PARAM_CLST_VALUE_OFFSET /**< Cmd parameter header length for report cluster cmd. */
#define PARAM_ATTR_HDR_LEN             PARAM_ATTR_VALUE_OFFSET /**< Cmd parameter header length for report attribute cmd. */

#define REPORT_CLST_HDR_LEN            (CMD_HDR_LEN + PARAM_CLST_HDR_LEN) /**< Length of report cluster header. */
#define REPORT_ATTR_HDR_LEN            (CMD_HDR_LEN + PARAM_ATTR_HDR_LEN) /**< Length of report attribute header. */
/*@}*/

/**
 * \name Authentication command
 * WiFi node authentication command  length definition.
 * @{
 */
// Authentication command
#define AUTHREQ_PARAM                  "AUTHREQ"
#define AUTHREQ_PARAM_LEN              7

#define RANCHAL_HDR_LEN                CMD_HDR_LEN
#define RANCHAL_PARAM_HDR              "RAMCHAL"
#define RANCHAL_PARAM_HDR_LEN          7
#define RANCHAL_PARAM_OFFSET           RANCHAL_PARAM_HDR_LEN
#define RANCHAL_PARAM_LEN              (RANCHAL_PARAM_HDR_LEN + 32) // PARAM_HDR_LEN + random number 32B

#define RANCMAC_HDR_LEN                CMD_HDR_LEN
#define RANCMAC_PARAM_HDR              "RNCMAC"
#define RANCMAC_PARAM_HDR_LEN          6
#define RANCMAC_PARAM_OFFSET           RANCMAC_PARAM_HDR_LEN
#define RANCMAC_PARAM_LEN              (RANCMAC_PARAM_HDR_LEN + 32 + 8) // PARAM_HDR_LEN + random number 32B + MAC addr 8B

#define PUBKEY_PARAM_HDR               "PUBKEY"
#define PUBKEY_PARAM_HDR_LEN           6
#define PUBKEY_PARAM_LEN               (PUBKEY_PARAM_HDR_LEN + 64) // PARAM_HDR_LEN + random number 64B

#define RAMSHARE_PARAM_HDR             "RAMSHARE"
#define RAMSHARE_PARAM_HDR_LEN         8
#define RAMSHARE_PARAM_LEN             (RAMSHARE_PARAM_HDR_LEN + 32) // PARAM_HDR_LEN + random number 32B

#define ACCREQ_PARAM_HDR               "ACCREQ"
#define ACCREQ_PARAM_HDR_LEN           6
#define ACCREQ_PARAM_LEN               (ACCREQ_PARAM_HDR_LEN + 32 + 64 + 32 + 64) 
                                       // PARAM_HDR_LEN + random number 32B + signature 64B + 
									   // new random 32B + new pub key 64B

#define DELSHARE_PARAM_HDR             "DELSHARE"
#define DELSHARE_PARAM_HDR_LEN         8
#define DELSHARE_PARAM_LEN             (DELSHARE_PARAM_HDR_LEN + 32) // PARAM_HDR_LEN + random number 32B
// Authentication response
#define AUTHREQ_RESP_HDR_LEN           CMD_HDR_LEN
#define AUTHREQ_RESP_PARAM_HDR         "AUTHREQ="
#define AUTHREQ_RESP_PARAM_HDR_LEN     8
#define AUTHREQ_RESP_PARAM_OFFSET      AUTHREQ_RESP_PARAM_HDR_LEN
#define AUTHREQ_RESP_PARAM_LEN         (AUTHREQ_RESP_PARAM_HDR_LEN + 64*4) // PARAM_HDR + KEY&SIG(64B * 4)

#define RANCHAL_RESP_HDR_LEN           CMD_HDR_LEN
#define RANCHAL_RESP_PARAM_HDR         "RAMCHAL="
#define RANCHAL_RESP_PARAM_HDR_LEN     8
#define RANCHAL_RESP_PARAM_OFFSET      RANCHAL_RESP_PARAM_HDR_LEN
#define RANCHAL_RESP_PARAM_LEN         (RANCHAL_RESP_PARAM_HDR_LEN + 64) // PARAM_HDR + random challenge resp 64B

#define RANCRID_RESP_HDR_LEN           CMD_HDR_LEN
#define RANCRID_RESP_PARAM_HDR         "RANCRID="
#define RANCRID_RESP_PARAM_HDR_LEN     8
#define RANCRID_RESP_PARAM_OFFSET      RANCRID_RESP_PARAM_HDR_LEN
#define RANCRID_RESP_PARAM_LEN         (RANCRID_RESP_PARAM_HDR_LEN + 64 + 32) 
                                       // PARAM_HDR + random challenge resp 64B + host random ID 32B
// Node auth response
#define AUTH_ACK_HDR_LEN              CMD_HDR_LEN
#define AUTH_ACK_PARAM                "+ok\r\n\r\n"
#define AUTH_ACK_PARAM_LEN            7
#define AUTH_ACK_PARAM_OFFSET         0
// Node send random challenge
#define RANCHAL

/*@}*/

/**
 * \name WiFi network entry command
 * WiFi network entry command length definition.
 * @{
 */
// WiFi Discovery command
#define DISCOVERY_PARAM                "Atmel_WiFi_Discovery"
#define DISCOVERY_PARAM_LEN            20
// WiFi Discovery response
#define DISCOVERY_RESP_HDR_LEN         CMD_HDR_LEN
#define DISCOVERY_RESP_PARAM_LEN       10 // product type ID (2 byte) + mac addr (8 byte)
#define DISCOVERY_RESP_PARAM_OFFSET    0
// Provision:CONFIG='SSID length'SSID'password length'password'securitytype'"
#define PROVCONF_PARAM_HDR             "CONFIG="
#define PROVCONF_PARAM_HDR_LEN         7
#define PROVDONE_PARAM_HDR             "CONDONE"
#define PROVDONE_PARAM_HDR_LEN         7
// Provision:AT+CONFIG='SSID length'SSID'password length'password'securitytype'"
//#define PROVCONF_PARAM_HDR             "AT+CONFIG="
//#define PROVCONF_PARAM_HDR_LEN         10
//#define PROVDONE_PARAM_HDR             "AT+Z"
//#define PROVDONE_PARAM_HDR_LEN         4

#define PROV_SSID_OFFSET               (PROVCONF_PARAM_HDR_LEN + 1) // 'SSID length'
// Provision response
#define PROV_RESP_HDR_LEN              CMD_HDR_LEN
#define PROV_RESP_PARAM                "+ok\r\n\r\n"
#define PROV_RESP_PARAM_LEN            7
#define PROV_RESP_PARAM_OFFSET         0
/*@}*/

/**
 * \name OTAU state command
 * OTAU state command length definition.
 * @{
 */
// OTAU host MCU
#define OTAU_HOST_HDR                  "OTA_HOST"
#define OTAU_HOST_HDR_LEN              8
// Host MCU image CRC
#define CRC_HOST_HDR                   "CRC_HOST"
#define CRC_HOST_HDR_LEN               8
// OTAU WiFi module
#define OTAU_WIFI_HDR                  "OTA_WIFI"
#define OTAU_WIFI_HDR_LEN              8
// Switching to new firmware
#define RUN_NEWF_HDR                   "RUN_NEWF"
#define RUN_NEWF_HDR_LEN               8
/*@}*/

/**
 * \name OTAU data frame
 * OTAU data frame length definition.
 * @{
 */
// OTAU host MCU data frame length
#define OTAU_DATA_FRAME_MAX_LEN        (APP_PAGE_SIZE * 4)
// 128K flash divided into OTAU sequences with 4 pages in each sequence
#define OTAU_DATA_MAX_SEQ              (APPLICATION_SIZE / (APP_PAGE_SIZE * 4)) 
// Sequence number length
#define OTAU_DATA_SEQ_NUM_LEN          2
/*@}*/

/**
 * \name OTAU response
 * Define OTAU response to state command and data frame.
 * @{
 */
// OTAU response
#define OTAU_RESP_HDR_LEN              CMD_HDR_LEN
#define OTAU_RESP_OK_PARAM             "+ok\r\n\r\n"
#define OTAU_RESP_PARAM_LEN            7
#define OTAU_RESP_PARAM_OFFSET         0
/*@}*/

/**
 * \name ERROR code 
 * ERROR code command length definition.
 * @{
 */
// Error code
#define REPORT_ERR_HDR_LEN             CMD_HDR_LEN
#define ERR_CODE_LEN                   1
#define ERR_CODE_OFFSET                0
/*@}*/

#define CHECKSUM_LEN                   4    /**< CRC32 checksum length. */

/**
 * \name Authentication command set definition
 * IOT spec Wi-Fi node authentication/security command set definition.
 * @{
 */
#define WIFI_NODE_AUTH                 0x02

#define WIFI_NODE_AUTH_RESP            0x82
/*@}*/

/**
 * \name Node discovery command set definition
 * IOT spec node discovery command set definition.Wi-Fi is used by default.
 * @{
 */
#define WIFI_NODE_DISCOVER             0x01
#define ZIGBEE_COMMISSION              0x02
#define SEND_NODE_INFO                 0x03   /**< phone app collect node info and send it to gateway. */

#define WIFI_NODE_DISC_RESP            0x81
#define ZIGBEE_COMMISSION_RESP         0x82
#define SEND_NODE_INFO_RESP            0x83   /**< Gateway responds to node info from phone app. */
/*@}*/

/**
 * \name Query command set definition
 * IOT spec query command set definition.
 * @{
 */
#define QUERY_ALL                      0x10
#define QUERY_ATTR                     0x11
#define QUERY_CLUSTER                  0x12

#define QUERY_ALL_PARAM                0xAA55
/*@}*/

/**
 * \name Control command set definition
 * IOT spec control command set definition.
 * @{
 */
#define FACTORY_RESET                  0x20
#define CONTROL_ATTR                   0x21
#define CONTROL_CLUSTER                0x22
#define TUNNELING_ZIGBEE               0x24
#define OTAU_DATA_FRAME                0x28
#define OTAU_STATE_CMD                 0x29

#define FACTORY_RESET_PARAM            0xAA55
/*@}*/

/**
 * \name Report command set definition
 * IOT spec report command set definition.
 * @{
 */
#define REPORT_ALL                     0x90
#define REPORT_ATTR                    0x91
#define REPORT_CLUSTER                 0x92
#define OTAU_DATA_FRAME_RESP           0xA8
#define OTAU_STATE_CMD_RESP            0xA9
#define REPORT_ERROR                   0x9C
/*@}*/

/**
 * \name Attribute ID definition
 * IOT spec attribute id definition.
 * @{
 */
#define CLUSTER_ONLY                   0x00
#define ATTR_INDEX1                    0x01
#define ATTR_INDEX2                    (ATTR_INDEX1 + 1)
#define ATTR_INDEX3                    (ATTR_INDEX2 + 1)
#define ATTR_INDEX4                    (ATTR_INDEX3 + 1)
#define ATTR_INDEX5                    (ATTR_INDEX4 + 1)
#define ATTR_INDEX6                    (ATTR_INDEX5 + 1)
#define ATTR_INDEX7                    (ATTR_INDEX6 + 1)
#define ATTR_INDEX8                    (ATTR_INDEX7 + 1)
#define ATTR_INDEX9                    (ATTR_INDEX8 + 1)
#define ATTR_INDEX10                   (ATTR_INDEX9 + 1)
#define ATTR_INDEX11                   (ATTR_INDEX10 + 1)
/*@}*/

/**
 * \name Error code definition
 * @{
 */
typedef enum iot_status_code {
	OPT_SUCCESS                    = 0x00,
	OPT_FAILURE                    = 0x01,
	PROV_BUSY_ERR                  = 0x50,
	AUTH_BUSY_ERR                  = 0x58,
	AUTH_SHARE_FAIL                = 0x59,
	OTAU_BUSY_ERR                  = 0x60,
	OTAU_SEQ_ERR                   = 0x61,
	OTAU_WRITE_ERR                 = 0x62,
	OTAU_VERIFY_ERR                = 0x63,
	OTAU_IMG_CRC_ERR               = 0x64,
	NOT_AUTHORIZED                 = 0x7E,
	UNSUPPORTED_CLUSTER            = 0x81,
	UNSUPPORTED_CMD                = 0x82,
	UNSUPPORTED_C_INDEX            = 0x85,
	UNSUPPORTED_ATTR               = 0x86,
	INVALID_VALUE                  = 0x87,
	READ_ONLY                      = 0x88,
	OPT_TIMEOUT                    = 0x94,
} iot_status_code_t;
/*@}*/

//COMPILER_PACK_SET(1)
/**
 * \name Frame header structure.
 * @{
 */
typedef struct cmd_header {
	uint8_t           cmd_sof;                                     /**< SOF */
	uint8_t           frame_len;                                   /**< Frame length */
	uint8_t           cmd_id;
	uint16_t          cmd_addr;
	uint16_t          cmd_port_num;
} cmd_header_t;
/*@}*/

/**
 * \name Parameter header of cluster/attribute operation.
 * @{
 */
typedef struct cmd_param_header {
	uint16_t          cluster_id;                                  /**< Cluster ID */
	uint8_t           cluster_index;                               /**< Cluster index */
	uint8_t           attr_id;                                     /**< Attribute ID */
} cmd_param_hdr_t;
/*@}*/
//COMPILER_PACK_RESET()

#ifdef __cplusplus
}
#endif

#endif /* CMDSET_H_INCLUDED */
