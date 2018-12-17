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

#ifndef __PROVISIONING_TASK_H__
#define __PROVISIONING_TASK_H__

#include "cryptoauthlib.h"
#include <stdio.h>

// AWS IoT ATECC508A Configuration
#define DEVICE_KEY_SLOT            (0)
#define METADATA_SLOT              (8)
#define SIGNER_CA_PUBLIC_KEY_SLOT  (15)


// ATECC508A Slot 8 Metadata
#define SLOT8_SIZE                (416)
#define SLOT8_SSID_SIZE           (32)
#define SLOT8_WIFI_PASSWORD_SIZE  (64)
#define SLOT8_HOSTNAME_SIZE       (128)

#define SLOT8_WIFI_PROVISIONED_VALUE   (0x72B0)  //! Value to determine if the ATECC508A is provisioned with wifi credentials
#define SLOT8_AWS_PROVISIONED_VALUE    (0xF309)  //! Value to determine if the ATECC508A is provisioned with AWS credentials

//ATECC508A Slot 8 Metadata structure
struct Ecc508A_Slot8_Metadata
{
	uint32_t provision_flag;             //! Flag to tell if the ATECC508A is provisioned
	uint32_t ssid_size;
	uint8_t  ssid[SLOT8_SSID_SIZE];
	uint32_t wifi_password_size;
	uint8_t  wifi_password[SLOT8_WIFI_PASSWORD_SIZE];
	uint32_t hostname_size;
	uint8_t  hostname[SLOT8_HOSTNAME_SIZE];
};



ATCA_STATUS cryptoauthlib_init(void);
ATCA_STATUS read_ecc_confiuration(void);
ATCA_STATUS genPublicKey(void);
ATCA_STATUS genKeyPair(void);
ATCA_STATUS genDeviceCSR(void);
#endif /*__LED_H__*/
