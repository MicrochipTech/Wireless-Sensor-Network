/**
 * \file
 *
 * \brief Platform Abstraction layer for BLE applications
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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

#include <asf.h>
#include "platform.h"
#include "conf_serialdrv.h"
#include "serial_drv.h"
#include "serial_fifo.h"
#include "ble_utils.h"
#include "timer_hw.h"

uint8_t bus_type = AT_BLE_UART;

extern volatile tenuTransportState_t slave_state;
#define GTL_EIF_CONNECT_REQ	0xA5
#define GTL_EIF_CONNECT_RESP 0x5A
#define BTLC1000_STARTUP_DELAY (3500)
#define BTLC1000_WAKEUP_DELAY (5)
#define PLATFORM_EVT_WAIT_TIMEOUT (4000)

static volatile uint32_t cmd_cmpl_flag = 0;
static volatile uint32_t event_flag = 0;

extern ser_fifo_desc_t ble_usart_rx_fifo;

/** data transmitted done interrupt event flag */
volatile uint8_t tx_done = 0;				//	TX Transfer complete flag
volatile uint8_t data_received = 0;			//	RX data received flag

volatile int init_done = 0;

#if UART_FLOWCONTROL_6WIRE_MODE == true
/* Enable Hardware Flow-control on BTLC1000 */
enum hw_flow_control ble_hardware_fc = ENABLE_HW_FC_PATCH;
#else
/* Disable Hardware Flow-control on BTLC1000 */
enum hw_flow_control ble_hardware_fc = DISABLE_HW_FC_PATCH;
#endif

#if ((UART_FLOWCONTROL_4WIRE_MODE == true) && (UART_FLOWCONTROL_6WIRE_MODE == true))
#error "Invalid UART Flow Control mode Configuration. Choose only one mode"
#endif


//#define BLE_DBG_ENABLE
#define DBG_LOG_BLE		DBG_LOG

#ifdef BLE_DBG_ENABLE
uint8_t rx_buf[256];
uint16_t rx_buf_idx;
#endif

void platform_process_rxdata(uint32_t t_rx_data);

void bus_activity_timer_callback(void)
{
	if (bus_type == AT_BLE_UART)
	{
		if (platform_serial_drv_tx_status() == STATUS_OK)
		{
			platform_set_sleep();
		}
		else
		{
			platform_reset_bus_timer();
		}
	}
}

void check_and_assert_ext_wakeup(uint8_t mode)
{
	if (init_done)
	{
		if (!ble_wakeup_pin_level())
		{
			platform_wakeup();
			if (mode == TX_MODE)
			{
				delay_ms(BTLC1000_WAKEUP_DELAY);
			}
			Platform_start_bus_timer(5);
		}
		else
		{						
			platform_reset_bus_timer();
		}
	}
}

at_ble_status_t platform_init(void* platform_params)
{	
	platform_config	*cfg = (platform_config *)platform_params;
	
	if (cfg->bus_type == AT_BLE_UART)
	{
		
		ble_configure_control_pin();
		
		configure_serial_drv();
		
		platform_configure_timer(bus_activity_timer_callback);
		
		delay_ms(BTLC1000_STARTUP_DELAY);
		
		bus_type = AT_BLE_UART;
		return AT_BLE_SUCCESS;
	}
	return AT_BLE_INVALID_PARAM;
	
}

at_ble_status_t platform_interface_send(uint8_t if_type, uint8_t* data, uint32_t len)
{
	if (if_type != AT_BLE_UART)
	{
		return AT_BLE_INVALID_PARAM;
	}
   
#ifdef BLE_DBG_ENABLE
	uint32_t i;
	
	DBG_LOG_BLE("\r\nRx:%d: ", rx_buf_idx);
	for (i = 0; i < rx_buf_idx; i++)
	{
		DBG_LOG_CONT("0x%X, ", rx_buf[i]);
	}
	rx_buf_idx = 0;
	DBG_LOG_BLE("\r\nTxLen:%d: ", len);
	for (i = 0; i < len; i++)
	{
		DBG_LOG_CONT("0x%X, ", data[i]);
		
	}
#endif
	platform_enter_critical_section();
	platform_set_serial_drv_tx_status();
	platform_leave_critical_section();
	check_and_assert_ext_wakeup(TX_MODE);

	serial_drv_send(data, len);
	return AT_BLE_SUCCESS;
}

void platform_cmd_cmpl_signal(void)
{
	cmd_cmpl_flag = 1;
}

int platform_interface_recv(uint8_t if_type, uint8_t* data, uint32_t len)
{
    ALL_UNUSED(data);
    ALL_UNUSED(len);
    
    if (if_type == AT_BLE_UART)
    {
            return STATUS_OK;
    }
    else
    {
            return -1;	
    }	
}

int platform_interface_send_sleep(void)
{
	return 0;
}

int platform_interface_send_wakeup(void)
{
	return 0;
}

uint32_t ticks = 0;
uint32_t ctr8 = 190; // 1 ms @ 8 MHz
uint32_t ctr48 = 615; // 1ms @ 48 MHz


/* Timeout in ms */
void start_timer(uint32_t timeout)
{

#if SAMG55	
	uint32_t main_clk  = sysclk_get_cpu_hz();
#endif

#if SAMD21 || SAML21
	uint32_t main_clk  = system_gclk_gen_get_hz(0);
#endif
	double ctr = 1.0;
	if (main_clk == 8000000)
		ticks = ctr8*timeout;
	else if (main_clk == 48000000)
		ticks = ctr48*timeout;
	else
	{
		ctr = 615.0 * (double)((double)main_clk/48000000);
		ticks = (uint32_t)ctr * timeout;
	}
	ticks *= 2;
}


