/** \file
 * \brief generic API adapter from upper levels to lower-level protocols. 
 * each wire protocol like I2C, SWI, or Atmel PHY will have an implementation of these
 *
 *  \author Atmel Crypto Products
 *
 *  \copyright Copyright (c) 2014 Atmel Corporation. All rights reserved.
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

#ifndef ATCA_PROTOCOL_ADAPTER
#define ATCA_PROTOCOL_ADAPTER

#include <stdint.h>
#include "stddef.h"

uint8_t atca_init(void);
uint8_t atca_sleep(void);
uint8_t atca_wakeup(uint8_t *response);
uint8_t atca_wake(void);
uint8_t atca_idle(void);
uint8_t atca_send_command(uint8_t count, uint8_t *command);
uint8_t atca_receive_response(uint8_t size, uint8_t *response);
uint8_t atca_resync(uint8_t size, uint8_t *response);
uint8_t atca_aes132_is_locked(void);
uint8_t atca_aes132_block_read_data(uint32_t page_address, size_t count, uint8_t* data);
uint8_t atca_aes132_read_memory(uint32_t address, size_t count, uint8_t* data);
uint8_t atca_aes132_write_memory(uint32_t address, size_t count, const uint8_t* data);
uint8_t atca_reset_io(void);

#endif /* ATCA_PROTOCOL_ADAPTER */