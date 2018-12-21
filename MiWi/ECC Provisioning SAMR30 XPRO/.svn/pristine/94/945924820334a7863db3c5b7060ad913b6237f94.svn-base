/**
 * \file
 *
 * \brief VEML60xx driver file
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include "veml60xx.h"
#include "conf_sensor.h"

enum status_code veml60xx_init(void)
{
	
	uint8_t buffer[3];
	buffer[0] = VEML60xx_CONFIG_CMD_CODE;
	buffer[1] = VEML60xx_INIT_VALUE_LSB;  //Config LSB value
	buffer[2] = VEML60xx_INIT_VALUE_MSB;
	
	enum status_code sensor_wr_status = STATUS_BUSY;
	
	/* Set up internal EEPROM addr write */
	struct i2c_master_packet sensor_wr_packet = {
		.address         = VEML60xx_I2C_ADDRESS2,
		.data_length     = (VEML60xx_CMD_LENGTH_BYTE +
							VEML60xx_DATA_LENGTH_BYTE),
		.data            = buffer,
		.ten_bit_address = false,
		.high_speed      = false,
		.hs_master_code  = 0x0,
	};
	
	sensor_wr_status = i2c_master_write_packet_wait(&i2c_master_instance,&sensor_wr_packet);
	

	return sensor_wr_status;
}

#if LIGHT_SENSOR_VEML6080 == true
/*---------------------------------------------*/
/*Function to read the UV data from veml6080*/
/*---------------------------------------------*/
enum status_code veml60xx_read_uvdata (uint16_t *uv_data)
{
	
		enum status_code sensor_wr_status = STATUS_BUSY;
		enum status_code sensor_rd_status = STATUS_BUSY;
		uint8_t cmd = VEML60xx_UVDATA_CMD_CODE;
		uint8_t buffer[2] = {0};
		uint16_t temp = 0; 
			
		/* Set up internal EEPROM addr write */
		struct i2c_master_packet sensor_wr_packet = {
			.address         = VEML60xx_I2C_ADDRESS2,
			.data_length     = VEML60xx_CMD_LENGTH_BYTE,
			.data            = &cmd,
			.ten_bit_address = false,
			.high_speed      = false,
			.hs_master_code  = 0x0,
		};
			
		sensor_wr_status = i2c_master_write_packet_wait_no_stop(&i2c_master_instance,
			&sensor_wr_packet);
			
		if (sensor_wr_status != STATUS_OK){
			return sensor_wr_status;
		}
			
		/* Set up internal EEPROM addr write */
		struct i2c_master_packet sensor_rd_packet = {
			.address         = VEML60xx_I2C_ADDRESS2,
			.data_length     = sizeof(buffer),
			.data            = buffer,
			.ten_bit_address = false,
			.high_speed      = false,
			.hs_master_code  = 0x0,
		};
			
		sensor_rd_status = i2c_master_read_packet_wait(&i2c_master_instance,&sensor_rd_packet);
			
		if (sensor_rd_status != STATUS_OK){
			return sensor_rd_status;
		}
		
		temp = ((buffer[1] << 8) | buffer[0]);
		
		*uv_data = temp;
			
		return STATUS_OK;
			
}

#endif
/*---------------------------------------------*/
/*Function to read the ALS data from veml60xx*/
/*---------------------------------------------*/
enum status_code veml60xx_read_alsdata (uint16_t *als_data)
{
	
	enum status_code sensor_wr_status = STATUS_BUSY;
	enum status_code sensor_rd_status = STATUS_BUSY;
	uint8_t cmd = VEML60xx_ALSDATA_CMD_CODE;
	uint8_t buffer[2] = {0};
	uint16_t temp = 0;
	
	/* Set up internal EEPROM addr write */
	struct i2c_master_packet sensor_wr_packet = {
		.address         = VEML60xx_I2C_ADDRESS2,
		.data_length     = VEML60xx_CMD_LENGTH_BYTE,
		.data            = &cmd,
		.ten_bit_address = false,
		.high_speed      = false,
		.hs_master_code  = 0x0,
	};
	
	sensor_wr_status = i2c_master_write_packet_wait_no_stop(&i2c_master_instance,
	&sensor_wr_packet);
	
	if (sensor_wr_status != STATUS_OK){
		return sensor_wr_status;
	}
	
	/* Set up internal EEPROM addr write */
	struct i2c_master_packet sensor_rd_packet = {
		.address         = VEML60xx_I2C_ADDRESS2,
		.data_length     = sizeof(buffer),
		.data            = buffer,
		.ten_bit_address = false,
		.high_speed      = false,
		.hs_master_code  = 0x0,
	};
	
	sensor_rd_status = i2c_master_read_packet_wait(&i2c_master_instance,&sensor_rd_packet);
	
	if (sensor_rd_status != STATUS_OK){
		return sensor_rd_status;
	}
	
	temp = ((buffer[1] << 8) | buffer[0]);
	
	*als_data = temp;
	
	return STATUS_OK;
	
}