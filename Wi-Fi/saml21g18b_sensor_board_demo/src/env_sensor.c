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
#include "env_sensor.h"
#include "bme280\bme280_support.h"
#include "conf_sensor.h"
#include "veml60xx\veml60xx.h"
#include "bhi160.h"
#include "bhi160\bhy_uc_driver.h"
#include "nvm_handle.h"
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

static void (*env_sensor_update_cb)(environment_data_t, unsigned char);




static void get_temp_sensor_data_from_chip(environment_data_t *env_data)
{
	s32 env_temperature;
	u32 env_pressure;
	u32 env_humidity;
	uint16_t als_data;
	
	bme280_set_power_mode(BME280_FORCED_MODE);
	
	if(ERROR == bme280_read_temperature(&env_temperature)){
		//error
		DBG_LOG("Reading BME280 has failed");
	}
	
	/* Temperature: No data processing required for Temperature data. Data with resolution 0.01(x100) directly txd*/
	env_data->temperature = (int16_t)env_temperature;
}

void env_sensor_data_init()
{
	environment_data_t environment_data;
	
	get_temp_sensor_data_from_chip(&environment_data);
	
	gi16Disp_temperature = environment_data.temperature;
	
	return;
}

void check_temperature_update()
{
	int16_t pre_temp = 0;
	int16_t new_temp = 0;
	int temp_cnt = nvm_get_skip_count();

	environment_data_t environment_data;
	get_temp_sensor_data_from_chip(&environment_data);
	new_temp = ((environment_data.temperature * 0.01)*1.8) + 32;
	pre_temp = nvm_get_temperature();	
	
	// check temperature
	if (pre_temp != new_temp ||temp_cnt >= 6)
	{
		// Temperature updated or an hour has elapsed (update battery level)
		printf("Temperature change, old %d, new %d, count %d\r\n",pre_temp,new_temp,temp_cnt);
		nvm_store_temperature(new_temp, 0);
	}
	else
	{
		//go back to sleep
		printf("No change, go back to sleep! %d\r\n",pre_temp);
		temp_cnt++;
		nvm_store_temperature(new_temp, temp_cnt);
		rtc_init();
		system_sleep();	
	}
}


void env_sensor_execute()
{
	static int16_t pre_temp = 0;
	static int temp_cnt;	
	unsigned char updateFlag = 0;

	environment_data_t environment_data;
	get_temp_sensor_data_from_chip(&environment_data);
	
	// check temperature
	if (pre_temp != environment_data.temperature)
	{
		temp_cnt = 0;
		pre_temp = environment_data.temperature;
	}
	else
		temp_cnt++;
	
	if ((temp_cnt >= TEMP_DISP_COUNTER) && gi16Disp_temperature!= pre_temp)
	{
		gi16Disp_temperature = pre_temp;
		updateFlag |= TEMP_UPDATE_BIT; 
	}
	
	env_sensor_update_cb(environment_data, updateFlag);
	
}

void get_env_sensor_data_for_display(environment_data_t *env_data)
{
	env_data->temperature = gi16Disp_temperature;
	env_data->humidity = gu8Disp_humidity;
	env_data->uv = gu32Disp_uv;
	env_data->pressure = gu16Disp_pressure;
	
}

void register_env_sensor_udpate_callback_handler(void* cb)
{
	env_sensor_update_cb = cb;
}
