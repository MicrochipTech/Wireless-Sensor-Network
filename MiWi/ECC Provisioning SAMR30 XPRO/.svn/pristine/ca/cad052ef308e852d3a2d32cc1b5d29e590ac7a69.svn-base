/**
 * \file
 *
 * \brief Wearable Demo Service declarations
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

#ifndef __WEARABLE_H__
#define __WEARABLE_H__

/*- Includes ---------------------------------------------------------------*/
#include "at_ble_api.h"
#include "ble_utils.h"
#include "ble_manager.h"

/* macros */

/** @brief count of included service in tx power service
  * 
  */
#define ST_INCLUDED_SERVICE_COUNT		(0)

/** @brief count of characteristics in tx power service
  * 
  */
#define ENVIRONMENT_CHARACTERISTIC_COUNT		(2)
#define DEVICE_ORIENTATION_CHARACTERISTIC_COUNT		(6)
#define BATTERY_LEVEL_CHARACTERISTIC_COUNT		(1)
#define TOUCH_GESTURE_CHARACTERISTIC_COUNT		(1)

/** @brief scan_resp_len is the length of the scan response data */
#define SCAN_RESP_LEN					(10)

/** @brief LL_ADV_DATA_UUID_LEN the size of LL service uuid */
#define ADV_DATA_UUID_LEN			(16)
/** @brief LL_ADV_DATA_UUID_TYPE the total sizeof LL service uuid*/
#define ADV_DATA_UUID_TYPE			(0x06)

/** @brief ADV_TYPE_LEN */
#define ADV_TYPE_LEN					(0x01)

/* @brief PXP_ADV_DATA_NAME_DATA the actual name of device */
#define WD_ADV_DATA_NAME_DATA			("AT-WBLE")

/** @brief PXP_ADV_DATA_NAME_LEN the  length of the device name */
#define WD_ADV_DATA_NAME_LEN			(7)

/** @brief PXP_ADV_DATA_NAME_TYPE the gap ad data type */
#define WD_ADV_DATA_NAME_TYPE			(0x09)

/** @brief APP_PXP_FAST_ADV between 0x0020 and 0x4000 in 0.625 ms units (20ms to 10.24s). */
#define APP_FAST_ADV				(0x20) //20 ms//(100) //100 ms

/** @brief APP_PXP_ADV_TIMEOUT Advertising time-out between 0x0001 and 0x3FFF in seconds, 0x0000 disables time-out.*/
#define APP_ADV_TIMEOUT				(1000) // 100 Secs

/* smart tag service UUIDs */

/* Environment service */
#define ENVIRONMENT_SERVICE_UUID ("\x1b\xc5\xd5\xa5\x02\x00\xa6\x87\xe5\x11\x36\x39\xc0\xba\x5a\xf0")

/* Environment service characteristics */
#define ENVIRONMENT_CHAR_UUID	("\x1b\xc5\xd5\xa5\x02\x00\xa6\x87\xe5\x11\x36\x39\xd0\xba\x5a\xf0")
#define ENV_ODR_CHAR_UUID	("\x1b\xc5\xd5\xa5\x02\x00\xa6\x87\xe5\x11\x36\x39\xd1\xba\x5a\xf0")
//#define PRESSURE_CHAR_UUID	("\x1b\xc5\xd5\xa5\x02\x00\xa6\x87\xe5\x11\x36\x39\xd2\xba\x5a\xf0")
//#define UV_CHAR_UUID	("\x1b\xc5\xd5\xa5\x02\x00\xa6\x87\xe5\x11\x36\x39\xd3\xba\x5a\xf0")

/* Device orientation service */
#define DEVICE_ORIENTATION_SERVICE_UUID	("\x1b\xc5\xd5\xa5\x02\x00\xa6\x87\xe5\x11\x36\x39\xc1\xba\x5a\xf0")

