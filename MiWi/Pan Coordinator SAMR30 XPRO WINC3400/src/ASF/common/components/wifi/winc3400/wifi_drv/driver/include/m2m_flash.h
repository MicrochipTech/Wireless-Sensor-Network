/**
 *
 * \file
 *
 * \brief WINC3400 Flash Interface.
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

/**@defgroup FLASHAPI FLASH
*/

#ifndef __M2M_FLASH_H__
#define __M2M_FLASH_H__

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
INCLUDES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#include "common/include/nm_common.h"
#include "driver/include/m2m_types.h"
#include <stdint.h>
/**@defgroup  FlashDefs Defines
 * @ingroup FLASHAPI
 * @{*/

/*!	Size of internal buffer: 4096 bytes. */
#define FA_SECTOR_SIZE						FLASH_SECTOR_SZ

/*!	Bit 0 of u8Flags parameter of @ref tpfFlashAccessCtlFn.\n
 *	Compare buffer against existing data, to avoid unnecessary operation. */
#define FA_FN_FLAGS_COMPARE_BEFORE			NBIT0
/*!	Bit 1 of u8Flags parameter of @ref tpfFlashAccessCtlFn.\n
 *	Fill uninitialized portion of buffer with existing data to avoid losing it in subsequent erase.
 *	Can be ignored when the buffer received is entirely initialized.
 *	Typically not set unless FA_FN_FLAGS_ERASE is set. */
#define FA_FN_FLAGS_READ_SURROUNDING		NBIT1
/*!	Bit 2 of u8Flags parameter of @ref tpfFlashAccessCtlFn.\n
 *	Save buffer to a backup persistent location in case of power loss after subsequent erase.
 *	A (persistent) record of the backup status must also be kept.
 *	Can be ignored when the buffer received is entirely initialized.
 *	Typically not set unless FA_FN_FLAGS_READ_SURROUNDING and FA_FN_FLAGS_ERASE are both set. */
#define FA_FN_FLAGS_BACKUP					NBIT2
/*!	Bit 3 of u8Flags parameter of @ref tpfFlashAccessCtlFn.\n
 *	Erase existing data before writing. */
#define FA_FN_FLAGS_ERASE					NBIT3
/*!	Bit 4 of u8Flags parameter of @ref tpfFlashAccessCtlFn.\n
 *	Write buffer. */
#define FA_FN_FLAGS_WRITE					NBIT4
/*!	Bit 5 of u8Flags parameter of @ref tpfFlashAccessCtlFn.\n
 *	Compare buffer against written data, to provide verification.
 *	Typically not set unless FA_FN_FLAGS_WRITE is set. */
#define FA_FN_FLAGS_COMPARE_AFTER			NBIT5
/*!	Bit 6 of u8Flags parameter of @ref tpfFlashAccessCtlFn.\n
 *	Read data to buffer. Typically this would be the only flag set, meaning read the existing data.
 *	However, if other flags are set, the read should be performed at the end. */
#define FA_FN_FLAGS_READ					NBIT6

/*!	Bit 0 of u8AccessOptions parameter of @ref m2m_flash_access_item and
 *	@ref m2m_flash_access_image.\n
 *	Request to erase existing data before writing.\n
 *	Only applies when module is providing data to the MCU application.\n
 *	@ref FA_FN_FLAGS_ERASE will be set in subsequent call to function of type
 *	@ref tpfFlashAccessCtlFn. */
#define FA_ACCESS_OPTION_ERASE_FIRST		NBIT0
/*!	Bit 1 of u8AccessOptions parameter of @ref m2m_flash_access_item and
 *	@ref m2m_flash_access_image.\n
 *	When set with @ref FA_ACCESS_OPTION_ERASE_FIRST, this is a request to do
 *	read-modify-erase-write (eg if MCU application is storing received data in flash).\n
 *	Only applies when module is providing data to the MCU application.\n
 *	@ref FA_FN_FLAGS_READ_SURROUNDING will be set in subsequent call to function of type
 *	@ref tpfFlashAccessCtlFn. */
