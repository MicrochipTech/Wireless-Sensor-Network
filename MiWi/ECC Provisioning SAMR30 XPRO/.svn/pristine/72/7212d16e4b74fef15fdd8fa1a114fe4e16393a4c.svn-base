
/*
 * veml6080.h
 *
 * Created: 11/17/2015 4:21:40 PM
 *  Author: gracy.angela
 */ 


#ifndef VEML6080_H_
#define VEML6080_H_

#include "status_codes.h"
#include "conf_sensor.h"

//When ADDR pin is tied to VDDIo
#define VEML6080_I2C_ADDRESS1  0x48
//When ADDR pin is tied to GND
#define VEML6080_I2C_ADDRESS2  0x10 


#define VEML6080_CMD_LENGTH_BYTE   1
#define VEML6080_DATA_LENGTH_BYTE  2

#define VEML6080_INIT_VALUE  0x00


//Command codes of VEML6080
#define VEML6080_CONFIG_CMD_CODE	0x00
#define VEML6080_UVDATA_CMD_CODE	0x04
#define VEML6080_ALSDATA_CMD_CODE	0x08


enum status_code veml6080_init(void);
enum status_code veml6080_read_uvdata (uint16_t *uv_data);
enum status_code veml6080_read_alsdata (uint16_t *als_data);



#endif /* VEML6080_H_ */