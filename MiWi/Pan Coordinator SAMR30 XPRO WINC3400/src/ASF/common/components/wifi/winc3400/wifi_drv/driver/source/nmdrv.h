/**
 *
 * \file
 *
 * \brief This module contains WINC3400 M2M driver APIs declarations.
 *
 * Copyright (c) 2017 Atmel Corporation. All rights reserved.
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

#ifndef _NMDRV_H_
#define _NMDRV_H_

#include "common/include/nm_common.h"

/**
*  @struct		tstrM2mRev
*  @brief		Structure holding firmware version parameters and build date/time
*/
typedef struct {
	uint16 u16FirmwareHifInfo; /* Fw HIF Info */
	uint8 u8FirmwareMajor; /* Version Major Number */
	uint8 u8FirmwareRsvd; /* Reserved */
	uint8 u8FirmwareMinor; /* Version Minor */
	uint8 u8FirmwarePatch; /* Patch Number */
	uint8 BuildDate[sizeof(__DATE__)]; // 12 bytes
	uint8 BuildTime[sizeof(__TIME__)]; // 9 bytes
} tstrM2mRev;

#ifdef __cplusplus
     extern "C" {
 #endif
/**
*	@fn		nm_get_hif_info(uint16 *pu16FwHifInfo, uint16 *pu16OtaHifInfo);
*	@brief	Get Hif info of images in both partitions (Firmware and Ota).
*	@param [out]	pu16FwHifInfo
*					Pointer holding Hif info of image in the active partition.
*	@param [out]	pu16OtaHifInfo
*					Pointer holding Hif info of image in the inactive partition.
*	@return	ZERO in case of success and Negative error code in case of failure
*/
sint8 nm_get_hif_info(uint16 *pu16FwHifInfo, uint16 *pu16OtaHifInfo);
/**
*	@fn		nm_get_firmware_full_info(tstrM2mRev* pstrRev)
*	@brief	Get Firmware version info
*	@param [out]	pstrRev
*					Pointer holds address of structure @ref tstrM2mRev that contains the version parameters
*					of image in the active partition.
*	@return	ZERO in case of success and Negative error code in case of failure
*/
sint8 nm_get_firmware_full_info(tstrM2mRev* pstrRev);
/**
*	@fn		nm_get_ota_firmware_info(tstrM2mRev* pstrRev)
*	@brief	Get Firmware version info
*	@param [out]	pstrRev
*					Pointer holds address of structure @ref tstrM2mRev that contains the version parameters
*					of image in the inactive partition.
*	@return	ZERO in case of success and Negative error code in case of failure
*/
sint8 nm_get_ota_firmware_info(tstrM2mRev* pstrRev);
/*
*	@fn		nm_drv_init
*	@brief	Initialize NMC1000 driver
*	@return	ZERO in case of success and Negative error code in case of failure
*/
sint8 nm_drv_init_download_mode(uint32 req_serial_number);

/*
*	@fn		nm_drv_init
*	@brief	Initialize NMC1000 driver
*	@return	M2M_SUCCESS in case of success and Negative error code in case of failure
*	@param [in]	arg
*				Generic argument TBD
*	@param [in]	req_serial_number
*	@return	ZERO in case of success and Negative error code in case of failure
*/
sint8 nm_drv_init(void * arg, uint32 req_serial_number);

/*
*	@fn		nm_drv_init_hold
*	@brief	First part of nm_drv_init, up to the point of initializing spi for flash access.
*	@see	nm_drv_init
*	@return	M2M_SUCCESS in case of success and Negative error code in case of failure
*	@param [in]	req_serial_number
*				Parameter inherited from nm_drv_init
*	@return	ZERO in case of success and Negative error code in case of failure
*/
sint8 nm_drv_init_hold(uint32 req_serial_number);

/*
*	@fn		nm_drv_init_start
*	@brief	Second part of nm_drv_init, continuing from where nm_drv_init_hold left off.
*	@see	nm_drv_init
*	@return	M2M_SUCCESS in case of success and Negative error code in case of failure
*	@param [in]	arg
*				Parameter inherited from nm_drv_init
*	@return	ZERO in case of success and Negative error code in case of failure
*/
sint8 nm_drv_init_start(void * arg);

/**
*	@fn		nm_drv_deinit
*	@brief	Deinitialize NMC1000 driver
*	@author	M. Abdelmawla
*   @param [in]	arg
*				Generic argument TBD
*	@return	ZERO in case of success and Negative error code in case of failure
*/
sint8 nm_drv_deinit(void * arg);

/**
*	@fn		nm_cpu_start(void)
*	@brief	Start CPU from the WINC module	
*	@return	ZERO in case of success and Negative error code in case of failure
*/

sint8 nm_cpu_start(void);

#ifdef __cplusplus
	 }
 #endif

#endif	/*_NMDRV_H_*/


