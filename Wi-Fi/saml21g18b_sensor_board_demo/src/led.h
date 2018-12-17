/**
 * \file
 *
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
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel
 * Support</a>
 */

#ifndef __LED_H__
#define __LED_H__

typedef enum Led_Color
{
	LED_COLOR_BLUE = 0,
	LED_COLOR_GREEN = 1,
	LED_COLOR_RED = 2,
	LED_COLOR_YELLOW = 3,
	LED_COLOR_Magneta = 4,
	LED_COLOR_Cyan = 5,
	LED_COLOR_WHTIE = 6,
}Led_Color;

typedef enum Led_Mode
{
	LED_MODE_NONE = 0,
	LED_MODE_TURN_OFF = 1,
	LED_MODE_TURN_ON = 2,
	LED_MODE_BLINK_NORMAL = 3,
	LED_MODE_BLINK_FAST = 4,
	LED_MODE_BLINK_SHORT = 5,
}Led_Mode;

void initialise_led(void);
void led_ctrl_set_color(Led_Color color, Led_Mode mode);
void led_ctrl_set_mode(Led_Mode mode);
void led_ctrl_execute(void);
void toggleLED(void);
Led_Color led_ctrl_get_color(void);

extern Led_Mode gu8LedMode;

#endif /*__LED_H__*/
