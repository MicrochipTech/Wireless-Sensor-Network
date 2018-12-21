/**
 * \file
 *
 * \brief wearable device ble profile
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
/*- Includes ---------------------------------------------------------------*/
#include "ble_manager.h"
#include "wearable.h"
#include "timer_hw.h"
#include "platform.h"
#include "rtc.h"
#include "bhi160.h"

/* macros */

/* Environment sensor data rate */
#define ENV_SENSOR_DATA_RATE_1Hz 1000/RTC_PERIOD //50	//1second
#define ENV_SENSOR_DATA_RATE_2Hz 500/RTC_PERIOD //25	//500ms
#define ENV_SENSOR_DATA_RATE_4Hz 250/RTC_PERIOD //12	//250ms
#define ENV_SENSOR_DATA_RATE_8Hz 125/RTC_PERIOD //6	//125ms
#define ENV_SENSOR_DATA_RATE_10Hz 100/RTC_PERIOD //5	//100ms

/* motion sensor data rate */
#define MOTION_SENSOR_DATA_RATE_12_5Hz 80/RTC_PERIOD //4	//80ms
#define MOTION_SENSOR_DATA_RATE_25Hz 40/RTC_PERIOD //2		//40ms
#define MOTION_SENSOR_DATA_RATE_50Hz 20/RTC_PERIOD //1		//20ms
#define MOTION_SENSOR_DATA_RATE_100Hz 20/RTC_PERIOD //1		//10ms
#define MOTION_SENSOR_DATA_RATE_200Hz 20/RTC_PERIOD //1		//5ms

#define DEFAULT_BLE_TASK_PERIOD 100/RTC_PERIOD //5	//100ms

#define NOTIFICATION_ENABLE 1
#define NOTIFICATION_DISABLE 0

/* global variables */

uint8_t env_sensor_periods[5] = {
	ENV_SENSOR_DATA_RATE_1Hz,
	ENV_SENSOR_DATA_RATE_2Hz,
	ENV_SENSOR_DATA_RATE_4Hz,
	ENV_SENSOR_DATA_RATE_8Hz,
	ENV_SENSOR_DATA_RATE_10Hz};

uint8_t env_sensor_period = ENV_SENSOR_DATA_RATE_1Hz;

uint8_t motion_sensor_periods[5] = {
	MOTION_SENSOR_DATA_RATE_12_5Hz,
	MOTION_SENSOR_DATA_RATE_25Hz,
	MOTION_SENSOR_DATA_RATE_50Hz,
	MOTION_SENSOR_DATA_RATE_100Hz,
	MOTION_SENSOR_DATA_RATE_200Hz};
	

	
uint8_t motion_sensor_period = MOTION_SENSOR_DATA_RATE_12_5Hz;
uint8_t ble_event_task_period = DEFAULT_BLE_TASK_PERIOD;
bool started_advertising;
uint16_t advertisement_timer;

/* service handlers */
gatt_environment_service_handler_t environment_service_handle;
gatt_device_orientation_service_handler_t device_orientation_service_handle;
gatt_battery_service_handler_t battery_service_handle;
gatt_touch_gesture_service_handler_t touch_getsure_service_handle;

/* initial values for characteristics values */
uint8_t env_sensors_odr_initial_value = 1;
environment_data_t env_initial_value = {0,0,0,0};
gyro_positions_t gyro_pos_initial_value = {0,0,0};
accelero_positions_t accelero_pos_initial_value = {0,0,0};
device_rotation_vector_t device_rotation_vector_initial_value = {0,0,0,0};
uint8_t motion_odr_initial_value = 1;
uint8_t accelero_drop_down_ind_initial_value = 0;
uint8_t accelero_step_inc_ind_initial_value = 0;
uint8_t battery_level_initial_value = 0;
uint8_t touch_gesture_initial_value = 0;

/** @brief Scan response data*/
uint8_t scan_rsp_data[SCAN_RESP_LEN] = {0x09, 0xff, 0x00, 0x06, 0xd6, 0xb2, 0xf0, 0x05, 0xf0, 0xf8};

bool volatile timer_cb_done = false;
volatile bool g_sent_notification = 1;
volatile bool connection_established_flag;
volatile bool env_char_notification_flag;
volatile bool gyro_char_notification_flag;
volatile bool accelero_char_notification_flag;
volatile bool rotation_vector_char_notification_flag;
volatile bool drop_char_notification_flag;
volatile bool step_inc_char_notification_flag;
volatile bool low_bat_char_notification_flag;
volatile bool touch_gest_char_notification_flag;
volatile bool env_sensor_enabled;
volatile bool motion_sensor_enabled;

/* external variables */

/** @brief information of the connected devices */
extern at_ble_connected_t ble_dev_info[BLE_MAX_DEVICE_CONNECTED];

/* statis prototypes */
static void wbe_service_init(void);
static void wbe_service_define(void);
static at_ble_status_t wbe_advertisement_data_set(void);
static at_ble_status_t ble_connected_app_event(void *param);
static at_ble_status_t ble_disconnected_app_event(void *param);
static at_ble_status_t ble_notification_confirmed_app_event(void *param);
static at_ble_status_t ble_char_changed_app_event(void *param);
static void wait_until_ble_ready(void);
static void init_environment_service(gatt_environment_service_handler_t  *environment_serv);
static void init_device_orientation_service(gatt_device_orientation_service_handler_t *device_orientation_serv);
static void init_battery_service(gatt_battery_service_handler_t *battery_serv);
static void init_touch_getsure_service(gatt_touch_gesture_service_handler_t *touch_gest_serv);
static at_ble_status_t wbe_environment_service_define(gatt_environment_service_handler_t *environment_service);
static at_ble_status_t wbe_device_orientation_service_define(gatt_device_orientation_service_handler_t *device_orientation_service);
static at_ble_status_t wbe_battery_service_define(gatt_battery_service_handler_t *battery_service);
static at_ble_status_t wbe_touch_gesture_service_define(gatt_touch_gesture_service_handler_t *touch_gesture_service);

/****************************************************************************************
*							        Implementations	                                    *
****************************************************************************************/