#define FA_ACCESS_OPTION_KEEP_SURROUNDING	NBIT1
/*!	Bit 2 of u8AccessOptions parameter of @ref m2m_flash_access_item and
 *	@ref m2m_flash_access_image.\n
 *	When set with @ref FA_ACCESS_OPTION_ERASE_FIRST and @ref FA_ACCESS_OPTION_KEEP_SURROUNDING,
 *	this is a request to keep a persistent backup of modified contents during read-modify-erase-write.\n
 *	Only applies when module is providing data to the MCU application.\n
 *	@ref FA_FN_FLAGS_BACKUP will be set in subsequent call to function of type
 *	@ref tpfFlashAccessCtlFn. */
#define FA_ACCESS_OPTION_USE_BACKUP			NBIT2
/*!	Bit 3 of u8AccessOptions parameter of @ref m2m_flash_access_item and
 *	@ref m2m_flash_access_image.\n
 *	Request to compare new data against existing data before erasing/writing, to avoid unnecessary operations.\n
 *	Applies to data transfer in either direction.\n
 *	@ref FA_FN_FLAGS_COMPARE_BEFORE will be set in subsequent call to function of type
 *	@ref tpfFlashAccessCtlFn. */
#define FA_ACCESS_OPTION_COMPARE_BEFORE		NBIT3
/*!	Bit 4 of u8AccessOptions parameter of @ref m2m_flash_access_item and
 *	@ref m2m_flash_access_image.\n
 *	Request for byte-wise verification of write.\n
 *	Applies to data transfer in either direction.\n
 *	@ref FA_FN_FLAGS_COMPARE_AFTER will be set in subsequent call to function of type
 *	@ref tpfFlashAccessCtlFn. */
#define FA_ACCESS_OPTION_COMPARE_AFTER		NBIT4
/*!	When modifying WINC flash contents, the module determines most options internally. Only two
 *	options are taken from the u8AccessOptions parameter.\n
 *	When providing data to the MCU application, the module takes all options from the
 *	u8AccessOptions parameter. */
#define FA_ACCESS_WINC_MASK					(FA_ACCESS_OPTION_COMPARE_BEFORE | FA_ACCESS_OPTION_COMPARE_AFTER)

/*!	Bit 0 of u8ModeOptions parameter of @ref m2m_flash_access_image.\n
 *	Request to write new firmware image to WINC inactive partition. */
#define FA_UPDATEIMAGE_OPTION_UPDATE		NBIT0
/*!	Bit 1 of u8ModeOptions parameter of @ref m2m_flash_access_image.\n
 *	Request to mark image in WINC inactive partition as valid. If set along with
 *	@ref FA_UPDATEIMAGE_OPTION_UPDATE, the image is not marked valid if the writing fails. */
#define FA_UPDATEIMAGE_OPTION_VALIDATE		NBIT1
/*!	Bit 2 of u8ModeOptions parameter of @ref m2m_flash_access_image.\n
 *	Request to switch the WINC active/inactive partitions. Use this to switch to a new image or to
 *	revert to an old one. The switch only succeeds if the image has been marked valid. */
#define FA_UPDATEIMAGE_OPTION_SWITCH		NBIT2

/*!	Unused. */
#define FA_RETURN_IDLE						0
/*!	Only used internally. */
#define FA_RETURN_OK						0

/*!	Bit 15 of @ref FlashAccessErr_t return code.\n
 *	Indicates that the attempted module operation did not succeed.\n
 *	Bits 1-14 may give further indication of the cause of failure and the resulting state. */
#define FA_RETURN_FLAG_ERR					NBIT15
/*!	Bit 14 of @ref FlashAccessErr_t return code.\n
 *	The parameters provided by the MCU application failed sanity checks. */
#define FA_RETURN_FLAG_ERR_PARAM			NBIT14
/*!	Bit 13 of @ref FlashAccessErr_t return code.\n
 *	A function of type @ref tpfFlashAccessCtlFn or @ref tpfFlashAccessFn returned failure. */
