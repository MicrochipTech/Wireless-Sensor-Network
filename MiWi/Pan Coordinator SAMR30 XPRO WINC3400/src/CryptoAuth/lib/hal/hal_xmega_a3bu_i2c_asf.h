/**
 * \file
 * \brief ATCA Hardware abstraction layer for XMEGA-A3BU I2C over ASF drivers.
 *
 * Prerequisite: add I2C Master Polled support to application in Atmel Studio
 *
 * \copyright Copyright (c) 2017 Microchip Technology Inc. and its subsidiaries (Microchip). All rights reserved.
 *
 * \page License
 *
 * You are permitted to use this software and its derivatives with Microchip
 * products. Redistribution and use in source and binary forms, with or without
 * modification, is permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Microchip may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with a
 *    Microchip integrated circuit.
 *
 * THIS SOFTWARE IS PROVIDED BY MICROCHIP "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL MICROCHIP BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef HAL_XMEGA_A3BU_I2C_ASF_H_
#define HAL_XMEGA_A3BU_I2C_ASF_H_

#include <asf.h>
#include "twi_master.h"

/** \defgroup hal_ Hardware abstraction layer (hal_)
 *
 * \brief
 * These methods define the hardware abstraction layer for communicating with a CryptoAuth device
 * using I2C driver of ASF.
 *
   @{ */


#define MAX_I2C_BUSES    4   // XMEGA A3BU has up to 4 PORT that can be configured as I2C

/** \brief this is the hal_data for ATCA HAL created using ASF
 */
typedef struct atcaI2Cmaster
{
    twi_master_t i2c_master_instance;
    int          ref_ct;
    // for conveniences during interface release phase
    int bus_index;
} ATCAI2CMaster_t;

void change_i2c_speed(ATCAIface iface, uint32_t speed);

/** @} */
#endif /* HAL_XMEGA_A3BU_I2C_ASF_H_ */