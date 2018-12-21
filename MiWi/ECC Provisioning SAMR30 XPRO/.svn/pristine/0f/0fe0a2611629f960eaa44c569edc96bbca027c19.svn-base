/** \file
 *  \brief  Communication Layer of ATCA Library
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

#include <string.h>
#include <atca_comm.h>                // definitions and declarations for the Communication module
#include <atca_config.h>                // definitions and declarations for the Communication module
#include <atca_lib_return_codes.h>    // declarations of function return codes
#include <atca_protocol_adapter.h>
#include <atca_crypto_utils.h>
#include "arch/common/atca_timer_utilities.h"     // definitions for delay functions
#include "atca_cmds.h"


/** \structure used to define delay's for various devices**/
struct atca_device_elements_t atca_delay;
struct atca_device_elements_t *ptr_atca_delay;
union dev_rev_t dev_rev;

uint8_t	atca_ecc108_delay[] = {
								ECC108_CHECKMAC_DELAY, ECC108_COUNTER_DELAY, ECC108_DERIVE_KEY_DELAY, ECC108_ECDH_DELAY,
								ECC108_GENDIG_DELAY, ECC108_GENKEY_DELAY, ECC108_HMAC_DELAY, ECC108_INFO_DELAY,
								ECC108_LOCK_DELAY, ECC108_MAC_DELAY, ECC108_NONCE_DELAY, ECC108_PAUSE_DELAY,
								ECC108_PRIVWRITE_DELAY, ECC108_RANDOM_DELAY, ECC108_READ_DELAY, ECC108_SIGN_DELAY,
								ECC108_SHA_DELAY, ECC108_UPDATE_DELAY, ECC108_VERIFY_DELAY, ECC108_WRITE_DELAY
};
uint8_t	atca_ecc508_delay[] = {	ECC508_CHECKMAC_DELAY, ECC508_COUNTER_DELAY, ECC508_DERIVE_KEY_DELAY, ECC508_ECDH_DELAY,
								ECC508_GENDIG_DELAY, ECC508_GENKEY_DELAY, ECC508_HMAC_DELAY, ECC508_INFO_DELAY,
								ECC508_LOCK_DELAY, ECC508_MAC_DELAY, ECC508_NONCE_DELAY, ECC508_PAUSE_DELAY,
								ECC508_PRIVWRITE_DELAY, ECC508_RANDOM_DELAY, ECC508_READ_DELAY, ECC508_SIGN_DELAY,
								ECC508_SHA_DELAY, ECC508_UPDATE_DELAY, ECC508_VERIFY_DELAY, ECC508_WRITE_DELAY
								};
uint8_t	atca_sha204_delay[] = {
								SHA204_CHECKMAC_DELAY, SHA204_COUNTER_DELAY, SHA204_DERIVE_KEY_DELAY, SHA204_ECDH_DELAY,
								SHA204_GENDIG_DELAY, SHA204_GENKEY_DELAY, SHA204_HMAC_DELAY, SHA204_INFO_DELAY,
								SHA204_LOCK_DELAY, SHA204_MAC_DELAY, SHA204_NONCE_DELAY, SHA204_PAUSE_DELAY,
								SHA204_PRIVWRITE_DELAY, SHA204_RANDOM_DELAY, SHA204_READ_DELAY, SHA204_SIGN_DELAY,
								SHA204_SHA_DELAY, SHA204_UPDATE_DELAY, SHA204_VERIFY_DELAY, SHA204_WRITE_DELAY
								};
								
