/**
 * \file
 *
 * \brief WiFi Provisioning Declarations
 *
 * Copyright (c) 2017 Atmel Corporation. All rights reserved.
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

#ifndef __WIFIPROV_PROFI_H__
#define __WIFIPROV_PROFI_H__

#include <asf.h>
#include "atmel_ble_api/include/wifiprov_api.h"
#include "driver/include/m2m_wifi.h"

#define	CREDENTIALS_NOT_VALID			0
#define CREDENTIALS_VALID				1

// states of WiFi provision state machine here
#define WIFI_PROV_IDLE					0xFF
#define WIFI_PROV_SUCCESS				0x00	// Should be same as what is in at_ble_wifiprov_complete_ind

//status to be returned to application
#define PROVISION_NULL					0x00
#define PROVISION_SUCCESS				0x10
#define PROVISION_FAILED				0x20

#define BLE_PROV_STATE_IDLE				0x00
#define BLE_PROV_STATE_IN_PROGRESS		0x01
#define BLE_PROV_STATE_SUCCESS			0x04
#define BLE_PROV_STATE_FAILED			0x80

typedef struct
{
	uint8_t sec_type;
	uint8_t ssid_length;
	uint8_t ssid[MAX_WIPROVTASK_SSID_LENGTH];
	uint8_t passphrase_length;
	uint8_t passphrase[MAX_WIPROVTASK_PASS_LENGTH];
} credentials;

//void ble_prov_process_event();
uint8_t ble_prov_start(void);
void ble_prov_stop(void);
uint8_t ble_prov_get_credentials(credentials *cred);
//Allows setting of localname in scan response
//Maximum length is 11 characters
void ble_prov_init(uint8_t* localname);
void ble_prov_scan_result(tstrM2mWifiscanResult* pstrScanResult, uint8_t resultsRemaining);
uint8_t ble_prov_get_provision_state(void);
#define ble_prov_wifi_con_update		wifiprov_wifi_con_update
void ble_prov_process_event(at_ble_events_t event, void* params);

#endif /* __WIFIPROV_PROFI_H__ */