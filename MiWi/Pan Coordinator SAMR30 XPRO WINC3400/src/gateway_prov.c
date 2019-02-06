/**
* \file
*
* \brief Multi-Role/Multi-Connect Application
*
* Copyright (c) 2016 Atmel Corporation. All rights reserved.
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
*    Atmel micro controller product.
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
* Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel
*Support</a>
*/

/**
* \mainpage
* \section preface Preface
* This is the reference manual for the Multi-Role/Multi-Connect Application
*/
/*- Includes ---------------------------------------------------------------*/


//#include "wifi_app.h"
#include "gateway_prov.h"
#include "string.h"
#include "main.h"


static prov_state_t prov_state = PROV_WAITING;

extern uint8_t gMacaddr[M2M_MAC_ADDRES_LEN];
extern uint8 gDefaultSSID[M2M_MAX_SSID_LEN];
extern uint8 gAuthType;
extern uint8 gDefaultKey[M2M_MAX_PSK_LEN];
extern uint8 gUuid[AWS_COGNITO_UUID_LEN];

#define _FILL_IOT_HEADER_\
		uint16_t param_len = PARAM_LEN;\
		send_buffer[SOF_OFFSET] = SOF_INDICATER;\
		send_buffer[CMD_LEN_OFFSET] = (HEADER_LEN + param_len - SOF_LEN - CMDLNTH_LEN) & 0xFF;\
		send_buffer[CMD_LEN_OFFSET + 1] = (((HEADER_LEN + param_len - SOF_LEN - CMDLNTH_LEN) & 0xFF00) >> 8);\
		send_buffer[CMD_ID_OFFSET] = CMD_ID;\
		send_buffer[CMD_ADDR_OFFSET] = 0x00;\
		send_buffer[CMD_ADDR_OFFSET + 1] = 0x00;\
		send_buffer[CMD_EPPN_OFFSET] = 0x00;\
		send_buffer[CMD_EPPN_OFFSET + 1] = 0x00;

#define _FILL_IOT_PARAM_HDR_\
		send_buffer[CMD_HDR_LEN + PARAM_CID_OFFSET] = (uint8_t)(cluster_id & 0xFF);\
		send_buffer[CMD_HDR_LEN + PARAM_CID_OFFSET + 1] = (uint8_t)((cluster_id & 0xFF00) >> 8);\
		send_buffer[CMD_HDR_LEN + PARAM_CINDEX_OFFSET] = cluster_index;\
		if(attr_id != CLUSTER_ONLY){ \
			send_buffer[CMD_HDR_LEN + PARAM_ATTRID_OFFSET] = attr_id; \
		}

#define _FILL_IOT_FRAME_LENGTH_\
		cmd_resp->length = HEADER_LEN + param_len;

#define MAC_ADDR_C_ATTR1_LEN	6



static void wifi_discovery_resp(cmd_resp_t *cmd_resp)
{
	#ifdef PARAM_LEN
	#undef PARAM_LEN
	#endif
	#ifdef HEADER_LEN
	#undef HEADER_LEN
	#endif
	#ifdef CMD_ID
	#undef CMD_ID
	#endif
	#define PARAM_LEN   DISCOVERY_RESP_PARAM_LEN
	#define HEADER_LEN  DISCOVERY_RESP_HDR_LEN
	#define CMD_ID      WIFI_NODE_DISC_RESP
	uint8_t *send_buffer = (uint8_t *)&cmd_resp->data;
	_FILL_IOT_HEADER_
	memcpy(&send_buffer[HEADER_LEN + DISCOVERY_RESP_PARAM_OFFSET], \
	gMacaddr, MAC_ADDR_C_ATTR1_LEN);
	_FILL_IOT_FRAME_LENGTH_
}

static void wifi_provision_resp(cmd_resp_t *cmd_resp)
{
	#ifdef PARAM_LEN
	#undef PARAM_LEN
	#endif
	#ifdef HEADER_LEN
	#undef HEADER_LEN
	#endif
	#ifdef CMD_ID
	#undef CMD_ID
	#endif
	#define PARAM_LEN   PROV_RESP_PARAM_LEN
	#define HEADER_LEN  PROV_RESP_HDR_LEN
	#define CMD_ID      WIFI_NODE_DISC_RESP
	uint8_t *send_buffer = (uint8_t *)&cmd_resp->data;
	_FILL_IOT_HEADER_
	memcpy(&send_buffer[HEADER_LEN + PROV_RESP_PARAM_OFFSET], \
	PROV_RESP_PARAM, PROV_RESP_PARAM_LEN);
	_FILL_IOT_FRAME_LENGTH_
}