static const ble_event_callback_t startup_template_app_gap_cb[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	ble_connected_app_event,
	ble_disconnected_app_event,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

static const ble_event_callback_t startup_template_app_gatt_server_cb[] = {
	ble_notification_confirmed_app_event,
	NULL,
	ble_char_changed_app_event,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

/* timer callback function */
void timer_callback_fn(void)
{
	/* Add timer callback functionality here */
}

/* @brief timer call back for rssi update
* enable the flags to execute the application taskc
*
*/
void timer_callback_handler(void)
{
	timer_cb_done = true;
}

/**@brief connected event (AT_BLE_CONNECTED) call back
*/
static at_ble_status_t ble_connected_app_event(void *param)
{
	/* set connection indication flag */
	connection_established_flag = true;
	started_advertising = false;
	advertisement_timer = 0;
	
	return AT_BLE_SUCCESS;
}

/* Callback registered for AT_BLE_DISCONNECTED event from stack */
static at_ble_status_t ble_disconnected_app_event(void *param)
{
	/* re-initialize connection and notification flags */
	g_sent_notification = 1;
	connection_established_flag = false;
	env_char_notification_flag = false;
	gyro_char_notification_flag = false;
	accelero_char_notification_flag = false;
	rotation_vector_char_notification_flag = false;
	drop_char_notification_flag = false;
	step_inc_char_notification_flag = false;
	low_bat_char_notification_flag = false;
	touch_gest_char_notification_flag = false;
	
	ble_event_task_period = DEFAULT_BLE_TASK_PERIOD;
	
	/* start advertisement */
	wbe_start_advertisement();
	
	ALL_UNUSED(param);
	return AT_BLE_SUCCESS;
}

/* Callback registered for AT_BLE_NOTIFICATION_CONFIRMED event from stack */
static at_ble_status_t ble_notification_confirmed_app_event(void *param)
{
	at_ble_cmd_complete_event_t *notification_status = (at_ble_cmd_complete_event_t *)param;
	if(!notification_status->status)
	{
		/* set notification sent flag (indicating previous notification is sent) */
		g_sent_notification = 1;
		DBG_LOG_DEV("Notification sent successfully");
		return AT_BLE_SUCCESS;
	}
	return AT_BLE_FAILURE;
}

/* Callback registered for AT_BLE_CHARACTERISTIC_CHANGED event from stack */
static at_ble_status_t ble_char_changed_app_event(void *param)
{
	ALL_UNUSED(param);
	return AT_BLE_SUCCESS;
}

/**@brief wait until previous notification is sent
*/
static void wait_until_ble_ready(void)
{
	/* wait until previous notification is sent */
	while(g_sent_notification == 0)
	{
		ble_event_task();
	}
	/* clear notification flag */
	g_sent_notification = 0;
}

/** @brief Update Environment characteristics and send notification to client 
  * 
  */
at_ble_status_t ble_notify_environment_data(environment_data_t* environ_data)
{	
	/* wait until previous notification sent */
	wait_until_ble_ready();	
	
	/* Updating the attribute data base */
	if ((at_ble_characteristic_value_set(environment_service_handle.serv_chars[0].char_val_handle, (uint8_t*)environ_data, sizeof(environment_data_t))) == AT_BLE_FAILURE)
	{
		DBG_LOG("updating the environment characteristic failed");
		return AT_BLE_FAILURE;
		} else {
		DBG_LOG_DEV("updating the environment characteristic value is successful");
	}

	/* sending notification to the peer about change in the environment data */
	if((at_ble_notification_send(ble_dev_info[0].handle, environment_service_handle.serv_chars[0].char_val_handle)) == AT_BLE_FAILURE) 
	{
		DBG_LOG("sending environment update notification to the peer failed");
		return AT_BLE_FAILURE;
	}
	else 
	{
		DBG_LOG("sending environment notification");
		DBG_LOG("Temperature: %d", environ_data->temperature);
		DBG_LOG("Humidity: %d", environ_data->humidity);
		DBG_LOG("Pressure: %d", environ_data->pressure);
		DBG_LOG("UV: %u", environ_data->uv);
				
		return AT_BLE_SUCCESS;
	}		
}

/** @brief Update Gyro positions characteristics and send notification to client 
  * 
  */
at_ble_status_t ble_notify_gyro_positions(gyro_positions_t* gyro_positions)
{
	/* wait until previous notification sent */
	wait_until_ble_ready();
		
	/* Updating the attribute data base */
	if ((at_ble_characteristic_value_set(device_orientation_service_handle.serv_chars[0].char_val_handle, (uint8_t*)gyro_positions, sizeof(gyro_positions_t))) == AT_BLE_FAILURE)
	{
		DBG_LOG("updating the Gyro positions characteristic failed");
		return AT_BLE_FAILURE;
		} else {
		DBG_LOG_DEV("updating the Gyro positions characteristic value is successful");
	}

	/* sending notification to the peer about change in the Gyro positions */
	if((at_ble_notification_send(ble_dev_info[0].handle, device_orientation_service_handle.serv_chars[0].char_val_handle)) == AT_BLE_FAILURE)
	{
		DBG_LOG("sending Gyro positions update notification to the peer failed");
		return AT_BLE_FAILURE;
	}
	else
	{
		DBG_LOG("sending Gyro positions notificationl");
		DBG_LOG("Gyro-X: %d", gyro_positions->x_pos);
		DBG_LOG("Gyro-Y: %d", gyro_positions->y_pos);
		DBG_LOG("Gyro-Z: %d", gyro_positions->z_pos);
		
		return AT_BLE_SUCCESS;
	}
}


/** @brief Update Accelero position characteristics and send notification to client 
  * 
  */
at_ble_status_t ble_notify_accelero_positions(accelero_positions_t* accelero_pos)
{
	/* wait until previous notification sent */
	wait_until_ble_ready();
		
	/* Updating the attribute data base */
	if ((at_ble_characteristic_value_set(device_orientation_service_handle.serv_chars[1].char_val_handle, (uint8_t*)accelero_pos, sizeof(accelero_positions_t))) == AT_BLE_FAILURE)
	{
		DBG_LOG("updating the Accelero positions characteristic failed");
		return AT_BLE_FAILURE;
		} else {
		DBG_LOG_DEV("updating the Accelero positions characteristic value is successful");
	}

	/* sending notification to the peer about change in the Accelero positions */
	if((at_ble_notification_send(ble_dev_info[0].handle, device_orientation_service_handle.serv_chars[1].char_val_handle)) == AT_BLE_FAILURE)
	{
		DBG_LOG("sending Accelero positions update notification to the peer failed");
		return AT_BLE_FAILURE;
	}
	else
	{
		DBG_LOG("sending Accelero position notification");
		
		DBG_LOG("Accelero X Pos: %d", accelero_pos->x_pos);
		DBG_LOG("Accelero Y Pos: %d", accelero_pos->y_pos);
		DBG_LOG("Accelero Z Pos: %d", accelero_pos->z_pos);

		return AT_BLE_SUCCESS;
	}
}

/** @brief Update Device rotation vector characteristics and send notification to client 
  * 
  */
at_ble_status_t ble_notify_device_rotation_vector(device_rotation_vector_t* rotation_vector)
{
	/* wait until previous notification sent */
	wait_until_ble_ready();
		
	/* Updating the attribute data base */
	if ((at_ble_characteristic_value_set(device_orientation_service_handle.serv_chars[2].char_val_handle, (uint8_t*)rotation_vector, sizeof(device_rotation_vector_t))) == AT_BLE_FAILURE)
	{
		DBG_LOG("updating the device rotation vector characteristic failed");
		return AT_BLE_FAILURE;
		} else {
		DBG_LOG_DEV("updating the device rotation vector characteristic value is successful");
	}

	/* sending notification to the peer about change in the Accelero X-pos */
	if((at_ble_notification_send(ble_dev_info[0].handle, device_orientation_service_handle.serv_chars[2].char_val_handle)) == AT_BLE_FAILURE)
	{
		DBG_LOG("sending device rotation vector update notification to the peer failed");
		return AT_BLE_FAILURE;
	}
	else
	{
		DBG_LOG("sending device rotation vector notification");
		
		DBG_LOG("rot_vector_X: %d", rotation_vector->x);
		DBG_LOG("rot_vector_Y: %d", rotation_vector->y);
		DBG_LOG("rot_vector_Z: %d", rotation_vector->z);
		DBG_LOG("rot_vector_W: %d", rotation_vector->w);

		return AT_BLE_SUCCESS;
	}
}

/** @brief Update accelerometer step detection notification to client 
  * 
  */
at_ble_status_t ble_notify_accelero_step_detection(void)
{
	/* wait until previous notification sent */
	wait_until_ble_ready();
		
	/* sending notification to the peer about accelerometer step detection */
	if((at_ble_notification_send(ble_dev_info[0].handle, device_orientation_service_handle.serv_chars[5].char_val_handle)) == AT_BLE_FAILURE)
	{
		DBG_LOG("sending Accelero step detection notification to the peer failed");
		return AT_BLE_FAILURE;
	}
	else
	{
		DBG_LOG("sending Accelero step detection");
		return AT_BLE_SUCCESS;
	}
}

/** @brief Update device drop detection notification to client 
  * 
  */
at_ble_status_t ble_notify_device_drop_detection(void)
{
	/* wait until previous notification sent */
	wait_until_ble_ready();
		
	/* sending notification to the peer about accelerometer drop detection */
	if((at_ble_notification_send(ble_dev_info[0].handle, device_orientation_service_handle.serv_chars[4].char_val_handle)) == AT_BLE_FAILURE)
	{
		DBG_LOG("sending device drop detection notification to the peer failed");
		return AT_BLE_FAILURE;
	}
	else
	{
		DBG_LOG("sending device drop detection");
		return AT_BLE_SUCCESS;
	}
}

/** @brief Update low battery notification to client 
  * 
  */
at_ble_status_t ble_notify_low_battery(void)
{
	/* wait until previous notification sent */
	wait_until_ble_ready();
		
	/* sending notification to the peer about low battery */
	if((at_ble_notification_send(ble_dev_info[0].handle, battery_service_handle.serv_chars.char_val_handle)) == AT_BLE_FAILURE)
	{
		DBG_LOG("sending Low Battery notification to the peer failed");
		return AT_BLE_FAILURE;
	}
	else
	{
		DBG_LOG("sending Low Battery notification");
		return AT_BLE_SUCCESS;
	}
}

/** @brief Update  Touch gesture characteristics and send notification to client 
  * 
  */
at_ble_status_t ble_notify_touch_gesture(uint8_t gesture)
{
	/* wait until previous notification sent */
	wait_until_ble_ready();
		
	/* Updating the attribute data base */
	if ((at_ble_characteristic_value_set(touch_getsure_service_handle.serv_chars.char_val_handle, &gesture, sizeof(gesture))) == AT_BLE_FAILURE)
	{
		DBG_LOG("updating the Touch Gesture characteristic failed");
		return AT_BLE_FAILURE;
		} else {
		DBG_LOG_DEV("updating the Touch Gesture  characteristic value is successful");
	}

	/* sending notification to the peer about change in the Touch Gesture */
	if((at_ble_notification_send(ble_dev_info[0].handle, touch_getsure_service_handle.serv_chars.char_val_handle)) == AT_BLE_FAILURE)
	{
		DBG_LOG("sending Touch Gesture update notification to the peer failed");
		return AT_BLE_FAILURE;
	}
	else
	{
		DBG_LOG("sending Touch Gesture notification");
		DBG_LOG("Touch Gesture: %d", gesture);
		return AT_BLE_SUCCESS;
	}
}


/** @brief wbe_char_changed_handler called by the ble manager after a
 * change in the characteristic
 *  @param[in] at_ble_characteristic_changed_t which contains handle of
 *characteristic and new value
 */
at_ble_status_t wbe_char_changed_handler(void *params)
{	
	at_ble_characteristic_changed_t *change_params = (at_ble_characteristic_changed_t*)params;
		
	/* environment char notification enable\disable */
	if(change_params->char_handle == environment_service_handle.serv_chars[0].client_config_handle)
	{
		if(change_params->char_new_value[0] == NOTIFICATION_ENABLE)
		{
			env_char_notification_flag = true;
			DBG_LOG("env noti enabled");
		}
		else if(change_params->char_new_value[0] == NOTIFICATION_DISABLE)
		{
			env_char_notification_flag = false;
			DBG_LOG("env noti disabled");
		}
	}
	/* gyro char notification enable\disable */
	else if(change_params->char_handle == device_orientation_service_handle.serv_chars[0].client_config_handle)
	{
		if(change_params->char_new_value[0] == NOTIFICATION_ENABLE)
		{
			enable_gyroscope();
			gyro_char_notification_flag = true;
			DBG_LOG("gyro noti enabled");
		}
		else if(change_params->char_new_value[0] == NOTIFICATION_DISABLE)
		{
			disable_gyroscope();
			gyro_char_notification_flag = false;
			DBG_LOG("gyro noti disabled");
		}
	}
	/* accelerometer char notification enable\disable */
	else if(change_params->char_handle == device_orientation_service_handle.serv_chars[1].client_config_handle)
	{
		if(change_params->char_new_value[0] == NOTIFICATION_ENABLE)
		{
			enable_accelerometer();
			accelero_char_notification_flag = true;
			DBG_LOG("acc noti enabled");
		}
		else if(change_params->char_new_value[0] == NOTIFICATION_DISABLE)
		{
			disable_accelerometer();
			accelero_char_notification_flag = false;
			DBG_LOG("acc noti disabled");
		}
	}
	/* rotation vector char notification enable\disable */
	else if(change_params->char_handle == device_orientation_service_handle.serv_chars[2].client_config_handle)
	{
		if(change_params->char_new_value[0] == NOTIFICATION_ENABLE)
		{
			enable_rotation_vector();
			rotation_vector_char_notification_flag = true;
			DBG_LOG("rot noti enabled");
		}
		else if(change_params->char_new_value[0] == NOTIFICATION_DISABLE)
		{
			disable_rotation_vector();
			rotation_vector_char_notification_flag = false;
			DBG_LOG("rot noti disabled");
		}
	}
	/* device drop char notification enable\disable */
	else if(change_params->char_handle == device_orientation_service_handle.serv_chars[4].client_config_handle)
	{
		if(change_params->char_new_value[0] == NOTIFICATION_ENABLE)
		{
			enable_drop_detector();
			drop_char_notification_flag = true;
			DBG_LOG("drop noti enabled");
		}
		else if(change_params->char_new_value[0] == NOTIFICATION_DISABLE)
		{
			disable_drop_detector();
			drop_char_notification_flag = false;
			DBG_LOG("drop noti disabled");
		}
	}
	/* step-inc char notification enable\disable */
	else if(change_params->char_handle == device_orientation_service_handle.serv_chars[5].client_config_handle)
	{
		if(change_params->char_new_value[0] == NOTIFICATION_ENABLE)
		{
			enable_step_detector();
			step_inc_char_notification_flag = true;
			DBG_LOG("step noti enabled");
		}
		else if(change_params->char_new_value[0] == NOTIFICATION_DISABLE)
		{
			disable_step_detector();
			step_inc_char_notification_flag = false;
			DBG_LOG("step noti disabled");
		}
	}
	/* battery char notification enable\disable */
	else if(change_params->char_handle == battery_service_handle.serv_chars.client_config_handle)
	{
		if(change_params->char_new_value[0] == NOTIFICATION_ENABLE)
		{
			low_bat_char_notification_flag = true;
			DBG_LOG("bat noti enabled");
		}
		else if(change_params->char_new_value[0] == NOTIFICATION_DISABLE)
		{
			low_bat_char_notification_flag = false;
			DBG_LOG("bat noti disabled");
		}
	}
	/* touch gesture char notification enable\disable */
	else if(change_params->char_handle == touch_getsure_service_handle.serv_chars.client_config_handle)
	{
		if(change_params->char_new_value[0] == NOTIFICATION_ENABLE)
		{
			touch_gest_char_notification_flag = true;
			DBG_LOG("touch noti enabled");
		}
		else if(change_params->char_new_value[0] == NOTIFICATION_DISABLE)
		{
			touch_gest_char_notification_flag = false;
			DBG_LOG("touch noti disabled");
		}
	}
	/* Environment sensor period(DOR) setting*/
	else if(change_params->char_handle == environment_service_handle.serv_chars[1].char_val_handle)
	{
		DBG_LOG("env sensor period %d",change_params->char_new_value[0]);
		env_sensor_period = env_sensor_periods[change_params->char_new_value[0] - 1];
	}
	/* Motion sensor period(DOR) setting*/
	else if(change_params->char_handle == device_orientation_service_handle.serv_chars[3].char_val_handle)
	{
		DBG_LOG("motion sensor period %d",change_params->char_new_value[0]);
		motion_sensor_period = motion_sensor_periods[change_params->char_new_value[0] - 1];		
	}
	
	/* check motion sensor enabled */
	if((gyro_char_notification_flag == false)
		&& (accelero_char_notification_flag == false)
		&& (rotation_vector_char_notification_flag == false)
		&& (drop_char_notification_flag == false)
		&& (step_inc_char_notification_flag == false))
		{
			motion_sensor_enabled = false;
		}
		else
		{
			motion_sensor_enabled = true;
		}
	
	/* set ble event task period */	
	if(motion_sensor_enabled == true)
	{
		ble_event_task_period = motion_sensor_period;
	}
	else if(env_char_notification_flag == true)
	{
		ble_event_task_period = env_sensor_period;
	}
	else
	{
		ble_event_task_period = DEFAULT_BLE_TASK_PERIOD;
	}
	
	DBG_LOG("ble_event_task_period %d",ble_event_task_period);	

	return AT_BLE_SUCCESS;
}


/** @brief start advertisement data over air
  * 
  */
at_ble_status_t wbe_start_advertisement(void)
{
	/* Start of advertisement */
	if(at_ble_adv_start(AT_BLE_ADV_TYPE_UNDIRECTED, AT_BLE_ADV_GEN_DISCOVERABLE, \
	NULL, AT_BLE_ADV_FP_ANY, APP_FAST_ADV, APP_ADV_TIMEOUT, 0) == \
	AT_BLE_SUCCESS)
	{
		DBG_LOG("BLE Started Advertisement");
		started_advertising = true;
		advertisement_timer = 0;
		return AT_BLE_SUCCESS;
	}
	else
	{
		DBG_LOG("BLE Advertisement start Failed");
	}
	return AT_BLE_FAILURE;
}

/**
* \Wearable demo service advertisement initialization
*/
static at_ble_status_t wbe_advertisement_data_set(void)
{
	uint8_t idx = 0;
	
	uint8_t adv_data [ WD_ADV_DATA_NAME_LEN + ADV_DATA_UUID_LEN   + (2*2)];
	
	adv_data[idx++] = ADV_DATA_UUID_LEN + ADV_TYPE_LEN;
	adv_data[idx++] = ADV_DATA_UUID_TYPE;
	
	/* Appending the UUID */
	memcpy(&adv_data[idx], ENVIRONMENT_SERVICE_UUID, ADV_DATA_UUID_LEN);
	idx += ADV_DATA_UUID_LEN;
	
	//Appending the complete name to the Ad packet
	adv_data[idx++] = WD_ADV_DATA_NAME_LEN + ADV_TYPE_LEN;
	adv_data[idx++] = WD_ADV_DATA_NAME_TYPE;
	
	memcpy(&adv_data[idx], WD_ADV_DATA_NAME_DATA, WD_ADV_DATA_NAME_LEN );
	idx += WD_ADV_DATA_NAME_LEN ;
	
	/* Adding the advertisement data and scan response data */
	if(!(at_ble_adv_data_set(adv_data, idx, scan_rsp_data, SCAN_RESP_LEN) == AT_BLE_SUCCESS) )
	{
		#ifdef DBG_LOG
		DBG_LOG("Failed to set adv data");
		#endif
	}
	
	return AT_BLE_SUCCESS;
}

/**@brief Register a Environment service instance inside stack.
*/
static at_ble_status_t wbe_environment_service_define(gatt_environment_service_handler_t *environment_service)
{
	return(at_ble_primary_service_define(&environment_service->serv_uuid,
	&environment_service->serv_handle,
	NULL, 0,
	environment_service->serv_chars, ENVIRONMENT_CHARACTERISTIC_COUNT));
}

/**@brief Register a device orientation service instance inside stack.
*/
static at_ble_status_t wbe_device_orientation_service_define(gatt_device_orientation_service_handler_t *device_orientation_service)
{
	return(at_ble_primary_service_define(&device_orientation_service->serv_uuid,
	&device_orientation_service->serv_handle,
	NULL, 0,
	device_orientation_service->serv_chars, DEVICE_ORIENTATION_CHARACTERISTIC_COUNT));
}

/**@brief Register a Battery service instance inside stack.
*/
static at_ble_status_t wbe_battery_service_define(gatt_battery_service_handler_t *battery_service)
{
	return(at_ble_primary_service_define(&battery_service->serv_uuid,
	&battery_service->serv_handle,
	NULL, 0,
	&battery_service->serv_chars, BATTERY_LEVEL_CHARACTERISTIC_COUNT));
}

/**@brief Register a Touch gesture service instance inside stack.
*/
static at_ble_status_t wbe_touch_gesture_service_define(gatt_touch_gesture_service_handler_t *touch_gesture_service)
{
	return(at_ble_primary_service_define(&touch_gesture_service->serv_uuid,
	&touch_gesture_service->serv_handle,
	NULL, 0,
	&touch_gesture_service->serv_chars, TOUCH_GESTURE_CHARACTERISTIC_COUNT));
}

/** @brief defines the services of the profile
 *
 */
static void wbe_service_define(void)
{
	/* Environment service */		
	if(wbe_environment_service_define(&environment_service_handle) == AT_BLE_SUCCESS)
	{
		DBG_LOG_DEV("env handle %d",environment_service_handle.serv_handle);
	}
	else
	{
		DBG_LOG("environment service define Failed");
	}
	
	/* Gyroscope service */	
	if(wbe_device_orientation_service_define(&device_orientation_service_handle) == AT_BLE_SUCCESS)
	{
		DBG_LOG_DEV("Device orientation handle %d",device_orientation_service_handle.serv_handle);
	}
	else
	{
		DBG_LOG("Device orientation service define Failed");
	}	
	
	/* Batter service */	
	if(wbe_battery_service_define(&battery_service_handle) == AT_BLE_SUCCESS)
	{
		DBG_LOG_DEV("battery handle %d",battery_service_handle.serv_handle);
	}
	else
	{
		DBG_LOG("Battery Service sefine Failed");
	}
		
	/* Touch Gesture service */	
	if(wbe_touch_gesture_service_define(&touch_getsure_service_handle) == AT_BLE_SUCCESS)
	{
		DBG_LOG_DEV("touch gesture handle %d",touch_getsure_service_handle.serv_handle);
	}
	else
	{
		DBG_LOG("Touch gesture service Define Failed");
	}
}

/**@brief Environment service and characteristic initialization
*
*/
static void init_environment_service(gatt_environment_service_handler_t  *environment_serv)
{
	environment_serv->serv_handle = 0;
	
	environment_serv->serv_uuid.type = AT_BLE_UUID_128;
	memcpy(&environment_serv->serv_uuid.uuid[0], ENVIRONMENT_SERVICE_UUID, 16);
	
	/* Characteristics - Temperature, Pressure, Humidity, UV */
	
	/* handle stored here */
	environment_serv->serv_chars[0].char_val_handle = 0;
	
	/* UUID : Temperature */
	environment_serv->serv_chars[0].uuid.type = AT_BLE_UUID_128;
	memcpy(&environment_serv->serv_chars[0].uuid.uuid[0], ENVIRONMENT_CHAR_UUID, 16);

	/* Properties */
	environment_serv->serv_chars[0].properties = AT_BLE_CHAR_READ | AT_BLE_CHAR_NOTIFY;
	/* value */
	environment_serv->serv_chars[0].init_value = (uint8_t*)&env_initial_value ;
	environment_serv->serv_chars[0].value_init_len = sizeof(environment_data_t);
	environment_serv->serv_chars[0].value_max_len = sizeof(environment_data_t);
	/* permissions */
	environment_serv->serv_chars[0].value_permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR);
	/* user defined name */
	environment_serv->serv_chars[0].user_desc = NULL;
	environment_serv->serv_chars[0].user_desc_len = 0;
	environment_serv->serv_chars[0].user_desc_max_len = 0;
	/*user description permissions*/
	environment_serv->serv_chars[0].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*client config permissions*/
	environment_serv->serv_chars[0].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*server config permissions*/
	environment_serv->serv_chars[0].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*user desc handles*/
	environment_serv->serv_chars[0].user_desc_handle = 0;
	/*client config handles*/
	environment_serv->serv_chars[0].client_config_handle = 0;
	/*server config handles*/
	environment_serv->serv_chars[0].server_config_handle = 0;
	/* presentation format */
	environment_serv->serv_chars[0].presentation_format = NULL;
	
	/* Characteristics - Environment sensor ODR */
	/* handle stored here */
	environment_serv->serv_chars[1].char_val_handle = 0;
	
	/* UUID : Temperature */
	environment_serv->serv_chars[1].uuid.type = AT_BLE_UUID_128;
	memcpy(&environment_serv->serv_chars[1].uuid.uuid[0], ENV_ODR_CHAR_UUID, 16);

	/* Properties */
	environment_serv->serv_chars[1].properties = AT_BLE_CHAR_READ | AT_BLE_CHAR_WRITE;
	/* value */
	environment_serv->serv_chars[1].init_value = (uint8_t*)&env_sensors_odr_initial_value ;
	environment_serv->serv_chars[1].value_init_len = sizeof(uint8_t);
	environment_serv->serv_chars[1].value_max_len = sizeof(uint8_t);
	/* permissions */
	environment_serv->serv_chars[1].value_permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR);
	/* user defined name */
	environment_serv->serv_chars[1].user_desc = NULL;
	environment_serv->serv_chars[1].user_desc_len = 0;
	environment_serv->serv_chars[1].user_desc_max_len = 0;
	/*user description permissions*/
	environment_serv->serv_chars[1].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*client config permissions*/
	environment_serv->serv_chars[1].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*server config permissions*/
	environment_serv->serv_chars[1].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*user desc handles*/
	environment_serv->serv_chars[1].user_desc_handle = 0;
	/*client config handles*/
	environment_serv->serv_chars[1].client_config_handle = 0;
	/*server config handles*/
	environment_serv->serv_chars[1].server_config_handle = 0;
	/* presentation format */
	environment_serv->serv_chars[1].presentation_format = NULL;
}

