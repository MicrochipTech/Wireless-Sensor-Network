/** \file
 *  \brief  Functions for I2C Physical Hardware Independent Layer of ATCA Library
 *  this module implements the API in atca_protocol_adapter.h
 * 
 * \author Atmel Crypto Products
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
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
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
 * \todo Develop solution to allow i2c, swi, and atphy to all live together in harmony. 
 *       Right now, you have to choose one
 */

#include <string.h>
#include <atca_config.h>
#include <atca_lib_return_codes.h>    // declarations of function return codes
#include <atca_protocol_adapter.h>
#include "arch/common/atca_timer_utilities.h"     // definitions for delay functions
#include "protocols/atca_kit.h"
#include "atca_comm.h"




/** \brief This function initializes the hardware.
 */
uint8_t atca_init(void)
{
	// Enable the kit communication to the device
	uint8_t ret = ATCA_SUCCESS;
	ret = kit_enable();

	return ret;
}


/** \brief This function generates a Wake-up pulse and waits the delay.
 * \return status of the operation
 */
uint8_t atca_wakeup(uint8_t *response)
{
	uint8_t ret = ATCA_SUCCESS;
	uint8_t expected_response[] = { 0x04, 0x11, 0x33, 0x43 };

	// Have the kit send a wake to the device
	ret = kit_send_wake();

	// Copy the expected respoonse upon success
	if (ret == ATCA_SUCCESS)
	{
		memcpy(response, expected_response, sizeof(expected_response));
	}

	return ret;
}

/** \brief This function attempts to wake up the device 
 * \return For successful wake, return ATCA_SUCCESS.  For failed wake, return ATCA_WAKE_FAILED.
 */
uint8_t atca_wake(void)
{
	uint8_t ret = ATCA_SUCCESS;
	uint8_t response[ATCA_RSP_SIZE_MIN];

	// Wake up the device
	ret = atca_wakeup(response);

	return ret; 
}


/** \brief This function sends a command to the device.
 * \param[in] command pointer to command buffer
 * \param[in] count number of bytes to send
 * \return status of the operation
 */
uint8_t atca_send_command(uint8_t count, uint8_t* command)
{
	uint8_t ret = ATCA_SUCCESS;

	// Send the bytes directly to the kit
	ret = kit_send_bytes(command, count);

	return ret;
}


/** \brief This function puts the device into idle state.
 * \return status of the operation
 */
uint8_t atca_idle(void)
{
	uint8_t ret = ATCA_SUCCESS;

	// Send the idle token to the kit
	ret = kit_send_idle();

	return ret;
}


/** \brief This function puts the device into low-power state.
 *  \return status of the operation
 */
uint8_t atca_sleep(void)
{
	uint8_t ret = ATCA_SUCCESS;

	// Send the idle token to the kit
	ret = kit_send_sleep();

	return ret;
}


/** \brief This function resets the I/O buffer of the device.
 * \return status of the operation
 */
uint8_t atca_reset_io(void)
{
	uint8_t ret = ATCA_SUCCESS;
	return ret;
}


/** \brief This function receives a response from the device.
 *
 * \param[in] size size of rx buffer
 * \param[out] response pointer to rx buffer
 * \return status of the operation
 */
uint8_t atca_receive_response(uint8_t size, uint8_t* rcv)
{
	uint8_t ret = ATCA_SUCCESS;
	uint16_t rcv_len = size;

	// Receive the bytes.
	ret = kit_receive_bytes(rcv, &rcv_len);

	return ret;
}


/** \brief This function resynchronizes communication. There is nothing to do for the kit.
 *
 * \param[in] size size of rx buffer
 * \param[out] response pointer to response buffer
 * \return status of the operation
 */
uint8_t atca_resync(uint8_t size, uint8_t *response)
{
	uint8_t ret = ATCA_SUCCESS;

	return ret;
}

/** \brief This function returns if ATAES132A is locked or not
 *
 * \return lock status
 */
uint8_t atca_aes132_is_locked(void)
{
	uint8_t ret = 0;
	uint8_t cmd[9] = {0x09, 0x10, 0x00, 0xF0, 0x20, 0x00, 0x01, 0xCB, 0x07};

	uint8_t rsp[5];

	memset(rsp, 0, sizeof(rsp));

	ret = atca_send_command(sizeof(cmd), cmd);
	if (ret != ATCA_SUCCESS)
		return ret;

	ret = atca_receive_response(sizeof(rsp), rsp);
	if (ret != ATCA_SUCCESS)
		return ret;

	if(rsp[2] == 0x00)
		ret = 1;
	else if(rsp[2] == 0x55)
		ret = 0;
	else
		ret = 0;

	return ret;
}


//! default flags for option parameter
#define AES132_OPTION_DEFAULT                   ((uint8_t) 0x00)
#define AES132_READ_MAX_SIZE					32
#define AES132_RESPONSE_IDX_DATA				2

/** \brief This function reads bytes from the device.
* \param[in] count number of bytes to read
* \param[in] address EEPROM page address to read from
* \param[out] data pointer to rx buffer
* \return status of the operation
*/
uint8_t atca_aes132_block_read_data(uint32_t page_address, size_t count, uint8_t* data)
{
	uint8_t ret = 0;
	size_t i = 0;
	size_t copy_count = 0;
	
	/**
	*\command: count(Number of bytes to be transferred), opcode, mode, param1, param1, parma2, parma2, option, check-sum
	*\PARAM2 : Upper byte is always 0x00
	*\PARAM2 : Lower byte is the number of bytes to read
	*/
	uint8_t cmd[8] = { 0x09, 0x10, 0x00,
		(uint8_t)page_address >> 8, (uint8_t)(page_address & 0xFF), 0x00, 0x20, AES132_OPTION_DEFAULT };

	uint8_t rsp[AES132_READ_MAX_SIZE + 4];
	while (i < count)
	{
		memset(rsp, 0, sizeof(rsp));

		ret = atca_send_command(sizeof(cmd), cmd);
		if (ret != 0)
			return ret;

		ret = atca_receive_response(sizeof(rsp), rsp);
		if (ret != 0)
			return ret;

		copy_count = (count - i > 32) ? 32 : count - i;
		memcpy(&data[i], &rsp[AES132_RESPONSE_IDX_DATA], copy_count);
		i += copy_count;
	}

	return ret;
}

/** \brief This function reads bytes from the device.
* \param[in] count number of bytes to read
* \param[in] address Data address to read from
* \param[out] data pointer to rx buffer
* \return status of the operation
*/
uint8_t atca_aes132_read_memory(uint32_t address, size_t count, uint8_t* data)
{
}

/** \brief This function writes bytes to the device.
* \param[in] count number of bytes to write
* \param[in] address Data address to write to
* \param[in] data pointer to tx buffer
* \return status of the operation
*/
uint8_t atca_aes132_write_memory(uint32_t address, size_t count, const uint8_t* data)
{
    uint8_t ret = 0;

    return ret;
}

/** @} */
