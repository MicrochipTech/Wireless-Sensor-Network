/**
 * \file
 *
 * \brief Platform Abstraction layer for BLE applications
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

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "at_ble_api.h"


/**
 *@defgroup	platform_group_datatypes Data types
 *@ingroup	platform_group
 *@brief	This group includes Macros defined to be used.
 * @{
 */
/** @}*/

/**
 *@defgroup	platform_group_functions Functions
 *@ingroup	platform_group
 *@brief	This group includes all function prototypes required to be used. 
 * @{
 */
/** @}*/

/// Time waiting for initialization bus response
#define BUS_RSP_TIMEOUT_VAL_IN_SEC	2

/**@enum	interface_type
 * @ingroup platform_group_datatypes
 * @brief	This enumeration used by @ref platform_config to select @ref platform_config::bus_type
 * 			either @ref UART or @ref SPI
 */
enum interface_type {
	/// UART Interface is used [Default]
	AT_BLE_UART = 1,
	/// SPI interface is used
	AT_BLE_SPI
};

typedef enum hw_flow_control
{
	ENABLE_HW_FC_PATCH = 1,
	DISABLE_HW_FC_PATCH
}hw_flow_control_t;


/**@struct	platform_config
 * @ingroup platform_group_datatypes
 * @brief	This platform structure used to define bus type and 
 *			info required to initialize bus interface
 * @var		platform_config::bus_type
 * @var		platform_config::bus_info
 */
typedef struct {
	/// One of @ref interface_type; either @ref UART or @ref SPI
	uint8_t bus_type;
	/// In case of using @ref UART as interface this is used to save COM port value [1 - 255]
	uint8_t bus_info;
}platform_config;

typedef struct{
	uint8_t (*wr_api32) (uint32_t memAddr,uint32_t* data,uint8_t size);
	uint8_t (*wr_api32_reset) (uint32_t memAddr,uint32_t* data,uint8_t size);
}wr_apis;

typedef enum tenuTransportState{
	PLATFORM_TRANSPORT_SLAVE_DISCONNECTED=0,
	PLATFORM_TRANSPORT_SLAVE_PATCH_DOWNLOAD,
	PLATFORM_TRANSPORT_SLAVE_CONNECTED
}tenuTransportState_t;

enum transfer_mode
{
	TX_MODE = 1,
	RX_MODE
};

#define BLE_SERIAL_START_BYTE (0x05)

#define BLE_SERIAL_HEADER_LEN (0x09)

 /**@ingroup platform_group_functions
  * @brief implements platform-specific initialization
  *
  * @param[in] platform_params platform specific parameters, this pointer is passed from the 
  * at_ble_init function and interpreted by the platform 
  *
  * @return Upon successful completion the function shall return @ref AT_BLE_SUCCESS, Otherwise the function shall return @ref at_ble_status_t 
  */
at_ble_status_t platform_init(void* platform_params);

 /**@ingroup platform_group_functions
  * @brief sends a message over the platform-specific bus
  *
  * Sends a message over the platform-specific bus that might be UART, SPI or other
  * if the BTLC1000 external wakeup signal is used, it is up to this function implementation
  * to assert/de-assert it appropriately
  *
  * @param[in] if_type interface bus type
  * @param[in] data data to send over the interface
  * @param[in] len length of data
  *
  */
at_ble_status_t platform_interface_send(uint8_t if_type, uint8_t *data, uint32_t len);

///@cond IGNORE_DOXYGEN 

 /** @brief sends wakeup signal over platform-specific bus
  *
  */
int platform_interface_send_wakeup(void);

 /** @brief sends sleep signal over the platform-specific bus
  *
  */
int platform_interface_send_sleep(void);

///@endcond 

 /**@ingroup platform_group_functions
  * @brief recv a message over the platform-specific bus
  *
  * receive a message over the platform-specific bus that might be UART, SPI or other
  * if the BTLC1000 external wakeup signal is used, it is up to this function implementation
  * to assert/de-assert it appropriately
  *
  * @param[in] if_type interface bus type
  * @param[in] data data to send over the interface
  * @param[in] len length of data
  *
  */
int platform_interface_recv(uint8_t if_type, uint8_t* data, uint32_t len);

 /**@ingroup platform_group_functions
  * @brief the callback to upper layers to process received packets
  *
  * This function is implemented by the upper layers (the event loop) and it is up
  * to the platform implementation to call it whenever data are received from the interface
  *
  * @param[in] data data received from the interface
  * @param[in] len length of data
  *
  */
void platform_interface_callback(uint8_t* data, uint32_t len);

void fw_patch_download_cb(uint8_t *pu8data, uint8_t length);
at_ble_status_t patch_init(void);

 /**@ingroup platform_group_functions
  * @brief fires the comand-complete signal
  *  @note more details at the platform porting guide
  *
  */
void platform_cmd_cmpl_signal(void);

 /**@ingroup platform_group_functions
  * @brief blocks until the command-complete signal is fired
  *  @note more details at the platform porting guide
  *
  * @param[out] timeout a flag that indicates if waiting timed out
  */
void platform_cmd_cmpl_wait(bool* timeout);

 /**@ingroup platform_group_functions
  * @brief fires the event signal
  *  @note more details at the platform porting guide
  *
  */
void platform_event_signal(void);

 /**@ingroup platform_group_functions
  * @brief blocks until the event signal is fired
  *  @note more details at the platform porting guide
  *
  * @param[in] timeout timeout in ms passed by user
  *
  */
at_ble_status_t platform_event_wait(uint32_t timeout);

uint8_t platform_sleep(uint32_t sleepms);

 /** @brief receives the bytes from platform driver
  *  @note more details at the platform porting guide
  *
  * @param[out] at_ble_status_t AT_BLE_SUCCESS or AT_BLE_FAILURE
  *
  */
at_ble_status_t platform_ble_event_data(void);

 /** @}*/

void platform_start_timer(uint32_t timeout); 
void platform_wakeup(void);
void platform_set_sleep(void);
void platform_enter_critical_section(void);
void platform_leave_critical_section(void);
void platform_cleanup(void);
void start_timer(uint32_t timeout);
uint32_t timer_done(void);
void stop_timer(void);
void bus_activity_timer_callback(void);
void check_and_assert_ext_wakeup(uint8_t mode);
void platform_configure_hw_fc_uart(void);
#define UNREFERENCED_PARAMETER(x) ((void)x)
void platform_dma_process_rxdata(uint8_t *buf, uint16_t len);
#endif // __PLATFORM_H__