uint8_t	atca_ecc108_max_delay[] = {
								ECC108_CHECKMAC_EXEC_MAX, ECC108_COUNTER_EXEC_MAX, ECC108_DERIVE_KEY_EXEC_MAX, ECC108_ECDH_EXEC_MAX,
								ECC108_GENDIG_EXEC_MAX, ECC108_GENKEY_EXEC_MAX, ECC108_HMAC_EXEC_MAX, ECC108_INFO_EXEC_MAX,
								ECC108_LOCK_EXEC_MAX, ECC108_MAC_EXEC_MAX, ECC108_NONCE_EXEC_MAX, ECC108_PAUSE_EXEC_MAX,
								ECC108_PRIVWRITE_EXEC_MAX, ECC108_RANDOM_EXEC_MAX, ECC108_READ_EXEC_MAX, ECC108_SIGN_EXEC_MAX,
								ECC108_SHA_EXEC_MAX, ECC108_UPDATE_EXEC_MAX, ECC108_VERIFY_EXEC_MAX, ECC108_WRITE_EXEC_MAX
};
uint8_t	atca_ecc508_max_delay[] = {
								ECC508_CHECKMAC_EXEC_MAX, ECC508_COUNTER_EXEC_MAX, ECC508_DERIVE_KEY_EXEC_MAX, ECC508_ECDH_EXEC_MAX,
								ECC508_GENDIG_EXEC_MAX, ECC508_GENKEY_EXEC_MAX, ECC508_HMAC_EXEC_MAX, ECC508_INFO_EXEC_MAX,
								ECC508_LOCK_EXEC_MAX, ECC508_MAC_EXEC_MAX, ECC508_NONCE_EXEC_MAX, ECC508_PAUSE_EXEC_MAX,
								ECC508_PRIVWRITE_EXEC_MAX, ECC508_RANDOM_EXEC_MAX, ECC508_READ_EXEC_MAX, ECC508_SIGN_EXEC_MAX,
								ECC508_SHA_EXEC_MAX, ECC508_UPDATE_EXEC_MAX, ECC508_VERIFY_EXEC_MAX, ECC508_WRITE_EXEC_MAX
};
uint8_t	atca_sha204_max_delay[] = {
								SHA204_CHECKMAC_EXEC_MAX, SHA204_COUNTER_EXEC_MAX, SHA204_DERIVE_KEY_EXEC_MAX, SHA204_ECDH_EXEC_MAX,
								SHA204_GENDIG_EXEC_MAX, SHA204_GENKEY_EXEC_MAX, SHA204_HMAC_EXEC_MAX, SHA204_INFO_EXEC_MAX,
								SHA204_LOCK_EXEC_MAX, SHA204_MAC_EXEC_MAX,SHA204_NONCE_EXEC_MAX, SHA204_PAUSE_EXEC_MAX,
								SHA204_PRIVWRITE_EXEC_MAX, SHA204_RANDOM_EXEC_MAX, SHA204_READ_EXEC_MAX, SHA204_SIGN_EXEC_MAX,
								SHA204_SHA_EXEC_MAX, SHA204_UPDATE_EXEC_MAX, SHA204_VERIFY_EXEC_MAX, SHA204_WRITE_EXEC_MAX
};

/** \brief This function runs a communication sequence.

 */
uint8_t atca_set_device_delay(void)
{
	uint8_t ret = ATCA_SUCCESS;
	uint8_t response[ATCA_RSP_SIZE_MIN];
	uint8_t devrev_response[ATCA_RSP_SIZE_4];
	uint8_t atca_device_string[40];

	// Assigning initial max delay value to info command to run command for first time
	atca_delay.atca_delay_to_apply[INFO_DELAY] = ATCA_COMMAND_EXEC_MAX;
	ret = atca_wakeup(response);
	ret = atca_info(devrev_response);
//	asm("nop");

	dev_rev.revision.dev_rev_byte0 = devrev_response[1];
	dev_rev.revision.dev_rev_byte1 = devrev_response[2];
	dev_rev.revision.dev_rev_byte2 = devrev_response[3];
	dev_rev.revision.dev_rev_byte3 = devrev_response[4];

	if (dev_rev.revision.dev_rev_byte1 == 0x02)
	{
		memcpy(atca_delay.atca_delay_to_apply, &atca_sha204_delay[CHECKMAC_DELAY], (ATCA_DELAY_ELEMENTS));
		memcpy(atca_delay.atca_max_delay_to_apply, &atca_sha204_max_delay[CHECKMAC_DELAY], (ATCA_DELAY_ELEMENTS));
		memcpy(atca_device_string, STRING_SHA204, sizeof(STRING_SHA204));
//		asm("nop");
	}else if (dev_rev.revision.dev_rev_byte2 == 0x50)
	{
		memcpy(atca_delay.atca_delay_to_apply, &atca_ecc508_delay[CHECKMAC_DELAY], (ATCA_DELAY_ELEMENTS));
		memcpy(atca_delay.atca_max_delay_to_apply, &atca_ecc508_max_delay[CHECKMAC_DELAY], (ATCA_DELAY_ELEMENTS));
		memcpy(atca_device_string, STRING_ECC508, sizeof(STRING_ECC508));
	}
	else if (dev_rev.revision.dev_rev_byte2 == 0x10)
	{
		memcpy(atca_delay.atca_delay_to_apply, &atca_ecc108_delay[CHECKMAC_DELAY], (ATCA_DELAY_ELEMENTS));
		memcpy(atca_delay.atca_max_delay_to_apply, &atca_ecc108_max_delay[CHECKMAC_DELAY], (ATCA_DELAY_ELEMENTS));
		memcpy(atca_device_string, STRING_ECC108, sizeof(STRING_ECC108));
	}else
	{
		ret = ATCA_CMD_FAIL;
	}

	return ret;
}


