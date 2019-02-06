/**
 * \file
 *
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
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel
 * Support</a>
 */

#ifndef __GATEWAY_PROV_H__
#define __GATEWAY_PROV_H__


#include "cmdset.h"
#include "socket/include/m2m_socket_host_if.h"
#include "driver/include/m2m_wifi.h"

#define PADDING_LEN     0

typedef enum _prov_state{
	PROV_WAITING,
	PROV_INIT,
	PROV_CONFIG,
	PROV_DONE,
	STA_INIT,
	STA_BCAST,
	STA_FOUND
} prov_state_t;

typedef struct _cmd_resp{
	uint16_t length;
	union {
		// The max length of cmd response: cluster length + header + checksum
		uint8_t base[MIN_CMD_LEN + PADDING_LEN];
		uint8_t discovery[DISCOVERY_RESP_PARAM_LEN + DISCOVERY_RESP_HDR_LEN + CHECKSUM_LEN + PADDING_LEN];
		uint8_t provision[PROV_RESP_PARAM_LEN + PROV_RESP_HDR_LEN + CHECKSUM_LEN + PADDING_LEN];
		uint8_t authpacket[AUTHREQ_RESP_PARAM_LEN + AUTHREQ_RESP_HDR_LEN  + CHECKSUM_LEN + PADDING_LEN];
		uint8_t rand_chal_resp[RANCHAL_RESP_PARAM_LEN + RANCHAL_RESP_HDR_LEN + CHECKSUM_LEN + PADDING_LEN];
		
	} data;
} cmd_resp_t;




prov_state_t get_prov_state(void);
void set_prov_state(prov_state_t state);
void parse_iot_cmd(SOCKET sock, uint8_t cmd, uint8_t *cmd_param, cmd_resp_t *cmd_resp);



#endif /*__WIFI_APP_H__*/
