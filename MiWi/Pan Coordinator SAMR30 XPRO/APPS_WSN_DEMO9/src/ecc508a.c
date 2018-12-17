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
//#include <asf.h>

#include "ecc508a.h"
#include <atca_cmds.h>
#include <atca_i2c.h>
#include <tomcrypt.h>
#include "node_plug.h"

#if defined(_USE_CRYPT_)

/* File Scope function definitions */
static uint8_t cademo_aes_crypt(uint8_t enc_mode, uint8_t* aes_key, uint8_t* plain_txt, uint16_t* plain_txt_size, uint8_t* cypher_txt, uint8_t* iv_seed);

#ifdef CRYPT_DEBUG
static uint8_t print_mem(uint8_t* mem, uint16_t mem_size, uint16_t  bytes_per_line);
#define PRINT_MEM(...)     do{print_mem(__VA_ARGS__);}while(0);
#else
#define PRINT_MEM(...)
#endif

/* File scope defines */
#define DISPLAY_SIZE		80
#define BYTES_PER_LINE		16

#define MODE_ENCRYPT		((uint8_t)1)
#define MODE_DECRYPT		((uint8_t)0)
#define AES_KEY_SIZE		SESS_KEY_SIZE
#define IV_SIZE				16
#define MAX_BLOCK_SIZE      1392 // Max x16 bytes < SOCKET_BUFFER_MAX_LENGTH
#define AES_BLOCK_SIZE      16

/* File scope global declarations */
static uint8_t host_pub[ATCA_PUB_KEY_SIZE];
static uint8_t ecdh_host_pub[ATCA_PUB_KEY_SIZE];
static uint8_t host_sig[ATCA_SIG_SIZE];
static uint8_t session_rand[RANDOM_NUM_SIZE];
static uint8_t pre_sess_num = TCP_SOCK_MAX;

/*! \brief Initialize ECC508A SERCOM
 *
 */
void configure_ecc508a(void)
{
	atca_set_device_address(I2CBUS_0, ECC508A_I2C_ADDR);
	atca_init();
}

/**
 * \brief Initialize the pub key from the host for ECDH
 *
 * \return Status of execution.  Return 0 for success.
*/
uint8_t cademo_set_host_ecdh(uint8_t* host_pub_key)
{
	memcpy(ecdh_host_pub, host_pub_key, ATCA_PUB_KEY_SIZE);

	return ATCA_SUCCESS;
}

/**
 * \brief Initialize the pub key from the host
 *
 * \return Status of execution.  Return 0 for success.
*/
uint8_t cademo_set_host_pub_key(uint8_t* host_pub_key)
{
	memcpy(host_pub, host_pub_key, sizeof(host_pub));

	return ATCA_SUCCESS;
}

