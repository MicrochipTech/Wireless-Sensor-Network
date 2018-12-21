/** \file
 *  \brief ATCA Library Return Code Definitions
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


#ifndef ATECC_LIB_RETURN_CODES_H
#   define ATECC_LIB_RETURN_CODES_H

#include <stddef.h>                 // data type definitions


/** \defgroup atecc_status Module 08: Library Return Codes
 *
@{ */

#define ATCA_SUCCESS              ((uint8_t)  0x00) //!< Function succeeded.
#define ATCA_CONFIG_ZONE_LOCKED	  ((uint8_t)  0x01)
#define ATCA_DATA_ZONE_LOCKED	  ((uint8_t)  0x02)
#define ATCA_WAKE_FAILED		  ((uint8_t)  0xD0) //!< response status byte indicates CheckMac failure (status byte = 0x01)
#define ATCA_CHECKMAC_FAILED		((uint8_t)  0xD1) //!< response status byte indicates CheckMac failure (status byte = 0x01)
#define ATCA_PARSE_ERROR          ((uint8_t)  0xD2) //!< response status byte indicates parsing error (status byte = 0x03)
#define ATCA_CMD_FAIL             ((uint8_t)  0xD3) //!< response status byte indicates command execution error (status byte = 0x0F)
#define ATCA_STATUS_CRC           ((uint8_t)  0xD4) //!< response status byte indicates CRC error (status byte = 0xFF)
#define ATCA_STATUS_UNKNOWN       ((uint8_t)  0xD5) //!< response status byte is unknown
#define ATCA_STATUS_ECC           ((uint8_t)  0xD6) //!< response status byte is ECC fault (status byte = 0x05)
#define ATCA_FUNC_FAIL            ((uint8_t)  0xE0) //!< Function could not execute due to incorrect condition / state.
#define ATCA_GEN_FAIL             ((uint8_t)  0xE1) //!< unspecified error
#define ATCA_BAD_PARAM            ((uint8_t)  0xE2) //!< bad argument (out of range, null pointer, etc.)
#define ATCA_INVALID_ID           ((uint8_t)  0xE3) //!< invalid device id, id not set
#define ATCA_INVALID_SIZE         ((uint8_t)  0xE4) //!< Count value is out of range or greater than buffer size.
#define ATCA_BAD_CRC              ((uint8_t)  0xE5) //!< incorrect CRC received
#define ATCA_RX_FAIL              ((uint8_t)  0xE6) //!< Timed out while waiting for response. Number of bytes received is > 0.
#define ATCA_RX_NO_RESPONSE       ((uint8_t)  0xE7) //!< Not an error while the Command layer is polling for a command response.
#define ATCA_RESYNC_WITH_WAKEUP   ((uint8_t)  0xE8) //!< Re-synchronization succeeded, but only after generating a Wake-up
#define ATCA_PARITY_ERROR         ((uint8_t)  0xE9) //!< for protocols needing parity 
#define ATCA_TX_TIMEOUT           ((uint8_t)  0xEA) //!< for Atmel PHY protocol, timeout on transmission waiting for master
#define ATCA_RX_TIMEOUT           ((uint8_t)  0xEB) //!< for Atmel PHY protocol, timeout on receipt waiting for master

#define ATCA_COMM_FAIL            ((uint8_t)  0xF0) //!< Communication with device failed. Same as in hardware dependent modules.
#define ATCA_TIMEOUT              ((uint8_t)  0xF1) //!< Timed out while waiting for response. Number of bytes received is 0.


#define ATCA_TBIT_TIMEOUT         2000   // in ms

/** @} */

#endif
