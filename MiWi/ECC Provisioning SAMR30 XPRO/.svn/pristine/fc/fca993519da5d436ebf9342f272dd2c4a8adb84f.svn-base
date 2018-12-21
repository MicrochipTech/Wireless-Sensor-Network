/**
 * \file
 *
 * \brief adc file
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
#include "adc_measure.h"

/* macros */
#define ADC_SAMPLES           10
#define ADC_REF_mV            1900
#define VBAT_THRESHOLD_mV     2500 //2.5V
#define ADC_2POWER_12BIT_RES  4096

/* variables */
uint16_t adc_result_buffer[ADC_SAMPLES]; 
uint16_t adc_avg_result = 0; 
uint16_t adc_result_mV = 0;
struct adc_module adc_instance;
volatile bool adc_window_hit = false; 
volatile bool low_battery_status = false;

/* prototypes */
void adc_window_hit_callback(struct adc_module *const module);

/*! \brief adc window callback
 *
 */
void adc_window_hit_callback(struct adc_module *const module)
{
	uint16_t adc_result_hex = 0;
	adc_result_hex = ADC->RESULT.reg;
	low_battery_status = true;
	adc_result_mV = (ADC_REF_mV * (adc_result_hex - 0x82) / (ADC_2POWER_12BIT_RES));
}

/*! \brief configure adc
 *
 */
void configure_adc(void)
{
	uint16_t adc_threshold = 0;
	
	struct adc_config config_adc;
	adc_get_config_defaults(&config_adc);
	
	//calculate threshold value
	adc_threshold = (VBAT_THRESHOLD_mV * ADC_2POWER_12BIT_RES) / ADC_REF_mV;
	adc_threshold /= 2;

	config_adc.clock_source					= GCLK_GENERATOR_1;
	config_adc.clock_prescaler				= ADC_CLOCK_PRESCALER_DIV16;
	config_adc.reference					= ADC_REFERENCE_INTVCC2;
	config_adc.positive_input				= ADC_POSITIVE_INPUT_PIN0;
	config_adc.resolution					= ADC_RESOLUTION_CUSTOM;
	config_adc.window.window_mode			= ADC_WINDOW_MODE_BELOW_UPPER;
	config_adc.window.window_upper_value	= adc_threshold + 0x82;//0x8D0;
	config_adc.accumulate_samples			= ADC_ACCUMULATE_SAMPLES_512;
	config_adc.divide_result				= ADC_DIVIDE_RESULT_16;

	adc_init(&adc_instance, ADC, &config_adc);
	adc_enable(&adc_instance);
	adc_register_callback(&adc_instance,adc_window_hit_callback, ADC_CALLBACK_WINDOW);
	adc_enable_callback(&adc_instance, ADC_CALLBACK_WINDOW);
}

/**
* \@brief Enable ADC peripheral
*/
void enable_adc(void)
{
	/* Enable Power Manager clock */
	system_apb_clock_set_mask(SYSTEM_CLOCK_APB_APBD, 1 << 0x03);
	
	/* Enable GCLK channel */
	system_gclk_chan_enable(0x1E);
	
	/* Enable ADC peripheral */
	adc_enable(&adc_instance);
}

/**
* \@brief Disable ADC peripheral
*/
void disable_adc(void)
{
	/* Disable Power Manager clock */
	adc_disable(&adc_instance);
	
	/* Disable GCLK channel */
	system_apb_clock_clear_mask(SYSTEM_CLOCK_APB_APBD, 1 << 0x03);
	
	/* Disable GCLK channel */
	system_gclk_chan_disable(0x1E);
}

/**
* \@brief read adc for battery voltage check
*/
void read_battery_voltage(void)
{
	enable_adc();
	adc_start_conversion(&adc_instance);
	disable_adc();
}