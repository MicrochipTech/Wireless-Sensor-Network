/** \file
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

#ifndef ATCA_KIT_H_
#define ATCA_KIT_H_

#include <stdint.h>

// Error codes for physical hardware dependent module
//#define KIT_SUCCESS     ((uint8_t) 0x00) //!< Communication with device succeeded.
//#define KIT_COMM_FAIL   ((uint8_t) 0xF0) //!< Communication with device failed.
//#define KIT_TIMEOUT     ((uint8_t) 0xF1) //!< Communication timed out.

// Preserve function naming when compiling in C++
#ifdef __cplusplus
extern "C" {
#endif

// Function prototypes to be implemented in the target kit_phys.c
uint8_t kit_enable(void);
uint8_t kit_disable(void);
uint8_t kit_num_kits_found(int8_t* num_kits);
uint8_t kit_select_kit(int8_t kit_id);
uint8_t kit_num_devices_found(int8_t* num_devices);
uint8_t kit_select_device(int8_t device_id);

uint8_t kit_get_version(char* version_str, uint16_t version_str_size);
uint8_t kit_get_device_info(char* info, uint16_t info_size);
uint8_t kit_send_bytes(uint8_t *data, uint16_t data_len);
uint8_t kit_receive_bytes(uint8_t *rcv, uint16_t* rcv_len);
uint8_t kit_talk(uint8_t *data, uint16_t data_len, uint8_t* rcv, uint16_t* rcv_len);
uint8_t kit_talk_cmd(uint8_t *cmd, uint16_t cmd_len, uint8_t* rcv, uint16_t* rcv_len);
uint8_t kit_send_wake(void);
uint8_t kit_send_idle(void);
uint8_t kit_send_sleep(void);

#ifdef __cplusplus
}
#endif

#endif /* ATCA_KIT_H_ */