#define FA_RETURN_FLAG_ERR_LOCAL_ACCESS		NBIT13
/*!	Bit 12 of @ref FlashAccessErr_t return code.\n
 *	There was an error while accessing WINC flash, or contents of WINC flash were not as expected. */
#define FA_RETURN_FLAG_ERR_WINC_ACCESS		NBIT12
/*!	Bit 11 of @ref FlashAccessErr_t return code.\n
 *	The transfer destination location was too small for the size of data to be transferred. */
#define FA_RETURN_FLAG_ERR_WINC_SZ			NBIT11
/*!	Bit 10 of @ref FlashAccessErr_t return code.\n
 *	The TLS root certificate identifier provided by the MCU application caused a failure. Either:\n
 *	- A certificate could not be added to the WINC flash store because the identifier matched an existing certificate.\n
 *	- A certificate could not be read or removed from the WINC flash store because the identifier did not match an existing certificate. */
#define FA_RETURN_FLAG_ERR_WINC_ITEM		NBIT10
/*!	Bit 9 of @ref FlashAccessErr_t return code.\n
 *	The MCU restarted during an attempted transfer, so the transfer is incomplete. This bit would
 *	be seen in the return code parameter of @ref m2m_flash_access_init. */
#define FA_RETURN_FLAG_ERR_INTERRUPTED		NBIT9
/*!	Bit 4 of @ref FlashAccessErr_t return code.\n
 *	Indicates that the MCU application should check the status of the backup store which may have
 *	been used by a function of type @ref tpfFlashAccessFn.\n
 *	This bit would be seen in the return code parameter of @ref m2m_flash_access_init if there was
 *	a transfer in progress with @ref FA_ACCESS_OPTION_USE_BACKUP. */
#define FA_RETURN_FLAG_INFO_CHECK_BACKUP	NBIT4
/*!	Bit 3 of @ref FlashAccessErr_t return code.\n
 *	Indicates that the WINC has been reset since the start of the transfer. If set, the MCU
 *	application may call APIs in this module, but should not call other WINC APIs until it has run
 *	all WINC initialization code, or a system reset.\n
 *	This bit is always set when @ref m2m_flash_access_init or @ref m2m_flash_access_retry is
 *	called. Otherwise it is only set if the most recent transfer caused reset of WINC.
 *	Applies to both complete and failed transfers. */
#define FA_RETURN_FLAG_INFO_WINC_RESET		NBIT3
/*!	Bit 2 of @ref FlashAccessErr_t return code.\n
 *	Indicates that the transfer parameters have been saved within the module. Hence the MCU
 *	application may call @ref m2m_flash_access_retry to retry a failed transfer.\n
 *	Applies to both complete and failed transfers, but @ref m2m_flash_access_retry cannot be used
 *	after a completed transfer. */
#define FA_RETURN_FLAG_INFO_SAVED			NBIT2
/*!	Bit 1 of @ref FlashAccessErr_t return code.\n
 *	Indicates that the transfer destination contents have changed.\n
 *	Applies to both complete and failed transfers. */
#define FA_RETURN_FLAG_INFO_CHANGED			NBIT1
/*!	Bit 0 of @ref FlashAccessErr_t return code.\n
 *	Indicates that the transfer has completed.\n
 *	Bits 1-3 may give further indication of the resulting state. */
#define FA_RETURN_FLAG_COMPLETE				NBIT0

 /**@}
 */
/**@defgroup  FlashEnums Enumeration/Typedefs
 * @ingroup FLASHAPI
 * @{*/

