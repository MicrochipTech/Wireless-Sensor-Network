/** \file
 *  \brief  common CryptoAuth commands that use the ATCA lib APIs
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
 *
 * \todo Refactor to integrate or remove #define EXAMPLE code cases
 */

#include <atcalib.h>
#include <atca_cmds.h>

#define DISPLAY_SIZE		80

/** \brief This function returns the revision of the IC been used
	\param[in] pointer to response
	\return status of the operation
	*/
uint8_t atca_info(uint8_t *oResponse)
{
	uint8_t devrev_cmd[INFO_COUNT];
	uint8_t ret_code;

	ret_code = atca_execute(ATCA_INFO, INFO_MODE_REVISION, INFO_NO_STATE, 0, NULL, 0, NULL,
		0, NULL, sizeof(devrev_cmd), devrev_cmd, INFO_RSP_SIZE, oResponse);

	atca_idle();
	return ret_code;
}

/** \brief This function returns serial number of the device
	\param[in] pointer to response
	\return status of the operation
	*/
uint8_t atca_read_serial_number(uint8_t* serial_number)
{
	uint8_t ret = ATCA_SUCCESS;
	uint8_t bytes_read[ATCA_BLOCK_SIZE];
	uint8_t block = 0;
	uint8_t cpyIndex = 0;
	uint8_t offset = 0;

	do
	{
		// Check the config_data pointer
		if (bad_pointer(serial_number)) break;

		memset(serial_number, 0x00, ATCA_SERIAL_NUM_SIZE);
		// Read first 32 byte block.  Copy the bytes into the config_data buffer
		block = 0;
		offset = 0;
		ret = atca_read_zone(ATCA_ZONE_CONFIG, 0, block, offset, bytes_read, ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;
		memcpy(&serial_number[cpyIndex], bytes_read, ATCA_WORD_SIZE);
		cpyIndex += ATCA_WORD_SIZE;

		block = 0;
		offset = 2;
		ret = atca_read_zone(ATCA_ZONE_CONFIG, 0, block, offset, bytes_read, ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;
		memcpy(&serial_number[cpyIndex], bytes_read, ATCA_WORD_SIZE);
		cpyIndex += ATCA_WORD_SIZE;

		block = 0;
		offset = 3;
		ret = atca_read_zone(ATCA_ZONE_CONFIG, 0, block, offset, bytes_read, ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;
		memcpy(&serial_number[cpyIndex], bytes_read, 1);

	} while (0);

	return ret;
}

/** \brief This function pads serial number to for s 32 byte array
	\param[in] oResponse pointer to response
	\return status of the operation
	*/
uint8_t atca_pad_serial(uint8_t *oResponse)
{
	uint8_t ret = ATCA_SUCCESS;
	uint8_t static serial_number[ATCA_SERIAL_NUM_SIZE];

	do
	{
		if (oResponse == NULL)
		{
			return ATCA_BAD_PARAM;
		}
		// Set the pad bytes
		memset(&oResponse[0], 0x77, ATCA_BLOCK_SIZE);

		// Read the serial number and write it into the first bytes
		ret = atca_read_serial_number(serial_number);
		if (ret != ATCA_SUCCESS) break;

		// If successfully read, then copy
		memcpy(&oResponse[0], &serial_number, ATCA_SERIAL_NUM_SIZE);

	} while (0);

	return ret;
}


/** \brief This runs derive key command, nonce command needs to be called prior to use
	\param[in] pointer to temp key if derive key command mode is PASSTHROUGH nonce
	\return status of the operation
	*/
uint8_t atca_derive_key_cmd(uint8_t mode, uint16_t key_slot)
{
	uint8_t ret = ATCA_SUCCESS;
	uint8_t param_1 = mode;

	uint8_t command[ATCA_CMD_SIZE_MIN];
	uint8_t response[DERIVE_KEY_RSP_SIZE];

	do{
		ret = atca_wake();
		if (ret != ATCA_SUCCESS) break;

		ret = atca_execute(ATCA_DERIVE_KEY, param_1, key_slot, 0, NULL, 0, NULL, 0, NULL,
			sizeof(command), command, sizeof(response), response);
		if (ret != ATCA_SUCCESS)  break;

		ret = atca_idle();
		if (ret != ATCA_SUCCESS)  break;
	} while (0);

	return ret;
}

/** \brief This function checks the response status byte and puts the device
		   to sleep if there was an error.
		   \param[in] ret_code return code of function
		   \param[in] response pointer to response buffer
		   \return status of the operation
		   */
uint8_t atca_check_response_status(uint8_t ret_code, uint8_t *response)
{
	if (ret_code != ATCA_SUCCESS) {
		atca_sleep();
		return ret_code;
	}
	ret_code = response[ATCA_RSP_DATA_IDX];
	if (ret_code != ATCA_SUCCESS) {
		atca_sleep();
	}

	return ret_code;
}


/** \brief This function checks the configuration lock status byte and puts the device
		   to sleep if there was an error.
		   \return status of the operation, ATCA_FUNC_FAIL if device is unlocked, ATCA_SUCCESS if device is locked
		   */
uint8_t atca_check_lock_status(int zone)
{
	// declared as "volatile" for easier debugging
	volatile uint8_t ret_code;

	uint16_t lock_config_address = 84;   // 0x54, start of block that contains lock config flag

	// Make the command buffer the size of a Read command.
	uint8_t command[READ_COUNT];

	// Make the response buffer the minimum size of a Read response.
	uint8_t response[READ_4_RSP_SIZE];

	// Make sure that configuration zone is locked.
	memset(response, 0, sizeof(response));
	ret_code = atca_execute(ATCA_READ, zone,
		lock_config_address >> 2, 0, NULL, 0, NULL, 0, NULL,
		sizeof(command), command, sizeof(response), response);
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	// Put client device to sleep.
	atca_sleep();

	// Check the configuration lock status.
	if (response[4] == 0x55) {
		// Configuration Zone has not been locked.
		// Throw error code.
		return ATCA_FUNC_FAIL;
	}

	return ATCA_SUCCESS;
}

/**\#define ATCA_ECDH ((uint8_t) 0x37) //!ECDH command op-code
	\#define ECDH_PREFIX_MODE ((uint8_t) 0x00)
	\#define ECDH_COUNT (47)
	\#define ECDH_RSP_SIZE (32)
	\#define ECDH_PUBKEYIN_SIZE (64)
	*/
uint8_t atca_send_ecdh(uint16_t key_id, uint8_t* pub_key, uint8_t* ret_ecdh)
{
	static uint8_t wakeup_response[ATCA_RSP_SIZE_MIN];
	uint8_t ret = ATCA_SUCCESS;
	uint16_t key_param = key_id;
	uint8_t command[ECDH_COUNT];
	uint8_t response[ECDH_RSP_SIZE];
	uint8_t cpy_size = ATCA_KEY_SIZE;

	do
	{
		if (pub_key == NULL || ret_ecdh == NULL)
		{
			return ATCA_BAD_PARAM;
		}
		// Set all of the bytes of the ret_ecdh return to 00
		memset(ret_ecdh, 0, sizeof(wakeup_response));
		memset(response, 0, sizeof(response));

		// Wake up the device.
		ret = atca_wake();
		if (ret != ATCA_SUCCESS) break;

		// Send the ECDH Command
		ret = atca_execute(ATCA_ECDH, ECDH_PREFIX_MODE, key_param, ECDH_PUBKEYIN_SIZE, pub_key, 0, NULL,
			0, NULL, sizeof(command), command, ECDH_RSP_SIZE, response);
		if (ret != ATCA_SUCCESS) break;

		// Determine if the response is 4 bytes or 35 bytes
		if (response[ATCA_COUNT_IDX] == ATCA_RSP_SIZE_MIN)
		{
			// Status only, response is 4 bytes.,  Copy one byte only.
			cpy_size = 1;
		}
		memcpy(ret_ecdh, &response[ATCA_RSP_DATA_IDX], cpy_size);

	} while (0);

	// Send Idle to preserve RNG seed
	atca_idle();
	return ret;
}

/** \brief Create a random private key or Generate an ECC public key
	\param[in] mode The key to be created
	\param[in] key_id The slot of specified key
	\param[out] response A random private key or an ECC public key
	\return status of the operation
	*/
uint8_t atca_send_genkey(uint8_t mode, uint16_t key_id, uint8_t* created_key)
{
	static uint8_t wakeup_response[ATCA_RSP_SIZE_MIN];
	uint8_t ret_code = ATCA_SUCCESS;
	uint8_t mode_param = mode;
	uint16_t key_param = key_id;
	uint8_t command[GENKEY_COUNT];
	uint8_t response[GENKEY_RSP_SIZE_LONG];
	uint8_t response_size = GENKEY_RSP_SIZE_LONG;

	if (created_key == NULL) {
		return ATCA_BAD_PARAM;
	}

	if ((mode_param == GENKEY_MODE_PRIVATE) || (mode_param == GENKEY_MODE_PUBLIC))
	{
		response_size = ATCA_RSP_SIZE_64;
	}
	else if ((mode_param == GENKEY_MODE_DIGEST) || (mode_param == GENKEY_MODE_ADD_DIGEST))
	{
		response_size = GENKEY_RSP_SIZE_SHORT;
	}

	memset(wakeup_response, 0, sizeof(wakeup_response));
	ret_code = atca_wakeup(wakeup_response);
	if (ret_code != ATCA_SUCCESS)
	{
		return ret_code;
	}

	ret_code = atca_execute(ATCA_GENKEY, mode_param, key_param, 0, NULL,
		0, NULL, 0, NULL, sizeof(command), command, response_size, response);
	if (ret_code != ATCA_SUCCESS)
	{
		return ret_code;
	}

	if ((mode == GENKEY_MODE_PRIVATE) || (mode == GENKEY_MODE_PUBLIC)){
		memcpy(created_key, &response[ATCA_RSP_DATA_IDX], ATCA_PUB_KEY_SIZE);
	}

	atca_idle();
	return ret_code;
}

/** \brief Read the count value from one of the two counters
	\param[in] mode The read or increment mode
	\param[in] counter_id The counter to be incremented or read
	\param[out] count The value of the specified counter
	\return status of the operation
	*/
uint8_t atca_send_counter(uint8_t mode, uint16_t counter_id, uint8_t* count)
{
	static uint8_t wakeup_response[COUNTER_RSP_SIZE];
	uint8_t ret_code = ATCA_SUCCESS;
	static uint8_t command[COUNTER_COUNT];
	uint8_t mode_param = mode;
	uint16_t key_param = counter_id;
	static uint8_t response[COUNTER_RSP_SIZE];

	memset(wakeup_response, 0, sizeof(wakeup_response));
	ret_code = atca_wakeup(wakeup_response);
	if (ret_code != ATCA_SUCCESS)
	{
		return ret_code;
	}

	//response will be the current binary value of the counter
	ret_code = atca_execute(ATCA_COUNTER, mode_param, key_param, 0, NULL,
		0, NULL, 0, NULL, sizeof(command), command, COUNTER_RSP_SIZE, response);

	if (ret_code != ATCA_SUCCESS)
	{
		return ret_code;
	}

	memcpy(count, &response[ATCA_RSP_DATA_IDX], ATCA_RSP_SIZE_MIN);

	atca_idle();

	return ret_code;
}

/** \brief Compute SHA256 or HMAC digest
	\param[in] mode The step to generate SHA266 digest
	\param[in] length The number of bytes in the Message parameter
	\param[in] message up to 64 bytes of data to be included into the hash operation.
	\param[out] digest_out The SHA256 digest
	\return status of the operation
	*/
uint8_t atca_send_sha(uint8_t mode, uint16_t length, uint8_t* message, uint8_t* digest_out)
{
	static uint8_t wakeup_response[ATCA_RSP_SIZE_MIN];
	uint8_t ret_code = ATCA_SUCCESS;
	uint8_t mode_param = mode;
	uint8_t length_param = (uint8_t)length;

	memset(wakeup_response, 0, sizeof(wakeup_response));
	ret_code = atca_wakeup(wakeup_response);
	if (ret_code != ATCA_SUCCESS) {
		atca_idle();
		return ret_code;
	}

	if (mode_param == SHA_SHA256_START_MASK)
	{

		static uint8_t command[SHA_COUNT_LONG];
		uint8_t response_size = SHA_RSP_SIZE_SHORT;
		static uint8_t response[SHA_RSP_SIZE_SHORT];

		ret_code = atca_execute(ATCA_SHA, mode_param, 0, 0, NULL, 0, NULL,
			0, NULL, sizeof(command), command, response_size, response);

		if (ret_code != ATCA_SUCCESS) {
			atca_idle();
			return ret_code;
		}
	}
	else if (mode_param == SHA_SHA256_UPDATE_MASK)
	{
		static uint8_t command[WRITE_COUNT_LONG_MAC];
		uint8_t response_size = SHA_RSP_SIZE_SHORT;
		static uint8_t response[SHA_RSP_SIZE_SHORT];

		if (message == NULL) {
			atca_idle();
			return ATCA_CMD_FAIL;
		}

		ret_code = atca_execute(ATCA_SHA, SHA_SHA256_UPDATE_MASK, 0, length_param, message, 0, NULL,
			0, NULL, sizeof(command), command, response_size, response);
		if (ret_code != ATCA_SUCCESS) {
			atca_idle();
			return ret_code;
		}
	}
	else if (mode_param == SHA_SHA256_END_MASK)
	{
		static uint8_t command[SHA_COUNT_LONG];
		uint8_t response_size = SHA_RSP_SIZE_LONG;
		static uint8_t response[SHA_RSP_SIZE_LONG];

		ret_code = atca_execute(ATCA_SHA, SHA_SHA256_END_MASK, 0, 0, NULL, 0, NULL,
			0, NULL, sizeof(command), command, response_size, response);

		if (ret_code != ATCA_SUCCESS) {
			atca_idle();
			return ret_code;
		}

		if (digest_out != NULL) {
			memcpy(digest_out, &response[ATCA_RSP_DATA_IDX], SHA_DIGEST_SIZE);
		}
	}

	atca_idle();

	return ret_code;
}

/** \brief verify if given signature is correct
	\param[in] pub_key The public key to be used for verification
	\param[in] message The message is used to sign
	\param[in] signature The ECDSA signature to be verified
	\param[out] verified Returns a value of zero if the signature of the message can be verified
	\return status of the operation
	*/
uint8_t atca_send_verify_extern(uint8_t* pub_key, uint8_t* message, uint8_t* signature, uint8_t* verified)
{
	uint8_t ret_code = ATCA_SUCCESS;
	uint16_t key_param = VERIFY_KEY_P256;
	uint8_t command[VERIFY_256_EXTERNAL_COUNT];
	uint8_t wakeup_response[ATCA_RSP_SIZE_MIN];
	uint8_t response_dummy_random[RANDOM_RSP_SIZE];
	uint8_t response[ATCA_RSP_SIZE_MIN];
	uint8_t verification[VERIFY_RSP_SIZE];

	memset(command, 0, sizeof(command));
	memset(wakeup_response, 0, sizeof(wakeup_response));
	ret_code = atca_wakeup(wakeup_response);
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	memset(response_dummy_random, 0, sizeof(response_dummy_random));
	ret_code = atca_execute(ATCA_RANDOM, RANDOM_SEED_UPDATE, 0x0000, 0,
		NULL, 0, NULL, 0, NULL, sizeof(command), command,
		sizeof(response_dummy_random), response_dummy_random);
	if (ret_code != ATCA_SUCCESS)
	{
		(void)atca_sleep();
		return ret_code;
	}

	memset(response, 0, sizeof(response));
	ret_code = atca_execute(ATCA_NONCE, NONCE_MODE_PASSTHROUGH,
		NONCE_MODE_RANDOM_OUT, NONCE_NUMIN_SIZE_PASSTHROUGH,
		(uint8_t *)message, 0, NULL, 0, NULL, NONCE_COUNT_SHORT,
		command, sizeof(response), response);

	if (ret_code != ATCA_SUCCESS) {
		(void)atca_sleep();
		return ret_code;
	}

	memset(verification, 0, sizeof(verification));
	ret_code = atca_execute(ATCA_VERIFY, VERIFY_MODE_EXTERNAL,
		key_param, VERIFY_256_SIGNATURE_SIZE, signature, VERIFY_256_KEY_SIZE,
		pub_key, 0, NULL, sizeof(command), command, VERIFY_RSP_SIZE, verification);

	if (ret_code != ATCA_SUCCESS) {
		(void)atca_sleep();
		return ret_code;
	}

	*verified = verification[1];
	atca_idle();

	return ret_code;
}

/** \brief This function is a simple example for how to use the library.
		   It wakes up the device, sends a Info command, receives its
		   response, and puts the device to sleep. It uses a total of
		   four library functions from all three layers, physical,
		   communication, and command marshaling layer.

		   Use this example to familiarize yourself with the library
		   and device communication before proceeding to examples that
		   deal with the security features of the device.
		   \return status of the operation
		   */
uint8_t atca_send_info_command(void)
{
	uint8_t ret_code;
	static uint8_t wakeup_response[ATCA_RSP_SIZE_MIN];
	uint8_t command[INFO_COUNT];
	uint8_t response[INFO_RSP_SIZE];

	// atca_set_device_id(CRYPTOAUTH_BUS, CRYPTOAUTH_SLAVE_ADDRESS);

	// Wake up the device.
	memset(wakeup_response, 0, sizeof(wakeup_response));
	ret_code = atca_wakeup(wakeup_response);
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	ret_code = atca_execute(ATCA_INFO, INFO_MODE_REVISION, INFO_NO_STATE, 0, NULL, 0, NULL,
		0, NULL, sizeof(command), command, sizeof(response), response);

	atca_idle();

	return ret_code;
}

/** \brief Get a 32 byte random number from the CryptoAuth device
	\param[out] random_number The RANDOM_NUM_SIZE (32 byte) random number
	\return status of the operation
	*/
uint8_t atca_get_random_number(uint8_t *rand_out)
{
	uint8_t ret_code;
	static uint8_t wakeup_response[ATCA_RSP_SIZE_MIN];
	uint8_t command[RANDOM_COUNT];
	uint8_t response[RANDOM_RSP_SIZE];

	// Wake up the device.
	memset(wakeup_response, 0, sizeof(wakeup_response));
	ret_code = atca_wakeup(wakeup_response);
	if (ret_code != ATCA_SUCCESS) return ret_code;

	ret_code = atca_execute(ATCA_RANDOM, RANDOM_SEED_UPDATE, 0, 0, NULL, 0, NULL,
		0, NULL, sizeof(command), command, RANDOM_RSP_SIZE, response);

	memcpy(rand_out, &response[ATCA_RSP_DATA_IDX], RANDOM_NUM_SIZE);

	atca_idle();

	return ret_code;
}

/** \brief Get a 32 byte MAC from the CryptoAuth device given a key ID and a challenge
	\param[in] key_id The key in the CryptoAuth device to use for the MAC
	\param[in] challenge The 32 byte challenge number
	\param[out] mac_response The response of the MAC command using the given challenge
	\return status of the operation
	*/
uint8_t atca_send_mac(uint8_t key_id, uint8_t mode, uint8_t* challenge, uint8_t* mac_response)
{
	uint8_t ret_code;
	static uint8_t wakeup_response[ATCA_RSP_SIZE_MIN];
	uint8_t command[MAC_COUNT_LONG];
	uint8_t response[MAC_RSP_SIZE];
	uint16_t key_param = key_id;

	// Set all bytes of the response to 00.  This makes it easy to see when this call fails
	memset(wakeup_response, 0, sizeof(wakeup_response));

	// Wake up the device.
	ret_code = atca_wakeup(wakeup_response);
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	// Execute the MAC
	// The key is passed LSB
	ret_code = atca_execute(ATCA_MAC, mode, key_param, MAC_CHALLENGE_SIZE, challenge, 0, NULL,
		0, NULL, sizeof(command), command, MAC_RSP_SIZE, response);

	memcpy(mac_response, &response[ATCA_RSP_DATA_IDX], MAC_SIZE);

	atca_idle();

	return ret_code;
}

uint8_t atca_send_nonce(uint8_t mode, uint8_t* numin, uint8_t* rand_out)
{
	uint8_t ret_code;
	static uint8_t wakeup_response[ATCA_RSP_SIZE_MIN];
	uint8_t command[NONCE_COUNT_LONG];
	uint8_t response[NONCE_RSP_SIZE_LONG];
	uint8_t resp_size;
	uint8_t numin_size;
	uint8_t numin_param[NONCE_NUMIN_SIZE_PASSTHROUGH];

	/* Set parameters and sizes based on mode */
	if (mode == NONCE_MODE_PASSTHROUGH)
	{
		resp_size = NONCE_RSP_SIZE_SHORT;
		numin_size = NONCE_NUMIN_SIZE_PASSTHROUGH;
		memcpy(numin_param, numin, numin_size);
	}
	else
	{
		resp_size = NONCE_RSP_SIZE_LONG;
		numin_size = NONCE_NUMIN_SIZE;
		memcpy(numin_param, numin, numin_size);
	}

	// Wake up the device.
	memset(wakeup_response, 0, sizeof(wakeup_response));
	ret_code = atca_wakeup(wakeup_response);
	if (ret_code != ATCA_SUCCESS)
	{
		return ret_code;
	}

	ret_code = atca_execute(ATCA_NONCE, mode, 0, numin_size, numin, 0, NULL,
		0, NULL, sizeof(command), command, resp_size, response);

	if (mode != NONCE_MODE_PASSTHROUGH)
	{
		memcpy(rand_out, &response[ATCA_RSP_DATA_IDX], RANDOM_NUM_SIZE);
	}

	atca_idle();

	return ret_code;
}

uint8_t atca_send_gendig(uint8_t zone, uint8_t key_id, uint8_t *other_data)
{
	uint8_t ret_code;
	static uint8_t wakeup_response[ATCA_RSP_SIZE_MIN];
	uint8_t command[ATCA_RSP_SIZE_72]; //temp value instead of 'GENDIG_COUNT' to prevent Visual Studio2013 error
	uint8_t response[GENDIG_RSP_SIZE];
	uint16_t key_param = key_id;

	// Wake up the device.
	memset(wakeup_response, 0, sizeof(wakeup_response));
	ret_code = atca_wakeup(wakeup_response);
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	ret_code = atca_execute(ATCA_GENDIG, zone, key_param, GENDIG_OTHER_DATA_SIZE, other_data, 0, NULL,
		0, NULL, sizeof(command), command, GENDIG_RSP_SIZE, response);
	ret_code = response[1];
	atca_idle();

	return ret_code;
}

uint8_t atca_send_checkmac(uint8_t zone, uint8_t key_id, uint8_t *client_challenge, uint8_t *client_response, uint8_t *other_data)
{
	uint8_t ret_code;
	uint8_t command[CHECKMAC_COUNT];
	uint8_t response[CHECKMAC_RSP_SIZE];
	uint16_t key_param = key_id;

	do{
		ret_code = atca_wake();
		if (ret_code != ATCA_SUCCESS)
			break;

		ret_code = atca_execute(ATCA_CHECKMAC, zone, key_param, CHECKMAC_CLIENT_CHALLENGE_SIZE, client_challenge,
			CHECKMAC_CLIENT_RESPONSE_SIZE, client_response,
			CHECKMAC_OTHER_DATA_SIZE, other_data, sizeof(command), command, GENDIG_RSP_SIZE, response);
		ret_code = response[1];
	} while (0);

	atca_idle();

	return ret_code;
}



/** \brief This function checks the configuration of the device for verify external example.
 *
 *         SlotConfig0 to 0x8F20
 *           - ReadKey      0xF
 *           - NoMac          0
 *           - SingleUse      0
 *           - EncryptRead    0
 *           - IsSecret       1
 *           - WriteKey     0x0
 *           - WriteConfig  0x2
 *         KeyConfig0 to 0x3300
 *           - Private      1
 *           - PubInfo      1
 *           - KeyType  0b100
 *           - Lockable     1
 *           - ReqRandom    0
 *           - ReqAuth      0
 *           - AuthKey    0x0
 *           - RFU        0x0
 * \return status of the configuration
 */
uint8_t atca_check_private_key_slot0_config(void)
{
	// declared as "volatile" for easier debugging
	volatile uint8_t ret_code;

	// Slot configuration address for key (e.g. 48, 49)
	uint16_t slot_config_address = 20;

	const uint8_t read_config = 0x8F;
	const uint8_t write_config = 0x20;

	// Key configuration address for key (e.g. 48, 49)
	uint16_t key_config_address = 96;

	const uint8_t key_config_lsb = 0x33;
	const uint8_t key_config_msb = 0x00;

	// Make the command buffer the size of a Read command.
	uint8_t command[READ_COUNT];

	// Make the response buffer the minimum size of a Read response.
	uint8_t response[READ_4_RSP_SIZE];

	// Wake up the client device.
	ret_code = atca_wakeup(response);
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	// Read device configuration of SlotConfig0.
	memset(response, 0, sizeof(response));
	ret_code = atca_execute(ATCA_READ, ATCA_ZONE_CONFIG, slot_config_address >> 2, 0, NULL,
		0, NULL, 0, NULL, sizeof(command), command, sizeof(response), response);
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	// Check the configuration of SlotConfig0.
	if (response[ATCA_RSP_DATA_IDX] != read_config ||
		response[ATCA_RSP_DATA_IDX + 1] != write_config) {
		// The Slot have not been configured correctly.
		// Throw error code.
		atca_sleep();
		return ATCA_FUNC_FAIL;
	}

	// Read device configuration of KeyConfig0.
	memset(response, 0, sizeof(response));
	ret_code = atca_execute(ATCA_READ, ATCA_ZONE_CONFIG, key_config_address >> 2, 0, NULL, 0, NULL,
		0, NULL, sizeof(command), command, sizeof(response), response);
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	// Check the configuration of KeyConfig0.
	if (response[ATCA_RSP_DATA_IDX] != key_config_lsb ||
		response[ATCA_RSP_DATA_IDX + 1] != key_config_msb) {
		// The Key have not been configured correctly.
		// Throw error code.
		atca_sleep();
		return ATCA_FUNC_FAIL;
	}

	// For this example, lock should be done by using ACES.
	// This function is only to show users how to lock the configuration zone
	// using a library function.
#if defined(ATCA_EXAMPLE_CONFIG_WITH_LOCK)
	atca_sleep();
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	ret_code = atca_lock_config_zone(ATCA_HOST_ADDRESS);
#endif

	// Check the configuration zone lock status
	ret_code = atca_check_lock_status(ATCA_ZONE_CONFIG);

	return ret_code;
}

/** \brief This function checks the configuration of the device for verify external using B283 example.
 *
 *         SlotConfig0 to 0x8F20
 *           - ReadKey      0xF
 *           - NoMac          0
 *           - SingleUse      0
 *           - EncryptRead    0
 *           - IsSecret       1
 *           - WriteKey     0x0
 *           - WriteConfig  0x2
 *         KeyConfig0 to 0x2300
 *           - Private      1
 *           - PubInfo      1
 *           - KeyType  0b000
 *           - Lockable     1
 *           - ReqRandom    0
 *           - ReqAuth      0
 *           - AuthKey    0x0
 *           - RFU        0x0
 * \return status of the configuration
 */
uint8_t atca_check_private_key_slot0_config_283(void)
{
	// declared as "volatile" for easier debugging
	volatile uint8_t ret_code;

	// Slot configuration address for key (e.g. 48, 49)
	uint16_t slot_config_address = 20;

	const uint8_t read_config = 0x8F;
	const uint8_t write_config = 0x20;

	// Key configuration address for key (e.g. 48, 49)
	uint16_t key_config_address = 96;

	const uint8_t key_config_lsb = 0x23;
	const uint8_t key_config_msb = 0x00;

	// Make the command buffer the size of a Read command.
	uint8_t command[READ_COUNT];

	// Make the response buffer the minimum size of a Read response.
	uint8_t response[READ_4_RSP_SIZE];

	// Wake up the client device.
	ret_code = atca_wakeup(response);
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	// Read device configuration of SlotConfig0.
	memset(response, 0, sizeof(response));
	ret_code = atca_execute(ATCA_READ, ATCA_ZONE_CONFIG, slot_config_address >> 2, 0, NULL,
		0, NULL, 0, NULL, sizeof(command), command, sizeof(response), response);
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	// Check the configuration of SlotConfig0.
	if (response[ATCA_RSP_DATA_IDX] != read_config ||
		response[ATCA_RSP_DATA_IDX + 1] != write_config) {
		// The Slot have not been configured correctly.
		// Throw error code.
		atca_sleep();
		return ATCA_FUNC_FAIL;
	}

	// Read device configuration of KeyConfig0.
	memset(response, 0, sizeof(response));
	ret_code = atca_execute(ATCA_READ, ATCA_ZONE_CONFIG, key_config_address >> 2, 0, NULL, 0, NULL,
		0, NULL, sizeof(command), command, sizeof(response), response);
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	// Check the configuration of KeyConfig0.
	if (response[ATCA_RSP_DATA_IDX] != key_config_lsb ||
		response[ATCA_RSP_DATA_IDX + 1] != key_config_msb) {
		// The Key have not been configured correctly.
		// Throw error code.
		atca_sleep();
		return ATCA_FUNC_FAIL;
	}

	// For this example, lock should be done by using ACES.
	// This function is only to show users how to lock the configuration zone
	// using a library function.
#if defined(ATCA_EXAMPLE_CONFIG_WITH_LOCK)
	atca_sleep();
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	ret_code = atca_lock_config_zone(ATCA_HOST_ADDRESS);
#endif

	// Check the configuration zone lock status
	ret_code = atca_check_lock_status(ATCA_ZONE_CONFIG);

	return ret_code;
}


/** \brief This function serves as an example for authenticating a client by
 *         using MAC and CheckMac commands.
 *
 *  \return status of the operation
 */
uint8_t atca_checkmac_device(void)
{
	// declared as "volatile" for easier debugging
	volatile uint8_t ret_code;

	// Make the command buffer the size of the CheckMac command.
	static uint8_t command[CHECKMAC_COUNT];

	// Make the response buffer the size of a MAC response.
	static uint8_t response_mac[MAC_RSP_SIZE];

	// First four bytes of Mac command are needed for CheckMac command.
	static uint8_t other_data[CHECKMAC_OTHER_DATA_SIZE];

	// CheckMac response buffer
	static uint8_t response_checkmac[CHECKMAC_RSP_SIZE];

	uint8_t response[READ_4_RSP_SIZE];


	// data for challenge in MAC mode 0 command
	const uint8_t challenge[MAC_CHALLENGE_SIZE] = {
		0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
		0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
		0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
		0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
	};

	// Initialize the hardware interface.
	// Depending on which interface you have linked the
	// library to, it initializes SWI UART, SWI GPIO, or TWI.
	atca_init();

	// The following command sequence wakes up the device, issues a MAC command in mode 0
	// using the Command Marshaling layer, puts the device to sleep.
	// Then it wakes up the same device, issues a CheckMac command supplying data obtained
	// from the previous Mac command, verifies the response status byte, and puts the
	// device to sleep.

	ret_code = atca_wakeup(response);
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	// Mac command with mode = 0.
	memset(response_mac, 0, sizeof(response_mac));
	ret_code = atca_execute(ATCA_MAC, MAC_MODE_CHALLENGE, ATCA_KEY_ID,
		sizeof(challenge), (uint8_t *)challenge, 0, NULL, 0, NULL,
		sizeof(command), command, sizeof(response_mac), response_mac);
	// Put client device to sleep.
	atca_sleep();
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	// Now check the MAC using the CheckMac command.

	ret_code = atca_wakeup(response);
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	// CheckMac command with mode = 0.
	memset(response_checkmac, 0, sizeof(response_checkmac));
	// Copy Mac command byte 1 to 5 (op-code, param1, param2) to other_data.
	memcpy(other_data, &command[ATCA_OPCODE_IDX], CHECKMAC_CLIENT_COMMAND_SIZE);
	// Set the remaining nine bytes of other_data to 0.
	memset(&other_data[CHECKMAC_CLIENT_COMMAND_SIZE - 1], 0, sizeof(other_data) - CHECKMAC_CLIENT_COMMAND_SIZE);
	ret_code = atca_execute(ATCA_CHECKMAC, CHECKMAC_MODE_CHALLENGE,
		ATCA_KEY_ID, sizeof(challenge), (uint8_t *)challenge,
		CHECKMAC_CLIENT_RESPONSE_SIZE, &response_mac[ATCA_RSP_DATA_IDX],
		sizeof(other_data), other_data, sizeof(command), command,
		sizeof(response_checkmac), response_checkmac);

	// Put host device to sleep.
	atca_sleep();

	// Status byte = 0 means success. This line serves only a debug purpose.
	ret_code = response_checkmac[ATCA_RSP_DATA_IDX];

	return ret_code;
}


/** \brief This function serves as an example for authenticating a client by
 *         using System software
 *
 *  \return status of the operation
 */
uint8_t atca_checkmac_firmware(void)
{
	// declared as "volatile" for easier debugging
	volatile uint8_t ret_code;
	uint8_t i;
	uint8_t comparison_result;
	uint8_t mac_mode = MAC_MODE_BLOCK1_TEMPKEY | MAC_MODE_BLOCK2_TEMPKEY;
	struct atca_nonce_in_out nonce_param;	//parameter for nonce helper function
	struct atca_gen_dig_in_out gendig_param;	//parameter for gendig helper function
	struct atca_mac_in_out mac_param;		//parameter for mac helper function
	struct atca_temp_key tempkey;			//tempkey parameter for nonce and mac helper function
	static uint8_t wakeup_response[ATCA_RSP_SIZE_MIN];
	static uint8_t tx_buffer[CHECKMAC_COUNT];
	static uint8_t rx_buffer[MAC_RSP_SIZE];
	static uint8_t mac[CHECKMAC_CLIENT_RESPONSE_SIZE];
	uint8_t num_in[NONCE_NUMIN_SIZE] = {
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
		0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
		0x30, 0x31, 0x32, 0x33
	};
	uint8_t key_slot_0[ATCA_KEY_SIZE] = {
		0x00, 0x00, 0xA1, 0xAC, 0x57, 0xFF, 0x40, 0x4E,
		0x45, 0xD4, 0x04, 0x01, 0xBD, 0x0E, 0xD3, 0xC6,
		0x73, 0xD3, 0xB7, 0xB8, 0x2D, 0x85, 0xD9, 0xF3,
		0x13, 0xB5, 0x5E, 0xDA, 0x3D, 0x94, 0x00, 0x00
	};

	// Initialize the hardware interface.
	// Depending on which interface you have linked the
	// library to, it initializes SWI UART, SWI GPIO, or TWI.
	atca_init();

	// Check the lock status. If the device has been configured, then the configuration
	// zone must have been locked.
	ret_code = atca_check_lock_status(ATCA_ZONE_CONFIG);
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	// The following command sequence wakes up the device, issues a Nonce, a GenDig, and
	// a Mac command and puts the device to sleep.
	// In parallel, it calculates in firmware the TempKey and the MAC using helper
	// functions and compares the Mac command response with the calculated result.

	// ----------------------- Nonce --------------------------------------------
	// Wake up the device.
	memset(wakeup_response, 0, sizeof(wakeup_response));
	ret_code = atca_wakeup(wakeup_response);
	if (ret_code != ATCA_SUCCESS) {
		(void)atca_sleep();
		return ret_code;
	}

	// Issue a Nonce command. When the configuration zone of the device is not locked the
	// random number returned is a constant 0xFFFF0000FFFF0000...
	memset(rx_buffer, 0, sizeof(rx_buffer));
	ret_code = atca_execute(ATCA_NONCE, NONCE_MODE_NO_SEED_UPDATE, 0,
		NONCE_NUMIN_SIZE, num_in, 0, NULL, 0, NULL, sizeof(tx_buffer),
		tx_buffer, sizeof(rx_buffer), rx_buffer);
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	// Put device into Idle mode since the TempKey calculation in firmware might take longer
	// than the device timeout. Putting the device into Idle instead of Sleep mode
	// maintains the TempKey.
	atca_idle();

	// Calculate TempKey using helper function.
	nonce_param.mode = NONCE_MODE_NO_SEED_UPDATE;
	nonce_param.num_in = num_in;
	nonce_param.rand_out = &rx_buffer[ATCA_RSP_DATA_IDX];
	nonce_param.temp_key = &tempkey;
	ret_code = atca_nonce(&nonce_param);
	if (ret_code != ATCA_SUCCESS) {
		atca_wakeup_sleep(wakeup_response);
		return ret_code;
	}

	// ----------------------- GenDig --------------------------------------------
	// Wake up the device from Idle mode.
	memset(wakeup_response, 0, sizeof(wakeup_response));
	ret_code = atca_wakeup(wakeup_response);
	if (ret_code != ATCA_SUCCESS) {
		(void)atca_sleep();
		return ret_code;
	}

	memset(rx_buffer, 0, sizeof(rx_buffer));
	ret_code = atca_execute(ATCA_GENDIG, GENDIG_ZONE_DATA, ATCA_KEY_ID,
		0, NULL, 0, NULL, 0, NULL, sizeof(tx_buffer), tx_buffer,
		sizeof(rx_buffer), rx_buffer);
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}
	// Check response status byte for error.
	if (rx_buffer[ATCA_RSP_DATA_IDX] != ATCA_SUCCESS) {
		(void)atca_sleep();
		return ret_code;
	}
	atca_idle();

	// Update TempKey using helper function.
	gendig_param.zone = GENDIG_ZONE_DATA;
	gendig_param.key_id = ATCA_KEY_ID;
	gendig_param.stored_value = key_slot_0;
	gendig_param.temp_key = &tempkey;
	ret_code = atca_gen_dig(&gendig_param);
	if (ret_code != ATCA_SUCCESS) {
		atca_wakeup_sleep(wakeup_response);
		return ret_code;
	}

	// ----------------------- Mac --------------------------------------------
	// Wake up the device from Idle mode.
	memset(wakeup_response, 0, sizeof(wakeup_response));
	ret_code = atca_wakeup(wakeup_response);
	if (ret_code != ATCA_SUCCESS) {
		(void)atca_sleep();
		return ret_code;
	}

	// Issue a Mac command with mode = 3.
	memset(rx_buffer, 0, sizeof(rx_buffer));
	ret_code = atca_execute(ATCA_MAC, mac_mode, ATCA_KEY_ID, 0, NULL, 0,
		NULL, 0, NULL, sizeof(tx_buffer), tx_buffer, sizeof(rx_buffer),
		rx_buffer);
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}
	atca_sleep();

	// Calculate MAC using helper function.
	mac_param.mode = mac_mode;
	mac_param.key_id = ATCA_KEY_ID;
	mac_param.challenge = NULL;
	mac_param.key = NULL;
	mac_param.otp = NULL;
	mac_param.sn = NULL;
	mac_param.response = mac;
	mac_param.temp_key = &tempkey;
	ret_code = atca_mac(&mac_param);
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	// Compare the Mac response with the calculated MAC.
	// Compare all bytes before exiting loop to prevent timing attack.
	comparison_result = 0;
	for (i = 0; i < sizeof(mac); i++) {
		comparison_result |= (rx_buffer[i + ATCA_RSP_DATA_IDX] != mac[i]);
	}
	ret_code = comparison_result ? ATCA_GEN_FAIL : ATCA_SUCCESS;

	return ret_code;
}

/**
 * \brief Perform an encrypted read using a random nonce.
 *
 * \param[in] slot            Slot to read to.
 * \param[in] block           Block with the slot to read to.
 * \param[in] read_key_slot   Slot specified by the SlotConfig.ReadKey parameter for the slot being read to.
 * \param[in] read_key        The read key itself (32 bytes).
 * \param[in] num_in          Host value incorperated into the generation of the random nonce (20 bytes).
 * \param[out] data           Decrypted data by XOR with session key (32 bytes).
 */
uint8_t atca_read_encrytped(uint8_t slot, uint8_t block, uint8_t read_key_slot,
	const uint8_t read_key[32], const uint8_t num_in[20], uint8_t *data)
{
	uint8_t ret = ATCA_SUCCESS;
	uint16_t read_key_id = read_key_slot;
	struct atca_nonce_in_out nonce_param;
	struct atca_gen_dig_in_out gendig_param;
	struct atca_temp_key tempkey;
	static uint8_t wakeup_response[ATCA_RSP_SIZE_MIN];
	static uint8_t tx_buffer[CHECKMAC_COUNT];
	static uint8_t rx_buffer[MAC_RSP_SIZE];
	static uint8_t cipher_text[ATCA_KEY_SIZE];

	memset(wakeup_response, 0, sizeof(wakeup_response));
	ret = atca_wakeup(wakeup_response);
	if (ret != ATCA_SUCCESS) {
		(void)atca_sleep();
		return ret;
	}

	memset(rx_buffer, 0, sizeof(rx_buffer));
	ret = atca_execute(ATCA_NONCE, NONCE_MODE_SEED_UPDATE, 0, NONCE_NUMIN_SIZE, (uint8_t *)num_in,
		0, NULL, 0, NULL, sizeof(tx_buffer), tx_buffer, sizeof(rx_buffer), rx_buffer);
	if (ret != ATCA_SUCCESS)
		return ret;

	atca_idle();

	nonce_param.mode = NONCE_MODE_SEED_UPDATE;
	nonce_param.num_in = (uint8_t *)num_in;
	nonce_param.rand_out = &rx_buffer[ATCA_RSP_DATA_IDX];
	nonce_param.temp_key = &tempkey;

	ret = atca_nonce(&nonce_param);
	if (ret != ATCA_SUCCESS) {
		atca_wakeup_sleep(wakeup_response);
		return ret;
	}

	memset(wakeup_response, 0, sizeof(wakeup_response));
	ret = atca_wakeup(wakeup_response);
	if (ret != ATCA_SUCCESS) {
		(void)atca_sleep();
		return ret;
	}

	memset(rx_buffer, 0, sizeof(rx_buffer));
	ret = atca_execute(ATCA_GENDIG, GENDIG_ZONE_DATA, read_key_id, 0, NULL, 0,
		NULL, 0, NULL, sizeof(tx_buffer), tx_buffer, sizeof(rx_buffer), rx_buffer);
	if (ret != ATCA_SUCCESS)
		return ret;

	atca_idle();

	memset(rx_buffer, 0, sizeof(rx_buffer));
	ret = atca_read_zone(ATCA_ZONE_DATA, slot, block, 0, rx_buffer, ATCA_BLOCK_SIZE);
	if (ret != ATCA_SUCCESS)
		return ret;
	memcpy(cipher_text, rx_buffer, ATCA_KEY_SIZE);

	gendig_param.zone = GENDIG_ZONE_DATA;
	gendig_param.key_id = read_key_id;
	gendig_param.stored_value = (uint8_t *)read_key;
	gendig_param.temp_key = &tempkey;

	ret = atca_gen_dig(&gendig_param);
	if (ret != ATCA_SUCCESS) {
		atca_wakeup_sleep(wakeup_response);
		return ret;
	}

	for (uint8_t i = 0; i < ATCA_KEY_SIZE; i++)
		data[i] = cipher_text[i] ^ tempkey.value[i];

	return ret;
}

/**
 * \brief Perform an encrypted write using a random nonce.
 *
 * \param[in] slot            Slot to write to.
 * \param[in] block           Block with the slot to write to.
 * \param[in] plain_text      Data to be written to the specified slot and block (32 bytes).
 * \param[in] write_key_slot  Slot specified by the SlotConfig.WriteKey parameter for the slot being written to.
 * \param[in] write_key       The write key itself (32 bytes).
 * \param[in] num_in          Host value incorperated into the generation of the random nonce (20 bytes).
 */
uint8_t atca_write_encrytped(uint8_t slot, uint8_t block, const uint8_t plain_text[32],
	uint8_t write_key_slot, const uint8_t write_key[32], const uint8_t num_in[20])
{
	uint8_t ret = ATCA_SUCCESS;
	uint16_t write_key_id = write_key_slot;
	uint16_t addr = 0x0000;
	struct atca_nonce_in_out nonce_param;
	struct atca_gen_dig_in_out gendig_param;
	struct atca_temp_key tempkey;
	static uint8_t wakeup_response[ATCA_RSP_SIZE_MIN];
	static uint8_t tx_buffer[CHECKMAC_COUNT];
	static uint8_t rx_buffer[MAC_RSP_SIZE];
	static uint8_t cipher_text[SHA_DIGEST_SIZE];

	memset(wakeup_response, 0, sizeof(wakeup_response));
	ret = atca_wakeup(wakeup_response);
	if (ret != ATCA_SUCCESS) {
		(void)atca_sleep();
		return ret;
	}

	memset(rx_buffer, 0, sizeof(rx_buffer));
	ret = atca_execute(ATCA_NONCE, NONCE_MODE_SEED_UPDATE, 0, NONCE_NUMIN_SIZE, (uint8_t *)num_in,
		0, NULL, 0, NULL, sizeof(tx_buffer), tx_buffer, sizeof(rx_buffer), rx_buffer);
	if (ret != ATCA_SUCCESS)
		return ret;

	atca_idle();

	nonce_param.mode = NONCE_MODE_SEED_UPDATE;
	nonce_param.num_in = (uint8_t *)num_in;
	nonce_param.rand_out = &rx_buffer[ATCA_RSP_DATA_IDX];
	nonce_param.temp_key = &tempkey;

	ret = atca_nonce(&nonce_param);
	if (ret != ATCA_SUCCESS) {
		atca_wakeup_sleep(wakeup_response);
		return ret;
	}

	memset(wakeup_response, 0, sizeof(wakeup_response));
	ret = atca_wakeup(wakeup_response);
	if (ret != ATCA_SUCCESS) {
		(void)atca_sleep();
		return ret;
	}

	memset(rx_buffer, 0, sizeof(rx_buffer));
	ret = atca_execute(ATCA_GENDIG, GENDIG_ZONE_DATA, write_key_id, 0, NULL, 0,
		NULL, 0, NULL, sizeof(tx_buffer), tx_buffer, sizeof(rx_buffer), rx_buffer);
	if (ret != ATCA_SUCCESS)
		return ret;

	atca_idle();

	gendig_param.zone = GENDIG_ZONE_DATA;
	gendig_param.key_id = write_key_id;
	gendig_param.stored_value = (uint8_t *)write_key;
	gendig_param.temp_key = &tempkey;

	ret = atca_gen_dig(&gendig_param);
	if (ret != ATCA_SUCCESS) {
		atca_wakeup_sleep(wakeup_response);
		return ret;
	}

	for (uint8_t i = 0; i < ATCA_KEY_SIZE; i++)
		cipher_text[i] = plain_text[i] ^ tempkey.value[i];

	memset(wakeup_response, 0, sizeof(wakeup_response));
	ret = atca_wakeup(wakeup_response);
	if (ret != ATCA_SUCCESS) {
		(void)atca_sleep();
		return ret;
	}

	ret = atca_get_addr(ATCA_ZONE_DATA, slot, block, 0, &addr);
	if (ret != ATCA_SUCCESS)
		return ret;

	gendig_param.zone = ATCA_ZONE_COUNT_FLAG | ATCA_ZONE_DATA;
	gendig_param.key_id = addr;
	gendig_param.stored_value = (uint8_t *)plain_text;
	gendig_param.temp_key = &tempkey;
	ret = atca_gen_mac(&gendig_param);
	if (ret != ATCA_SUCCESS)
		return ret;

	ret = atca_write_slot(ATCA_ZONE_DATA, slot, block, cipher_text, tempkey.value, ATCA_KEY_SIZE);
	if (ret != ATCA_SUCCESS)
		return ret;

	return ret;
}

/** \brief This function serves as an example for verifying a signature by using
 *         Sign and Verify commands.
 *
 *         First, Slot0 is configured to store the P256 private key. The SlotConfig0
 *         is configured to 0x8F20, while KeyConfig is configured to 0x3300. Then
 *         the Configuration need to be locked to enable GenKey command to write
 *         the private key to the slot.
 *         The next sequence is to generate a Signature using Sign command. The
 *         message to be signed is passed to the device by using Nonce command in
 *         passthrough mode.
 *         The following sequence is to verify the signature using Verify command.
 *         The message to be verified is passed to the device using Nonce command,
 *         while the signature and the public key is passed as input of Verify
 *         command.
 * \return status of the operation
 */
uint8_t atca_verify_external(void)
{
	// declared as "volatile" for easier debugging
	volatile uint8_t ret_code;

	// Make the command buffer the size of the Verify command.
	static uint8_t command[VERIFY_256_EXTERNAL_COUNT];

	// Random response buffer
	static uint8_t response_random[RANDOM_RSP_SIZE];

	// Make the response buffer the minimum size.
	static uint8_t response_status[ATCA_RSP_SIZE_MIN];

	// GenKey response buffer
	static uint8_t response_genkey[GENKEY_RSP_SIZE_LONG];

	// Dymmy random response buffer
	static uint8_t response_dummy_random[RANDOM_RSP_SIZE];

	// Sign response buffer
	static uint8_t response_sign[SIGN_RSP_SIZE];

	static uint8_t wakeup_response[ATCA_RSP_SIZE_MIN];

	// Initialize the hardware interface.
	// Depending on which interface you have linked the
	// library to, it initializes SWI UART, SWI GPIO, or TWI.
	atca_init();

#if defined(ATCA_EXAMPLE_ACTIVATE_GPIO_AUTH_MODE) && (defined(ATCA_SWI_UART) || defined(ATCA_SWI_BITBANG))
	// Set the GPIO in Authorization Output mode
	ret_code = atca_activate_gpio_auth_mode(ATCA_SET_HIGH, ATCA_KEY_ID);
	if (ret_code != ATCA_SUCCESS) {
		if (ret_code == ATCA_FUNC_FAIL) {
			// The configuration zone has been locked.
			// Do nothing
		} else {
			return ret_code;
		}
	}
#endif

	// Check configuration of Slot0 to store private key for signing process.
	ret_code = atca_check_private_key_slot0_config();
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	// Obtain random challenge from host device
	ret_code = atca_wakeup(wakeup_response);
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	ret_code = atca_execute(ATCA_RANDOM, RANDOM_SEED_UPDATE, 0x0000, 0,
		NULL, 0, NULL, 0, NULL, sizeof(command), command,
		sizeof(response_random), response_random);

	// Put host device to sleep.
	atca_sleep();
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	// Wake up client device
	ret_code = atca_wakeup(wakeup_response);
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	memset(response_genkey, 0, sizeof(response_genkey));
#ifdef ATCA_EXAMPLE_GENERATE_PRIVATE_KEY
	// Generate Private Key on slot0 using GenKey command with mode = 0x04.
	// This step is required if slot0 has not been programmed with private key.
	ret_code = atca_execute(ATCA_GENKEY, GENKEY_MODE_PRIVATE,
		ATCA_KEY_ID, 0, NULL, 0, NULL, 0, NULL, sizeof(command),
		command, sizeof(response_genkey), response_genkey);
#else
	// Generate only public key from the existing private key
	ret_code = atca_execute(ATCA_GENKEY, GENKEY_MODE_PUBLIC,
		ATCA_KEY_ID, 0, NULL, 0, NULL, 0, NULL, sizeof(command),
		command, sizeof(response_genkey), response_genkey);
#endif
	if (ret_code != ATCA_SUCCESS) {
		(void)atca_sleep();
		return ret_code;
	}

	// Perform dummy random command for updating the random seed
	ret_code = atca_execute(ATCA_RANDOM, RANDOM_SEED_UPDATE, 0x0000, 0,
		NULL, 0, NULL, 0, NULL, sizeof(command), command,
		sizeof(response_dummy_random), response_dummy_random);
	if (ret_code != ATCA_SUCCESS) {
		(void)atca_sleep();
		return ret_code;
	}

	// Pass the message to be signed using Nonce command with mode = 0x03.
	memset(response_status, 0, sizeof(response_status));
	ret_code = atca_execute(ATCA_NONCE, NONCE_MODE_PASSTHROUGH,
		NONCE_MODE_RANDOM_OUT, NONCE_NUMIN_SIZE_PASSTHROUGH,
		(uint8_t *)&response_random[ATCA_RSP_DATA_IDX], 0, NULL,
		0, NULL, sizeof(command), command, sizeof(response_status),
		response_status);
	ret_code = atca_check_response_status(ret_code, response_status);
	if (ret_code != ATCA_SUCCESS) {
		(void)atca_sleep();
		return ret_code;
	}

	// Sign the message using Sign command with mode = 0x80.
	memset(response_sign, 0, sizeof(response_sign));
	ret_code = atca_execute(ATCA_SIGN, SIGN_MODE_EXTERNAL, ATCA_KEY_ID,
		0, NULL, 0, NULL, 0, NULL, sizeof(command), command,
		sizeof(response_sign), response_sign);

	// Put client device to sleep.
	atca_sleep();
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	// Now check the Signature using the Verify command.

	ret_code = atca_wakeup(response_status);
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	// Pass the message which has been signed using Nonce command with mode = 0x03.
	memset(response_status, 0, sizeof(response_status));
	ret_code = atca_execute(ATCA_NONCE, NONCE_MODE_PASSTHROUGH,
		NONCE_MODE_RANDOM_OUT, NONCE_NUMIN_SIZE_PASSTHROUGH,
		(uint8_t *)&response_random[ATCA_RSP_DATA_IDX], 0, NULL,
		0, NULL, sizeof(command), command, sizeof(response_status),
		response_status);
	ret_code = atca_check_response_status(ret_code, response_status);
	if (ret_code != ATCA_SUCCESS) {
		(void)atca_sleep();
		return ret_code;
	}

	// Verify Signature by using Verify command with mode = 0x02.
	memset(response_status, 0, sizeof(response_status));
	ret_code = atca_execute(ATCA_VERIFY, VERIFY_MODE_EXTERNAL,
		VERIFY_KEY_P256, VERIFY_256_SIGNATURE_SIZE,
		&response_sign[ATCA_RSP_DATA_IDX], VERIFY_256_KEY_SIZE,
		&response_genkey[ATCA_RSP_DATA_IDX], 0, NULL,
		sizeof(command), command, sizeof(response_status),
		response_status);
	ret_code = atca_check_response_status(ret_code, response_status);
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	// Put host device to sleep.
	atca_sleep();

	return ret_code;
}


/** \brief This function serves as an example for verifying a signature by using
 *         Sign and Verify commands. In this example, a 283-bit key is used.
 *
 *         First, Slot0 is configured to store the B283 private key. The SlotConfig0
 *         is configured to 0x8F20, while KeyConfig is configured to 0x2300. Then
 *         the Configuration need to be locked to enable GenKey command to write
 *         the private key to the slot.
 *         The next sequence is to generate a Signature using Sign command. The
 *         message to be signed is passed to the device by using Nonce command in
 *         passthrough mode.
 *         The following sequence is to verify the signature using Verify command.
 *         The message to be verified is passed to the device using Nonce command,
 *         while the signature and the public key is passed as input of Verify
 *         command.
 * \return status of the operation
 */
uint8_t atca_verify_external_283(void)
{
	// declared as "volatile" for easier debugging
	volatile uint8_t ret_code;

	// Make the command buffer the size of the Verify command.
	static uint8_t command[VERIFY_283_EXTERNAL_COUNT];

	// Random response buffer
	static uint8_t response_random[RANDOM_RSP_SIZE];

	// Make the response buffer the minimum size.
	static uint8_t response_status[ATCA_RSP_SIZE_MIN];

	// GenKey response buffer
	static uint8_t response_genkey[GENKEY_RSP_SIZE_LONG];

	// Dymmy random response buffer
	static uint8_t response_dummy_random[RANDOM_RSP_SIZE];

	// Sign response buffer
	static uint8_t response_sign[SIGN_RSP_SIZE];

	// Initialize the hardware interface.
	// Depending on which interface you have linked the
	// library to, it initializes SWI UART, SWI GPIO, or TWI.
	atca_init();

#if defined(ATCA_EXAMPLE_ACTIVATE_GPIO_AUTH_MODE) && (defined(ATCA_SWI_UART) || defined(ATCA_SWI_BITBANG))
	// Set the GPIO in Authorization Output mode
	ret_code = atca_activate_gpio_auth_mode(ATCA_SET_HIGH, ATCA_KEY_ID);
	if (ret_code != ATCA_SUCCESS) {
		if (ret_code == ATCA_FUNC_FAIL) {
			// The configuration zone has been locked.
			// Do nothing
		} else {
			return ret_code;
		}
	}
#endif

	// Check configuration of Slot0 to store private key for signing process.
	ret_code = atca_check_private_key_slot0_config_283();
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	// Obtain random challenge from host device
	ret_code = atca_wakeup(response_status);
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	ret_code = atca_execute(ATCA_RANDOM, RANDOM_SEED_UPDATE, 0x0000, 0,
		NULL, 0, NULL, 0, NULL, sizeof(command), command,
		sizeof(response_random), response_random);

	// Put host device to sleep.
	atca_sleep();
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	// Wake up client device
	ret_code = atca_wakeup(response_status);
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	memset(response_genkey, 0, sizeof(response_genkey));
#ifdef ATCA_EXAMPLE_GENERATE_PRIVATE_KEY
	// Generate Private Key on slot0 using GenKey command with mode = 0x04.
	// This step is required if slot0 has not been programmed with private key.
	ret_code = atca_execute(ATCA_GENKEY, GENKEY_MODE_PRIVATE,
		ATCA_KEY_ID, 0, NULL, 0, NULL, 0, NULL, sizeof(command),
		command, sizeof(response_genkey), response_genkey);
#else
	// Generate only public key from the existing private key
	ret_code = atca_execute(ATCA_GENKEY, GENKEY_MODE_PUBLIC,
		ATCA_KEY_ID, 0, NULL, 0, NULL, 0, NULL, sizeof(command),
		command, sizeof(response_genkey), response_genkey);
#endif
	if (ret_code != ATCA_SUCCESS) {
		(void)atca_sleep();
		return ret_code;
	}

	// Perform dummy random command for updating the random seed
	ret_code = atca_execute(ATCA_RANDOM, RANDOM_SEED_UPDATE, 0x0000, 0,
		NULL, 0, NULL, 0, NULL, sizeof(command), command,
		sizeof(response_dummy_random), response_dummy_random);
	if (ret_code != ATCA_SUCCESS) {
		(void)atca_sleep();
		return ret_code;
	}

	// Pass the message to be signed using Nonce command with mode = 0x03.
	memset(response_status, 0, sizeof(response_status));
	ret_code = atca_execute(ATCA_NONCE, NONCE_MODE_PASSTHROUGH,
		NONCE_MODE_RANDOM_OUT, NONCE_NUMIN_SIZE_PASSTHROUGH,
		(uint8_t *)&response_random[ATCA_RSP_DATA_IDX], 0, NULL,
		0, NULL, sizeof(command), command, sizeof(response_status),
		response_status);
	ret_code = atca_check_response_status(ret_code, response_status);
	if (ret_code != ATCA_SUCCESS) {
		(void)atca_sleep();
		return ret_code;
	}

	// Sign the message using Sign command with mode = 0x80.
	memset(response_sign, 0, sizeof(response_sign));
	ret_code = atca_execute(ATCA_SIGN, SIGN_MODE_EXTERNAL, ATCA_KEY_ID,
		0, NULL, 0, NULL, 0, NULL, sizeof(command), command,
		sizeof(response_sign), response_sign);

	// Put client device to sleep.
	atca_sleep();
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	// Now check the Signature using the Verify command.

	ret_code = atca_wakeup(response_status);
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	// Pass the message which has been signed using Nonce command with mode = 0x03.
	memset(response_status, 0, sizeof(response_status));
	ret_code = atca_execute(ATCA_NONCE, NONCE_MODE_PASSTHROUGH,
		NONCE_MODE_RANDOM_OUT, NONCE_NUMIN_SIZE_PASSTHROUGH,
		(uint8_t *)&response_random[ATCA_RSP_DATA_IDX], 0, NULL,
		0, NULL, sizeof(command), command, sizeof(response_status),
		response_status);
	ret_code = atca_check_response_status(ret_code, response_status);
	if (ret_code != ATCA_SUCCESS) {
		(void)atca_sleep();
		return ret_code;
	}

	// Verify Signature by using Verify command with mode = 0x02.
	memset(response_status, 0, sizeof(response_status));
	ret_code = atca_execute(ATCA_VERIFY, VERIFY_MODE_EXTERNAL,
		VERIFY_KEY_B283, VERIFY_283_SIGNATURE_SIZE,
		&response_sign[ATCA_RSP_DATA_IDX], VERIFY_283_KEY_SIZE,
		&response_genkey[ATCA_RSP_DATA_IDX], 0, NULL,
		sizeof(command), command, sizeof(response_status),
		response_status);
	ret_code = atca_check_response_status(ret_code, response_status);
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	// Put host device to sleep.
	atca_sleep();

	return ret_code;
}


/** \brief This function serves as an example for verifying a signature by using
 *         host algorithm.
 *
 *         First, Slot0 is configured to store the P256 private key. The SlotConfig0
 *         is configured to 0x8F20, while KeyConfig is configured to 0x3300. Then
 *         the Configuration need to be locked to enable GenKey command to write
 *         the private key to the slot.
 *         The next sequence is to generate a Signature using Sign command. The
 *         message to be signed is passed to the device by using Nonce command in
 *         passthrough mode.
 *         The following sequence is to verify the signature using host algorithm.
 *         The message to be verified is passed to the device using Nonce software,
 *         while the signature and the public key is passed as input of Verify
 *         software.
 * \return status of the operation
 */
uint8_t atca_verify_host(void)
{
	// declared as "volatile" for easier debugging
	volatile uint8_t ret_code;

	// Make the command buffer the size of the Verify command.
	static uint8_t command[VERIFY_256_EXTERNAL_COUNT];

	// Random response buffer
	static uint8_t response_random[RANDOM_RSP_SIZE];

	// Make the response buffer the minimum size.
	static uint8_t response_status[ATCA_RSP_SIZE_MIN];

	// GenKey response buffer
	static uint8_t response_genkey[GENKEY_RSP_SIZE_LONG];

	// Dymmy random response buffer
	static uint8_t response_dummy_random[RANDOM_RSP_SIZE];

	// Sign response buffer
	static uint8_t response_sign[SIGN_RSP_SIZE];

	// Parameter for nonce helper function
	struct atca_nonce_in_out nonce_param;

	// Tempkey parameter for nonce and verify helper function
	struct atca_temp_key tempkey;

	// Parameter for verify helper function
	struct atca_verify_in_out verify_param;

	// Initialize the hardware interface.
	// Depending on which interface you have linked the
	// library to, it initializes SWI UART, SWI GPIO, or TWI.
	atca_init();

	// Check configuration of Slot0 to store private key for signing process.
	ret_code = atca_check_private_key_slot0_config();
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	// Generate random challenge
	ret_code = atca_wakeup(response_status);
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	ret_code = atca_execute(ATCA_RANDOM, RANDOM_SEED_UPDATE, 0x0000, 0,
		NULL, 0, NULL, 0, NULL, sizeof(command), command,
		sizeof(response_random), response_random);

	memset(response_genkey, 0, sizeof(response_genkey));
#ifdef ATCA_EXAMPLE_GENERATE_PRIVATE_KEY
	// Generate Private Key on slot0 using GenKey command with mode = 0x04.
	// This step is required if slot0 has not been programmed with private key.
	ret_code = atca_execute(ATCA_GENKEY, GENKEY_MODE_PRIVATE,
		ATCA_KEY_ID, 0, NULL, 0, NULL, 0, NULL, sizeof(command),
		command, sizeof(response_genkey), response_genkey);
#else
	// Generate only public key from the existing private key
	ret_code = atca_execute(ATCA_GENKEY, GENKEY_MODE_PUBLIC,
		ATCA_KEY_ID, 0, NULL, 0, NULL, 0, NULL, sizeof(command),
		command, sizeof(response_genkey), response_genkey);
#endif
	if (ret_code != ATCA_SUCCESS) {
		(void)atca_sleep();
		return ret_code;
	}

	// Perform dummy random command for updating the random seed
	ret_code = atca_execute(ATCA_RANDOM, RANDOM_SEED_UPDATE, 0x0000, 0,
		NULL, 0, NULL, 0, NULL, sizeof(command), command,
		sizeof(response_dummy_random), response_dummy_random);
	if (ret_code != ATCA_SUCCESS) {
		(void)atca_sleep();
		return ret_code;
	}

	// Pass the message to be signed using Nonce command with mode = 0x03.
	memset(response_status, 0, sizeof(response_status));
	ret_code = atca_execute(ATCA_NONCE, NONCE_MODE_PASSTHROUGH,
		NONCE_MODE_RANDOM_OUT, NONCE_NUMIN_SIZE_PASSTHROUGH,
		(uint8_t *)&response_random[ATCA_RSP_DATA_IDX], 0, NULL,
		0, NULL, sizeof(command), command, sizeof(response_status),
		response_status);
	ret_code = atca_check_response_status(ret_code, response_status);
	if (ret_code != ATCA_SUCCESS) {
		(void)atca_sleep();
		return ret_code;
	}

	// Sign the message using Sign command with mode = 0x80.
	memset(response_sign, 0, sizeof(response_sign));
	ret_code = atca_execute(ATCA_SIGN, SIGN_MODE_EXTERNAL, ATCA_KEY_ID,
		0, NULL, 0, NULL, 0, NULL, sizeof(command), command,
		sizeof(response_sign), response_sign);

	// Put the device to sleep.
	atca_sleep();
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	// Now check the Signature using the Verify command.

	// Pass the message which has been signed using Nonce in software
	// Execute Nonce using helper function.
	nonce_param.mode = NONCE_MODE_PASSTHROUGH;
	nonce_param.num_in = &response_random[ATCA_RSP_DATA_IDX];
	nonce_param.temp_key = &tempkey;
	ret_code = atca_nonce(&nonce_param);
	if (ret_code != ATCA_SUCCESS) {
		atca_wakeup_sleep(response_status);
		return ret_code;
	}

	// Verify Signature by using Verify in software
	verify_param.curve_type = VERIFY_KEY_P256;
	verify_param.signature = &response_sign[ATCA_RSP_DATA_IDX];
	verify_param.public_key = &response_genkey[ATCA_RSP_DATA_IDX];
	verify_param.temp_key = &tempkey;
	ret_code = atca_verify(&verify_param);
	if (ret_code != ATCA_SUCCESS) {
		atca_wakeup_sleep(response_status);
		return ret_code;
	}

	return ret_code;
}


/** \brief This function serves as an example for verifying a signature by using
 *         host algorithm. The kry type used is B283
 *
 *         First, Slot0 is configured to store the B283 private key. The SlotConfig0
 *         is configured to 0x8F20, while KeyConfig is configured to 0x2300. Then
 *         the Configuration need to be locked to enable GenKey command to write
 *         the private key to the slot.
 *         The next sequence is to generate a Signature using Sign command. The
 *         message to be signed is passed to the device by using Nonce command in
 *         passthrough mode.
 *         The following sequence is to verify the signature using host algorithm.
 *         The message to be verified is passed to the device using Nonce software,
 *         while the signature and the public key is passed as input of Verify
 *         software.
 * \return status of the operation
 */
uint8_t atca_verify_host_283(void)
{
	// declared as "volatile" for easier debugging
	volatile uint8_t ret_code;

	// Make the command buffer the size of the Verify command.
	static uint8_t command[VERIFY_283_EXTERNAL_COUNT];

	// Random response buffer
	static uint8_t response_random[RANDOM_RSP_SIZE];

	// Make the response buffer the minimum size.
	static uint8_t response_status[ATCA_RSP_SIZE_MIN];

	// GenKey response buffer
	static uint8_t response_genkey[GENKEY_RSP_SIZE_LONG];

	// Dymmy random response buffer
	static uint8_t response_dummy_random[RANDOM_RSP_SIZE];

	// Sign response buffer
	static uint8_t response_sign[SIGN_RSP_SIZE];

	// Parameter for nonce helper function
	struct atca_nonce_in_out nonce_param;

	// Tempkey parameter for nonce and verify helper function
	struct atca_temp_key tempkey;

	// Parameter for verify helper function
	struct atca_verify_in_out verify_param;

	// Initialize the hardware interface.
	// Depending on which interface you have linked the
	// library to, it initializes SWI UART, SWI GPIO, or TWI.
	atca_init();

	// Check configuration of Slot0 to store private key for signing process.
	ret_code = atca_check_private_key_slot0_config_283();
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	// Generate random challenge
	ret_code = atca_wakeup(response_status);
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	ret_code = atca_execute(ATCA_RANDOM, RANDOM_SEED_UPDATE, 0x0000, 0,
		NULL, 0, NULL, 0, NULL, sizeof(command), command,
		sizeof(response_random), response_random);

	memset(response_genkey, 0, sizeof(response_genkey));
#ifdef ATCA_EXAMPLE_GENERATE_PRIVATE_KEY
	// Generate Private Key on slot0 using GenKey command with mode = 0x04.
	// This step is required if slot0 has not been programmed with private key.
	ret_code = atca_execute(ATCA_GENKEY, GENKEY_MODE_PRIVATE,
		ATCA_KEY_ID, 0, NULL, 0, NULL, 0, NULL, sizeof(command),
		command, sizeof(response_genkey), response_genkey);
#else
	// Generate only public key from the existing private key
	ret_code = atca_execute(ATCA_GENKEY, GENKEY_MODE_PUBLIC,
		ATCA_KEY_ID, 0, NULL, 0, NULL, 0, NULL, sizeof(command),
		command, sizeof(response_genkey), response_genkey);
#endif
	if (ret_code != ATCA_SUCCESS) {
		(void)atca_sleep();
		return ret_code;
	}

	// Perform dummy random command for updating the random seed
	ret_code = atca_execute(ATCA_RANDOM, RANDOM_SEED_UPDATE, 0x0000, 0,
		NULL, 0, NULL, 0, NULL, sizeof(command), command,
		sizeof(response_dummy_random), response_dummy_random);
	if (ret_code != ATCA_SUCCESS) {
		(void)atca_sleep();
		return ret_code;
	}

	// Pass the message to be signed using Nonce command with mode = 0x03.
	memset(response_status, 0, sizeof(response_status));
	ret_code = atca_execute(ATCA_NONCE, NONCE_MODE_PASSTHROUGH,
		NONCE_MODE_RANDOM_OUT, NONCE_NUMIN_SIZE_PASSTHROUGH,
		(uint8_t *)&response_random[ATCA_RSP_DATA_IDX], 0, NULL,
		0, NULL, sizeof(command), command, sizeof(response_status),
		response_status);
	ret_code = atca_check_response_status(ret_code, response_status);
	if (ret_code != ATCA_SUCCESS) {
		(void)atca_sleep();
		return ret_code;
	}

	// Sign the message using Sign command with mode = 0x80.
	memset(response_sign, 0, sizeof(response_sign));
	ret_code = atca_execute(ATCA_SIGN, SIGN_MODE_EXTERNAL, ATCA_KEY_ID,
		0, NULL, 0, NULL, 0, NULL, sizeof(command), command,
		sizeof(response_sign), response_sign);

	// Put the device to sleep.
	atca_sleep();
	if (ret_code != ATCA_SUCCESS) {
		return ret_code;
	}

	// Now check the Signature using the Verify command.

	// Pass the message which has been signed using Nonce in software
	// Execute Nonce using helper function.
	nonce_param.mode = NONCE_MODE_PASSTHROUGH;
	nonce_param.num_in = &response_random[ATCA_RSP_DATA_IDX];
	nonce_param.temp_key = &tempkey;
	ret_code = atca_nonce(&nonce_param);
	if (ret_code != ATCA_SUCCESS) {
		atca_wakeup_sleep(response_status);
		return ret_code;
	}

	// Verify Signature by using Verify in software
	verify_param.curve_type = VERIFY_KEY_B283;
	verify_param.signature = &response_sign[ATCA_RSP_DATA_IDX];
	verify_param.public_key = &response_genkey[ATCA_RSP_DATA_IDX];
	verify_param.temp_key = &tempkey;
	ret_code = atca_verify(&verify_param);
	if (ret_code != ATCA_SUCCESS) {
		atca_wakeup_sleep(response_status);
		return ret_code;
	}

	return ret_code;
}


/** \brief This function changes the I2C address of a device.
		   Running it will access the device with I2C address ATCA_HOST_ADDRESS
		   and change it to the desired address as long as the configuration zone is
		   not locked (byte under address 87 = 0x55). Be aware that bit 3 of the I2C address
		   is also used as a TTL enable bit. So make sure you give it a value that
		   agrees with your system (see data sheet).
		   * \param[in] i2c_address to be set
		   * \return status of the operation
		   */
uint8_t atca_config_i2c_address(uint8_t i2c_address)
{
	// declared as "volatile" for easier debugging
	volatile uint8_t ret_code;

	uint16_t config_address;

	// Make the command buffer the minimum size of the Write command.
	uint8_t command[WRITE_COUNT_SHORT];

	uint8_t config_data[ATCA_ZONE_ACCESS_4];

	// Make the response buffer the size of a Read response.
	uint8_t response[READ_4_RSP_SIZE];

	size_t config_data_size = 0;

	atca_init();

	ret_code = atca_wakeup(response);
	if (ret_code != ATCA_SUCCESS)
		return ret_code;

	// Make sure that configuration zone is not locked.
	memset(response, 0, sizeof(response));
	config_address = 84;
	ret_code = atca_execute(ATCA_READ, ATCA_ZONE_CONFIG, config_address >> 2, 0, NULL, 0, NULL,
		0, NULL, sizeof(command), command, sizeof(response), response);
	if (ret_code != ATCA_SUCCESS) {
		atca_sleep();
		return ret_code;
	}
	if (response[4] != 0x55) {
		// Configuration zone is locked. We cannot change the I2C address.
		atca_sleep();
		return ATCA_FUNC_FAIL;
	}

	// Read device configuration at address 16 that contains the I2C address.
	memset(response, 0, sizeof(response));
	config_address = 16;
	ret_code = atca_execute(ATCA_READ, ATCA_ZONE_CONFIG, config_address >> 2, 0, NULL, 0, NULL,
		0, NULL, sizeof(command), command, sizeof(response), response);
	if (ret_code != ATCA_SUCCESS) {
		atca_sleep();
		return ret_code;
	}
	config_data[0] = i2c_address;
	config_data_size = sizeof(config_data - 1);
	memcpy(&config_data[1], &response[ATCA_RSP_DATA_IDX + 1], config_data_size);

	// Write I2C address
	ret_code = atca_execute(ATCA_WRITE, ATCA_ZONE_CONFIG, config_address >> 2, sizeof(config_data),
		config_data, 0, NULL, 0, NULL, sizeof(command), command, sizeof(response), response);

	atca_sleep();
	if (ret_code != ATCA_SUCCESS)
		return ret_code;

#if defined(ATCA_I2C)
	// Check whether we had success.
	atca_set_device_address(i2c_address);
	ret_code = atca_wakeup(response);
	atca_sleep();
#endif

	return ret_code;
}


/** \brief This function reads all 128 bytes from the configuration zone.
Obtain the data by putting a breakpoint after every read and inspecting "response".

* \param[out] config_data pointer to all 128 bytes in configuration zone.
Not used if NULL.
* \return status of the operation
*/
uint8_t atca_read_config_zone(uint8_t* config_data)
{
	uint8_t ret = ATCA_SUCCESS;
	uint8_t bytes_read[ATCA_BLOCK_SIZE];
	uint8_t block = 0;
	uint8_t cpyIndex = 0;
	uint8_t offset = 0;
	uint8_t isEcc = (dev_rev.dev_revision & ECC508_MASK) || (dev_rev.dev_revision & ECC108_MASK);
	uint8_t configSize = isEcc ? ATCA_ECC_CONFIG_SIZE : ATCA_SHA_CONFIG_SIZE;

	do
	{
		// Check the config_data pointer
		if (bad_pointer(config_data)) break;
		memset(config_data, 0x00, configSize);

		// Read first 32 byte block.  Copy the bytes into the config_data buffer
		block = 0;
		ret = atca_read_zone(ATCA_ZONE_CONFIG, 0, block, 0, bytes_read, ATCA_BLOCK_SIZE);
		if (ret != ATCA_SUCCESS) break;
		memcpy(&config_data[cpyIndex], bytes_read, ATCA_BLOCK_SIZE);
		cpyIndex += ATCA_BLOCK_SIZE;

		// Read second 32 byte block.  Copy the bytes into the config_data buffer
		block = 1;
		ret = atca_read_zone(ATCA_ZONE_CONFIG, 0, block, 0, bytes_read, ATCA_BLOCK_SIZE);
		if (ret != ATCA_SUCCESS) break;
		memcpy(&config_data[cpyIndex], bytes_read, ATCA_BLOCK_SIZE);
		cpyIndex += ATCA_BLOCK_SIZE;

		// Write bytes [64:83] using five word writes
		block = 2;
		offset = 0;
		ret = atca_read_zone(ATCA_ZONE_CONFIG, 0, block, offset, bytes_read, ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;
		memcpy(&config_data[cpyIndex], bytes_read, ATCA_WORD_SIZE);
		cpyIndex += ATCA_WORD_SIZE;

		block = 2;
		offset = 1;
		ret = atca_read_zone(ATCA_ZONE_CONFIG, 0, block, offset, bytes_read, ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;
		memcpy(&config_data[cpyIndex], bytes_read, ATCA_WORD_SIZE);
		cpyIndex += ATCA_WORD_SIZE;

		block = 2;
		offset = 2;
		ret = atca_read_zone(ATCA_ZONE_CONFIG, 0, block, offset, bytes_read, ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;
		memcpy(&config_data[cpyIndex], bytes_read, ATCA_WORD_SIZE);
		cpyIndex += ATCA_WORD_SIZE;

		block = 2;
		offset = 3;
		ret = atca_read_zone(ATCA_ZONE_CONFIG, 0, block, offset, bytes_read, ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;
		memcpy(&config_data[cpyIndex], bytes_read, ATCA_WORD_SIZE);
		cpyIndex += ATCA_WORD_SIZE;

		block = 2;
		offset = 4;
		ret = atca_read_zone(ATCA_ZONE_CONFIG, 0, block, offset, bytes_read, ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;
		memcpy(&config_data[cpyIndex], bytes_read, ATCA_WORD_SIZE);
		cpyIndex += ATCA_WORD_SIZE;

		block = 2;
		offset = 5;
		ret = atca_read_zone(ATCA_ZONE_CONFIG, 0, block, offset, bytes_read, ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;
		memcpy(&config_data[cpyIndex], bytes_read, ATCA_WORD_SIZE);
		cpyIndex += ATCA_WORD_SIZE;



		if (isEcc)
		{
			block = 2;
			offset = 6;
			ret = atca_read_zone(ATCA_ZONE_CONFIG, 0, block, offset, bytes_read, ATCA_WORD_SIZE);
			if (ret != ATCA_SUCCESS) break;
			memcpy(&config_data[cpyIndex], bytes_read, ATCA_WORD_SIZE);
			cpyIndex += ATCA_WORD_SIZE;

			block = 2;
			offset = 7;
			ret = atca_read_zone(ATCA_ZONE_CONFIG, 0, block, offset, bytes_read, ATCA_WORD_SIZE);
			if (ret != ATCA_SUCCESS) break;
			memcpy(&config_data[cpyIndex], bytes_read, ATCA_WORD_SIZE);
			cpyIndex += ATCA_WORD_SIZE;

			// Read second 32 byte block.  Copy the bytes into the config_data buffer
			block = 3;
			ret = atca_read_zone(ATCA_ZONE_CONFIG, 0, block, 0, bytes_read, ATCA_BLOCK_SIZE);
			if (ret != ATCA_SUCCESS) break;
			memcpy(&config_data[cpyIndex], bytes_read, ATCA_BLOCK_SIZE);
			cpyIndex += ATCA_BLOCK_SIZE;
		}

	} while (0);

	return ret;
}

/** \brief This function reads all 128 bytes from the ECC508's configuration zone.
Obtain the data by putting a breakpoint after every read and inspecting "response".
* \param[out] config_data pointer to all 128 bytes in configuration zone.
Not used if NULL.
* \return status of the operation
*/
uint8_t atca_read_ecc508_config_zone(uint8_t* config_data)
{
	uint8_t ret = ATCA_SUCCESS;
	uint8_t bytes_read[ATCA_BLOCK_SIZE];
	uint8_t block = 0;
	uint8_t cpyIndex = 0;
	uint8_t offset = 0;
	uint8_t configSize = ATCA_ECC_CONFIG_SIZE;

	do
	{
		// Check the config_data pointer
		if (bad_pointer(config_data)) break;
		memset(config_data, 0x00, configSize);

		// Read first 32 byte block.  Copy the bytes into the config_data buffer
		block = 0;
		ret = atca_read_zone(ATCA_ZONE_CONFIG, 0, block, 0, bytes_read, ATCA_BLOCK_SIZE);
		if (ret != ATCA_SUCCESS) break;
		memcpy(&config_data[cpyIndex], bytes_read, ATCA_BLOCK_SIZE);
		cpyIndex += ATCA_BLOCK_SIZE;

		// Read second 32 byte block.  Copy the bytes into the config_data buffer
		block = 1;
		ret = atca_read_zone(ATCA_ZONE_CONFIG, 0, block, 0, bytes_read, ATCA_BLOCK_SIZE);
		if (ret != ATCA_SUCCESS) break;
		memcpy(&config_data[cpyIndex], bytes_read, ATCA_BLOCK_SIZE);
		cpyIndex += ATCA_BLOCK_SIZE;

		// Write bytes [64:83] using five word writes
		block = 2;
		offset = 0;
		ret = atca_read_zone(ATCA_ZONE_CONFIG, 0, block, offset, bytes_read, ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;
		memcpy(&config_data[cpyIndex], bytes_read, ATCA_WORD_SIZE);
		cpyIndex += ATCA_WORD_SIZE;

		block = 2;
		offset = 1;
		ret = atca_read_zone(ATCA_ZONE_CONFIG, 0, block, offset, bytes_read, ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;
		memcpy(&config_data[cpyIndex], bytes_read, ATCA_WORD_SIZE);
		cpyIndex += ATCA_WORD_SIZE;

		block = 2;
		offset = 2;
		ret = atca_read_zone(ATCA_ZONE_CONFIG, 0, block, offset, bytes_read, ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;
		memcpy(&config_data[cpyIndex], bytes_read, ATCA_WORD_SIZE);
		cpyIndex += ATCA_WORD_SIZE;

		block = 2;
		offset = 3;
		ret = atca_read_zone(ATCA_ZONE_CONFIG, 0, block, offset, bytes_read, ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;
		memcpy(&config_data[cpyIndex], bytes_read, ATCA_WORD_SIZE);
		cpyIndex += ATCA_WORD_SIZE;

		block = 2;
		offset = 4;
		ret = atca_read_zone(ATCA_ZONE_CONFIG, 0, block, offset, bytes_read, ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;
		memcpy(&config_data[cpyIndex], bytes_read, ATCA_WORD_SIZE);
		cpyIndex += ATCA_WORD_SIZE;

		block = 2;
		offset = 5;
		ret = atca_read_zone(ATCA_ZONE_CONFIG, 0, block, offset, bytes_read, ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;
		memcpy(&config_data[cpyIndex], bytes_read, ATCA_WORD_SIZE);
		cpyIndex += ATCA_WORD_SIZE;

		block = 2;
		offset = 6;
		ret = atca_read_zone(ATCA_ZONE_CONFIG, 0, block, offset, bytes_read, ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;
		memcpy(&config_data[cpyIndex], bytes_read, ATCA_WORD_SIZE);
		cpyIndex += ATCA_WORD_SIZE;

		block = 2;
		offset = 7;
		ret = atca_read_zone(ATCA_ZONE_CONFIG, 0, block, offset, bytes_read, ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;
		memcpy(&config_data[cpyIndex], bytes_read, ATCA_WORD_SIZE);
		cpyIndex += ATCA_WORD_SIZE;

		// Read second 32 byte block.  Copy the bytes into the config_data buffer
		block = 3;
		ret = atca_read_zone(ATCA_ZONE_CONFIG, 0, block, 0, bytes_read, ATCA_BLOCK_SIZE);
		if (ret != ATCA_SUCCESS) break;
		memcpy(&config_data[cpyIndex], bytes_read, ATCA_BLOCK_SIZE);
		cpyIndex += ATCA_BLOCK_SIZE;

	} while (0);

	return ret;
}

/** \brief This function compares all writable bytes in the configuration zone that is passed in to the bytes on the device
 *
 * \config_data[in] config_data pointer to all 128 bytes in configuration zone. Not used if NULL.
 * \return status of the operation
 */
uint8_t atca_cmp_config_zone(uint8_t* config_data)
{
	//uint8_t ret = ATCA_SUCCESS;
	//uint8_t device_config_data[ATCA_ECC_CONFIG_SIZE];
	//
	//// Check the config_data pointer
	//if (bad_pointer(config_data)) return false;

	//// Read all of the configuration bytes from the device
	//ret = atca_read_config_zone(device_config_data);
	//if (ret != ATCA_SUCCESS) return false;
	//
	//// Compare writable bytes 16-79
	//if (memcmp(&device_config_data[16], &config_data[16], 80-16) != 0) return false;
	//
	//// Compare writable bytes 85-127
	//if (memcmp(&device_config_data[85], &config_data[85], 127-85) != 0) return false;

	return true;
}



/** \brief This function writes all writable bytes in the configuration zone.

The configuration zone is 128 bytes.  Writable bytes are [16:83] and [88:127]

* \config_data[in] config_data pointer to all 128 bytes in configuration zone.
Not used if NULL.
* \return status of the operation
*/
uint8_t atca_write_ecc_config_zone(uint8_t* config_data)
{
	uint8_t ret = ATCA_SUCCESS;
	uint8_t block = 0;
	uint8_t offset = 0;
	uint8_t isEcc = (dev_rev.dev_revision & ECC508_MASK) || (dev_rev.dev_revision & ECC108_MASK);

	do
	{
		// Check the config_data pointer
		if (bad_pointer(config_data)) break;

		// Write bytes [16:31] using four word writes
		block = 0;
		offset = 4;
		ret = atca_write_zone(ATCA_ZONE_CONFIG, 0, block, offset, &config_data[16], ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;

		block = 0;
		offset = 5;
		ret = atca_write_zone(ATCA_ZONE_CONFIG, 0, block, offset, &config_data[20], ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;

		block = 0;
		offset = 6;
		ret = atca_write_zone(ATCA_ZONE_CONFIG, 0, block, offset, &config_data[24], ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;

		block = 0;
		offset = 7;
		ret = atca_write_zone(ATCA_ZONE_CONFIG, 0, block, offset, &config_data[28], ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;

		// Write bytes [32:63} using a block write
		block = 1;
		offset = 0;
		ret = atca_write_zone(ATCA_ZONE_CONFIG, 0, block, offset, &config_data[32], ATCA_BLOCK_SIZE);
		if (ret != ATCA_SUCCESS) break;

		// Write bytes [64:83] using five word writes
		block = 2;
		offset = 0;
		ret = atca_write_zone(ATCA_ZONE_CONFIG, 0, block, offset, &config_data[64], ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;

		block = 2;
		offset = 1;
		ret = atca_write_zone(ATCA_ZONE_CONFIG, 0, block, offset, &config_data[68], ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;

		block = 2;
		offset = 2;
		ret = atca_write_zone(ATCA_ZONE_CONFIG, 0, block, offset, &config_data[72], ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;

		block = 2;
		offset = 3;
		ret = atca_write_zone(ATCA_ZONE_CONFIG, 0, block, offset, &config_data[76], ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;

		block = 2;
		offset = 4;
		ret = atca_write_zone(ATCA_ZONE_CONFIG, 0, block, offset, &config_data[80], ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;

		if (isEcc)
		{
			block = 2;
			offset = 6;
			ret = atca_write_zone(ATCA_ZONE_CONFIG, 0, block, offset, &config_data[88], ATCA_WORD_SIZE);
			if (ret != ATCA_SUCCESS) break;

			block = 2;
			offset = 7;
			ret = atca_write_zone(ATCA_ZONE_CONFIG, 0, block, offset, &config_data[92], ATCA_WORD_SIZE);
			if (ret != ATCA_SUCCESS) break;

			// Write bytes [96:127] using a block write
			block = 3;
			offset = 0;
			ret = atca_write_zone(ATCA_ZONE_CONFIG, 0, block, offset, &config_data[96], ATCA_BLOCK_SIZE);
			if (ret != ATCA_SUCCESS) break;
		}
	} while (0);

	return ret;
}

/** \brief This function writes all writable bytes in the configuration zone.

The configuration zone is 128 bytes.  Writable bytes are [16:83] and [88:127]

* \config_data[in] config_data pointer to all 128 bytes in configuration zone.
Not used if NULL.
* \return status of the operation
*/
uint8_t atca_write_ecc508_config_zone(uint8_t* config_data)
{
	uint8_t ret = ATCA_SUCCESS;
	uint8_t block = 0;
	uint8_t offset = 0;

	do
	{
		// Check the config_data pointer
		if (bad_pointer(config_data)) break;

		// Write bytes [16:31] using four word writes
		block = 0;
		offset = 4;
		ret = atca_write_zone(ATCA_ZONE_CONFIG, 0, block, offset, &config_data[16], ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;

		block = 0;
		offset = 5;
		ret = atca_write_zone(ATCA_ZONE_CONFIG, 0, block, offset, &config_data[20], ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;

		block = 0;
		offset = 6;
		ret = atca_write_zone(ATCA_ZONE_CONFIG, 0, block, offset, &config_data[24], ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;

		block = 0;
		offset = 7;
		ret = atca_write_zone(ATCA_ZONE_CONFIG, 0, block, offset, &config_data[28], ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;

		// Write bytes [32:63} using a block write
		block = 1;
		offset = 0;
		ret = atca_write_zone(ATCA_ZONE_CONFIG, 0, block, offset, &config_data[32], ATCA_BLOCK_SIZE);
		if (ret != ATCA_SUCCESS) break;

		// Write bytes [64:83] using five word writes
		block = 2;
		offset = 0;
		ret = atca_write_zone(ATCA_ZONE_CONFIG, 0, block, offset, &config_data[64], ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;

		block = 2;
		offset = 1;
		ret = atca_write_zone(ATCA_ZONE_CONFIG, 0, block, offset, &config_data[68], ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;

		block = 2;
		offset = 2;
		ret = atca_write_zone(ATCA_ZONE_CONFIG, 0, block, offset, &config_data[72], ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;

		block = 2;
		offset = 3;
		ret = atca_write_zone(ATCA_ZONE_CONFIG, 0, block, offset, &config_data[76], ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;

		block = 2;
		offset = 4;
		ret = atca_write_zone(ATCA_ZONE_CONFIG, 0, block, offset, &config_data[80], ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;

		block = 2;
		offset = 6;
		ret = atca_write_zone(ATCA_ZONE_CONFIG, 0, block, offset, &config_data[88], ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;

		block = 2;
		offset = 7;
		ret = atca_write_zone(ATCA_ZONE_CONFIG, 0, block, offset, &config_data[92], ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) break;

		// Write bytes [96:127] using a block write
		block = 3;
		offset = 0;
		ret = atca_write_zone(ATCA_ZONE_CONFIG, 0, block, offset, &config_data[96], ATCA_BLOCK_SIZE);
		if (ret != ATCA_SUCCESS) break;

	} while (0);

	return ret;
}
/** \brief This function locks the configuration zone.

	It first reads it and calculates the CRC of its content.
	It then sends a Lock command to the device.

	Once the configuration zone is locked, the Random
	command returns a number from its high quality random
	number generator instead of a 0xFFFF0000FFFF0000...
	sequence.

	\return status of the operation
	*/
uint8_t atca_lock_config_zone(void)
{
	uint8_t ret = ATCA_SUCCESS;
	uint8_t config_data[ATCA_ECC_CONFIG_SIZE];
	uint8_t crc_array[ATCA_CRC_SIZE];
	uint16_t crc;
	uint8_t crc_size;
	uint8_t command[LOCK_COUNT];
	uint8_t response[LOCK_RSP_SIZE];

	do
	{
		// Check whether the data zone is locked already.  Return if already locked.
		if (atca_is_locked(LOCK_ZONE_CONFIG))
		{
			ret = ATCA_CONFIG_ZONE_LOCKED;
			break;
		}

		// Read the config zone to calculate the CRC
		ret = atca_read_config_zone(config_data);
		if (ret != ATCA_SUCCESS) break;

		if (dev_rev.dev_revision & SHA204_MASK)
		{
			crc_size = SHA204_CONF_ZONE_SIZE;
		}
		else
		{
			crc_size = ECCxxx_CONF_ZONE_SIZE;
		}

		// Get the CRC
		atca_calc_crc(crc_size, config_data, crc_array);
		crc = (crc_array[1] << 8) + crc_array[0];

		// Wake up the device to execute the command
		ret = atca_wake();
		if (ret != ATCA_SUCCESS) break;

		ret = atca_execute(ATCA_LOCK, LOCK_ZONE_CONFIG, crc, 0, NULL, 0, NULL, 0, NULL,
			sizeof(command), command, sizeof(response), response);
		if (ret != ATCA_SUCCESS) break;

	} while (0);

	atca_idle();

	return ret;
}

/** \brief This function locks the ATECC508's configuration zone.
	\return status of the operation
	*/
uint8_t atca_lock_ecc508_config_zone(void)
{
	uint8_t ret = ATCA_SUCCESS;
	uint8_t config_data[ATCA_ECC_CONFIG_SIZE];
	uint8_t crc_array[ATCA_CRC_SIZE];
	uint16_t crc;
	uint8_t crc_size;
	uint8_t command[LOCK_COUNT];
	uint8_t response[LOCK_RSP_SIZE];

	do
	{
		// Check whether the data zone is locked already.  Return if already locked.
		if (atca_is_locked(LOCK_ZONE_CONFIG))
		{
			ret = ATCA_CONFIG_ZONE_LOCKED;
			break;
		}

		// Read the config zone to calculate the CRC
		ret = atca_read_ecc508_config_zone(config_data);
		if (ret != ATCA_SUCCESS) break;

		crc_size = ECCxxx_CONF_ZONE_SIZE;

		// Get the CRC
		atca_calc_crc(crc_size, config_data, crc_array);
		crc = (crc_array[1] << 8) + crc_array[0];

		// Wake up the device to execute the command
		ret = atca_wake();
		if (ret != ATCA_SUCCESS) break;

		ret = atca_execute(ATCA_LOCK, LOCK_ZONE_CONFIG, crc, 0, NULL, 0, NULL, 0, NULL,
			sizeof(command), command, sizeof(response), response);
		if (ret != ATCA_SUCCESS) break;

	} while (0);

	atca_idle();

	return ret;
}

/** \brief This function locks the data zone.
	Once the data zone is locked, the ECDH command is active.

	\return status of the operation
	*/
uint8_t atca_lock_data_zone(void)
{
	uint8_t ret = ATCA_SUCCESS;
	uint8_t command[LOCK_COUNT];
	uint8_t response[LOCK_RSP_SIZE];

	do
	{
		// Check whether the data zone is locked already.  Return if already locked.
		if (atca_is_locked(LOCK_ZONE_DATA))
		{
			break;
		}

		// Wake up the device to execute the lock command
		ret = atca_wake();
		if (ret != ATCA_SUCCESS) break;

		ret = atca_execute(ATCA_LOCK, LOCK_ZONE_NO_CRC | LOCK_ZONE_DATA, 0, 0, NULL, 0, NULL, 0, NULL,
			sizeof(command), command, sizeof(response), response);
		if (ret != ATCA_SUCCESS) break;

	} while (0);

	return ret;
}

/** \brief This function locks each slot of data zone.
		   Once the slot is locked, the slot cannot be modified under any circumstance.
		   *  \param[in] slot to be locked in data zone
		   \return status of the operation
		   */
uint8_t atca_lock_data_slot(uint8_t slot)
{
	uint8_t ret = ATCA_SUCCESS;
	uint8_t command[LOCK_COUNT];
	uint8_t response[LOCK_RSP_SIZE];

	do
	{
		// Wake up the device to execute the lock command
		ret = atca_wake();
		if (ret != ATCA_SUCCESS) break;

		ret = atca_execute(ATCA_LOCK, (slot << 2) | LOCK_ZONE_DATA_SLOT, 0, 0, NULL, 0, NULL, 0, NULL,
			sizeof(command), command, sizeof(response), response);

		if (ret != ATCA_SUCCESS) break;

	} while (0);

	return ret;
}


/** \brief Query to see if the specified zone is locked
 *  \param[in] zone The zone to query for locked (use LOCK_ZONE_CONFIG or LOCK_ZONE_DATA)
 *  \return true if the specified zone is locked
 */
uint8_t atca_is_locked(uint8_t zone)
{
	uint8_t ret = ATCA_SUCCESS;
	uint8_t word_data[ATCA_WORD_SIZE];
	uint8_t locked = false;
	uint8_t zone_idx = 2;

	do
	{
		// Read the word with the lock bytes (UserExtra, Selector, LockValue, LockConfig) (config block = 2, word offset = 5)
		ret = atca_read_zone(ATCA_ZONE_CONFIG, 0, 2/*block*/, 5/*offset*/, word_data, ATCA_WORD_SIZE);
		if (ret != ATCA_SUCCESS) {
			break;
		}

		// Determine the index into the word_data based on the zone we are querying for
		if (zone == LOCK_ZONE_DATA) zone_idx = 2;
		if (zone == LOCK_ZONE_CONFIG) zone_idx = 3;

		// Set the locked return variable base on the value.
		if (word_data[zone_idx] == 0) locked = true;

	} while (0);

	return locked;
}


/** \brief This function sets the GPIO feature in Authorization Output mode.
		   This function is only for Single Wire Interface device.
		   * \param[in] default_state of SCL pin
		   * \param[in] key_id for the GPIO authorizing key
		   * \return status of the operation
		   */
uint8_t atca_activate_gpio_auth_mode(uint8_t default_state, uint8_t key_id)
{
	// declared as "volatile" for easier debugging
	volatile uint8_t ret_code;

	uint8_t gpio_mode_config;

	default_state = (default_state == ATCA_SET_HIGH) ? ATCA_GPIO_STATE_HIGH : ATCA_GPIO_STATE_LOW;

	gpio_mode_config = ATCA_GPIO_MODE_AUTH_OUTPUT | default_state | (key_id << 4);

	// We use atca_config_i2c_address() function since the GPIO configuration is
	// located in the same byte as I2C address
	ret_code = atca_config_i2c_address(gpio_mode_config);

	return ret_code;
}

/** \brief get public key from given slot
 *   puts only key bytes into pubkey (64 bytes)
 *   assumes P256 curve
 *  returns ATCA status
 */
uint8_t atca_get_pubkey(uint8_t slot, uint8_t *pubkey)
{
	volatile int cmdStatus;
	static uint8_t wakeup_response[4];
	uint8_t command[ATCA_RSP_SIZE_64];
	uint8_t response[ATCA_RSP_SIZE_64];

	// check the pubkey
	if (pubkey == NULL)
	{
		return ATCA_BAD_PARAM;
	}

	// Set the pubkey to all 00
	memset(pubkey, 0, ATCA_PUB_KEY_SIZE);

	// Wakeup the device
	cmdStatus = atca_wakeup(wakeup_response);
	if (cmdStatus != ATCA_SUCCESS)
	{
		return cmdStatus;
	}

	// Generate public key from the existing private key
	cmdStatus = atca_execute(ATCA_GENKEY, GENKEY_MODE_PUBLIC,
		(uint16_t)slot, 0, NULL, 0, NULL, 0, NULL,
		sizeof(command), command, ATCA_RSP_SIZE_64, response);

	// Idle the device
	atca_idle();

	// Check the return code
	if (cmdStatus != ATCA_SUCCESS)
	{
		return ATCA_GEN_FAIL;
	}

	// Success, copy the key to pubkey
	memcpy(pubkey, &response[ATCA_RSP_DATA_IDX], ATCA_PUB_KEY_SIZE);

	return ATCA_SUCCESS;
}

/** \brief generate private key for given slot
 * 	 warning, every time this is executed, it will regenerate the private key
 *   puts only key bytes into pubkey (64 bytes)
 *  returns ATCA status
 */
uint8_t atca_create_key(uint8_t slot, uint8_t *pubkey)
{
	volatile int cmdStatus;
	static uint8_t wakeup_response[4];
	uint8_t command[ATCA_RSP_SIZE_64];
	uint8_t response[ATCA_RSP_SIZE_64];

	// check the pubkey
	if (pubkey == NULL)
	{
		return ATCA_BAD_PARAM;
	}

	// Set the pubkey to all 00
	memset(pubkey, 0, ATCA_PUB_KEY_SIZE);

	// Wakeup the device
	cmdStatus = atca_wakeup(wakeup_response);
	if (cmdStatus != ATCA_SUCCESS)
	{
		return cmdStatus;
	}

	// Generate Private Key on slot using GenKey command with mode = 0x04.
	cmdStatus = atca_execute(ATCA_GENKEY, GENKEY_MODE_PRIVATE,
		(uint16_t)slot, 0, NULL, 0, NULL, 0, NULL, sizeof(command),
		command, ATCA_RSP_SIZE_64, response);

	// Idle the device
	atca_idle();

	// Check the return code 
	if (cmdStatus != ATCA_SUCCESS)
	{
		return ATCA_GEN_FAIL;
	}

	// Success, copy the key to pubkey
	memcpy(pubkey, &response[ATCA_RSP_DATA_IDX], ATCA_PUB_KEY_SIZE);

	return cmdStatus;
}

/** \brief generate private key for given slot to implement signing system
 * 	 warning, every time this is executed, it will regenerate the private key
 *   puts only key bytes into pubkey (64 bytes)
 *  returns ATCA status
 */
uint8_t atca_create_ecc508_key(uint8_t slot, uint8_t *pubkey)
{
	uint8_t cmdStatus = 0;
	static uint8_t wakeup_response[4];
	//uint8_t command[ATCA_RSP_SIZE_64];
	uint8_t command[ATCA_RSP_SIZE_72];
	uint8_t response[ATCA_RSP_SIZE_72];

	// check the pubkey
	if (pubkey == NULL)
	{
		return ATCA_BAD_PARAM;
	}

	// Set the pubkey to all 00
	memset(command, 0, sizeof(command));
	memset(response, 0, sizeof(response));
	// Wakeup the device
	cmdStatus = atca_wakeup(wakeup_response);
	if (cmdStatus != ATCA_SUCCESS)
	{
		return cmdStatus;
	}

	// Generate Private Key on slot using GenKey command with mode = 0x04.
	cmdStatus = atca_execute(ATCA_GENKEY, GENKEY_MODE_PRIVATE,
		(uint16_t)slot, 0, NULL, 0, NULL, 0, NULL, sizeof(command),
		command, sizeof(response), response);

	// Idle the device
	atca_idle();

	// Check the return code 
	if (cmdStatus != ATCA_SUCCESS)
	{
		return ATCA_GEN_FAIL;
	}

	// Success, copy the key to pubkey
	memcpy(pubkey, &response[ATCA_RSP_DATA_IDX], ATCA_PUB_KEY_SIZE);
	return cmdStatus;
}

/** \brief Compute the address given the slot, block, and offset
 *
 * see ECC108A datasheet, datazone address values, table 9-8
 *
 * returns The 16 bit address
 */
uint16_t atca_get_addr(uint8_t zone, uint8_t slot, uint8_t block, uint8_t offset, uint16_t* addr)
{
	int ret = ATCA_SUCCESS;

	do
	{
		// Check the input parameters
		if (addr == NULL) break;
		if (zone != ATCA_ZONE_CONFIG && zone != ATCA_ZONE_DATA && zone != ATCA_ZONE_OTP)
		{
			break;
		}
		// slot
		// block
		// offset

		// Initialize the addr to 00
		*addr = 0;
		// Mask the offset
		offset = offset & (uint8_t)0x07;

		switch (zone)
		{
		case ATCA_ZONE_OTP:
		case ATCA_ZONE_CONFIG:
			*addr = block << 3;
			*addr |= offset;
			break;
		case ATCA_ZONE_DATA:
			*addr = slot << 3;
			*addr |= offset;
			*addr |= block << 8;
			break;
		default:
			ret = ATCA_BAD_PARAM;
			break;
			break;
		}
	} while (0);

	return ret;
}


/** \brief write an ECC private key into a slot in the device zone
*
* \param[in] zone : Once the Data zone is locked, the input data (priv) is encrypted using TempKey.
* \param[in] slot : Key slot to be written.
* \param[in] priv : the p256 ECC private key 32 bytes
* \param[in] mac  : Message authentication code to validate address and data
* \return ATCA status of the operation
*/
uint8_t atca_send_privWrite(uint8_t zone, uint8_t slot, uint8_t *priv, uint8_t* mac)
{
	int ret = ATCA_SUCCESS;
	uint8_t privateKey[PRIVWRITE_DATA_SIZE];
	uint8_t mac_data[MAC_SIZE];
	uint8_t response[PRIVWRITE_RSP_SIZE];
	uint8_t command[PRIVWRITE_COUNT];

	do
	{
		// Check the input parameters
		if (priv == NULL || mac == NULL)
		{
			ret = ATCA_BAD_PARAM;
			break;
		}

		memset(command, 0, sizeof(command));
		memset(response, 0, sizeof(response));

		memcpy(privateKey, priv, sizeof(privateKey));
		memcpy(mac_data, mac, MAC_SIZE);


		// Wake up the device
		ret = atca_wake();
		if (ret != ATCA_SUCCESS) break;

		// Execute the command
		ret = atca_execute(ATCA_PRIVWRITE, zone, (uint16_t)slot, PRIVWRITE_DATA_SIZE, privateKey,
			MAC_SIZE, mac_data, 0, NULL, sizeof(command), command, sizeof(response), response);

		// Check the response
		if (response[ATCA_RSP_DATA_IDX] != ATCA_SUCCESS)
		{
			ret = ATCA_CMD_FAIL;
			break;
		}
	} while (0);

	atca_idle();

	return ret;
}

/** \brief write either 4 or 32 bytes of data into the device zone
 *
 * see ECC108A datasheet, datazone address values, table 9-8
 *
 * \param[in] zone
 * \param[in] slot
 * \param[in] block
 * \param[in] offset
 * \param[in] data
 * \param[in] len  Must be either 4 or 32
 * \return ATCA status of the operation
 */
uint8_t atca_write_zone(uint8_t zone, uint8_t slot, uint8_t block, uint8_t offset, uint8_t *data, uint8_t len)
{
	int ret = ATCA_SUCCESS;
	uint8_t response[ATCA_RSP_SIZE_MIN];
	uint8_t command[ATCA_RSP_SIZE_64];
	uint16_t addr;

	do
	{
		// Check the input parameters
		if (data == NULL)
		{
			break;
		}
		// The get address function checks the remaining variables
		ret = atca_get_addr(zone, slot, block, offset, &addr);
		if (ret != ATCA_SUCCESS) break;

		// If there are 32 bytes to write, then xor the bit into the mode
		if (len == ATCA_BLOCK_SIZE)
		{
			zone = zone | ATCA_ZONE_COUNT_FLAG;
		}

		// Wake up the device
		ret = atca_wake();
		if (ret != ATCA_SUCCESS) break;
		// Execute the command
		memset(command, 0, sizeof(command));

		ret = atca_execute(ATCA_WRITE, zone, addr, len, data, 0, NULL, 0, NULL, sizeof(command), command, sizeof(response), response);
		// Check the response
		if (response[ATCA_RSP_DATA_IDX] != ATCA_SUCCESS)
		{
			ret = ATCA_CMD_FAIL;
			break;
		}
	} while (0);

	atca_idle();

	return ret;
}


/** \brief read either 4 or 32 bytes of data into given slot
 *
 * for 32 byte read, offset is ignored
 * *data receives the contents read from the slot
 *
 *  data zone must be locked and the slot configuration must not be secret for a slot to be successfully read
 *
 * \param[in] zone
 * \param[in] slot
 * \param[in] block
 * \param[in] offset
 * \param[in] data
 * \param[in] len  Must be either 4 or 32
 * returns ATCA status
 */
uint8_t atca_read_zone(uint8_t zone, uint8_t slot, uint8_t block, uint8_t offset, uint8_t *data, uint8_t len)
{
	volatile int ret;
	uint8_t command[ATCA_RSP_SIZE_64];
	uint16_t addr;
	uint8_t response[ATCA_RSP_SIZE_32];

	do
	{
		// Check the input parameters
		if (data == NULL) break;

		// The get address function checks the remaining variables
		ret = atca_get_addr(zone, slot, block, offset, &addr);
		if (ret != ATCA_SUCCESS) break;

		// Set the data to all zero.  This makes failed reads more obvious
		memset(data, 0, len);

		// If there are 32 bytes to write, then xor the bit into the mode
		if (len == ATCA_BLOCK_SIZE)
		{
			zone = zone | ATCA_ZONE_COUNT_FLAG;
		}

		// Wake up the device
		ret = atca_wake();
		if (ret != ATCA_SUCCESS) break;

		// Execute the command
		ret = atca_execute(ATCA_READ, zone, addr, 0, NULL, 0, NULL, 0, NULL, sizeof(command), command, len, response);

		// Check the response.  If there are less than four data bytes, then the read failed
		if (response[ATCA_COUNT_IDX] < ATCA_RSP_SIZE_4)
		{
			ret = ATCA_CMD_FAIL;
			break;
		}
		// Copy the data bytes out of the response
		memcpy(data, &response[ATCA_RSP_DATA_IDX], len);

	} while (0);

	atca_idle();

	return ret;
}

/** \brief encrypted write either 4 or 32 bytes of data into the device zone
 *
 * see ECC108A datasheet, datazone address values, table 9-8
 *
 * \param[in] zone
 * \param[in] slot
 * \param[in] block
 * \param[in] cipher
 * \param[in] mac
 * \param[in] len  Must be either 4 or 32
 * \return ATCA status of the operation
 */
uint8_t atca_write_slot(uint8_t zone, uint8_t slot, uint8_t block, uint8_t *cipher, uint8_t *mac, uint8_t len)
{
	int ret = ATCA_SUCCESS;
	uint8_t command[ATCA_RSP_SIZE_72];
	uint8_t response[ATCA_RSP_SIZE_72];
	uint16_t addr = 0;

	do
	{
		if (cipher == NULL || mac == NULL)  return ATCA_BAD_PARAM;

		ret = atca_get_addr(zone, slot, block, 0, &addr);
		if (ret != ATCA_SUCCESS) break;

		if (len == ATCA_BLOCK_SIZE)
			zone = zone | ATCA_ZONE_COUNT_FLAG;

		ret = atca_wake();
		if (ret != ATCA_SUCCESS) break;

		memset(response, 0, sizeof(response));
		memset(command, 0, sizeof(command));
		ret = atca_execute(ATCA_WRITE, zone, addr, len, cipher, len, mac, 0, NULL, sizeof(command), command, sizeof(response), response);

		if (response[ATCA_RSP_DATA_IDX] != ATCA_SUCCESS) {
			ret = ATCA_CMD_FAIL;
			break;
		}

	} while (0);

	atca_idle();

	return ret;
}
uint8_t atca_sign_data(uint8_t slot, uint8_t *data, size_t data_len, uint8_t *signature)
{
	uint8_t ret = ATCA_SUCCESS;
	uint8_t digest[ATCA_BLOCK_SIZE];

	do
	{
		// Calculate the SHA-256 on the data
		atca_sha256(data_len, data, digest);

		// Send the digest to the sign command
		ret = atca_sign(slot, digest, signature);
	} while (0);

	return ret;
}

/** \brief sign a buffer using private key in given slot, stuff the signature
 *
 *  msg should point to a 32 byte buffer
 *  signature should point to buffer SIGN_RSP_SIZE big
 *
 * returns ATCA result code
 */
uint8_t atca_sign(uint16_t slot, uint8_t *msg, uint8_t *signature)
{
	volatile uint8_t cmdStatus;
	static uint8_t command[VERIFY_256_EXTERNAL_COUNT];
	static uint8_t response_dummy_random[RANDOM_RSP_SIZE];
	static uint8_t response_status[ATCA_RSP_SIZE_MIN];
	static uint8_t wakeup_response[ATCA_RSP_SIZE_MIN];
	static uint8_t sign_response[ATCA_RSP_SIZE_64];

	cmdStatus = atca_wakeup(wakeup_response);
	if (cmdStatus != ATCA_SUCCESS)
	{
		atca_idle();
		return cmdStatus;
	}

	// Perform dummy random command for updating the random seed
	cmdStatus = atca_execute(ATCA_RANDOM, RANDOM_SEED_UPDATE, 0x0000, 0,
		NULL, 0, NULL, 0, NULL, sizeof(command), command,
		sizeof(response_dummy_random), response_dummy_random);
	if (cmdStatus != ATCA_SUCCESS)
	{
		(void)atca_sleep();
		return cmdStatus;
	}

	// Pass the message to be signed using Nonce command with passthrough mode
	memset(response_status, 0, sizeof(response_status));
	cmdStatus = atca_execute(ATCA_NONCE, NONCE_MODE_PASSTHROUGH,
		NONCE_MODE_RANDOM_OUT, NONCE_NUMIN_SIZE_PASSTHROUGH,
		msg, 0, NULL,
		0, NULL, sizeof(command), command, sizeof(response_status),
		response_status);

	if (cmdStatus != ATCA_SUCCESS)
	{
		atca_idle();
		return cmdStatus;
	}

	atca_idle();
	cmdStatus = atca_wakeup(wakeup_response);
	if (cmdStatus != ATCA_SUCCESS)
	{
		atca_idle();
		return cmdStatus;
	}

	// Sign the message using Sign command with external mode = 0x80.
	memset(signature, 0, ATCA_SIG_SIZE);
	cmdStatus = atca_execute(ATCA_SIGN, SIGN_MODE_EXTERNAL, slot,
		0, NULL, 0, NULL, 0, NULL, sizeof(command), command,
		SIGN_RSP_SIZE, sign_response);

	atca_idle();

	if (cmdStatus != ATCA_SUCCESS)
	{
		return cmdStatus;
	}
	memcpy(signature, &sign_response[ATCA_RSP_DATA_IDX], ATCA_SIG_SIZE);

	return ATCA_SUCCESS;
}

/** \brief use the ECC108 SHA command to compute the SHA256 of the public key in the given slot
 *  loads the sha256_digest buffer with the sha 256 computed value
 *
 * earlier ECC108 devices did not support the SHA command
 *
 * returns ATCA status
 */
uint8_t atca_sha256_pubkey(uint8_t slot, uint8_t *sha256_digest)
{
	/* SHA command using start mode with CRC
	07 47 00 00 00 2e 85
	*/

	/* SHA command using Public mode with slot 0 as the Param2, with CRC
	07 47 03 00 00 2E 8A
	*/
	return ATCA_SUCCESS;
}

uint8_t atca_read_pubkey(uint8_t slot8toF, uint8_t *pubkey)
{
	uint8_t ret = ATCA_SUCCESS;
	uint8_t read_buf[ATCA_BLOCK_SIZE];
	uint8_t block = 0;
	uint8_t offset = 0;
	uint8_t cpyIndex = 0;
	uint8_t cpySize = 0;
	uint8_t readIndex = 0;

	do
	{
		// Check the pointers
		if (pubkey == NULL)
		{
			ret = ATCA_BAD_PARAM;
			break;
		}
		// Check the value of the slot
		if (slot8toF < 8 || slot8toF > 0xF)
		{
			ret = ATCA_BAD_PARAM;
			break;
		}
		// The 64 byte P256 public key gets written to a 72 byte slot in the following pattern
		// | Block 1                     | Block 2                                      | Block 3       |
		// | Pad: 4 Bytes | PubKey[0:27] | PubKey[28:31] | Pad: 4 Bytes | PubKey[32:55] | PubKey[56:63] |

		// Read the block
		block = 0;
		ret = atca_read_zone(ATCA_ZONE_DATA, slot8toF, block, offset, read_buf, ATCA_BLOCK_SIZE);
		if (ret != ATCA_SUCCESS)
		{
			break;
		}
		// Copy.  Account for 4 byte pad
		cpySize = ATCA_BLOCK_SIZE - ATCA_PUB_KEY_PAD;
		readIndex = ATCA_PUB_KEY_PAD;
		memcpy(&pubkey[cpyIndex], &read_buf[readIndex], cpySize);
		cpyIndex += cpySize;

		// Read the next block
		block = 1;
		ret = atca_read_zone(ATCA_ZONE_DATA, slot8toF, block, offset, read_buf, ATCA_BLOCK_SIZE);
		if (ret != ATCA_SUCCESS)
		{
			break;
		}
		// Copy.  First four bytes
		cpySize = ATCA_PUB_KEY_PAD;
		readIndex = 0;
		memcpy(&pubkey[cpyIndex], &read_buf[readIndex], cpySize);
		cpyIndex += cpySize;
		// Copy.  Skip four bytes
		readIndex = ATCA_PUB_KEY_PAD + ATCA_PUB_KEY_PAD;
		cpySize = ATCA_BLOCK_SIZE - readIndex;
		memcpy(&pubkey[cpyIndex], &read_buf[readIndex], cpySize);
		cpyIndex += cpySize;

		// Read the next block
		block = 2;
		ret = atca_read_zone(ATCA_ZONE_DATA, slot8toF, block, offset, read_buf, ATCA_BLOCK_SIZE);
		if (ret != ATCA_SUCCESS)
		{
			break;
		}
		// Copy.  The remaining 8 bytes
		cpySize = ATCA_PUB_KEY_PAD + ATCA_PUB_KEY_PAD;
		readIndex = 0;
		memcpy(&pubkey[cpyIndex], &read_buf[readIndex], cpySize);

	} while (0);

	return ret;
}

uint8_t atca_read_sig(uint8_t slot8toF, uint8_t *sig)
{
	uint8_t ret = ATCA_SUCCESS;
	uint8_t read_buf[ATCA_BLOCK_SIZE];
	uint8_t block = 0;
	uint8_t offset = 0;
	uint8_t cpyIndex = 0;

	do
	{
		// Check the pointers
		if (sig == NULL)
		{
			break;
		}
		// Check the value of the slot
		if (slot8toF < 8 || slot8toF > 0xF)
		{
			break;
		}
		// Read the first block
		block = 0;
		ret = atca_read_zone(ATCA_ZONE_DATA, slot8toF, block, offset, read_buf, ATCA_BLOCK_SIZE);
		if (ret != ATCA_SUCCESS)
		{
			break;
		}
		// Copy.  first 32 bytes
		memcpy(&sig[cpyIndex], &read_buf[0], ATCA_BLOCK_SIZE);
		cpyIndex += ATCA_BLOCK_SIZE;

		// Read the second block
		block = 1;
		ret = atca_read_zone(ATCA_ZONE_DATA, slot8toF, block, offset, read_buf, ATCA_BLOCK_SIZE);
		if (ret != ATCA_SUCCESS)
		{
			break;
		}
		// Copy.  next 32 bytes
		memcpy(&sig[cpyIndex], &read_buf[0], ATCA_BLOCK_SIZE);
		cpyIndex += ATCA_BLOCK_SIZE;

	} while (0);

	return ret;
}

uint8_t atca_write_pubkey(uint8_t slot8toF, uint8_t *pubkey)
{
	uint8_t ret = ATCA_SUCCESS;
	uint8_t write_block[ATCA_BLOCK_SIZE];
	uint8_t block = 0;
	uint8_t offset = 0;
	uint8_t cpyIndex = 0;
	uint8_t cpySize = 0;
	uint8_t writeIndex = 0;

	do
	{
		// Check the pointers
		if (pubkey == NULL)
		{
			ret = ATCA_BAD_PARAM;
			break;
		}
		// Check the value of the slot
		if (slot8toF < 8 || slot8toF > 0xF)
		{
			ret = ATCA_BAD_PARAM;
			break;
		}
		// The 64 byte P256 public key gets written to a 72 byte slot in the following pattern
		// | Block 1                     | Block 2                                      | Block 3       |
		// | Pad: 4 Bytes | PubKey[0:27] | PubKey[28:31] | Pad: 4 Bytes | PubKey[32:55] | PubKey[56:63] |

		// Setup the first write block accounting for the 4 byte pad
		block = 0;
		writeIndex = ATCA_PUB_KEY_PAD;
		memset(write_block, 0, sizeof(write_block));
		cpySize = ATCA_BLOCK_SIZE - ATCA_PUB_KEY_PAD;
		memcpy(&write_block[writeIndex], &pubkey[cpyIndex], cpySize);
		cpyIndex += cpySize;
		// Write the first block 
		ret = atca_write_zone(ATCA_ZONE_DATA, slot8toF, block, offset, write_block, ATCA_BLOCK_SIZE);
		if (ret != ATCA_SUCCESS)
		{
			break;
		}

		// Setup the second write block accounting for the 4 byte pad
		block = 1;
		writeIndex = 0;
		memset(write_block, 0, sizeof(write_block));
		// Setup for write 4 bytes starting at 0
		cpySize = ATCA_PUB_KEY_PAD;
		memcpy(&write_block[writeIndex], &pubkey[cpyIndex], cpySize);
		cpyIndex += cpySize;
		// Setup for write skip 4 bytes and fill the remaining block
		writeIndex += cpySize + ATCA_PUB_KEY_PAD;
		cpySize = ATCA_BLOCK_SIZE - writeIndex;
		memcpy(&write_block[writeIndex], &pubkey[cpyIndex], cpySize);
		cpyIndex += cpySize;
		// Write the second block 
		ret = atca_write_zone(ATCA_ZONE_DATA, slot8toF, block, offset, write_block, ATCA_BLOCK_SIZE);
		if (ret != ATCA_SUCCESS)
		{
			break;
		}

		// Setup the third write block 
		block = 2;
		writeIndex = 0;
		memset(write_block, 0, sizeof(write_block));
		// Setup for write 8 bytes starting at 0
		cpySize = ATCA_PUB_KEY_PAD + ATCA_PUB_KEY_PAD;
		memcpy(&write_block[writeIndex], &pubkey[cpyIndex], cpySize);
		// Write the third block 
		ret = atca_write_zone(ATCA_ZONE_DATA, slot8toF, block, offset, write_block, ATCA_BLOCK_SIZE);
		if (ret != ATCA_SUCCESS)
		{
			break;
		}

	} while (0);

	return ret;
}

uint8_t atca_write_sig(uint8_t slot8toF, uint8_t *sig)
{
	uint8_t ret = ATCA_SUCCESS;
	uint8_t block = 0;
	uint8_t offset = 0;
	uint8_t cpyIndex = 0;

	do
	{
		// Check the pointers
		if (sig == NULL)
		{
			break;
		}
		// Check the value of the slot
		if (slot8toF < 8 || slot8toF > 0xF)
		{
			break;
		}
		// Write the first block
		block = 0;
		ret = atca_write_zone(ATCA_ZONE_DATA, slot8toF, block, offset, &sig[cpyIndex], ATCA_BLOCK_SIZE);
		if (ret != ATCA_SUCCESS)
		{
			break;
		}
		cpyIndex += ATCA_BLOCK_SIZE;

		// Write the second block
		block = 1;
		ret = atca_write_zone(ATCA_ZONE_DATA, slot8toF, block, offset, &sig[cpyIndex], ATCA_BLOCK_SIZE);
		if (ret != ATCA_SUCCESS)
		{
			break;
		}

	} while (0);

	return ret;
}

uint8_t atca_write_privKey(uint8_t mode, uint8_t slot, uint8_t* data, uint8_t* priv, uint8_t* pubkey)
{
	uint8_t ret = ATCA_SUCCESS;
	uint8_t mac[MAC_SIZE];
	uint8_t privKey[ATCA_KEY_SIZE];
	uint8_t pubkey_result[ATCA_PUB_KEY_SIZE];
	uint8_t accessKey[ATCA_KEY_SIZE];
	uint8_t accessKeyID = 0x01; //must match SlotConfig.WriteKey slot
	struct atca_nonce_in_out nonce_param;
	struct atca_gen_dig_in_out gendig_param;
	struct atca_privWrite_mac_in_out authMac_param;
	struct atca_temp_key tempkey;
	uint8_t tx_buffer[CHECKMAC_COUNT];
	uint8_t rx_buffer[MAC_RSP_SIZE];
	uint8_t input_text[ATCA_PLAIN_TEXT_SIZE];
	uint8_t encypted_text[ATCA_PLAIN_TEXT_SIZE];
	uint8_t key_writeIndex = 4;
	uint8_t num_in[NONCE_NUMIN_SIZE] = {
		0x50, 0xDF, 0xD7, 0x82, 0x5B, 0x10, 0x0F, 0x2D, 0x8C, 0xD2, 0x0A, 0x91, 0x15, 0xAC, 0xED, 0xCF,
		0x5A, 0xEE, 0x76, 0x94 };

	do
	{
		memset(mac, 0, MAC_SIZE);
		memset(pubkey_result, 0, ATCA_PUB_KEY_SIZE);
		memset(input_text, 0, ATCA_PLAIN_TEXT_SIZE);
		memset(encypted_text, 0, ATCA_PLAIN_TEXT_SIZE);
		memcpy(accessKey, data, sizeof(accessKey));
		memcpy(privKey, priv, sizeof(privKey));

		// input_text : 4bytes zeros + 32 bytes private key
		// for private key write 4 bytes starting at 0

		memcpy(&input_text[key_writeIndex], &privKey, sizeof(privKey));

		ret = atca_wake();
		if (ret != ATCA_SUCCESS) {
			break;
		}

		ret = atca_execute(ATCA_NONCE, NONCE_MODE_SEED_UPDATE, 0, NONCE_NUMIN_SIZE, num_in,
			0, NULL, 0, NULL, sizeof(tx_buffer), tx_buffer, sizeof(rx_buffer), rx_buffer);
		if (ret != ATCA_SUCCESS) {
			break;
		}
		nonce_param.mode = NONCE_MODE_SEED_UPDATE;
		nonce_param.num_in = num_in;
		nonce_param.rand_out = &rx_buffer[ATCA_RSP_DATA_IDX];
		nonce_param.temp_key = &tempkey;
		ret = atca_nonce(&nonce_param);
		if (ret != ATCA_SUCCESS) {
			break;
		}
		ret = atca_send_gendig(GENDIG_ZONE_DATA, accessKeyID, tempkey.value);
		if (ret != ATCA_SUCCESS) {
			break;
		}
		gendig_param.zone = GENDIG_ZONE_DATA;
		gendig_param.key_id = accessKeyID;
		gendig_param.stored_value = accessKey;
		gendig_param.temp_key = &tempkey;
		ret = atca_gen_dig(&gendig_param);
		if (ret != ATCA_SUCCESS)
		{
			break;
		}

		// An input authenticating MAC is computed with SHA-256

		authMac_param.zone = PRIVWRITE_MODE_ENCRYPT;
		authMac_param.key_id = slot;
		authMac_param.ecc_private_key = privKey;
		authMac_param.input_data = input_text;
		authMac_param.crypto_data = encypted_text;
		authMac_param.temp_key = &tempkey;
		authMac_param.mac = mac;
		ret = atca_auth_mac(&authMac_param);
		if (ret != ATCA_SUCCESS)
		{
			break;
		}

		// Once the Data Zone is locked, the input data must be encrypted using TempKey.

		ret = atca_send_privWrite(PRIVWRITE_MODE_ENCRYPT, slot, encypted_text, mac);
		if (ret != ATCA_SUCCESS)
		{
			break;
		}

		ret = atca_send_genkey(GENKEY_MODE_PUBLIC, (uint16_t)slot, pubkey_result);
		if (ret != ATCA_SUCCESS)
		{
			break;
		}
		memcpy(pubkey, pubkey_result, ATCA_PUB_KEY_SIZE);
	} while (0);

	(void)atca_sleep();

	return ret;
}

// Restart the Watchdog Timer by sending an idle-wake
uint8_t atca_restart_wdt(void)
{
	uint8_t ret_code = ATCA_SUCCESS;
	uint8_t wakeup_response[ATCA_RSP_SIZE_MIN];

	/* IDLE */
	ret_code = atca_idle();
	if (ret_code == ATCA_SUCCESS)
	{
		//printf("Idle success!\r\n\n");
	}
	else
	{
		//printf("Idle failed!\r\n");
		//printf("Idle ret_code = 0x%02X\r\n\n",ret_code);
	}

	/* WAKE UP DEVICE */
	ret_code = atca_wakeup(wakeup_response);
#ifdef	CHECK_RET_CODE
	if (ret_code == ATCA_SUCCESS)
#else
	if ((wakeup_response[0] == 0x04) && (wakeup_response[1] == 0x11))
#endif
	{
		//printf("Wakeup success!\r\n\n");
	}
	else
	{
		//printf("Wakeup failed!\r\n\n");
	}

	return ret_code;
}