/** \brief This function runs a communication sequence.
 *
 * Append CRC to tx buffer, send command, delay, and verify response after receiving it.
 *
 * The first byte in tx buffer must be the byte count of the packet.
 * If CRC or count of the response is incorrect, or a command byte did not get acknowledged
 * this function requests re-sending the response.
 * If the response contains an error status, this function resends the command.
 *
 * \param[in] tx_buffer pointer to command
 * \param[in] rx_size size of response buffer
 * \param[out] rx_buffer pointer to response buffer
 * \param[in] execution_delay Start polling for a response after this many ms.
 * \param[in] execution_timeout polling timeout in ms
 * \return status of the operation
 */
uint8_t atca_send_and_receive(uint8_t *tx_buffer, uint8_t rx_size, uint8_t *rx_buffer,
			uint8_t execution_delay, uint8_t execution_timeout)
{
	volatile uint8_t ret_code = ATCA_FUNC_FAIL;
	uint8_t ret_code_resync;
	uint8_t n_retries_send;
	uint8_t n_retries_receive;
	uint8_t i;
	uint8_t status_byte;
	uint8_t count = tx_buffer[ATCA_COUNT_IDX];
	uint8_t count_minus_crc = count - ATCA_CRC_SIZE;
	uint32_t execution_timeout_us = ((uint32_t) execution_timeout * 1000) + ATCA_RESPONSE_TIMEOUT;
	volatile uint32_t timeout_countdown;

	// Append CRC.
	atca_calc_crc(count_minus_crc, tx_buffer, tx_buffer + count_minus_crc);

	// Retry loop for sending a command and receiving a response.
	n_retries_send = ATCA_RETRY_COUNT + 1;

	while ((n_retries_send-- > 0) && (ret_code != ATCA_SUCCESS)) {

		// Send command.
		ret_code = atca_send_command(count, tx_buffer);
		if (ret_code != ATCA_SUCCESS) {
			if (atca_resync(rx_size, rx_buffer) == ATCA_RX_NO_RESPONSE)
				// The device seems to be dead in the water.
				return ret_code;
			else
				continue;
		}

		// Wait minimum command execution time and then start polling for a response.
		atca_delay_ms(execution_delay);

		// Retry loop for receiving a response.
		n_retries_receive = ATCA_RETRY_COUNT + 1;
		while (n_retries_receive-- > 0) {

			// Reset response buffer.
			for (i = 0; i < rx_size; i++)
				rx_buffer[i] = 0;

			// Poll for response.
			timeout_countdown = execution_timeout_us;
			do {
				ret_code = atca_receive_response(rx_size, rx_buffer);
				if (ret_code == ATCA_COMM_FAIL)
				{
					ret_code = ATCA_RX_NO_RESPONSE;		//Pulling for response not an error
				}
				timeout_countdown -= ATCA_RESPONSE_TIMEOUT;
			} while ((timeout_countdown > ATCA_RESPONSE_TIMEOUT) && (ret_code == ATCA_RX_NO_RESPONSE));

			if (ret_code == ATCA_RX_NO_RESPONSE) {
				// We did not receive a response. Re-synchronize and send command again.
				if (atca_resync(rx_size, rx_buffer) == ATCA_RX_NO_RESPONSE)
					// The device seems to be dead in the water.
					return ret_code;
				else
					break;
			}

			// Check whether we received a valid response.
			if (ret_code == ATCA_INVALID_SIZE) {
				// We see 0xFF for the count when communication got out of sync.
				ret_code_resync = atca_resync(rx_size, rx_buffer);
				if (ret_code_resync == ATCA_SUCCESS)
					// We did not have to wake up the device. Try receiving response again.
					continue;
				if (ret_code_resync == ATCA_RESYNC_WITH_WAKEUP)
					// We could re-synchronize, but only after waking up the device.
					// Re-send command.
					break;
				else
					// We failed to re-synchronize.
					return ret_code;
			}

			// We received a response of valid size.
			// Check the consistency of the response.
			ret_code = atca_check_crc(rx_buffer);
			if (ret_code == ATCA_SUCCESS) {
				// Received valid response.
				if (rx_buffer[ATCA_COUNT_IDX] > ATCA_RSP_SIZE_MIN)
					// Received non-status response. We are done.
					return ret_code;

				// Received status response.
				status_byte = rx_buffer[ATCA_RSP_DATA_IDX];

				// Translate the four possible device status error codes
				// into library return codes.
				if (status_byte == ATCA_STATUS_BYTE_PARSE)
					return ATCA_PARSE_ERROR;
				if (status_byte == ATCA_STATUS_BYTE_EXEC)
					return ATCA_CMD_FAIL;
				if (status_byte == ATCA_STATUS_BYTE_COMM) {
					// In case of the device status byte indicating a communication
					// error this function exits the retry loop for receiving a response
					// and enters the overall retry loop
					// (send command / receive response).
					ret_code = ATCA_STATUS_CRC;
					break;
				}
				if (status_byte == ATCA_STATUS_BYTE_ECC) {
					// In case of the device status byte indicating an ECC fault
					// this function exits the retry loop for receiving a response
					// and enters the overall retry loop
					// (send command / receive response).
					ret_code = ATCA_STATUS_ECC;
					break;
				}

				// Received status response from CheckMAC, DeriveKey, GenDig,
				// Lock, Nonce, Pause, UpdateExtra, Verify, or Write command.
				return ret_code;
			}

			else {
				// Received response with incorrect CRC.
				ret_code_resync = atca_resync(rx_size, rx_buffer);
				if (ret_code_resync == ATCA_SUCCESS)
					// We did not have to wake up the device. Try receiving response again.
					continue;
				if (ret_code_resync == ATCA_RESYNC_WITH_WAKEUP)
					// We could re-synchronize, but only after waking up the device.
					// Re-send command.
					break;
				else
					// We failed to re-synchronize.
					return ret_code;
			} // block end of check response consistency

		} // block end of receive retry loop

	} // block end of send and receive retry loop

	return ret_code;
}