/*!
@brief	Return code for module operations. It should be interpreted as a bitfield. If cast as a @ref sint16, negative values indicate errors.

@see	@ref FA_RETURN_FLAG_ERR
@see	@ref FA_RETURN_FLAG_ERR_PARAM
@see	@ref FA_RETURN_FLAG_ERR_LOCAL_ACCESS
@see	@ref FA_RETURN_FLAG_ERR_WINC_ACCESS
@see	@ref FA_RETURN_FLAG_ERR_WINC_SZ
@see	@ref FA_RETURN_FLAG_ERR_WINC_ITEM
@see	@ref FA_RETURN_FLAG_ERR_INTERRUPTED
@see	@ref FA_RETURN_FLAG_INFO_CHECK_BACKUP
@see	@ref FA_RETURN_FLAG_INFO_WINC_RESET
@see	@ref FA_RETURN_FLAG_INFO_SAVED
@see	@ref FA_RETURN_FLAG_INFO_CHANGED
@see	@ref FA_RETURN_FLAG_COMPLETE
 */
typedef uint16 FlashAccessErr_t;

/*!
@enum	tenuFlashAccessFnCtl

@brief	Control parameter for @ref tpfFlashAccessCtlFn.
 */
typedef enum {
	/*!	Data access about to start. Check and save parameters as required, ready for sequential
	 *	data accesses. */
	FA_FN_CTL_INITIALIZE,
	/*!	Data access aborted. Did not complete and will not be continued. */
	FA_FN_CTL_TERMINATE,
	/*!	Data access complete. */
	FA_FN_CTL_COMPLETE,
	/*!	Check record of backup status and restore the backed-up data if appropriate. */
	FA_FN_CTL_CHECK_BACKUP
}tenuFlashAccessFnCtl;

/*!
@enum	tenuFlashAccessMode

@brief	Transfer modes available.
@note	In addition to these modes, writing/validating/switching WINC firmware image is available
		via @ref m2m_flash_access_image.
@see	m2m_flash_access_image
@see	m2m_flash_access_item
@see	m2m_flash_access_item_remove
 */
typedef enum {
	/*!	Unused. */
	FA_NONE,
	/*!	Unused. */
	FA_WRITE_IMAGE_UPDATE,
	/*!	Unused. */
	FA_WRITE_IMAGE_OVERWRITE,
	/*!	Recover space in the TLS root certificate flash store.\n
	 *	Certificate removal results in dead space. This mode recovers the dead space. */
	FA_PRUNE_ROOTCERT,
	/*!	Unused. */
	FA_PRUNE_LOCALCERT,
	/*!	Add an entry to the TLS root certificate flash store. */
	FA_ADD_ROOTCERT,
	/*!	Unused. */
	FA_ADD_LOCALCERT,
	/*!	Remove an entry from the TLS root certificate flash store.
	 *	@warning	This mode fails when used with @ref m2m_flash_access_item. Use dedicated API
	 *	@ref m2m_flash_access_item_remove instead. */
	FA_REMOVE_ROOTCERT,
	/*!	Unused. */
	FA_REMOVE_LOCALCERT,
	/*!	Unused. */
	FA_READ_IMAGE_UPDATE,
	/*!	Read an entry from the TLS root certificate flash store, using an identifier. */
	FA_READ_ROOTCERT,
	/*!	Unused. */
	FA_READ_LOCALCERT,
	/*!	Read an entry from the TLS root certificate flash store, using an index. */
	FA_READIDX_ROOTCERT,
	/*!	Unused. */
	FA_READIDX_LOCALCERT
}tenuFlashAccessMode;