/**@brief Device-orientation service and characteristic initialization
*
*/
static void init_device_orientation_service(gatt_device_orientation_service_handler_t *device_orientation_serv)
{
	device_orientation_serv->serv_handle = 0; //orientation

	device_orientation_serv->serv_uuid.type = AT_BLE_UUID_128;
	memcpy(&device_orientation_serv->serv_uuid.uuid[0], DEVICE_ORIENTATION_SERVICE_UUID, 16);
	
	/*Characteristic Info for gyroscope meter coordinate positions (X,Y,Z) Value*/
	
	/* handle stored here */
	device_orientation_serv->serv_chars[0].char_val_handle = 0;
	
	/* UUID : Gyro X-Pos */
	device_orientation_serv->serv_chars[0].uuid.type = AT_BLE_UUID_128;
	memcpy(&device_orientation_serv->serv_chars[0].uuid.uuid[0], GYRO_POSITIONS_CHAR_UUID, 16);

	/* Properties */
	device_orientation_serv->serv_chars[0].properties = AT_BLE_CHAR_READ | AT_BLE_CHAR_NOTIFY;
	/* value */
	device_orientation_serv->serv_chars[0].init_value = (uint8_t*)&gyro_pos_initial_value;
	device_orientation_serv->serv_chars[0].value_init_len = sizeof(gyro_positions_t);
	device_orientation_serv->serv_chars[0].value_max_len = sizeof(gyro_positions_t);
	/* permissions */
	device_orientation_serv->serv_chars[0].value_permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR);
	/* user defined name */
	device_orientation_serv->serv_chars[0].user_desc = NULL;
	device_orientation_serv->serv_chars[0].user_desc_len = 0;
	device_orientation_serv->serv_chars[0].user_desc_max_len = 0;
	/*user description permissions*/
	device_orientation_serv->serv_chars[0].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*client config permissions*/
	device_orientation_serv->serv_chars[0].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*server config permissions*/
	device_orientation_serv->serv_chars[0].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*user desc handles*/
	device_orientation_serv->serv_chars[0].user_desc_handle = 0;
	/*client config handles*/
	device_orientation_serv->serv_chars[0].client_config_handle = 0;
	/*server config handles*/
	device_orientation_serv->serv_chars[0].server_config_handle = 0;
	/* presentation format */
	device_orientation_serv->serv_chars[0].presentation_format = NULL;
	
	/*Characteristic Info for accelerometer coordinate positions (X,Y,Z) Value*/
	
	/* handle stored here */
	device_orientation_serv->serv_chars[1].char_val_handle = 0;
	
	/* UUID : Accelero Y-Pos */
	device_orientation_serv->serv_chars[1].uuid.type = AT_BLE_UUID_128;
	memcpy(&device_orientation_serv->serv_chars[1].uuid.uuid[0], ACCELERO_POSITIONS_CHAR_UUID, 16);

	/* Properties */
	device_orientation_serv->serv_chars[1].properties = AT_BLE_CHAR_READ | AT_BLE_CHAR_NOTIFY;
	/* value */
	device_orientation_serv->serv_chars[1].init_value = (uint8_t*)&accelero_pos_initial_value ;
	device_orientation_serv->serv_chars[1].value_init_len = sizeof(accelero_positions_t);
	device_orientation_serv->serv_chars[1].value_max_len = sizeof(accelero_positions_t);
	/* permissions */
	device_orientation_serv->serv_chars[1].value_permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR);
	/* user defined name */
	device_orientation_serv->serv_chars[1].user_desc = NULL;
	device_orientation_serv->serv_chars[1].user_desc_len = 0;
	device_orientation_serv->serv_chars[1].user_desc_max_len = 0;
	/*user description permissions*/
	device_orientation_serv->serv_chars[1].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*client config permissions*/
	device_orientation_serv->serv_chars[1].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*server config permissions*/
	device_orientation_serv->serv_chars[1].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*user desc handles*/
	device_orientation_serv->serv_chars[1].user_desc_handle = 0;
	/*client config handles*/
	device_orientation_serv->serv_chars[1].client_config_handle = 0;
	/*server config handles*/
	device_orientation_serv->serv_chars[1].server_config_handle = 0;
	/* presentation format */
	device_orientation_serv->serv_chars[1].presentation_format = NULL;
	
	/*Characteristic Info for device rotation vector(Quaternion+) (X,Y,Z,W) Value*/
	
	/* handle stored here */
	device_orientation_serv->serv_chars[2].char_val_handle = 0;
	
	/* UUID : Accelero Y-Pos */
	device_orientation_serv->serv_chars[2].uuid.type = AT_BLE_UUID_128;
	memcpy(&device_orientation_serv->serv_chars[2].uuid.uuid[0], ROTATION_VECTOR_CHAR_UUID, 16);

	/* Properties */
	device_orientation_serv->serv_chars[2].properties = AT_BLE_CHAR_READ | AT_BLE_CHAR_NOTIFY;
	/* value */
	device_orientation_serv->serv_chars[2].init_value = (uint8_t*)&device_rotation_vector_initial_value ;
	device_orientation_serv->serv_chars[2].value_init_len = sizeof(device_rotation_vector_t);
	device_orientation_serv->serv_chars[2].value_max_len = sizeof(device_rotation_vector_t);
	/* permissions */
	device_orientation_serv->serv_chars[2].value_permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR);
	/* user defined name */
	device_orientation_serv->serv_chars[2].user_desc = NULL;
	device_orientation_serv->serv_chars[2].user_desc_len = 0;
	device_orientation_serv->serv_chars[2].user_desc_max_len = 0;
	/*user description permissions*/
	device_orientation_serv->serv_chars[2].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*client config permissions*/
	device_orientation_serv->serv_chars[2].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*server config permissions*/
	device_orientation_serv->serv_chars[2].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*user desc handles*/
	device_orientation_serv->serv_chars[2].user_desc_handle = 0;
	/*client config handles*/
	device_orientation_serv->serv_chars[2].client_config_handle = 0;
	/*server config handles*/
	device_orientation_serv->serv_chars[2].server_config_handle = 0;
	/* presentation format */
	device_orientation_serv->serv_chars[2].presentation_format = NULL;
	
	/*Characteristic Info for output data rate(ODR) Value*/
	
	/* handle stored here */
	device_orientation_serv->serv_chars[3].char_val_handle = 0;
	
	/* UUID : Accelero Y-Pos */
	device_orientation_serv->serv_chars[3].uuid.type = AT_BLE_UUID_128;
	memcpy(&device_orientation_serv->serv_chars[3].uuid.uuid[0], MOTION_ODR_CHAR_UUID, 16);

	/* Properties */
	device_orientation_serv->serv_chars[3].properties = AT_BLE_CHAR_READ | AT_BLE_CHAR_WRITE;
	/* value */
	device_orientation_serv->serv_chars[3].init_value = (uint8_t*)&motion_odr_initial_value ;
	device_orientation_serv->serv_chars[3].value_init_len = sizeof(uint8_t);
	device_orientation_serv->serv_chars[3].value_max_len = sizeof(uint8_t);
	/* permissions */
	device_orientation_serv->serv_chars[3].value_permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR);
	/* user defined name */
	device_orientation_serv->serv_chars[3].user_desc = NULL;
	device_orientation_serv->serv_chars[3].user_desc_len = 0;
	device_orientation_serv->serv_chars[3].user_desc_max_len = 0;
	/*user description permissions*/
	device_orientation_serv->serv_chars[3].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*client config permissions*/
	device_orientation_serv->serv_chars[3].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*server config permissions*/
	device_orientation_serv->serv_chars[3].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*user desc handles*/
	device_orientation_serv->serv_chars[3].user_desc_handle = 0;
	/*client config handles*/
	device_orientation_serv->serv_chars[3].client_config_handle = 0;
	/*server config handles*/
	device_orientation_serv->serv_chars[3].server_config_handle = 0;
	/* presentation format */
	device_orientation_serv->serv_chars[3].presentation_format = NULL;
	
	/*Characteristic Info for drop-down indication */
	
	/* handle stored here */
	device_orientation_serv->serv_chars[4].char_val_handle = 0;
	
	/* UUID : Accelero Drop Indication */
	device_orientation_serv->serv_chars[4].uuid.type = AT_BLE_UUID_128;
	memcpy(&device_orientation_serv->serv_chars[4].uuid.uuid[0], ACCELERO_DROP_IND_UUID, 16);

	/* Properties */
	device_orientation_serv->serv_chars[4].properties = AT_BLE_CHAR_READ | AT_BLE_CHAR_NOTIFY;
	/* value */
	device_orientation_serv->serv_chars[4].init_value = &accelero_drop_down_ind_initial_value ;
	device_orientation_serv->serv_chars[4].value_init_len = sizeof(uint8_t);
	device_orientation_serv->serv_chars[4].value_max_len = sizeof(uint8_t);
	/* permissions */
	device_orientation_serv->serv_chars[4].value_permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR);
	/* user defined name */
	device_orientation_serv->serv_chars[4].user_desc = NULL;
	device_orientation_serv->serv_chars[4].user_desc_len = 0;
	device_orientation_serv->serv_chars[4].user_desc_max_len = 0;
	/*user description permissions*/
	device_orientation_serv->serv_chars[4].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*client config permissions*/
	device_orientation_serv->serv_chars[4].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*server config permissions*/
	device_orientation_serv->serv_chars[4].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*user desc handles*/
	device_orientation_serv->serv_chars[4].user_desc_handle = 0;
	/*client config handles*/
	device_orientation_serv->serv_chars[4].client_config_handle = 0;
	/*server config handles*/
	device_orientation_serv->serv_chars[4].server_config_handle = 0;
	/* presentation format */
	device_orientation_serv->serv_chars[4].presentation_format = NULL;

	/*Characteristic Info for step-increment indication */
	
	/* handle stored here */
	device_orientation_serv->serv_chars[5].char_val_handle = 0;
	
	/* UUID : Accelero Step Increment indication */

	device_orientation_serv->serv_chars[5].uuid.type = AT_BLE_UUID_128;
	memcpy(&device_orientation_serv->serv_chars[5].uuid.uuid[0], ACCELERO_STEP_INC_IND_UUID, 16);

	/* Properties */
	device_orientation_serv->serv_chars[5].properties = AT_BLE_CHAR_READ | AT_BLE_CHAR_NOTIFY;
	/* value */
	device_orientation_serv->serv_chars[5].init_value = &accelero_step_inc_ind_initial_value ;
	device_orientation_serv->serv_chars[5].value_init_len = sizeof(uint8_t);
	device_orientation_serv->serv_chars[5].value_max_len = sizeof(uint8_t);
	/* permissions */
	device_orientation_serv->serv_chars[5].value_permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR);
	/* user defined name */
	device_orientation_serv->serv_chars[5].user_desc = NULL;
	device_orientation_serv->serv_chars[5].user_desc_len = 0;
	device_orientation_serv->serv_chars[5].user_desc_max_len = 0;
	/*user description permissions*/
	device_orientation_serv->serv_chars[5].user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*client config permissions*/
	device_orientation_serv->serv_chars[5].client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*server config permissions*/
	device_orientation_serv->serv_chars[5].server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*user desc handles*/
	device_orientation_serv->serv_chars[5].user_desc_handle = 0;
	/*client config handles*/
	device_orientation_serv->serv_chars[5].client_config_handle = 0;
	/*server config handles*/
	device_orientation_serv->serv_chars[5].server_config_handle = 0;
	/* presentation format */
	device_orientation_serv->serv_chars[5].presentation_format = NULL;
}


