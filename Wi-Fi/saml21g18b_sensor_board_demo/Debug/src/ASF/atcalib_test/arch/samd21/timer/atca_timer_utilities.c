/** \file
 *  \brief Timer Utility Functions
 *  \author Atmel Crypto Products
 *  \date June 20, 2013
 * \copyright Copyright (c) 2014 Atmel Corporation. All rights reserved.
 *
 * \atmel_crypto_device_library_license_start
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
 *    Atmel integrated circuit.
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
 * \atmel_crypto_device_library_license_stop
 */

#include <stdint.h>                           // data type definitions
#include "arch/common/atca_timer_utilities.h"
#include "asf.h"

/** \defgroup crypto_device_timer_utilities Module 09: Timers
 *
 * This module implements timers used during communication.
 * They are implemented using loop counters. But if you have hardware
 * timers available, you can implement the functions using them.
@{ */

//! Fill the inner loop of delay_10us() with these CPU instructions to achieve 10 us per iteration.
#   define   TIME_UTILS_US_CALIBRATION           //__asm__ volatile ("\n\tnop\n\tnop\n\tnop\n")

/** Decrement the inner loop of delay_10us() this many times to achieve 10 us per
 *  iteration of the outer loop.
 */
#   define   TIME_UTILS_LOOP_COUNT            ((uint8_t)  14)

//! The delay_ms function calls delay_10us with this parameter.
#   define   TIME_UTILS_MS_CALIBRATION        ((uint8_t) 104)

void atca_delay_us(uint32_t delay)
{
	// Loop delay
	volatile uint32_t delay_1us;
	int32_t loop_count = TIME_UTILS_LOOP_COUNT/10;

	for (; delay > 0; delay--) {
		for (delay_1us = loop_count; delay_1us > 0; delay_1us--)
		;
		TIME_UTILS_US_CALIBRATION;
	}
}

/** \brief This function delays for a number of tens of microseconds.
 *
 * \param[in] delay number of 0.01 milliseconds to delay
 */
void atca_delay_10us(uint32_t delay)
{
	// Loop delay
	volatile uint32_t delay_10us;

	for (; delay > 0; delay--) {
		for (delay_10us = TIME_UTILS_LOOP_COUNT; delay_10us > 0; delay_10us--)
		;
		TIME_UTILS_US_CALIBRATION;
	}
	
	// use ASF supplied delay
//	delay_us( delay * 10 );
	
}


/** \brief This function delays for a number of milliseconds.
 *
 *         You can override this function if you like to do
 *         something else in your system while delaying.
 * \param[in] delay number of milliseconds to delay
 */

/* ASF already has delay_ms - see delay.h */
void atca_delay_ms(uint32_t delay)
{
	// Loop delay
	//uint32_t i;
	//for (i = delay; i > 0; i--)
	//atca_delay_10us(TIME_UTILS_MS_CALIBRATION);

	// use ASF supplied delay
	delay_ms(delay);
}

/** @} */