/*!
@typedef \
	tpfFlashAccessCtlFn

@brief
	A function of this type is used to control local data access (read, erase or write).

@param [in]		u32LocationId
					Identifier for data location.\n
					Parameter is ignored if enuCtl is @ref FA_FN_CTL_CHECK_BACKUP.
@param [in]		u32TotalSize
					Total size of data to be accessed in subsequent data access.\n
					Parameter is valid only if enuCtl is @ref FA_FN_CTL_INITIALIZE.
@param [in]		u8Flags
					Flags indicating type of subsequent data access.\n
					Parameter is valid only if enuCtl is @ref FA_FN_CTL_INITIALIZE.
@param [in]		enuCtl
					Control parameter.

@see	tpfFlashAccessFn
@see	tstrFlashAccessInfo
@see	tenuFlashAccessFnCtl
@see	FA_FN_FLAGS_COMPARE_BEFORE
@see	FA_FN_FLAGS_READ_SURROUNDING
@see	FA_FN_FLAGS_BACKUP
@see	FA_FN_FLAGS_ERASE
@see	FA_FN_FLAGS_WRITE
@see	FA_FN_FLAGS_COMPARE_AFTER
@see	FA_FN_FLAGS_READ

@return			Zero for success. Non-zero otherwise.
*/
typedef sint8 (*tpfFlashAccessCtlFn) (uint32 u32LocationId, uint32 u32TotalSize, uint8 u8Flags, tenuFlashAccessFnCtl enuCtl);
/*!
@typedef \
	tpfFlashAccessFn

@brief
	A function of this type is used to access local data (read, erase or write), according to the
	parameters contained in most recent call to the corresponding function of type
	@ref tpfFlashAccessCtlFn.\n
	Multiple calls to this function access data sequentially, until there is another call to the
	corresponding function of type @ref tpfFlashAccessCtlFn.

@note
	Correspondence between instances of @ref tpfFlashAccessCtlFn and @ref tpfFlashAccessFn can be
	tracked via an instance of @ref tstrFlashAccessInfo.

@param [in]		pu8Buf
					Buffer to be written to or read from.
@param [in]		u32BufSize
					Total size of the buffer.
@param [in]		u32DataOffset
					Offset of data within the buffer.
@param [in]		u32DataSize
					Size of data to be written or read.

@warning
	The data write/read does not necessarily coincide with pu8Buf. Refer to u32DataOffset.

@see	tpfFlashAccessCtlFn
@see	tstrFlashAccessInfo

@return			Zero for success. Non-zero otherwise.
*/
typedef sint8 (*tpfFlashAccessFn) (uint8_t *pu8Buf, uint32_t u32BufSize, uint32_t u32DataOffset, uint32_t u32DataSize);

/*!
@struct	\
	tstrFlashAccessInfo
@brief
	This structure contains information required for data access (read, erase or write).
	It is passed to the module via @ref m2m_flash_access_item or @ref m2m_flash_access_image.
@see	m2m_flash_access_item
@see	m2m_flash_access_image
 */
typedef struct {
	/*!	Function to indicate beginning or end of data access. May be NULL only if u32LocationSize is 0. */
	tpfFlashAccessCtlFn		pfCtlFunction;
	/*!	Function to carry data. May be NULL only if u32LocationSize is 0. */
	tpfFlashAccessFn		pfFunction;
	/*!	Data location identifier. This will be used in calls to pfCtlFunction. */
	uint32					u32LocationId;
	/*!	Data size available. The actual size accessed may be smaller. */
	uint32					u32LocationSize;
	/*!	Block size, to assist with alignment. Typically set this to:\n
	 *	- @ref FA_SECTOR_SIZE if accessing flash memory with erase block size <= @ref FA_SECTOR_SIZE.\n
	 *	- 0 if accessing normal memory. */
	uint32					u32AlignmentSize;
	/*!	Offset of start, relative to u32AlignmentSize. Field is ignored if u32AlignmentSize < 2. */
	uint32					u32StartAlignment;
}tstrFlashAccessInfo;

/*!
@struct	\
	tstrFlashAccessReturn
@brief
	This structure contains information about an attempted transfer.
@see	m2m_flash_access_init
 */
typedef struct {
	/*!	Return code of type @ref FlashAccessErr_t. */
	uint16	u16Ret;
	/*!	Transfer identifier provided by MCU application. */
	uint16	u16AppId;
}tstrFlashAccessReturn;

/**@}
*/

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
FUNCTION PROTOTYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
/** \defgroup FLASHFUNCTIONS Functions
*  @ingroup FLASHAPI
*/

