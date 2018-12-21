/*
 * veml6080.c
 *
 * Created: 11/17/2015 4:21:29 PM
 *  Author: gracy.angela
 */ 

#include <asf.h>
#include "veml6080.h"
#include "conf_sensor.h"



enum status_code veml6080_init(void)
{
	
	uint8_t buffer[3];
	buffer[0] = VEML6080_CONFIG_CMD_CODE;
	buffer[1] = VEML6080_INIT_VALUE;  //Config LSB value
	buffer[2] = VEML6080_INIT_VALUE;
	
	enum status_code sensor_wr_status = STATUS_BUSY;
	
	/* Set up internal EEPROM addr write */
	struct i2c_master_packet sensor_wr_packet = {
		.address         = VEML6080_I2C_ADDRESS2,
		.data_length     = (VEML6080_CMD_LENGTH_BYTE +
							VEML6080_DATA_LENGTH_BYTE),
		.data            = buffer,
		.ten_bit_address = false,
		.high_speed      = false,
		.hs_master_code  = 0x0,
	};
	
	sensor_wr_status = i2c_master_write_packet_wait(&i2c_master_instance,&sensor_wr_packet);
	

	return sensor_wr_status;
}
/*---------------------------------------------*/
/*Function to read the UV data from veml6080*/
/*---------------------------------------------*/
enum status_code veml6080_read_uvdata (uint16_t *uv_data)
{
	
		enum status_code sensor_wr_status = STATUS_BUSY;
		enum status_code sensor_rd_status = STATUS_BUSY;
		uint8_t cmd = VEML6080_UVDATA_CMD_CODE;
		uint8_t buffer[2] = {0};
		uint16_t temp = 0; 
			
		/* Set up internal EEPROM addr write */
		struct i2c_master_packet sensor_wr_packet = {
			.address         = VEML6080_I2C_ADDRESS2,
			.data_length     = VEML6080_CMD_LENGTH_BYTE,
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
			.address         = VEML6080_I2C_ADDRESS2,
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

/*---------------------------------------------*/
/*Function to read the ALS data from veml6080*/
/*---------------------------------------------*/
enum status_code veml6080_read_alsdata (uint16_t *als_data)
{
	
	enum status_code sensor_wr_status = STATUS_BUSY;
	enum status_code sensor_rd_status = STATUS_BUSY;
	uint8_t cmd = VEML6080_ALSDATA_CMD_CODE;
	uint8_t buffer[2] = {0};
	uint16_t temp = 0;
	
	/* Set up internal EEPROM addr write */
	struct i2c_master_packet sensor_wr_packet = {
		.address         = VEML6080_I2C_ADDRESS2,
		.data_length     = VEML6080_CMD_LENGTH_BYTE,
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
		.address         = VEML6080_I2C_ADDRESS2,
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