uint8_t atca_check_parameters(uint8_t op_code, uint8_t param1, uint16_t param2,
		uint8_t datalen1, uint8_t *data1, uint8_t datalen2, uint8_t *data2, uint8_t datalen3, uint8_t *data3,
		uint8_t tx_size, uint8_t *tx_buffer, uint8_t rx_size, uint8_t *rx_buffer);

/** \ingroup ATCA_command_marshaling
 * \brief This function checks the parameters for ateccm_execute().
 *
 *
 * \param[in] op_code command op-code
 * \param[in] param1 first parameter
 * \param[in] param2 second parameter
 * \param[in] datalen1 number of bytes in first data block
 * \param[in] data1 pointer to first data block
 * \param[in] datalen2 number of bytes in second data block
 * \param[in] data2 pointer to second data block
 * \param[in] datalen3 number of bytes in third data block
 * \param[in] data3 pointer to third data block
 * \param[in] tx_size size of tx buffer
 * \param[in] tx_buffer pointer to tx buffer
 * \param[in] rx_size size of rx buffer
 * \param[out] rx_buffer pointer to rx buffer
 * \return status of the operation
 */
uint8_t atca_check_parameters(uint8_t op_code, uint8_t param1, uint16_t param2,
		uint8_t datalen1, uint8_t *data1, uint8_t datalen2, uint8_t *data2, uint8_t datalen3, uint8_t *data3,
		uint8_t tx_size, uint8_t *tx_buffer, uint8_t rx_size, uint8_t *rx_buffer)
{
#ifdef ATCA_CHECK_PARAMETERS

	uint8_t len = datalen1 + datalen2 + datalen3 + ATCA_CMD_SIZE_MIN;
	if (!tx_buffer || (tx_size < len) || (rx_size < ATCA_RSP_SIZE_MIN) || !rx_buffer)
		return ATCA_BAD_PARAM;

	if ((datalen1 > 0 && !data1) || (datalen2 > 0 && !data2) || (datalen3 > 0 && !data3))
		return ATCA_BAD_PARAM;

	// Check parameters depending on op-code.
	switch (op_code) {
	case ATCA_CHECKMAC:
		if (!data1 || !data2 || (param1 & ~CHECKMAC_MODE_MASK) || (param2 > ATCA_KEY_ID_MAX))
			// Neither data1 nor data2 can be null.
			// param1 has to match an allowed CheckMac mode.
			// key_id > 15 not allowed
			return ATCA_BAD_PARAM;
		break;

	case ATCA_DERIVE_KEY:
		if (param2 > ATCA_KEY_ID_MAX)
			// key_id > 15 not allowed
			return ATCA_BAD_PARAM;
		break;

	case ATCA_GENDIG:
		if ((param1 > GENDIG_ZONE_DATA) || (param2 > ATCA_KEY_ID_MAX))
			// param1 has to match an allowed GenDig mode.
			// key_id > 15 not allowed
			return ATCA_BAD_PARAM;
		break;

	case ATCA_GENKEY:
		if ((param1 & ~GENKEY_MODE_MASK) || (param2 > ATCA_KEY_ID_MAX))
			// param1 has to match an allowed GenKey mode.
			// key_id > 15 not allowed
			return ATCA_BAD_PARAM;
		break;

	case ATCA_HMAC:
		if (param1 & ~HMAC_MODE_MASK)
			// param1 has to match an allowed HMAC mode.
			return ATCA_BAD_PARAM;
		break;

	case ATCA_INFO:
		if ((param1 > INFO_MODE_MAX) || (param2 > ATCA_KEY_ID_MAX))
			// param1 has to match an allowed Info mode.
			// param2 > 15 not allowed (when mode = KeyValid)
			return ATCA_BAD_PARAM;
		break;

	case ATCA_LOCK:
		if ((param1 & ~LOCK_ZONE_MASK)
					|| ((param1 & LOCK_ZONE_NO_CRC) && param2))
			// param1 has to match an allowed Lock mode.
			// If no CRC is required the CRC should be 0.
			return ATCA_BAD_PARAM;
		break;

	case ATCA_MAC:
		if ((param1 & ~MAC_MODE_MASK)
					|| (!(param1 & MAC_MODE_BLOCK2_TEMPKEY) && !data1))
			// param1 has to match an allowed MAC mode.
			// If the MAC mode requires challenge data, data1 should not be null.
			return ATCA_BAD_PARAM;
		break;

	case ATCA_NONCE:
		if (!data1 || (param1 > NONCE_MODE_PASSTHROUGH)	|| (param1 == NONCE_MODE_INVALID))
			// data1 cannot be null.
			// param1 has to match an allowed Nonce mode.
			return ATCA_BAD_PARAM;
		break;

	case ATCA_PAUSE:
		// param1 can have any value. param2 and data are not used by this command.
		break;

	case ATCA_PRIVWRITE:
		if (!data1 || (param1 &  ~PRIVWRITE_ZONE_MASK) || (param2 > ATCA_KEY_ID_MAX))
			// data1 cannot be null.
			// param1 has to match an allowed PrivWrite mode.
			// key_id > 15 not allowed
			return ATCA_BAD_PARAM;
		break;

	case ATCA_RANDOM:
		if (param1 > RANDOM_NO_SEED_UPDATE)
			// param1 has to match an allowed Random mode.
			return ATCA_BAD_PARAM;
		break;

	case ATCA_READ:
		if (param1 & ~READ_ZONE_MASK)
			// param1 has to match an allowed Read mode.
			return ATCA_BAD_PARAM;
		break;

	case ATCA_SIGN:
		if ((param1 & ~SIGN_MODE_MASK) || (param2 > ATCA_KEY_ID_MAX))
			// param1 has to match an allowed Sign mode.
			// key_id > 15 not allowed
			return ATCA_BAD_PARAM;
		break;

	case ATCA_SHA:
		if (param1 & ~SHA_MODE_MASK)
			// param1 has to match an allowed SHA mode.
			return ATCA_BAD_PARAM;
		break;
		

	case ATCA_UPDATE_EXTRA:
		if (param1 > UPDATE_CONFIG_BYTE_85)
			// param1 has to match an allowed UpdateExtra mode.
			return ATCA_BAD_PARAM;
		break;

	case ATCA_VERIFY:
		if (param1 & ~VERIFY_MODE_MASK)
			// param1 has to match an allowed Verify mode.
			return ATCA_BAD_PARAM;
		break;

	case ATCA_WRITE:
		if (!data1 || (param1 & ~WRITE_ZONE_MASK))
			// data1 cannot be null.
			// param1 has to match an allowed Write mode.
			return ATCA_BAD_PARAM;
		break;

	default:
		// unknown op-code
		return ATCA_BAD_PARAM;
	}
#endif

	return ATCA_SUCCESS;
}


