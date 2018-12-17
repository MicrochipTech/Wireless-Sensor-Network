/** \file
 *  \brief Functions of Hardware Dependent Part of Crypto Device Physical
 *         Layer Using I2C For Communication
 *  \author Atmel Crypto Products
 *  \date  June 24, 2013
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


#include <asf.h>
#include "i2c_phys.h"		// definitions and declarations for the hardware dependent I2C module
#include "i2c_master.h"		// definitions for I2C on the R21 dev kit
#include "atca_config.h"

// File scope globals
// I2C software module.
struct i2c_master_module i2c_master_instance;
volatile uint8_t slave_address = (ATCA_I2C_DEFAULT_ADDRESS >> 1);


/** \brief This function sets the address of the I2C peripheral.
			NOTE: Shifts bits right by 1 (addr >> 1) since the driver shifts left when sending address
 * */
void i2c_set_address(uint8_t addr)
{
	slave_address = (addr >> 1);
}

/** \brief This function initializes and enables the I2C peripheral.
 * */
void i2c_enable(uint32_t speed)
{
	/* Initialize config structure and software module. */
	//! [init_conf]
	struct i2c_master_config config_i2c_master;
	i2c_master_get_config_defaults(&config_i2c_master);
	config_i2c_master.pinmux_pad0 = ECC508A_I2C_SDA;
	config_i2c_master.pinmux_pad1 = ECC508A_I2C_SCL;
	config_i2c_master.baud_rate = speed/1000;
	//! [init_conf]

	/* Change buffer timeout to something longer. */
	//! [conf_change]
	config_i2c_master.buffer_timeout = 10000;
	//! [conf_change]

	/* Initialize and enable device with config. */
	//! [init_module]
	i2c_master_init(&i2c_master_instance, ECC508A_SERCOM, &config_i2c_master);
	//! [init_module]

	//! [enable_module]
	i2c_master_enable(&i2c_master_instance);
	//! [enable_module]
}


/** \brief This function disables the I2C peripheral. */
void i2c_disable(void)
{
	i2c_master_disable(&i2c_master_instance);
}


/** \brief This function creates a Start condition (SDA low, then SCL low).
 * \return status of the operation
 */
uint8_t i2c_send_start(void)
{
	// Do nothing, return success
	return I2C_SUCCESS;
}


/** \brief This function creates a Stop condition (SCL high, then SDA high).
 * \return status of the operation
 */
uint8_t i2c_send_stop(void)
{
	SercomI2cm *const i2c_module = &(i2c_master_instance.hw->I2CM);

	/* Send stop command unless arbitration is lost. */
	_i2c_master_wait_for_sync(&i2c_master_instance);
	i2c_module->CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);

	return I2C_SUCCESS;
}


/** \brief This function sends bytes to an I2C device.
 * \param[in] count number of bytes to send
 * \param[in] data pointer to tx buffer
 * \return status of the operation
 */
uint8_t i2c_send_bytes(uint8_t count, uint8_t *data)
{
	enum status_code statusCode = STATUS_OK;
	struct i2c_master_packet packet = {
		.address     = slave_address,
		.data_length = count,
		.data        = data,
		.ten_bit_address = false,
		.high_speed      = false,
		.hs_master_code  = 0x0,
	};

//	statusCode = i2c_master_write_packet_wait(&i2c_master_instance, &packet);
	statusCode = i2c_master_write_packet_wait_no_stop(&i2c_master_instance, &packet);
	if (statusCode != STATUS_OK) return I2C_COMM_FAIL;

	// Send the stop
	statusCode = i2c_send_stop();
	if (statusCode != STATUS_OK) return I2C_COMM_FAIL;
	
	return I2C_SUCCESS;
}


/** \brief This function receives one byte from an I2C device.
 *
 * \param[out] data pointer to received byte
 * \return status of the operation
 */
uint8_t i2c_receive_byte(uint8_t *data)
{
	return i2c_receive_bytes(1, data);
}


/** \brief This function receives bytes from an I2C device
 *         and sends a Stop.
 *
 * \param[in] count number of bytes to receive
 * \param[out] data pointer to rx buffer
 * \return status of the operation
 */
uint8_t i2c_receive_bytes(uint8_t count, uint8_t *data)
{
	enum status_code statusCode = STATUS_OK;
	struct i2c_master_packet packet = {
		.address     = slave_address,
		.data_length = count,
		.data        = data,
		.ten_bit_address = false,
		.high_speed      = false,
		.hs_master_code  = 0x0,
	};

	statusCode = i2c_master_read_packet_wait(&i2c_master_instance, &packet);
	if (statusCode != STATUS_OK)
	{
		return I2C_COMM_FAIL;
	}
	
	statusCode = i2c_send_stop();
	if (statusCode != STATUS_OK) return I2C_COMM_FAIL;
	
	return statusCode;
}

uint8_t i2c_send_wake()
{
	enum status_code statusCode = STATUS_OK;
	// Send the wake by writing to an address of 0x00
	struct i2c_master_packet packet = {
		.address     = 0x00,
		.data_length = 0,
		.data        = NULL,
		.ten_bit_address = false,
		.high_speed      = false,
		.hs_master_code  = 0x0,
	};

	// Send the 00 address as the wake pulse
	statusCode = i2c_master_write_packet_wait(&i2c_master_instance, &packet);
	
	// A NACK of the address is a successful wake
	return (statusCode == STATUS_ERR_BAD_ADDRESS) ? I2C_SUCCESS : statusCode;
}

///////////////////////////////////////////////////////////////////////////////
// Master implementation - wrapper functions
void phy_i2c_master_enable(i2c_bus_t bus, uint32_t speed)
{
	i2c_enable(I2C_SPEED_100KHZ);
}

void phy_i2c_master_disable(i2c_bus_t bus)
{
	i2c_disable();
}

uint8_t phy_i2c_master_send_start(i2c_bus_t bus)
{
	return i2c_send_start();
}

uint8_t phy_i2c_master_send_stop(i2c_bus_t bus)
{
	return i2c_send_stop();
}

uint8_t i2c_master_send_bytes(i2c_bus_t bus, uint8_t count, uint8_t *data)
{
	return i2c_send_bytes(count, data);
}

uint8_t i2c_master_receive_byte(i2c_bus_t bus, uint8_t *data)
{
	return i2c_receive_byte(data);
}

uint8_t i2c_master_receive_bytes(i2c_bus_t bus, uint8_t count, uint8_t *data)
{
	return i2c_receive_bytes(count, data);
}

void i2c_master_set_slave_address(i2c_bus_t bus, uint8_t addr)
{
	i2c_set_address(addr);
}

uint8_t i2c_master_send_wake(i2c_bus_t bus)
{
	return i2c_send_wake();
}