/**@{*/
/*!
@fn	\
	FlashAccessErr_t m2m_flash_access_image(uint8 u8ModeOptions, uint8 u8AccessOptions, uint16 u16Id, tstrFlashAccessInfo *pstrSourceInfo);

@brief	Write/validate/switch a WINC firmware image.

@param [in]		u8ModeOptions
					Bitfield indicates the required combination of write / validate / switch.
@param [in]		u8AccessOptions
					Bitfield indicates whether the module should check each sector of the WINC flash image:\n
					- before erasing (in order to reduce unnecessary erase operations).\n
					- after writing (in order to verify successful write).\n
					Checking after writing is recommended.
@param [in]		u16Id
					Transfer identifier, not interpreted by this module.
@param [in]		pstrSourceInfo
					Information required by the module for obtaining the image from the MCU application.

@see	FA_UPDATEIMAGE_OPTION_UPDATE
@see	FA_UPDATEIMAGE_OPTION_VALIDATE
@see	FA_UPDATEIMAGE_OPTION_SWITCH
@see	FA_ACCESS_OPTION_COMPARE_BEFORE
@see	FA_ACCESS_OPTION_COMPARE_AFTER
@see	tstrFlashAccessInfo

@return			Bitfield. Bit 15 indicates failure. Bit 0 indicates success. See @ref FlashAccessErr_t for further interpretation.
*/
FlashAccessErr_t m2m_flash_access_image(uint8 u8ModeOptions, uint8 u8AccessOptions, uint16 u16Id, tstrFlashAccessInfo *pstrSourceInfo);
/*!
@fn	\
	FlashAccessErr_t m2m_flash_access_item(tenuFlashAccessMode enuMode, uint8 u8ModeOptions, uint8 u8AccessOptions, uint16 u16Id, tstrFlashAccessInfo *pstrInfo);

@brief	Add, read or remove an entry from the WINC TLS root certificate store.

@param [in]		enuMode
					Required mode:\n
					@ref	FA_PRUNE_ROOTCERT\n
					@ref	FA_ADD_ROOTCERT\n
					@ref	FA_REMOVE_ROOTCERT\n
					@ref	FA_READ_ROOTCERT\n
					@ref	FA_READIDX_ROOTCERT
@param [in]		u8ModeOptions
					In mode @ref FA_READIDX_ROOTCERT, this is the (0-based) index of the entry to read.\n
					Ignored in all other modes.
@param [in]		u8AccessOptions
					Bitfield indicating data access options to be used in the transfer.
@param [in]		u16Id
					Transfer identifier, not interpreted by this module.
@param [in]		pstrInfo
					Information required by the module for transferring the entry to/from the MCU application.\n
					Ignored in mode @ref FA_PRUNE_ROOTCERT\n

@note		Mode @ref FA_ADD_ROOTCERT requires that the data location pstrInfo->u32LocationId
			contains the entry to be added. The format of a TLS root certificate entry is
			@ref tstrRootCertEntryHeader followed by the certificate public key.
@note		Mode @ref FA_READ_ROOTCERT requires that the data location pstrInfo->u32LocationId
			already contains the entry identifier (i.e. the 20-byte SHA1 digest of certificate
			issuer). The module will read this identifier before searching for the required
			entry in WINC flash. If the entry identifier is not known, consider using
			@ref FA_READIDX_ROOTCERT instead.
@warning	Mode @ref FA_REMOVE_ROOTCERT fails when this function is used. Use dedicated API
			@ref m2m_flash_access_item_remove instead.

@see	tenuFlashAccessMode
@see	FA_ACCESS_OPTION_ERASE_FIRST
@see	FA_ACCESS_OPTION_KEEP_SURROUNDING
@see	FA_ACCESS_OPTION_USE_BACKUP
@see	FA_ACCESS_OPTION_COMPARE_BEFORE
@see	FA_ACCESS_OPTION_COMPARE_AFTER

@return			Bitfield. Bit 15 indicates failure. Bit 0 indicates success. See @ref FlashAccessErr_t for further interpretation.
*/
FlashAccessErr_t m2m_flash_access_item(tenuFlashAccessMode enuMode, uint8 u8ModeOptions, uint8 u8AccessOptions, uint16 u16Id, tstrFlashAccessInfo *pstrInfo);
/*!
@fn	\
	FlashAccessErr_t m2m_flash_access_item_remove(uint16 u16Id, void* pvItem, uint32 u32ItemLen);

@brief	Remove an entry from the WINC TLS root certificate store.

@param [in]		u16Id
					Transfer identifier, not interpreted by this module.
@param [in]		pvItem
					Pointer to buffer containing entry identifier (i.e. the 20-byte SHA1 digest of certificate issuer).
@param [in]		u32ItemLen
					Length of buffer in bytes.

@return			Bitfield. Bit 15 indicates failure. Bit 0 indicates success. See @ref FlashAccessErr_t for further interpretation.
*/
FlashAccessErr_t m2m_flash_access_item_remove(uint16 u16Id, void* pvItem, uint32 u32ItemLen);

