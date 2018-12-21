/**
 *
 * \file
 *
 * \brief CryptoAuth driver.
 *
 * Copyright (c) 2014 Atmel Corporation. All rights reserved.
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



#ifndef ECC508A_H_INCLUDED
#define ECC508A_H_INCLUDED

#include "conf_ecc508a.h"
#include "atca_comm.h"

// Slot definitions 
#define DEMO_SIGMOD_PRIV_SLOT	(uint8_t)(0x00)
#define DEMO_AUTH_PRIV_SLOT		(uint8_t)(0x00)
#define DEMO_ENCREAD_PARENT_SLOT (uint8_t)(0x01)
#define DEMO_ECDH_PRIV_SLOT		(uint8_t)(0x02)
#define DEMO_ECDH_PM_KEY_SLOT	(uint8_t)(0x03)
#define DEMO_ECDH_KEY1_SLOT		(uint8_t)(0x04)
#define DEMO_ECDH_KEY2_SLOT		(uint8_t)(0x05)
#define DEMO_SIGNER_PRIV_SLOT	(uint8_t)(0x06)
#define DEMO_ISSUER_PRIV_SLOT	(uint8_t)(0x07)
#define DEMO_FLASH_MEM416_SLOT	(uint8_t)(0x08)
#define DEMO_SUBJECT_PUBKEY_SLOT	(uint8_t)(0x09)
#define DEMO_SUBJECT_SIG_SLOT	(uint8_t)(0x0A)
#define DEMO_AUTHORITY_PUBKEY_SLOT	(uint8_t)(0x0B)
#define DEMO_AUTHORITY_SIG_SLOT	(uint8_t)(0x0C)
#define DEMO_ECDH_PUBKEY_SLOT	(uint8_t)(0x0D)
#define DEMO_ECDH_SIG_SLOT		(uint8_t)(0x0E)
#define DEMO_FLASH_MEM72_SLOT	(uint8_t)(0x0F)

#ifdef __cplusplus
extern "C" {
#endif

//#define CRYPT_DEBUG

#ifdef CRYPT_DEBUG
#define CRYPT_INFO(...)       do{puts(__VA_ARGS__);}while(0)
#define CRYPT_SPRINT(...)     do{sprintf(__VA_ARGS__);}while(0)
#else
#define CRYPT_INFO(...)       
#define CRYPT_SPRINT(...)     
#endif

/** Authentication packet definitions. */
typedef struct _auth_packet {
	uint8_t pub_d[ATCA_PUB_KEY_SIZE];
	uint8_t sig_d[ATCA_SIG_SIZE];
	uint8_t pub_s[ATCA_PUB_KEY_SIZE];
	uint8_t sig_s[ATCA_SIG_SIZE];
} auth_packet_t;

typedef struct _encrypt_packet {
	uint16_t data_len;
	uint8_t *data;
	uint16_t en_data_len;
	uint8_t *en_data;
	uint8_t iv[16];
	uint8_t pub_x[ATCA_PUB_KEY_SIZE];
} encrypt_packet_t;


// Function prototypes - CryptoAuth entry points
void configure_ecc508a(void);

uint8_t cademo_set_auth_packet(auth_packet_t *packet);
uint8_t cademo_set_host_ecdh(uint8_t* host_pub_key);
uint8_t cademo_set_host_pub_key(uint8_t* host_pub_key);

uint8_t cademo_sign_host_rand(uint8_t *challenge_sig, uint8_t *host_random);
uint8_t cademo_set_host_sig(uint8_t* auth_sig);
uint8_t cademo_generate_node_rand(uint8_t *rand_num);
uint8_t cademo_set_sessionn_random(uint8_t* rand_num);
uint8_t cademo_verify_challenge_resp(uint8_t* verified);
uint8_t cademo_enc_frame(encrypt_packet_t* msg_enc_report, uint8_t session_num);
uint8_t cademo_dec_frame(encrypt_packet_t* msg_enc_report, uint8_t session_num);

#ifdef __cplusplus
}
#endif

#endif /* ECC508A_H_INCLUDED */