/**
 * \brief Get the Authentication information for the packet
 *
 * \param[in,out] packet The authentication packet. Authentication information will be added.
 *
 * \return Status of execution.  Return 0 for success.
*/
uint8_t cademo_set_auth_packet(auth_packet_t* packet)
{
	uint8_t ret = ATCA_SUCCESS;
	char display_str[DISPLAY_SIZE];

	do
	{
		/* Check the pointers */
		if (packet == NULL)
		{
			CRYPT_SPRINT(display_str, "cademo_set_auth_packet: NULL packet");
			CRYPT_INFO(display_str);
			ret = ATCA_BAD_PARAM;
			break;
		}
		/* Set the keys to all 00s */
		memset(packet->pub_d, 0, sizeof(packet->pub_d));
		memset(packet->sig_d, 0, sizeof(packet->sig_d));
		memset(packet->pub_s, 0, sizeof(packet->pub_s));
		memset(packet->sig_s, 0, sizeof(packet->sig_s));

		/* Get the Public Key and Signature for the device */
		ret = atca_read_pubkey(DEMO_SUBJECT_PUBKEY_SLOT, packet->pub_d);
		CRYPT_INFO("\npub_d");
		PRINT_MEM(packet->pub_d,64,16);
		if (ret != ATCA_SUCCESS)
		{
			CRYPT_SPRINT(display_str, "atca_read_pubkey: Failed");
			CRYPT_INFO(display_str);
			break;
		}
		
		ret = atca_read_sig(DEMO_SUBJECT_SIG_SLOT, packet->sig_d);
		CRYPT_INFO("\nsig_d");
		PRINT_MEM(packet->sig_d,64,16);
		if (ret != ATCA_SUCCESS)
		{
			CRYPT_SPRINT(display_str, "atca_read_sig: Failed");
			CRYPT_INFO(display_str);
			break;
		}

		/* Get the Public Key and Signature for the signer */
		ret = atca_read_pubkey(DEMO_AUTHORITY_PUBKEY_SLOT, packet->pub_s);
		CRYPT_INFO("\npub_s");
		PRINT_MEM(packet->pub_s,64,16);
		if (ret != ATCA_SUCCESS)
		{
			CRYPT_SPRINT(display_str, "atca_read_pubkey: Failed");
			CRYPT_INFO(display_str);
			break;
		}
		
		ret = atca_read_sig(DEMO_AUTHORITY_SIG_SLOT, packet->sig_s);
		CRYPT_INFO("\nsig_s");
		PRINT_MEM(packet->sig_s,64,16);
		if (ret != ATCA_SUCCESS)
		{
			CRYPT_SPRINT(display_str, "atca_read_sig: Failed");
			CRYPT_INFO(display_str);
			break;
		}

	} while (0);

	return ret;
}

/**
 * \brief Function to encrypt or decrypt bytes.
 * 
 * \param[in] enc_mode Set whether to encrypt or decrypt
 * \param[in] aes_key The bytes to encrypt
 * \param[in] plain_txt The bytes to encrypt
 * \param[in] plain_txt_size The number of bytes to encrypt
 * \param[in] cypher_txt The encryption of plain_text
 * \param[in] iv The initialization vector 
 *
 * \return Status of execution.  Return 0 for success.
 */
static uint8_t cademo_aes_crypt(uint8_t enc_mode, uint8_t* aes_key, uint8_t* plain_txt, uint16_t* plain_txt_size, uint8_t* cypher_txt, uint8_t* iv)
{
	uint8_t ret = ATCA_SUCCESS;
	uint16_t keysize = AES_KEY_SIZE;
	
	// LibTomCrypt
	register_cipher(&aes_desc);

	int32_t cypher_idx = find_cipher("aes");
	symmetric_CBC cbc;
	
	ret = cbc_start(cypher_idx, iv, aes_key, keysize, 0, &cbc);
	if (enc_mode == MODE_ENCRYPT) {
		ret = cbc_encrypt(plain_txt, cypher_txt, *plain_txt_size, &cbc);
	} 
	else if (enc_mode == MODE_DECRYPT) {
		/* plain_txt is the encrypted data by calling cademo_aes_crypt(),
		   while cypher_txt is the place to store decrypted data. */
		ret = cbc_decrypt(plain_txt, cypher_txt, *plain_txt_size, &cbc);
	}
	
	return ret;
}

#ifdef CRYPT_DEBUG
/**
 * \brief Function to print memory buffers.
 * 
 * \param[in] mem The memory to print.
 * \param[in] mem_size The number of bytes to print.
 * \param[in] bytes_per_line The number of bytes per line to print.
 *
 * \return Status of execution.  Return 0 for success.
 */
static uint8_t print_mem(uint8_t* mem, uint16_t mem_size, uint16_t  bytes_per_line)
{
	char display_str[DISPLAY_SIZE];
	uint8_t chars_per_byte = 3;
	uint16_t i = 0;
	uint16_t j = 0;

	// Check the inCRYPT_INFO
	if(mem == NULL || bytes_per_line*chars_per_byte > DISPLAY_SIZE)
	{
		return ATCA_FUNC_FAIL;
	}

	// Print the memory
	memset(display_str, 0, sizeof(display_str));
	for (i = 0; i < mem_size; i++)
	{
		CRYPT_SPRINT(&display_str[j*chars_per_byte], "%02x ", mem[i]);
		j++;
		
		if ((j == (bytes_per_line)) || (i == (mem_size-1)))
		{
			// Print the line
			CRYPT_INFO(display_str);
			j = 0;
			memset(display_str, 0, sizeof(display_str));
		}
	}
	return ATCA_SUCCESS;
}
#endif