/**@brief Battery service and characteristic initialization
*
*/
static void init_battery_service(gatt_battery_service_handler_t *battery_serv)
{
	battery_serv->serv_handle = 0;

	battery_serv->serv_uuid.type = AT_BLE_UUID_128;
	memcpy(&battery_serv->serv_uuid.uuid[0], BATTERY_SERVICE_UUID, 16);
	
	/*Characteristic Info for X-Pos Value*/
	
	/* handle stored here */
	battery_serv->serv_chars.char_val_handle = 0;
	
	/* UUID : Battery Level */
	battery_serv->serv_chars.uuid.type = AT_BLE_UUID_128;
	memcpy(&battery_serv->serv_chars.uuid.uuid[0], BATTERY_LEVEL_CHAR_UUID, 16);

	/* Properties */
	battery_serv->serv_chars.properties = AT_BLE_CHAR_READ | AT_BLE_CHAR_NOTIFY;
	/* value */
	battery_serv->serv_chars.init_value = &battery_level_initial_value ;
	battery_serv->serv_chars.value_init_len = sizeof(uint8_t);
	battery_serv->serv_chars.value_max_len = sizeof(uint8_t);
	/* permissions */
	battery_serv->serv_chars.value_permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR);
	/* user defined name */
	battery_serv->serv_chars.user_desc = NULL;
	battery_serv->serv_chars.user_desc_len = 0;
	battery_serv->serv_chars.user_desc_max_len = 0;
	/*user description permissions*/
	battery_serv->serv_chars.user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*client config permissions*/
	battery_serv->serv_chars.client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*server config permissions*/
	battery_serv->serv_chars.server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*user desc handles*/
	battery_serv->serv_chars.user_desc_handle = 0;
	/*client config handles*/
	battery_serv->serv_chars.client_config_handle = 0;
	/*server config handles*/
	battery_serv->serv_chars.server_config_handle = 0;
	/* presentation format */
	battery_serv->serv_chars.presentation_format = NULL;
}

