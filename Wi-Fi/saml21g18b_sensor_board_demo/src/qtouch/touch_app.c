/**
 * \file
 *
 * \brief touch application file
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
#include "asf.h"
#include "touch.h"
#include "touch_api_ptc.h"
#include "touch_app.h"
#include "ble_manager.h"
#include "wearable.h"
#include "rtc.h"
#include "conf_board.h"
#include "serial_drv.h"
#include "i2c.h"
#include "timer_hw.h"

/* macros */
#define TOUCH_LONG_PRESS_TIME (1000/DEF_TOUCH_MEASUREMENT_PERIOD_MS)
/* Event user  */
#define EVENT_USER   EVSYS_ID_USER_PTC_STCONV
/* Event generator  */
#define EVENT_GEN    (4+DEF_LOWPOWER_SENSOR_EVENT_PERIODICITY_OFFSET)

#define SWIPE_TIMEOUT (300/DEF_TOUCH_MEASUREMENT_PERIOD_MS) //300ms

#define LEFT_SWIPE 1
#define RIGHT_SWIPE 2

/* type definitions */
typedef enum state_tag{
	STATE_INIT,
	STATE_ACTIVE_MEASURE,
	STATE_LOWPOWER_MEASURE,
	STATE_FROM_LOWPOWER_MEASURE_TO_ACTIVE_MEASURE
}state_t;

/* variables */
struct events_resource tch_event_rsc;
state_t current_state = STATE_INIT;
uint8_t drift_wakeup=0;
uint8_t sleep_Enable=0;
uint8_t long_press_timer;
struct events_config   events_conf;
volatile uint8_t snsrs_disabled=0;
/* The event channel handle */
struct events_resource events;
uint8_t system_state = SYSTEM_STANDBY;
uint8_t swipe_in_progress = 0;
uint8_t swipe_counter;
uint8_t first_sensor_in_detect;
uint8_t last_sensor_in_detect;
bool power_up_flag = true;


/* prototypes */

/*! \brief Check left and right swipe gestures
 *
 */
void touch_check_swipe(void);

/*! \brief Disable events
 *
 */
void disable_events(void);

/*! \brief Enable events
 *
 */
void enable_events(void);

/*! \brief configure time to measure periodicity count
 *
 */
void set_time_to_measure_count(void);

/*! \brief configure time to drift periodicity count
 *
 */
void set_time_to_drift_count(void);

/*! \brief initialize event system
 *
 */
void init_config_event_system(void);

/*! \brief calculates RTC period n value from the GCLK RTC Hz and event periodicity mS
 *
 */
uint32_t  cal_rtc_event_period_n(uint32_t fglck_rtc,uint32_t  event_periodicity);

/* inline functions */

/* turn on touch LED */
static inline void turn_on_touch_led(void)
{
}

/* turn off tocuh LED */
static inline void turn_off_touch_led(void)
{
}


/* function definitions */

/*! \brief Check left and right swipe gestures
 *
 */

void touch_check_swipe(void)
{	
	/* check the detect state for buttons */
	uint8_t detect_state = p_selfcap_measure_data->p_sensor_states[0] & 0x03 ;
	
	/* if some button is detected */
	if(detect_state)
	{
		if(swipe_in_progress == 0)
		{
			/* if its first detect, note the sensor in detect */
			first_sensor_in_detect = detect_state;
			swipe_in_progress = 1;
		}
		else
		{
			/* increment swipe counter and note the last button in detect */
			swipe_counter++;
			last_sensor_in_detect = detect_state;
		}
	}
	else /* no buttons are in detect */
	{
		/* if swipe was in progress */
		if(swipe_in_progress == 1)
		{
			/* if swipe is in without timeout */
			if(swipe_counter < SWIPE_TIMEOUT) // timeout 300ms
			{
					if((first_sensor_in_detect == 0x01) && (last_sensor_in_detect == 0x02))
					{
						turn_on_touch_led();
						/* right swipe detected, send ble notification */
						if(is_ble_touch_gest_char_notification_enabled())
						{
							ble_notify_touch_gesture(RIGHT_SWIPE);
						}		
					}
					else if((first_sensor_in_detect == 0x02) && (last_sensor_in_detect == 0x01))
					{
						turn_on_touch_led();
						/* left swipe detected, send ble notification */
						if(is_ble_touch_gest_char_notification_enabled())
						{
							ble_notify_touch_gesture(LEFT_SWIPE);
						}
					}
			}
			
			/* clear variables */
			swipe_counter = 0;
			swipe_in_progress = 0;
		}
	}
}