/* Device orientation service characteristics */
#define GYRO_POSITIONS_CHAR_UUID	("\x1b\xc5\xd5\xa5\x02\x00\xa6\x87\xe5\x11\x36\x39\xd4\xba\x5a\xf0")
#define ACCELERO_POSITIONS_CHAR_UUID	("\x1b\xc5\xd5\xa5\x02\x00\xa6\x87\xe5\x11\x36\x39\xd7\xba\x5a\xf0")
#define ROTATION_VECTOR_CHAR_UUID	("\x1b\xc5\xd5\xa5\x02\x00\xa6\x87\xe5\x11\x36\x39\xd8\xba\x5a\xf0")
#define MOTION_ODR_CHAR_UUID	("\x1b\xc5\xd5\xa5\x02\x00\xa6\x87\xe5\x11\x36\x39\xd9\xba\x5a\xf0")
#define ACCELERO_DROP_IND_UUID	("\x1b\xc5\xd5\xa5\x02\x00\xa6\x87\xe5\x11\x36\x39\xda\xba\x5a\xf0")
#define ACCELERO_STEP_INC_IND_UUID	("\x1b\xc5\xd5\xa5\x02\x00\xa6\x87\xe5\x11\x36\x39\xdb\xba\x5a\xf0")


//#define GYRO_Y_POS_CHAR_UUID	("\x1b\xc5\xd5\xa5\x02\x00\xa6\x87\xe5\x11\x36\x39\xd5\xba\x5a\xf0")
//#define GYRO_Z_POS_CHAR_UUID	("\x1b\xc5\xd5\xa5\x02\x00\xa6\x87\xe5\x11\x36\x39\xd6\xba\x5a\xf0")
/* Accelerometer service */
//#define ACCELEROMETER_SERVICE_UUID	("\x1b\xc5\xd5\xa5\x02\x00\xa6\x87\xe5\x11\x36\x39\xc2\xba\x5a\xf0") //(0xF803)

/* Battery level service */
#define BATTERY_SERVICE_UUID	("\x1b\xc5\xd5\xa5\x02\x00\xa6\x87\xe5\x11\x36\x39\xc3\xba\x5a\xf0") 

/* Battery level service characteristics */
#define BATTERY_LEVEL_CHAR_UUID	("\x1b\xc5\xd5\xa5\x02\x00\xa6\x87\xe5\x11\x36\x39\xdc\xba\x5a\xf0")

/* Touch gesture service */
#define TOUCH_GESTURE_SERVICE_UUID	("\x1b\xc5\xd5\xa5\x02\x00\xa6\x87\xe5\x11\x36\x39\xc4\xba\x5a\xf0")

/* Touch gesture service characteristics */
#define TOUCH_GESTURE_CHAR_UUID	("\x1b\xc5\xd5\xa5\x02\x00\xa6\x87\xe5\x11\x36\x39\xdd\xba\x5a\xf0")

/* type definitions */

/** @brief GATT services handles
*/
typedef struct gatt_environment_service_handler
{
	/// service uuid
	at_ble_uuid_t	serv_uuid;
	/// service handle
	at_ble_handle_t	serv_handle;
	/// service characteristic
	at_ble_characteristic_t	serv_chars[2];
}gatt_environment_service_handler_t;

typedef struct gatt_device_orientation_service_handler
{
	/// service uuid
	at_ble_uuid_t	serv_uuid;
	/// service handle
	at_ble_handle_t	serv_handle;
	/// service characteristic
	at_ble_characteristic_t	serv_chars[6];
}gatt_device_orientation_service_handler_t;

typedef struct gatt_battery_service_handler
{
	/// service uuid
	at_ble_uuid_t	serv_uuid;
	/// service handle
	at_ble_handle_t	serv_handle;
	/// service characteristic
	at_ble_characteristic_t	serv_chars;
}gatt_battery_service_handler_t;

typedef struct gatt_touch_gesture_service_handler
{
	/// service uuid
	at_ble_uuid_t	serv_uuid;
	/// service handle
	at_ble_handle_t	serv_handle;
	/// service characteristic
	at_ble_characteristic_t	serv_chars;
}gatt_touch_gesture_service_handler_t;

typedef struct gyro_positions
{
	int16_t x_pos;
	int16_t y_pos;
	int16_t z_pos;
}gyro_positions_t;

typedef struct accelero_positions
{
	int16_t x_pos;
	int16_t y_pos;
	int16_t z_pos;
}accelero_positions_t;

typedef struct device_rotation_vector
{
	int16_t x;
	int16_t y;
	int16_t z;
	int16_t w;
	
}device_rotation_vector_t;