/*!
@fn	\
	sint8 m2m_flash_access_init(tstrFlashAccessReturn *pstrRet);

@brief	Initialize the module.

@param [out]	pstrRet
					Information about the most recent transfer (whether complete or failed).

@warning	This API must be called at a particular point during MCU application initialization.

\section FlashAccessExample1 Example
This example demonstrates how to add @ref m2m_flash_access_init and @ref m2m_flash_access_reset
to MCU application initialization code.

Original code:
@code
{
	tstrWifiInitParam param = {...}
	sint8 status = 0;

	status = m2m_wifi_init(param);
	if (status != 0)
	{
		// Wifi init failed.
	}
}
@endcode
New code:
@code
{
	tstrWifiInitParam param = {...}
	sint8 status = 0;

	status = m2m_wifi_init_hold();
	if (status == 0)
	{
		tstrFlashAccessReturn	strFlashAccessReturn;

		m2m_flash_access_init(&strFlashAccessReturn);
		printf("FlashAccess:%d:%x\n", strFlashAccessReturn.u16Ret, strFlashAccessReturn.u16AppId);

		if (strFlashAccessReturn.u16Ret & FA_RETURN_FLAG_ERR)
		{
			switch (strFlashAccessReturn.u16AppId)
			{
				// Application code dependent on meaning of u16AppId.
			}
			m2m_flash_access_reset();
		}
		status = m2m_wifi_init_start(param);
	}
	if (status != 0)
	{
		// Wifi init failed.
	}
}
@endcode

@return		Zero for successful initialization of module. Negative value otherwise.
*/
sint8 m2m_flash_access_init(tstrFlashAccessReturn *pstrRet);
/*!
@fn	\
	sint8 m2m_flash_access_reset(void);

@brief	Reset the module, clearing history of previous transfer attempt.

@warning	This API must only be called at a time when the WINC has been reset. This could be
			during MCU initialization sequence (see @ref FlashAccessExample1). Or it could be after
			an attempted transfer in which @ref FA_RETURN_FLAG_INFO_WINC_RESET has been set.

@return		Zero for successful initialization of module. Negative value otherwise.
*/
sint8 m2m_flash_access_reset(void);
/*!
@fn	\
	FlashAccessErr_t m2m_flash_access_retry(void);

@brief	Retry the previous transfer attempt, if it failed to complete.

@warning	This API must only be called after an attempted transfer in which
			@ref FA_RETURN_FLAG_ERR, @ref FA_RETURN_FLAG_INFO_WINC_RESET and
			@ref FA_RETURN_FLAG_INFO_SAVED are all set.

@return		Bitfield. Bit 15 indicates failure. Bit 0 indicates success. See @ref FlashAccessErr_t for further interpretation.
*/
FlashAccessErr_t m2m_flash_access_retry(void);

 /**@}*/
#endif /* __M2M_FLASH_H__ */
