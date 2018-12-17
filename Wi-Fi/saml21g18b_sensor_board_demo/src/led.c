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
#include "conf_board.h"
#include "led.h"
#include "rtc.h"

unsigned char gu8Blue;
unsigned char gu8Red;
unsigned char gu8Green;
unsigned char gu8OnOffState;
Led_Mode gu8LedMode;
Led_Color gu8Color;


void toggleLED()
{
	if (gu8OnOffState == 1)
	{
		
		gu8OnOffState = 0;
		port_pin_set_output_level(BLUE_LED, 1);
		port_pin_set_output_level(GREEN_LED, 1);
		port_pin_set_output_level(RED_LED, 1);
	}
	else
	{
		gu8OnOffState = 1;
		port_pin_set_output_level(BLUE_LED, gu8Blue);
		port_pin_set_output_level(GREEN_LED, gu8Green);
		port_pin_set_output_level(RED_LED, gu8Red);
	}
}

static inline void turnOnLED(void)
{
	gu8OnOffState = 1;
	port_pin_set_output_level(BLUE_LED, gu8Blue);
	port_pin_set_output_level(GREEN_LED, gu8Green);
	port_pin_set_output_level(RED_LED, gu8Red);
}

static inline void turnOffLED(void)
{
	gu8OnOffState = 0;
	port_pin_set_output_level(BLUE_LED, 1);
	port_pin_set_output_level(GREEN_LED, 1);
	port_pin_set_output_level(RED_LED, 1);
}

void initialise_led(void)
{
	gu8Blue = 0;
	gu8Green = 1;
	gu8Red = 1;
	gu8Color = LED_COLOR_BLUE;
	
	/* led port pin initialization */
	struct port_config config_port_pin;
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(BLUE_LED, &config_port_pin);
	port_pin_set_config(GREEN_LED, &config_port_pin);
	port_pin_set_config(RED_LED, &config_port_pin);
	port_pin_set_output_level(BLUE_LED, gu8Blue);
	port_pin_set_output_level(GREEN_LED, gu8Green);
	port_pin_set_output_level(RED_LED, gu8Red);
}


Led_Color led_ctrl_get_color()
{
	return gu8Color;
}


void led_ctrl_set_color(Led_Color color, Led_Mode mode)
{
	switch(color)
	{
		case LED_COLOR_BLUE:
			gu8Blue = 0;
			gu8Green = 1;
			gu8Red = 1;
			gu8Color = LED_COLOR_BLUE;
			break;
		case LED_COLOR_GREEN:
			gu8Blue = 1;
			gu8Green = 0;
			gu8Red = 1;
			gu8Color = LED_COLOR_GREEN;
			break;
		case LED_COLOR_RED:
			gu8Blue = 1;
			gu8Green = 1;
			gu8Red = 0;
			gu8Color = LED_COLOR_RED;
			break;
		case LED_COLOR_YELLOW:
			gu8Blue = 1;
			gu8Green = 0;
			gu8Red = 0;
			gu8Color = LED_COLOR_YELLOW;
			break;
		case LED_COLOR_Magneta:
			gu8Blue = 0;
			gu8Green = 1;
			gu8Red = 0;
			gu8Color = LED_COLOR_Magneta;
			break;
		case LED_COLOR_Cyan:
			gu8Blue = 0;
			gu8Green = 0;
			gu8Red = 1;
			gu8Color = LED_COLOR_Cyan;
			break;
		case LED_COLOR_WHTIE:
			gu8Blue = 1;
			gu8Green = 1;
			gu8Red = 1;
			gu8Color = LED_COLOR_WHTIE;
			break;
			
		default:
			break;
		
	}
	
	if (mode == LED_MODE_NONE)
	{
		if (gu8OnOffState == 1)
			turnOnLED();	// color change take effect
		return;
	}
	
	gu8LedMode = mode;
	
}

void led_ctrl_set_mode(Led_Mode mode)
{
	gu8LedMode = mode;
	switch (mode)
	{
		case LED_MODE_TURN_OFF:
			turnOffLED();
			break;
		case LED_MODE_TURN_ON:
			turnOnLED();
			break;
		
		default:
			break;
	}

}

void led_ctrl_execute()
{
	switch (gu8LedMode)
	{
		case LED_MODE_TURN_OFF:
			turnOffLED();
			break;
		case LED_MODE_TURN_ON:
			turnOnLED();
			break;
		
		case LED_MODE_BLINK_NORMAL:
			if (tick_500ms)
			{
				tick_500ms = 0;
				if (gu8OnOffState == 1)
					turnOffLED();
				else
					turnOnLED();
			}
			break;
		case LED_MODE_BLINK_FAST:
			if (tick_100ms)
			{
				tick_100ms = 0;
				if (gu8OnOffState == 1)
					turnOffLED();
				else
					turnOnLED();
			}
			break;
		
		default:
			break;
	}
}