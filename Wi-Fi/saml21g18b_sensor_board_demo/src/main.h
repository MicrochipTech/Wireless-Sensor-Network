/**
 * \file version.h
 *
 * \brief gesture file
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
#include "driver/include/m2m_wifi.h"

#ifndef _MAIN_H_
#define _MAIN_H_

#ifdef DEBUG_SUPPORT
#define DBG_LOG		printf("\r\n");\
printf
#else
#define DBG_LOG		ALL_UNUSED
#endif


#define FIRMWARE_MAJOR_VER	"1"
#define FIRMWARE_MINOR_VER	"2"


/** Wi-Fi Settings */
#define MAIN_WLAN_SSID                    "MASTERS" /**< Destination SSID */
#define MAIN_WLAN_AUTH                    M2M_WIFI_SEC_WPA_PSK /**< Security manner */
#define MAIN_WLAN_PSK                     "microchip" /**< Password for Destination SSID */
#define MAIN_WLAN_CHANNEL				(6)
#define MAIN_WIFI_M2M_PRODUCT_NAME        "NMCTemp"
#define MAIN_WIFI_M2M_SERVER_IP           0xc0a80164 //0xFFFFFFFF /* 255.255.255.255 */
#define MAIN_WIFI_M2M_SERVER_PORT         (6666)
#define MAIN_WIFI_M2M_REPORT_INTERVAL     (1000)

#define MAIN_WIFI_M2M_BUFFER_SIZE          1460


#define DEVICE_NAME		"Node17"
#define DEVICE_TYPE		"wifiSensorBoard"

#define AWS_COGNITO_UUID_LEN	60


typedef struct _wifi_nvm_data {
	uint8_t valid;
	uint8_t ssid[M2M_MAX_SSID_LEN];
	uint8_t ssidlen;
	uint8_t key[M2M_MAX_PSK_LEN];
	uint8_t keylen;
	uint8_t authtype;
	uint8_t uuid[AWS_COGNITO_UUID_LEN];
} wifi_nvm_data_t;

extern void MQTTSubscribeCBCallbackHandler(int topic_len, char* topic_name, int payload_len, char* payload);

#endif // _MAIN_H_