typedef struct environment_data
{
	int16_t temperature;
	uint16_t pressure;
	uint32_t uv;
	uint8_t humidity;
}environment_data_t;

/* extern variables */
extern gatt_environment_service_handler_t environment_service_handle;
extern volatile bool connection_established_flag;
extern volatile bool env_char_notification_flag;
extern volatile bool gyro_char_notification_flag;
extern volatile bool accelero_char_notification_flag;
extern volatile bool rotation_vector_char_notification_flag;
extern volatile bool drop_char_notification_flag;
extern volatile bool step_inc_char_notification_flag;
extern volatile bool low_bat_char_notification_flag;
extern volatile bool touch_gest_char_notification_flag;
extern uint16_t advertisement_timer;
extern bool started_advertising;
extern uint8_t env_sensor_period;
extern uint8_t motion_sensor_period;
extern uint8_t ble_event_task_period;

/* inline functions */

/* check ble connection established */
static inline bool is_ble_connection_established(void)
{
	return (connection_established_flag);
}

/* check environment characteristic notification enabled */
static inline bool is_ble_env_char_notification_enabled(void)
{
	return (env_char_notification_flag);
}

/* check gyro characteristic notification enabled */
static inline bool is_ble_gyro_char_notification_enabled(void)
{
	return (gyro_char_notification_flag);
}

/* check accelero characteristic notification enabled */
static inline bool is_ble_accelero_char_notification_enabled(void)
{
	return (accelero_char_notification_flag);
}

/* check rotation vector characteristic notification enabled */
static inline bool is_ble_rotation_vector_char_notification_enabled(void)
{
	return (rotation_vector_char_notification_flag);
}

/* check drop characteristic notification enabled */
static inline bool is_ble_drop_char_notification_enabled(void)
{
	return (drop_char_notification_flag);
}

/* check step-inc characteristic notification enabled */
static inline bool is_ble_step_inc_char_notification_enabled(void)
{
	return (step_inc_char_notification_flag);
}

/* check low battery characteristic notification enabled */
static inline bool is_ble_low_bat_char_notification_enabled(void)
{
	return (low_bat_char_notification_flag);
}

/* check touch gesture characteristic notification enabled */
static inline bool is_ble_touch_gest_char_notification_enabled(void)
{
	return (touch_gest_char_notification_flag);
}

/**@brief Function used to update characteristic value
 *
 * @param[in] lock_command_serv gatt service information
 * @param[in] char_len length of the new characteristic value
 * @param[in] char_data new characteristic value information
 * @return @ref AT_BLE_SUCCESS operation completed successfully
 * @return @ref AT_BLE_FAILURE Generic error.
 */
at_ble_status_t key_update_char_value (gatt_environment_service_handler_t *lock_command_serv , uint8_t char_data);

/**
* \smart tag service advertisement initialization and adv start
*/
void wbe_service_adv(void);

/**
* \Lock command Initialization which initializes service,defines and start adv
*/
void wbe_init(void *param);

/**
 * @brief Event for connection 
 *
 * @param[in] 
 *
 */
void st_connection_event(at_ble_handle_t ble_handle);

/* @brief timer call back for rssi update
* enable the flags to execute the application taskc
*
*/
void timer_callback_handler(void);

/**
 * @brief Event for connection 
 *
 * @param[in] 
 *
 */
void wbe_connection_event(at_ble_handle_t ble_handle);

/* Application interfaces */
at_ble_status_t ble_notify_environment_data(environment_data_t* environ_data);
at_ble_status_t ble_notify_gyro_positions(gyro_positions_t* gyro_positions);
at_ble_status_t ble_notify_accelero_positions(accelero_positions_t* accelero_pos);
at_ble_status_t ble_notify_device_rotation_vector(device_rotation_vector_t* rotation_vector);
at_ble_status_t ble_notify_accelero_step_detection(void);
at_ble_status_t ble_notify_device_drop_detection(void);
at_ble_status_t ble_notify_low_battery(void);
at_ble_status_t ble_notify_touch_gesture(uint8_t gesture);
at_ble_status_t wbe_char_changed_handler(void *params);
void wbe_profile_init(void);
at_ble_status_t wbe_start_advertisement(void);
void timer_callback_fn(void);

#endif /* __WEARABLE_H__ */