/** \brief This function creates a command packet, sends it, and receives its response.
 *
 * \param[in] op_code command op-code
 * \param[in] param1 first parameter
 * \param[in] param2 second parameter
 * \param[in] datalen1 number of bytes in first data block
 * \param[in] data1 pointer to first data block
 * \param[in] datalen2 number of bytes in second data block
 * \param[in] data2 pointer to second data block
 * \param[in] datalen3 number of bytes in third data block
 * \param[in] data3 pointer to third data block
 * \param[in] tx_size size of tx buffer
 * \param[in] tx_buffer pointer to tx buffer
 * \param[in] rx_size size of rx buffer
 * \param[out] rx_buffer pointer to rx buffer
 * \return status of the operation
 */
uint8_t atca_execute(uint8_t op_code, uint8_t param1, uint16_t param2,
			uint8_t datalen1, uint8_t *data1, uint8_t datalen2, uint8_t *data2, uint8_t datalen3, uint8_t *data3,
			uint8_t tx_size, uint8_t *tx_buffer, uint8_t rx_size, uint8_t *rx_buffer)
{
	static uint8_t poll_delay = 0; 
	static uint8_t poll_timeout = 0; 
	static uint8_t response_size = 0;
	uint8_t *p_buffer = NULL;
	uint8_t len = 0;

	// Define ATCA_CHECK_PARAMETERS to compile and link this feature.
	uint8_t ret_code = atca_check_parameters(op_code, param1, param2,
				datalen1, data1, datalen2, data2, datalen3, data3,
				tx_size, tx_buffer, rx_size, rx_buffer);
	if (ret_code != ATCA_SUCCESS) {
		(void) atca_sleep();
		return ret_code;
	}

	// Supply delays and response size.
	switch (op_code) {
	case ATCA_CHECKMAC:
		poll_delay = (uint8_t)atca_delay.atca_delay_to_apply[CHECKMAC_DELAY];
		poll_timeout = (uint8_t)atca_delay.atca_max_delay_to_apply[CHECKMAC_EXEC_MAX] - poll_delay;
		response_size = CHECKMAC_RSP_SIZE;
		break;

	case ATCA_COUNTER:
		poll_delay = (uint8_t)atca_delay.atca_delay_to_apply[COUNTER_DELAY];
		poll_timeout = (uint8_t)atca_delay.atca_max_delay_to_apply[COUNTER_EXEC_MAX] - poll_delay;
		response_size = COUNTER_RSP_SIZE;
		break;

	case ATCA_DERIVE_KEY:
		poll_delay = (uint8_t)atca_delay.atca_delay_to_apply[DERIVE_KEY_DELAY];
		poll_timeout = (uint8_t)atca_delay.atca_max_delay_to_apply[DERIVE_KEY_EXEC_MAX] - poll_delay;
		response_size = DERIVE_KEY_RSP_SIZE;
		break;

	case ATCA_ECDH:
		poll_delay = (uint8_t)atca_delay.atca_delay_to_apply[ECDH_DELAY];
		poll_timeout = (uint8_t)atca_delay.atca_max_delay_to_apply[ECDH_EXEC_MAX] - poll_delay;
		response_size = ECDH_RSP_SIZE;
		break;

	case ATCA_GENDIG:
		poll_delay = (uint8_t)atca_delay.atca_delay_to_apply[GENDIG_DELAY];
		poll_timeout = (uint8_t)atca_delay.atca_max_delay_to_apply[GENDIG_EXEC_MAX] - poll_delay;
		response_size = GENDIG_RSP_SIZE;
		break;

	case ATCA_GENKEY:
		poll_delay = (uint8_t)atca_delay.atca_delay_to_apply[GENKEY_DELAY];
		poll_timeout = (uint8_t)atca_delay.atca_max_delay_to_apply[GENKEY_EXEC_MAX] - poll_delay;
		response_size = param1 == GENKEY_MODE_DIGEST
							? GENKEY_RSP_SIZE_SHORT : GENKEY_RSP_SIZE_LONG;
		break;

	case ATCA_HMAC:
		poll_delay = (uint8_t)atca_delay.atca_delay_to_apply[HMAC_DELAY];
		poll_timeout = (uint8_t)atca_delay.atca_max_delay_to_apply[HMAC_EXEC_MAX] - poll_delay;
		response_size = HMAC_RSP_SIZE;
		break;

	case ATCA_INFO:
		poll_delay = (uint8_t)atca_delay.atca_delay_to_apply[INFO_DELAY];
		poll_timeout = (uint8_t)atca_delay.atca_max_delay_to_apply[INFO_EXEC_MAX] - poll_delay;
		response_size = INFO_RSP_SIZE;
		break;

	case ATCA_LOCK:
		poll_delay = (uint8_t)atca_delay.atca_delay_to_apply[LOCK_DELAY];
		poll_timeout = (uint8_t)atca_delay.atca_max_delay_to_apply[LOCK_EXEC_MAX] - poll_delay;
		response_size = LOCK_RSP_SIZE;
		break;

	case ATCA_MAC:
		poll_delay = (uint8_t)atca_delay.atca_delay_to_apply[MAC_DELAY];
		poll_timeout = (uint8_t)atca_delay.atca_max_delay_to_apply[MAC_EXEC_MAX] - poll_delay;
		response_size = MAC_RSP_SIZE;
		break;

	case ATCA_NONCE:
		poll_delay = (uint8_t)atca_delay.atca_delay_to_apply[NONCE_DELAY];
		poll_timeout = (uint8_t)atca_delay.atca_max_delay_to_apply[NONCE_EXEC_MAX] - poll_delay;
		response_size = param1 == NONCE_MODE_PASSTHROUGH
							? NONCE_RSP_SIZE_SHORT : NONCE_RSP_SIZE_LONG;
		break;

	case ATCA_PAUSE:
		poll_delay = (uint8_t)atca_delay.atca_delay_to_apply[PAUSE_DELAY];
		poll_timeout = (uint8_t)atca_delay.atca_max_delay_to_apply[PAUSE_EXEC_MAX] - poll_delay;
		response_size = PAUSE_RSP_SIZE;
		break;

	case ATCA_PRIVWRITE:
		poll_delay = (uint8_t)atca_delay.atca_delay_to_apply[PRIVWRITE_DELAY];
		poll_timeout = (uint8_t)atca_delay.atca_max_delay_to_apply[PRIVWRITE_EXEC_MAX] - poll_delay;
		response_size = PRIVWRITE_RSP_SIZE;
		break;

	case ATCA_RANDOM:
		poll_delay = (uint8_t)atca_delay.atca_delay_to_apply[RANDOM_DELAY];
		poll_timeout = (uint8_t)atca_delay.atca_max_delay_to_apply[RANDOM_EXEC_MAX] - poll_delay;
		response_size = RANDOM_RSP_SIZE;
		break;

	case ATCA_READ:
		poll_delay = (uint8_t)atca_delay.atca_delay_to_apply[READ_DELAY];
		poll_timeout = (uint8_t)atca_delay.atca_max_delay_to_apply[READ_EXEC_MAX] - poll_delay;
		response_size = (param1 & ATCA_ZONE_COUNT_FLAG)
							? READ_32_RSP_SIZE : READ_4_RSP_SIZE;
		break;

	case ATCA_SIGN:
		poll_delay = (uint8_t)atca_delay.atca_delay_to_apply[SIGN_DELAY];
		poll_timeout = (uint8_t)atca_delay.atca_max_delay_to_apply[SIGN_EXEC_MAX] - poll_delay;
		response_size = SIGN_RSP_SIZE;
		break;

	case ATCA_SHA:
		poll_delay = (uint8_t)atca_delay.atca_delay_to_apply[SHA_DELAY];
		poll_timeout = (uint8_t)atca_delay.atca_max_delay_to_apply[SHA_EXEC_MAX] - poll_delay;
		
		response_size = (param1 & SHA_SHA256_END_MASK)
							? SHA_RSP_DIGEST_SIZE : SHA_RSP_SIZE;
		break;

	case ATCA_UPDATE_EXTRA:
		poll_delay = (uint8_t)atca_delay.atca_delay_to_apply[UPDATE_DELAY];
		poll_timeout = (uint8_t)atca_delay.atca_max_delay_to_apply[UPDATE_EXEC_MAX] - poll_delay;
		response_size = UPDATE_RSP_SIZE;
		break;

	case ATCA_VERIFY:
		poll_delay = (uint8_t)atca_delay.atca_delay_to_apply[VERIFY_DELAY];
		poll_timeout = (uint8_t)atca_delay.atca_max_delay_to_apply[VERIFY_EXEC_MAX] - poll_delay;
		response_size = VERIFY_RSP_SIZE;
	break;

	case ATCA_WRITE:
		poll_delay = (uint8_t)atca_delay.atca_delay_to_apply[WRITE_DELAY];
		poll_timeout = (uint8_t)atca_delay.atca_max_delay_to_apply[WRITE_EXEC_MAX] - poll_delay;
		response_size = WRITE_RSP_SIZE;
		break;

	default:
		poll_delay = 0;
		poll_timeout = ATCA_COMMAND_EXEC_MAX;
		response_size = rx_size;
	}

	
	// Assemble command.
	len = datalen1 + datalen2 + datalen3 + ATCA_CMD_SIZE_MIN;
	p_buffer = tx_buffer;
	*p_buffer++ = len;
	*p_buffer++ = op_code;
	*p_buffer++ = param1;
	*p_buffer++ = param2 & 0xFF;
	*p_buffer++ = param2 >> 8;

	if (datalen1 > 0) {
		memcpy(p_buffer, data1, datalen1);
		p_buffer += datalen1;
	}
	if (datalen2 > 0) {
		memcpy(p_buffer, data2, datalen2);
		p_buffer += datalen2;
	}
	if (datalen3 > 0) {
		memcpy(p_buffer, data3, datalen3);
		p_buffer += datalen3;
	}

	atca_calc_crc(len - ATCA_CRC_SIZE, tx_buffer, p_buffer);

	// Send command and receive response.
	ret_code = atca_send_and_receive(&tx_buffer[0], response_size,
				&rx_buffer[0],	poll_delay, poll_timeout);

	// Put device to sleep if command fails
	if (ret_code != ATCA_SUCCESS)
		(void) atca_sleep();

	return ret_code;
}