/**
 * \brief Sign random number from the host to generate signature.
 *
 * \return Status of execution.  Return 0 for success.
*/
uint8_t cademo_sign_host_rand(uint8_t *challenge_sig, uint8_t *host_random)
{
	uint8_t ret = ATCA_SUCCESS;
	uint8_t auth_sig[ATCA_SIG_SIZE];
	char display_str[DISPLAY_SIZE];
	
	/* Print the random number that was used */
	CRYPT_INFO("CryptoAuth Host Random: ");
	PRINT_MEM(host_random, RANDOM_NUM_SIZE, BYTES_PER_LINE);
	do {
		/* Sign the random challenge for authenticity */
		ret = atca_sign_data(DEMO_AUTH_PRIV_SLOT, host_random, RANDOM_NUM_SIZE, auth_sig);
		if (ret != ATCA_SUCCESS)
		{
			CRYPT_SPRINT(display_str, "atca_sign: Failed - get authentication Signature");
			CRYPT_INFO(display_str);
			break;
		}
		/* Print the signature using PRINT_MEM */
		CRYPT_INFO("\nCryptoAuth Get Authentication Signature for AUTHETICITY: ");
		PRINT_MEM(auth_sig, sizeof(auth_sig), BYTES_PER_LINE);

		/* Copy the authentication signature into the temp_report */
		memcpy(challenge_sig, auth_sig, sizeof(auth_sig));
	} while(0);
	
	return ret;
}

/**
 * \brief Initialize the random challenge response from the host
 *
 * \return Status of execution.  Return 0 for success.
*/
uint8_t cademo_set_host_sig(uint8_t* auth_sig)
{
	memcpy(host_sig, auth_sig, sizeof(host_sig));

	return ATCA_SUCCESS;
}

uint8_t cademo_generate_node_rand(uint8_t *rand_num)
{
	uint8_t ret = ATCA_SUCCESS;
	uint8_t rand_out[RANDOM_NUM_SIZE];
	char display_str[DISPLAY_SIZE];
	do
	{
		/* Get Random Number from ECC508 */
		ret = atca_get_random_number(rand_out);
		if (ret != ATCA_SUCCESS)
		{
			CRYPT_SPRINT(display_str, "CryptoAuth Random: Failed");
			CRYPT_INFO(display_str);
			break;
		}
		/* Copy the random number to be used as challenge to host */
		memcpy(rand_num, rand_out, sizeof(rand_out));
		CRYPT_INFO("\nNode Generated Random Number: ");
		PRINT_MEM(rand_out, sizeof(rand_out), BYTES_PER_LINE);
	} while (0);
	
	return ret;
}

/**
 * \brief Initialize the random number for creating session key
 *
 * \return Status of execution.  Return 0 for success.
*/
uint8_t cademo_set_sessionn_random(uint8_t* rand_num)
{
	memcpy(session_rand, rand_num, sizeof(session_rand));

	return ATCA_SUCCESS;
}

uint8_t cademo_verify_challenge_resp(uint8_t* verified)
{
	uint8_t ret = ATCA_SUCCESS;
	char display_str[DISPLAY_SIZE];

	uint8_t* pub_key = host_pub;
	uint8_t* message = session_rand;
	uint8_t* signature = host_sig;
	uint8_t digest[SHA_DIGEST_SIZE] = {0};
		
	CRYPT_INFO("\nCryptoAuth Host Message: ");
	PRINT_MEM(message, RANDOM_NUM_SIZE, BYTES_PER_LINE);
	CRYPT_INFO("\nCryptoAuth Host Pub Key: ");
	PRINT_MEM(pub_key, ATCA_PUB_KEY_SIZE, BYTES_PER_LINE);
	CRYPT_INFO("\nCryptoAuth Host Signature: ");
	PRINT_MEM(signature, ATCA_SIG_SIZE, BYTES_PER_LINE);
	
	atca_sha256(RANDOM_NUM_SIZE, message, digest);

	ret = atca_send_verify_extern(pub_key, digest, signature, verified);
	if (ret != ATCA_SUCCESS || *verified != ATCA_SUCCESS)
	{
		CRYPT_SPRINT(display_str, "atca_send_verify_extern: Failed");
		CRYPT_INFO(display_str);
	}
	else {
		CRYPT_SPRINT(display_str, "Host Signature Verified");
		CRYPT_INFO(display_str);
	}
	return ret;
}

