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
volatile uint8_t tick_1min;

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
uint8_t sec60Count = 0;
/*! \brief RTC timer overflow callback
 *
 */
void rtc_overflow_callback(void)
{
	uint16_t power_led_period = POWER_LED_PERIOD;
	
	/* Do something on RTC overflow here */
	rtc_count_clear_compare_match(&rtc_instance,RTC_COUNT_COMPARE_0);
	//touch_time.time_to_measure_touch = 1u;

	/* 20ms tick */
	rtc_timer++;
	ms_ticks +=20;
	
	tick_bhi = 1;

	if(rtc_timer % 3000 == 0)
	{
		
		tick_60second = 1;
		sec60Count++;
		
	}
	if(sec60Count == 60)
	{
		printf("sec60count= %d\r\n",sec60Count);
		tick_1min = 1;
		sec60Count = 0;
	}
	if(rtc_timer % 500 == 0)
	{
		tick_10second = 1;
	}	

	if(rtc_timer % 1000 == 0)
	{
		tick_20second = 1;
	}

	if(rtc_timer % 1500 == 0)
	{
		tick_30second = 1;
	}
	if(rtc_timer % env_sensor_period == 0)
	{
		tick_env_sensor = 1;
	}


	if(rtc_timer % 250 == 0)
	{
		tick_5second = 1;
	}
	if(rtc_timer % 100 == 0)
	{
		tick_2second = 1;
	}
	if(rtc_timer % 50 == 0)
	{
		tick_1second = 1;
		if (rotation_data_timer)
			rotation_data_timer--;
	}
	if(rtc_timer % 25 == 0)
	{
		//if (gu8LedMode == LED_MODE_BLINK_NORMAL)
			//toggleLED();
		tick_rotation_data = 1;
		tick_500ms = 1;
	}
	if(rtc_timer % 5 == 0)
	{
		//if (gu8LedMode == LED_MODE_BLINK_FAST)
			//toggleLED();
		tick_100ms = 1;
	}
	
	
	/* power LED handling */
	power_led_timer++;
	
	if(low_battery_flag == 1)
	{
		power_led_period = POWER_LED_LOW_BATTERY_PERIOD; 	
	}
	
	
}

/*! \brief Configure the RTC timer callback
 *
 */
void configure_rtc_callbacks(void)
{
	/* register callback */
	rtc_count_register_callback(&rtc_instance,
			rtc_overflow_callback, RTC_COUNT_CALLBACK_COMPARE_0);
	/* Enable callback */
	rtc_count_enable_callback(&rtc_instance, RTC_COUNT_CALLBACK_COMPARE_0);
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

bool SecExpired(int value)
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
	else if (value == 3600)
	{
		if (tick_1min == 1)
		{
			tick_1min = 0;
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
	volatile uint16_t temp;
	
	struct rtc_count_events config_rtc_event
		 = { .generate_event_on_periodic[(7u)] = true };
	struct rtc_count_config config_rtc_count;
	rtc_count_get_config_defaults(&config_rtc_count);

	config_rtc_count.prescaler           = RTC_MODE0_CTRLA_PRESCALER_DIV1;
	config_rtc_count.mode                = RTC_COUNT_MODE_32BIT;
	config_rtc_count.clear_on_match =true;

	/* initialize rtc */
	rtc_count_init(&rtc_instance, RTC, &config_rtc_count);

	/* Enable RTC events */
	config_rtc_event.generate_event_on_periodic[(7u)] = true;
	
	rtc_count_enable_events(&rtc_instance, &config_rtc_event);

	temp = TIME_PERIOD_1MSEC * 20u;

	rtc_count_set_compare(&rtc_instance,temp,RTC_COUNT_COMPARE_0);

	/* enable rtc */
	rtc_count_enable(&rtc_instance);

}

/*! \brief Initialize RTC timer
 *
 */
void rtc_init(void)
{
	/* Configure and enable RTC */
	configure_rtc_count();

	/* Configure and enable callback */
	configure_rtc_callbacks();
}

