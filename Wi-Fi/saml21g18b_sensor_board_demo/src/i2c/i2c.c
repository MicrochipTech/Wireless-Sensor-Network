/**
 * \file
 *
 * \brief i2c file
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
#include "conf_sensor.h"
#include "i2c.h"



/*! \brief Configure I2C peripheral
 *
 */
void configure_sensor_i2c(void)
{
	/* Initialize config structure and software module */

	struct i2c_master_config config_i2c_master;
	i2c_master_get_config_defaults(&config_i2c_master);
	
	config_i2c_master.pinmux_pad0 = SENSOR_I2C_PINMUX_PAD0;
	config_i2c_master.pinmux_pad1 = SENSOR_I2C_PINMUX_PAD1;
	//config_i2c_master.generator_source = GCLK_GENERATOR_1;
	//config_i2c_master.generator_source = GCLK_GENERATOR_2;
	/* Initialize and enable device with config */
	while(i2c_master_init(&i2c_master_instance, SENSOR_I2C, &config_i2c_master)     \
	!= STATUS_OK);

	i2c_master_enable(&i2c_master_instance);
}

/*! \brief Enable I2C peripheral
 *
 */
void enable_i2c(void)
{
	/* Enable Power Manager clock */
	system_apb_clock_set_mask(SYSTEM_CLOCK_APB_APBC, 1 << 0x04);
	
	/* Enable GCLK channel */
	system_gclk_chan_enable(22);

	/* Enable i2c peripheral */
	i2c_master_enable(&i2c_master_instance);
}

/*! \brief Disable I2C peripheral
 *
 */
void disable_i2c(void)
{
	/* Disable Power Manager clock */
	i2c_master_disable(&i2c_master_instance);
	
	/* Disable GCLK channel */
	system_apb_clock_clear_mask(SYSTEM_CLOCK_APB_APBC, 1 << 0x04);
	
	/* Disable GCLK channel */
	system_gclk_chan_disable(22);
}
