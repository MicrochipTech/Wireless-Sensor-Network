/**
* \file
*
* \brief Multi-Role/Multi-Connect Application
*
* Copyright (c) 2016 Atmel Corporation. All rights reserved.
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
*    Atmel micro controller product.
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
* Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel
*Support</a>
*/

/**
* \mainpage
* \section preface Preface
* This is the reference manual for the Multi-Role/Multi-Connect Application
*/
/*- Includes ---------------------------------------------------------------*/
#include <asf.h>
#include "wearable.h"
#include "motion_sensor.h"
#include "conf_sensor.h"
#include "bhi160.h"
#include "rtc.h"


#define TEMP_DISP_COUNTER	1	// update when same value is get 1 time
#define HUM_DISP_COUNTER	1
#define UV_DISP_COUNTER 1
#define PRESSURE_DISP_COUNTER 1

int16_t gi16Disp_temperature;
uint8_t gu8Disp_humidity;
uint32_t gu32Disp_uv;
uint16_t gu16Disp_pressure;
//environment data to BLE
environment_data_t environ_data;

unsigned char gu8UpdateFlag = 1;

void (*rotation_vector_update_cb)(device_rotation_vector_t, unsigned char);



void motion_sensor_data_init()
{
	/*
	environment_data_t environment_data;
	
	get_env_sensor_data_from_chip(&environment_data);
	
	gi16Disp_temperature = environment_data.temperature;
	gu8Disp_humidity = environment_data.humidity;
	gu32Disp_uv	= environment_data.uv;
	gu16Disp_pressure = environment_data.pressure;
	*/
	return;
}

void set_motion_sensor_update_timer(unsigned char time)	// arg time is in second unit
{
	rotation_data_timer = time; // 5 second timeout to update 
}

void motion_sensor_execute()
{
	
	if (rotation_data_timer > 0)
	{
		DBG_LOG("\r\nprocess motion\r\n");
		process_motion_sensor_data();
		DBG_LOG("rotation data = %d, %d, %d, %d\r\n", quaternion_data.w, quaternion_data.x, quaternion_data.y, quaternion_data.z);
		
		rotation_vector_update_cb(quaternion_data, rotation_data_timer);
	}
}

void register_rotation_vector_udpate_callback_handler(void* cb)
{
	rotation_vector_update_cb = cb;
}