/*! \brief  Active and low power measurement state machine
 *
 */
void touch_meas_state_machine(void)
{
	touch_ret_t touch_ret_lp = TOUCH_SUCCESS;
    uint8_t counter=0;

	switch(current_state)
     {
          case STATE_INIT:
                     /* STATE_INIT:Initialize event system  */
					init_config_event_system();
					current_state=STATE_ACTIVE_MEASURE;
                    break;

          case STATE_ACTIVE_MEASURE:

                    /* STATE_ACTIVE_MEASURE: Active touch measurement  */
					if(p_selfcap_measure_data->measurement_done_touch==0)
					{
						touch_sensors_measure();
						sleep_Enable=1;
					}

					if(p_selfcap_measure_data->measurement_done_touch==1)
                    {
                       p_selfcap_measure_data->measurement_done_touch=0;
					   
					   if(system_state == SYSTEM_STANDBY)
					   {
						   if(p_selfcap_measure_data->p_sensor_states[0] & 0x04)
						   {							   
							   long_press_timer++;
							   if(long_press_timer > TOUCH_LONG_PRESS_TIME)
							   {
									system_state = SYSTEM_ACTIVE;
									long_press_timer = 0;
									
									/* enable hw timer used by BLE */
									enable_hw_timer();
									/* enable ble uart */
									enable_usart();
									/* enable i2c peripheral */
									enable_i2c();
									
									/* Start the advertisement */
									wbe_start_advertisement();
									DBG_LOG("System Active");							   
							   }
						   }
					   }
					   else
					   {
						   /* check swipe gesture */
						   touch_check_swipe();
					   }					   	

					   if ((p_selfcap_measure_data->p_sensor_states[0] != 0) )
                       {
						    if(drift_wakeup==1)
						    {
							    set_time_to_measure_count();
								drift_wakeup=0;
								sleep_Enable=0;
						    }

							if(snsrs_disabled==1)
						    {
                                snsrs_disabled=0;
								for(counter=0;counter<DEF_SELFCAP_NUM_SENSORS;counter++)
								{

									if(counter!=DEF_LOWPOWER_SENSOR_ID)
									{
										touch_ret_lp=touch_selfcap_sensor_reenable(counter,0);//NO_RECAL_RE_EN

										if(touch_ret_lp!=TOUCH_SUCCESS)
										{
											while(1)
											{}
										}										

									}
								}
						    }
					    }
						else
						{
							turn_off_touch_led();
						}

					   if((advertisement_timer >= (BLE_ADVERTISEMENT_TIMEOUT/DEF_TOUCH_MEASUREMENT_PERIOD_MS)) || (power_up_flag == true))					   
                       {
						   if(power_up_flag == false)
						   {
							   	/* stop BLE advertising */
							   	at_ble_adv_stop();
							   	started_advertising = false;
								/* disable uart to save power */
								disable_usart();
								/* disable i2c peripheral */
								disable_i2c();
								/* disable hw timer used by BLE */
								disable_hw_timer();
																
							   	advertisement_timer = 0;
						   }
						   power_up_flag = false;
						   
						   sleep_Enable=0;

						   if(snsrs_disabled==0)
						   {
                                snsrs_disabled=1;
								for(counter=0;counter<DEF_SELFCAP_NUM_SENSORS;counter++)
								{
									if(counter!=DEF_LOWPOWER_SENSOR_ID)
									{

									   touch_ret_lp=touch_selfcap_sensor_disable(counter);

										if(touch_ret_lp!=TOUCH_SUCCESS)
										{
											while(1)
											{}
										}
									}
								}
						   }

						   current_state = STATE_LOWPOWER_MEASURE;
						   
						   system_state = SYSTEM_STANDBY;
						   
						   DBG_LOG("System standby");
					    }
                     }
                     break;


		  case STATE_LOWPOWER_MEASURE:
					 /* STATE_LOWPOWER_MEASURE: Lowpower event triggered touch measurement  */
					 wake_up_touch=0;
					 while(1)
					 {
						touch_ret_lp=touch_self_lowpower_sensor_enable_event_measure(DEF_LOWPOWER_SENSOR_ID);
						if(touch_ret_lp==TOUCH_SUCCESS)
							break;
					 }

					 if(drift_wakeup!=1)
					 {
						 set_time_to_drift_count();
					 }

						   drift_wakeup=0;
						   sleep_Enable=1;

					 enable_events();
					 current_state=STATE_FROM_LOWPOWER_MEASURE_TO_ACTIVE_MEASURE;
                           
                     break;


		  case STATE_FROM_LOWPOWER_MEASURE_TO_ACTIVE_MEASURE:
                       /* STATE_FROM_LOWPOWER_MEASURE_TO_ACTIVE_MEASURE: Switch from Lowpower to active touch measurement  */
					  disable_events();
					  
					  sleep_Enable=0;

					  if(drift_wakeup!=1)
					  {
						  set_time_to_measure_count();
					  }
					  current_state=STATE_ACTIVE_MEASURE;

					  p_selfcap_measure_data->measurement_done_touch=0;
					  touch_time.time_to_measure_touch = 1;
					  break;

          default:break;
      }

}