void set_prov_state(prov_state_t state)
{
	prov_state = state;
}

prov_state_t get_prov_state(void)
{
	return prov_state;
}


void parse_iot_cmd(SOCKET sock, uint8_t cmd, uint8_t *cmd_param, cmd_resp_t *cmd_resp)
{
	iot_status_code_t cmd_status_code = OPT_SUCCESS;
	switch (cmd)
	{
		
		case WIFI_NODE_DISCOVER:
		{
			printf("WIFI_NODE_DISCOVER receive\r\n");
			if (memcmp(cmd_param, DISCOVERY_PARAM, DISCOVERY_PARAM_LEN) == 0) {
					printf("WIFI_NODE_DISCOVER DISC PARAM receive\r\n");
					// provision not done, in AP mode
					if (prov_state == PROV_WAITING || prov_state == PROV_INIT) {
						prov_state = PROV_INIT;
						printf("WIFI_NODE_DISCOVER DISC PARAM log2\r\n");
						wifi_discovery_resp(cmd_resp);
					}
					
			}
			else if (memcmp(cmd_param, PROVCONF_PARAM_HDR, PROVCONF_PARAM_HDR_LEN) == 0) {
				printf("WIFI_NODE_DISCOVERY PROVCONF PARAM receive\r\n");
				if (prov_state == PROV_INIT) {
					uint8_t ssidlength = *(cmd_param + PROVCONF_PARAM_HDR_LEN);
					memcpy((char *)gDefaultSSID, (char *)(cmd_param + PROV_SSID_OFFSET), ssidlength);
					gDefaultSSID[ssidlength] = 0; // last byte '/n'
					uint8_t pswlength = *(cmd_param + PROV_SSID_OFFSET + ssidlength);
					uint8_t pswoffset = PROV_SSID_OFFSET + ssidlength + sizeof(pswlength);
					memcpy((char *)gDefaultKey, (char *)(cmd_param + pswoffset), pswlength);
					gDefaultKey[pswlength] = 0; // last byte '/n'
					uint8_t authtype = *(cmd_param + PROV_SSID_OFFSET + ssidlength + 1 + pswlength);
					uint8_t uuidlength = *(cmd_param + PROV_SSID_OFFSET + ssidlength + 1 + pswlength + 1);
					memcpy((char *)gUuid, (char *)(cmd_param + PROV_SSID_OFFSET + ssidlength + 1 + pswlength + 1 + 1), uuidlength);
					gUuid[uuidlength] = 0;
					
					printf("gDefaultSSID=%s\r\n", gDefaultSSID);
					printf("gDefaultKey=%s\r\n", gDefaultKey);
					printf("authtype=%d\r\n", authtype);
					printf("gUuid=%s\r\n", gUuid);
					if (((authtype <= M2M_WIFI_SEC_802_1X && authtype > M2M_WIFI_SEC_OPEN && pswlength) ||
					(authtype == M2M_WIFI_SEC_OPEN && pswlength == 0)) && ssidlength){
						gAuthType = authtype;
						printf("wifi_provision_resp\r\n");
						wifi_provision_resp(cmd_resp);
						prov_state = PROV_CONFIG;
					}
					else {
						// invalid wifi auth type / ssid length / psw length
						prov_state = PROV_INIT;
						cmd_status_code = INVALID_VALUE;
					}
				}
				else {
					prov_state = PROV_INIT;
					cmd_status_code = PROV_BUSY_ERR;
				}
			}
			else if (memcmp(cmd_param, PROVDONE_PARAM_HDR, PROVDONE_PARAM_HDR_LEN) == 0) {
				if (prov_state == PROV_CONFIG) {
					wifi_provision_resp(cmd_resp);
					prov_state = PROV_DONE;
					printf("prov_state = PROV_DONE\r\n");
				}
				else {
					prov_state = PROV_INIT;
					cmd_status_code = PROV_BUSY_ERR;
				}
			}
			else {
				prov_state = PROV_INIT;
				cmd_status_code = INVALID_VALUE;
			}
		}
		break;
	}
	return;
}
