/**
 * \file
 *
 * \brief VEML60xx header file
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

#ifndef VEML60xx_H_
#define VEML60xx_H_

#include "status_codes.h"
#include "conf_sensor.h"

//When ADDR pin is tied to VDDIo
#define VEML60xx_I2C_ADDRESS1  0x48
//When ADDR pin is tied to GND
#define VEML60xx_I2C_ADDRESS2  0x10 


#define VEML60xx_CMD_LENGTH_BYTE   1
#define VEML60xx_DATA_LENGTH_BYTE  2



#if LIGHT_SENSOR_VEML6080 == true
//Command codes of VEML6080
#define VEML60xx_CONFIG_CMD_CODE	0x00
#define VEML60xx_UVDATA_CMD_CODE	0x04
#define VEML60xx_ALSDATA_CMD_CODE	0x08
#define VEML60xx_INIT_VALUE_LSB  0x00
#define VEML60xx_INIT_VALUE_MSB  0x00
#endif

#if LIGHT_SENSOR_VEML6030 == true
//Command codes of VEML6030
#define VEML60xx_CONFIG_CMD_CODE	0x00
#define VEML60xx_ALSDATA_CMD_CODE	0x04
#define VEML60xx_INIT_VALUE_LSB  0x00
#define VEML60xx_INIT_VALUE_MSB  0x00
/*Note: VEML6030 doesn't have UV measurement.*/
#endif


enum status_code veml60xx_init(void);
enum status_code veml60xx_read_uvdata (uint16_t *uv_data);
enum status_code veml60xx_read_alsdata (uint16_t *als_data);



#endif /* VEML60xx_H_ */