/**@brief Touch Gesture service and characteristic initialization
*
*/
static void init_touch_getsure_service(gatt_touch_gesture_service_handler_t *touch_gest_serv)
{
	touch_gest_serv->serv_handle = 0;

	touch_gest_serv->serv_uuid.type = AT_BLE_UUID_128;
	memcpy(&touch_gest_serv->serv_uuid.uuid[0], TOUCH_GESTURE_SERVICE_UUID, 16);
	
	/*Characteristic Info for X-Pos Value*/
	
	/* handle stored here */
	touch_gest_serv->serv_chars.char_val_handle = 0;
	
	/* UUID : Touch Gesture */
	touch_gest_serv->serv_chars.uuid.type = AT_BLE_UUID_128;
	memcpy(&touch_gest_serv->serv_chars.uuid.uuid[0], TOUCH_GESTURE_CHAR_UUID, 16);

	/* Properties */
	touch_gest_serv->serv_chars.properties = AT_BLE_CHAR_READ | AT_BLE_CHAR_NOTIFY;
	/* value */
	touch_gest_serv->serv_chars.init_value = &touch_gesture_initial_value ;
	touch_gest_serv->serv_chars.value_init_len = sizeof(uint8_t);
	touch_gest_serv->serv_chars.value_max_len = sizeof(uint8_t);
	/* permissions */
	touch_gest_serv->serv_chars.value_permissions = (AT_BLE_ATTR_READABLE_NO_AUTHN_NO_AUTHR | AT_BLE_ATTR_WRITABLE_NO_AUTHN_NO_AUTHR);
	/* user defined name */
	touch_gest_serv->serv_chars.user_desc = NULL;
	touch_gest_serv->serv_chars.user_desc_len = 0;
	touch_gest_serv->serv_chars.user_desc_max_len = 0;
	/*user description permissions*/
	touch_gest_serv->serv_chars.user_desc_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*client config permissions*/
	touch_gest_serv->serv_chars.client_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*server config permissions*/
	touch_gest_serv->serv_chars.server_config_permissions = AT_BLE_ATTR_NO_PERMISSIONS;
	/*user desc handles*/
	touch_gest_serv->serv_chars.user_desc_handle = 0;
	/*client config handles*/
	touch_gest_serv->serv_chars.client_config_handle = 0;
	/*server config handles*/
	touch_gest_serv->serv_chars.server_config_handle = 0;
	/* presentation format */
	touch_gest_serv->serv_chars.presentation_format = NULL;
}

/**
 * \brief Initialization of Wearable demo profile services
 */
static void wbe_service_init(void)
{
	init_environment_service(&environment_service_handle);
	init_device_orientation_service(&device_orientation_service_handle);
	init_battery_service(&battery_service_handle);
	init_touch_getsure_service(&touch_getsure_service_handle);	
}

/**
* \@brief initializes and defines wearable demo profile services and advertisement data
*/

void wbe_profile_init(void)
{
	/* st services initialization*/
	wbe_service_init();
	
	/* st services definition		*/
	wbe_service_define();
	
	/* set the advertisement data */
	wbe_advertisement_data_set();
		
	/* Register callbacks for gap related events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_GAP_EVENT_TYPE,
	startup_template_app_gap_cb);
		
	/* Register callbacks for gatt server related events */
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_GATT_SERVER_EVENT_TYPE,
	startup_template_app_gatt_server_cb);	
}