uint32_t timer_done(void)
{
	return --ticks; 
}

void platform_enter_critical_section(void)
{
	Disable_global_interrupt();
}
 
void platform_leave_critical_section(void)
{
	Enable_global_interrupt();
}


void platform_process_rxdata(uint32_t t_rx_data)
{
	if(slave_state == PLATFORM_TRANSPORT_SLAVE_CONNECTED)
	{
		platform_enter_critical_section();
		ser_fifo_push_uint8(&ble_usart_rx_fifo, (uint8_t)t_rx_data);
		platform_leave_critical_section();
	}			
	else if(slave_state == PLATFORM_TRANSPORT_SLAVE_PATCH_DOWNLOAD)
	{
		fw_patch_download_cb((uint8_t*)&t_rx_data, 1);
	}
	else if(slave_state == PLATFORM_TRANSPORT_SLAVE_DISCONNECTED)
	{
		if(t_rx_data == GTL_EIF_CONNECT_RESP)
		{
			slave_state = PLATFORM_TRANSPORT_SLAVE_PATCH_DOWNLOAD;
		}				
	}
}

void platform_dma_process_rxdata(uint8_t *buf, uint16_t len)
{
	if(slave_state == PLATFORM_TRANSPORT_SLAVE_CONNECTED)
	{
		uint16_t idx;
		for (idx = 0; idx < len; idx++)
		{
			platform_enter_critical_section();
			ser_fifo_push_uint8(&ble_usart_rx_fifo, buf[idx]);
			platform_leave_critical_section();
		}
	}
	else if(slave_state == PLATFORM_TRANSPORT_SLAVE_PATCH_DOWNLOAD)
	{
		fw_patch_download_cb(buf, len);
	}
	else if(slave_state == PLATFORM_TRANSPORT_SLAVE_DISCONNECTED)
	{
		if(buf[0] == GTL_EIF_CONNECT_RESP)
		{
			slave_state = PLATFORM_TRANSPORT_SLAVE_PATCH_DOWNLOAD;
		}
	}
}

void platform_cmd_cmpl_wait(bool* timeout)
{
	uint32_t t_rx_data;
	start_timer(PLATFORM_EVT_WAIT_TIMEOUT);
	do 
	{
		platform_enter_critical_section();
		if(ser_fifo_pull_uint8(&ble_usart_rx_fifo, (uint8_t *)&t_rx_data) == SER_FIFO_OK)
		{		
			platform_leave_critical_section();	
			platform_interface_callback((uint8_t*)&t_rx_data, 1);
		}
		platform_leave_critical_section();
	}while((cmd_cmpl_flag != 1) && (timer_done()>0));

	if (cmd_cmpl_flag == 1)
	{
	#ifdef BLE_DBG_ENABLE
		DBG_LOG_BLE("\r\nAS\n");
	#endif
		cmd_cmpl_flag = 0;
		stop_timer();
	}
	else
	{
		*timeout = true;
	#ifdef BLE_DBG_ENABLE
		DBG_LOG_BLE("\r\nAF\n");
	#endif
	}
}

void platform_event_signal(void)
{
	event_flag = 1;
}

at_ble_status_t platform_event_wait(uint32_t timeout)
{
	at_ble_status_t status = AT_BLE_SUCCESS;
	uint8_t platform_buf[1];
	uint32_t t_rx_data;
	start_timer(timeout);
	do
	{
		platform_enter_critical_section();
		if(ser_fifo_pull_uint8(&ble_usart_rx_fifo, (uint8_t *)&t_rx_data) == SER_FIFO_OK)
		{
			platform_leave_critical_section();
			platform_buf[0] = (uint8_t)t_rx_data;
			platform_interface_callback(platform_buf, 1);
		}
		platform_leave_critical_section();
	}while((event_flag != 1) && (timer_done()>0));
	if (event_flag == 1)
	{
		event_flag = 0;
		#ifdef BLE_DBG_ENABLE
		DBG_LOG_BLE("\r\nSS\n");
		#endif
		stop_timer();
	}
	else
	{
		status = AT_BLE_TIMEOUT;
		#ifdef BLE_DBG_ENABLE
		DBG_LOG_BLE("\r\nSF\n");
		#endif
	}	
	return status;
}

uint8_t platform_sleep(uint32_t sleepms)
{
	delay_ms(sleepms);
	return true;
}

void serial_rx_callback(void) 
{
	check_and_assert_ext_wakeup(RX_MODE);
}

void serial_tx_callback(void)
{
	tx_done = 1;
}

 void platform_wakeup(void)
 {
	 ble_wakeup_pin_set_high();
 }
 
 void platform_set_sleep(void)
 {
	 #if defined ENABLE_POWER_SAVE
 	#ifndef DTM_MODE
		ble_wakeup_pin_set_low();
	#endif
	 #endif
 }
 
void platform_start_timer(uint32_t timeout)
 {
	 ALL_UNUSED(timeout); //To avoid compiler warning
 }
 
 void platform_cleanup(void)
 {
	 
 }
 
 void stop_timer(void)
 {
	 ticks = 0;
 }

 void platform_configure_hw_fc_uart(void)
 {
 	configure_usart_after_patch();
 }