/*! \brief initialize event system
 *
 */
void init_config_event_system(void)
{
	enum status_code status_ret=STATUS_OK;

	/* Get default event channel configuration */
	events_get_config_defaults(&events_conf);

	events_conf.path           = EVENTS_PATH_ASYNCHRONOUS;
	events_conf.generator      = EVENT_GEN;
	events_conf.run_in_standby = true;
	events_conf.on_demand      = true;

	status_ret=events_allocate(&events, &events_conf);

	if(status_ret!=STATUS_OK)
	{
		while(1)
		{}
	}
}

/*! \brief Disable events
 *
 */
void disable_events(void)
{
	enum status_code status_ret=STATUS_OK;
	status_ret=events_detach_user(&events, EVENT_USER);

	if(status_ret!=STATUS_OK)
	{
		while(1)
		{}
	}
}

/*! \brief Enable events
 *
 */
void enable_events(void)
{
	enum status_code status_ret=STATUS_OK;
	status_ret=events_attach_user(&events, EVENT_USER);

	if(status_ret!=STATUS_OK)
	{
		while(1)
		{}
	}
}

/*! \brief configure time to measure periodicity count
 *
 */
void set_time_to_measure_count(void)
{
	enum status_code status_ret=STATUS_OK;
	uint16_t temp;

	NVIC_ClearPendingIRQ(RTC_IRQn);

	status_ret=rtc_count_clear_compare_match(&rtc_instance,RTC_COUNT_COMPARE_0);

	if(status_ret!=STATUS_OK)
	{
		while(1)
		{}
	}

	status_ret=rtc_count_set_count(&rtc_instance,((uint32_t) 0x00));

	if(status_ret!=STATUS_OK)
	{
		while(1)
		{}
	}

	temp = TIME_PERIOD_1MSEC * DEF_TOUCH_MEASUREMENT_PERIOD_MS;

	status_ret=rtc_count_set_compare(&rtc_instance,temp,RTC_COUNT_COMPARE_0);

	if(status_ret!=STATUS_OK)
	{
		while(1)
		{}
	}
}

/*! \brief configure time to drift periodicity count
 *
 */
void set_time_to_drift_count(void)
{
	enum status_code status_ret=STATUS_OK;

	NVIC_ClearPendingIRQ(RTC_IRQn);

	status_ret=rtc_count_clear_compare_match(&rtc_instance,RTC_COUNT_COMPARE_0);

	if(status_ret!=STATUS_OK)
	{
		while(1)
		{}
	}

	status_ret=rtc_count_set_count(&rtc_instance,((uint32_t) 0x00));

	if(status_ret!=STATUS_OK)
	{
		while(1)
		{}
	}

	status_ret=rtc_count_set_compare(&rtc_instance,DEF_LOWPOWER_SENSOR_DRIFT_PERIODICITY_MS,RTC_COUNT_COMPARE_0);

	if(status_ret!=STATUS_OK)
	{
		while(1)
		{}
	}
}

