/** \file
 *  \brief  common CryptAuth commands
 *  \author Atmel Crypto Products
 *
 * \copyright Copyright (c) 2014 Atmel Corporation. All rights reserved.
 *
 * \atmel_crypto_device_library_license_start
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
 *    Atmel integrated circuit.
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
 * \atmel_crypto_device_library_license_stop
 */

#ifndef ATCA_CMDS_H_
#define ATCA_CMDS_H_

#include <stdint.h> 
#include <string.h>
#include <atca_lib_return_codes.h>
#include <atca_protocol_adapter.h>
#include <atca_comm.h>
#include <atca_crypto_utils.h>

uint8_t atca_get_random_number(uint8_t *rand_out);
uint8_t atca_read_serial_number(uint8_t* serial_number);
uint8_t atca_pad_serial( uint8_t *oResponse);
uint8_t atca_derive_key_cmd(uint8_t mode, uint16_t key_slot);
uint8_t atca_send_mac(uint8_t key_id, uint8_t mode, uint8_t* challenge, uint8_t* mac_response);
uint8_t atca_send_nonce(uint8_t mode, uint8_t* numin, uint8_t* rand_out);
uint8_t atca_send_gendig(uint8_t zone, uint8_t key_id, uint8_t *other_data);
uint8_t atca_send_checkmac(uint8_t zone, uint8_t key_id, uint8_t *client_challenge, uint8_t *client_response, uint8_t *other_data);
uint8_t atca_send_info_command(void);
uint8_t atca_send_privWrite(uint8_t zone, uint8_t slot, uint8_t *priv, uint8_t* mac);

uint8_t atca_send_ecdh(uint16_t key_id, uint8_t* pub_key, uint8_t* ret_ecdh);
uint8_t atca_send_verify_extern(uint8_t* pub_key, uint8_t* message, uint8_t* signature, uint8_t* verified);
uint8_t atca_send_counter(uint8_t mode, uint16_t counter_id, uint8_t* count);
uint8_t atca_send_genkey(uint8_t mode, uint16_t key_id, uint8_t* created_key);
uint8_t atca_send_sha(uint8_t mode, uint16_t length, uint8_t* message, uint8_t* digest_out);

uint8_t atca_info( uint8_t *oResponse );
uint8_t atca_check_response_status(uint8_t ret_code, uint8_t *oResponse);
uint8_t atca_check_lock_status(int zone);

uint8_t atca_lock_config_zone(void);
uint8_t atca_lock_ecc508_config_zone(void);
uint8_t atca_lock_data_zone(void);
uint8_t atca_lock_data_slot(uint8_t slot);
uint8_t atca_is_locked(uint8_t zone);
uint8_t atca_check_private_key_slot0_config(void);
uint8_t atca_check_private_key_slot0_config_283(void);
uint8_t atca_checkmac_device(void);
uint8_t atca_checkmac_firmware(void);
uint8_t atca_read_encrytped(uint8_t slot, uint8_t block, uint8_t read_key_slot, 
                            const uint8_t read_key[32], const uint8_t num_in[20], uint8_t *data);
uint8_t atca_write_encrytped(uint8_t slot, uint8_t block, const uint8_t plain_text[32], 
                                uint8_t write_key_slot, const uint8_t write_key[32], const uint8_t num_in[20]);
uint8_t atca_verify_external(void);
uint8_t atca_verify_external_283(void);
uint8_t atca_verify_host(void);
uint8_t atca_verify_host_283(void);
uint8_t atca_config_i2c_address(uint8_t i2c_address);
uint8_t atca_read_config_zone(uint8_t* config_data);
uint8_t atca_read_ecc508_config_zone(uint8_t* config_data);
uint8_t atca_write_ecc_config_zone(uint8_t* config_data);
uint8_t atca_write_ecc508_config_zone(uint8_t* config_data);
uint8_t atca_cmp_config_zone(uint8_t* config_data);
uint8_t atca_activate_gpio_auth_mode(uint8_t default_state, uint8_t key_id);

uint8_t atca_get_pubkey(uint8_t slot, uint8_t *pubkey);
uint8_t atca_create_key(uint8_t slot, uint8_t *pubkey);
uint8_t atca_create_ecc508_key(uint8_t slot, uint8_t *pubkey);
uint8_t atca_write_zone(uint8_t zone, uint8_t slot, uint8_t block, uint8_t offset, uint8_t *data, uint8_t len);
uint8_t atca_read_zone(uint8_t zone, uint8_t slot, uint8_t block, uint8_t offset, uint8_t *data, uint8_t len);
uint8_t atca_write_slot(uint8_t zone, uint8_t slot, uint8_t block, uint8_t *cipher, uint8_t *mac, uint8_t len);
uint8_t atca_sign(uint16_t slot, uint8_t *msg, uint8_t *signature);
uint8_t atca_sign_data(uint8_t slot, uint8_t *data, size_t data_len, uint8_t *signature);

uint8_t atca_read_pubkey(uint8_t slot8toF, uint8_t *pubkey);
uint8_t atca_read_sig(uint8_t slot8toF, uint8_t *sig);
uint8_t atca_write_pubkey(uint8_t slot8toF, uint8_t *pubkey);
uint8_t atca_write_sig(uint8_t slot8toF, uint8_t *sig);
uint8_t atca_write_privKey(uint8_t mode, uint8_t slot, uint8_t* data, uint8_t* priv, uint8_t* pubkey);

uint16_t atca_get_addr(uint8_t zone, uint8_t slot, uint8_t block, uint8_t offset, uint16_t* addr);
uint8_t atca_sha256_pubkey(uint8_t slot, uint8_t *sha256_digest);
uint8_t atca_restart_wdt(void);


// non-successful operation action
#define atca_wakeup_sleep(response)  { atca_wakeup(response); atca_sleep();}
	
// \TODO these needs to be parameterized or eliminated - they were defined originally as part of example code
#define ATCA_KEY_ID   (0x0000)
#define ATCA_GPIO_MODE_AUTH_OUTPUT (0x01)
#define ATCA_GPIO_STATE_HIGH  (0x04)
#define ATCA_GPIO_STATE_LOW  (0x00)
#define ATCA_SET_HIGH  1
#define ATECC_CLIENT_ADDRESS   (0xC0)
#define ATSHA_CLIENT_ADDRESS   (0xC8)


#endif /* ATCA_CMDS_H_ */