uint8_t cademo_dec_frame(encrypt_packet_t* msg_enc_report, uint8_t session_num)
{
	uint8_t ret = ATCA_SUCCESS;
	uint8_t *temp_enc = msg_enc_report->en_data;
	uint8_t aes_sess_key[AES_KEY_SIZE] = {0};
	uint8_t *iv = msg_enc_report->iv;
	
	if (get_session(session_num)->sess_key_valid)
	{
		memcpy(aes_sess_key, get_session(session_num)->sess_key, AES_KEY_SIZE);
		/* Decrypt the message using the session key */
		ret = cademo_aes_crypt(MODE_DECRYPT, aes_sess_key, temp_enc, &msg_enc_report->en_data_len, msg_enc_report->data, iv);
	}
	else {
		return ATCA_FUNC_FAIL; // session key is not valid
	}
	
	return ret;
}

/**
 * \brief Encrypt message using ECDH session key and session random number
 * \note ECDH private key is stored in ECC508A, and the ECDH pub key is from host
 * \return Status of execution.  Return 0 for success.
*/
uint8_t cademo_enc_frame(encrypt_packet_t* msg_enc_report, uint8_t session_num)
{
	uint8_t ret = ATCA_SUCCESS;
	uint8_t sess_key[MAC_SIZE] = {0};
	uint8_t msg_digest[SHA_DIGEST_SIZE] = {0};
	uint8_t msg_mac[MAC_SIZE] = {0};
	uint8_t *temp_enc = msg_enc_report->en_data;
	uint8_t rand_out[RANDOM_NUM_SIZE] = {0};
	uint8_t ret_ecdh[ATCA_BLOCK_SIZE] = {0};
	uint8_t aes_sess_key[AES_KEY_SIZE] = {0};
	uint8_t iv[IV_SIZE] = {0};
	uint16_t data_len = msg_enc_report->data_len; // including checksum
	uint16_t data_block_len = 0;
	uint8_t data[MAX_BLOCK_SIZE] = {0};
	char display_str[DISPLAY_SIZE];
	
	do
	{
		/* ---- Return the ECDH PubKey: ------------------------------------------------- */
		/* Get the Public Key and Signature for the device */
		ret = atca_read_pubkey(DEMO_ECDH_PUBKEY_SLOT, msg_enc_report->pub_x);
		if (ret != ATCA_SUCCESS)
		{
			break;
		}
		/* Print the ECDH Client public key  */
		CRYPT_INFO("CryptoAuth ECDH Client public key (pub-x): ");
		PRINT_MEM(msg_enc_report->pub_x, sizeof(msg_enc_report->pub_x), BYTES_PER_LINE);
		/* ------------------------------------------------------------------------------ */

		/* ------------------------------------------------------------------------------ */
		// Check if need run ECDH
		if(!get_session(session_num)->sess_key_valid || session_num != pre_sess_num)
		{
			cademo_set_host_ecdh(get_session(session_num)->user_ptr->pub_key);
			/* Print the ECDH HOST public key (pub-host) */
			CRYPT_INFO("CryptoAuth ECDH Host public key (pub-host): ");
			PRINT_MEM(ecdh_host_pub, sizeof(ecdh_host_pub), BYTES_PER_LINE);
			/* ---- ECDH Key Agreement: ----------------------------------------------------- */
			/* Execute the ECDH command on the shared secret slot */
			/* The pre-master key will be copied into the adjacent slot upon success */
			ret = atca_send_ecdh(DEMO_ECDH_PRIV_SLOT, ecdh_host_pub, ret_ecdh);
			
			if (ret != ATCA_SUCCESS || ret_ecdh[0] != ATCA_SUCCESS)
			{
				CRYPT_SPRINT(display_str, "atca_send_ecdh: Failed");
				CRYPT_INFO(display_str);
				break;
			}
			/* Get the session key */
			/* Send the session_rand as a MAC challenge to create a session key with the pre-master key */
			ret = atca_send_mac(DEMO_ECDH_PM_KEY_SLOT, MAC_MODE_CHALLENGE, session_rand, sess_key);
			if (ret != ATCA_SUCCESS)
			{
				CRYPT_SPRINT(display_str, "atca_send_mac: Failed - get session key");
				CRYPT_INFO(display_str);
				break;
			}
			/* Print the Session Key using PRINT_MEM */
			CRYPT_INFO("CryptoAuth Get Session Key to encrypt message for CONFIDENTIALITY: ");
			PRINT_MEM(sess_key, sizeof(sess_key), BYTES_PER_LINE);
			// session key generated
			get_session(session_num)->sess_key_valid = true;
			memcpy(get_session(session_num)->sess_key, sess_key, AES_KEY_SIZE);
			pre_sess_num = session_num;
			/* ------------------------------------------------------------------------------ */
		}
		/* ------------------------------------------------------------------------------ */
		
		/* ---- Data Integrity: --------------------------------------------------------- */
		/* Get the message digest */
		atca_sha256(data_len, msg_enc_report->data, msg_digest);
		/* Print the digest using PRINT_MEM */
		CRYPT_INFO("CryptoAuth Get Message Digest for MAC");
		PRINT_MEM(msg_digest, sizeof(msg_digest), BYTES_PER_LINE);

		/* Get the message MAC */
		ret = atca_send_mac(DEMO_ECDH_PM_KEY_SLOT, MAC_MODE_CHALLENGE, msg_digest, msg_mac);
		if (ret != ATCA_SUCCESS)
		{
			CRYPT_SPRINT(display_str, "CryptoAuth MAC: Failed - get message MAC");
			CRYPT_INFO(display_str);
			break;
		}
		/* Print the MAC using PRINT_MEM */
		CRYPT_INFO("CryptoAuth Get Message MAC for INTEGRITY CHECK");
		PRINT_MEM(msg_mac, sizeof(msg_mac), BYTES_PER_LINE);
		
		/* Setup the message to encrypt. Concatenate the message with the data integrity MAC */
		memcpy(&msg_enc_report->data[data_len], msg_mac, sizeof(msg_mac));
		data_len += MAC_SIZE;
		/* ------------------------------------------------------------------------------ */

		/* ---- Confidentiality: -------------------------------------------------------- */
		/* Copy the session key to the AES key */
		memcpy(aes_sess_key, get_session(session_num)->sess_key, sizeof(aes_sess_key));
		
		/* Generate the initialization vector as: IV = iv_seed[0..15] : Get the Random Number */
		ret = atca_get_random_number(rand_out);
		if (ret != ATCA_SUCCESS)
		{
			CRYPT_SPRINT(display_str, "CryptoAuth Random: Failed");
			CRYPT_INFO(display_str);
			break;
		}
		/* Copy the IV to be used with AES encryption */
		memcpy(iv, rand_out, sizeof(iv));
		
		/* Copy the IV into the msg_enc_report structure */
		memcpy(msg_enc_report->iv, iv, sizeof(msg_enc_report->iv));

		/* Make sure the data block length is multiple 16 bytes */
		data_block_len = (data_len%AES_BLOCK_SIZE ? data_len/AES_BLOCK_SIZE + 1 : data_len/AES_BLOCK_SIZE);
		memcpy(data, msg_enc_report->data, msg_enc_report->data_len);
		msg_enc_report->en_data_len = data_block_len * AES_BLOCK_SIZE;
		msg_enc_report->data_len = data_len; // update plain text length 
		
		/* Encrypt the message using the session key */
		cademo_aes_crypt(MODE_ENCRYPT, aes_sess_key, data, &msg_enc_report->en_data_len, temp_enc, iv);
		/* ------------------------------------------------------------------------------ */
	} while (0);

	return ret;
}

#endif // _USE_CRYPT_