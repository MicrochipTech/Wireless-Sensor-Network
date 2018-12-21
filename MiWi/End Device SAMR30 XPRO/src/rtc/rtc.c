/**
 * \file
 *
 * \brief main file
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

/*- Includes ---------------------------------------------------------------*/
#include <asf.h>
#include "rtc.h"
//#include "touch_api_ptc.h"
//#include "touch_app.h"
//#include "wearable.h"
//#include "led.h"

/**
 * Macros
 */
#define POWER_LED_LOW_BATTERY_PERIOD (10000/RTC_PERIOD)
#define POWER_LED_PERIOD (60000/RTC_PERIOD)
#define BLE_LED_PERIOD (1000/RTC_PERIOD)

/**
 * Prototypes
 */
/*! \brief Initialize timer
 *
 */
void timer_init( void );

/*! \brief RTC timer overflow callback
 *
 */
void rtc_overflow_callback(void);

/*! \brief Configure the RTC timer callback
 *
 */
void configure_rtc_callbacks(void);

/*! \brief Configure the RTC timer count after which interrupts comes
 *
 */
void configure_rtc_count(void);

/**
 * Variables
 */
struct rtc_module rtc_instance;

volatile uint32_t ms_ticks = 0;
uint32_t rtc_timer;
volatile uint8_t tick_100ms;
volatile uint8_t tick_500ms;
volatile uint8_t tick_rotation_data;
volatile uint8_t tick_1second;
volatile uint8_t tick_2second;
volatile uint8_t tick_5second;
volatile uint8_t tick_env_sensor;
volatile uint8_t tick_motion_sensor;
volatile uint8_t tick_ble_event_task;
volatile uint8_t tick_10second;
volatile uint8_t tick_20second;
volatile uint8_t tick_30second;
volatile uint8_t tick_bhi;
volatile uint8_t tick_60second;
unsigned char rotation_data_timer;
/* Environment sensor data rate */
#define ENV_SENSOR_DATA_RATE_1Hz 1000/RTC_PERIOD //50	//1second
#define ENV_SENSOR_DATA_RATE_2Hz 500/RTC_PERIOD //25	//500ms
#define ENV_SENSOR_DATA_RATE_4Hz 250/RTC_PERIOD //12	//250ms
#define ENV_SENSOR_DATA_RATE_8Hz 125/RTC_PERIOD //6	//125ms
#define ENV_SENSOR_DATA_RATE_10Hz 100/RTC_PERIOD //5	//100ms
uint8_t env_sensor_period = ENV_SENSOR_DATA_RATE_1Hz;
uint8_t ble_led_timer;
bool low_battery_flag = false;
uint16_t power_led_timer;

/*! \brief RTC timer overflow callback
 *
 */
void rtc_overflow_callback(void)
{
	/* Do something on RTC overflow here */
	rtc_count_disable(&rtc_instance);	
	
}

/*! \brief Configure the RTC timer callback
 *
 */
void configure_rtc_callbacks(void)
{
	/*Register rtc callback*/
	rtc_count_register_callback(
	&rtc_instance, rtc_overflow_callback,
	RTC_COUNT_CALLBACK_OVERFLOW);
	rtc_count_enable_callback(&rtc_instance, RTC_COUNT_CALLBACK_OVERFLOW);
}

/*! \brief Configure the RTC timer count after which interrupts comes
 *
 */

bool Ten_SecExpired(void)
{
	if (tick_10second == 1)
	{
		tick_10second = 0;
		return true;
	}
	return false;
}

bool SecExpired(uint8_t value)
{
	if (value == 5)
	{
		if (tick_5second == 1)
		{
			tick_5second = 0;
			return true;
		}
	}
	else if (value == 10)
	{
		if (tick_10second == 1)
		{
			tick_10second = 0;
			return true;
		}
	}
	else if (value == 20)
	{
		if (tick_20second == 1)
		{
			tick_20second = 0;
			return true;
		}
		
	}
	else if (value == 30)
	{
		if (tick_30second == 1)
		{
			tick_30second = 0;
			return true;
		}
		
	}
	else if (value == 60)
	{
		if (tick_60second == 1)
		{
			tick_60second = 0;
			return true;
		}
	}
	else // for Unknown input value use 10 sec default time count
	{
		if (tick_10second == 1)
		{
			tick_10second = 0;
			return true;
		}
	}

	return false;
}
void configure_rtc_count(void)
{
	struct rtc_count_config config_rtc_count;
	rtc_count_get_config_defaults(&config_rtc_count);

	config_rtc_count.prescaler           = RTC_COUNT_PRESCALER_DIV_16;
	config_rtc_count.mode                = RTC_COUNT_MODE_16BIT;
	//config_rtc_count.clear_on_match =true;

	/* initialize rtc */
#ifdef FEATURE_RTC_CONTINUOUSLY_UPDATED
	/** Continuously update the counter value so no synchronization is
	 *  needed for reading. */
	config_rtc_count.continuously_update = true;
#endif
	rtc_count_init(&rtc_instance, RTC, &config_rtc_count);
	configure_rtc_callbacks();

}

void setSleepPeriod(uint32_t interval)
{
	interval = interval * 1000;
	rtc_count_set_period(&rtc_instance, interval);
	configure_rtc_callbacks();
	rtc_count_enable(&rtc_instance);

	/*put the MCU in standby mode with RTC as wakeup source*/
	system_set_sleepmode(SYSTEM_SLEEPMODE_BACKUP);
	system_sleep();
}

/*! \brief Initialize RTC timer
 *
 */
void rtc_init(void)
{
	configure_rtc_count();
}

