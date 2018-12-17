/**
 * \file
 *
 * \brief RTC header file
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


#ifndef __RTC_H__
#define __RTC_H__

//#include "touch_api_ptc.h"
#include "conf_board.h"
#include "rtc.h"

/* macros */
#define RTC_PERIOD 20u
#define BLE_ADVERTISEMENT_TIMEOUT  60000u //10second
#define TIME_PERIOD_1MSEC 1u


/* external variables */
extern struct rtc_module rtc_instance;
extern volatile uint8_t tick_100ms;
extern volatile uint8_t tick_500ms;
extern volatile uint8_t tick_rotation_data;
extern volatile uint8_t tick_1second;
extern volatile uint8_t tick_2second;
extern volatile uint8_t tick_5second;
extern volatile uint8_t tick_env_sensor;
extern volatile uint8_t tick_motion_sensor;
extern volatile uint8_t tick_ble_event_task;
extern volatile uint8_t tick_10second;
extern volatile uint8_t tick_bhi;
extern volatile uint8_t tick_60second;
extern bool low_battery_flag;

extern unsigned char rotation_data_timer;

/* prototypes */
void rtc_init(void);
bool Ten_SecExpired(void);

void setSleepPeriod(uint32_t interval);
bool SecExpired(uint8_t value);

#endif /* __RTC_H__ */