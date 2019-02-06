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

/**@defgroup FLASHAPI Flash
*/

#ifndef __NM_FLASH_H__
#define __NM_FLASH_H__

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
INCLUDES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#include "common/include/nm_common.h"
#include "driver/include/m2m_types.h"
#include <stdint.h>
#include <stddef.h>

/**@defgroup  FlashDefs Defines
 * @ingroup FLASHAPI
 * @{*/

#define FA_SIGNATURE						HOST_CONTROL_FLASH_SIG

#define FA_MODE_FLAGS_CS					NBIT0
#define FA_MODE_FLAGS_CS_SWITCH				NBIT1
#define FA_MODE_FLAGS_CS_SWITCH_TARGET		NBIT2
#define FA_MODE_FLAGS_CS_VALIDATE_IMAGE		NBIT3
#define FA_MODE_FLAGS_UNCHANGED				NBIT4
#define FA_MODE_FLAGS_CHECK_BACKUP			NBIT5

#define ROOTCERT_SECTION_HEADER_SZ			20

 /**@}
 */
/**@defgroup  FlashEnums Enumeration/Typedefs
 * @ingroup FLASHAPI
 * @{*/

typedef enum {
	CS_INITIALIZE,
	CS_INVALIDATE_RB,
	CS_VALIDATE_RB,
	CS_SWITCH,
	CS_VALIDATE_SWITCH,
	CS_GET_ACTIVE,
	CS_GET_INACTIVE,
	CS_DEINITIALIZE
}tenuCSOp;

typedef enum {
	/*
	 *	Status values arranged so that status can be updated without any erase operation.
	 */
	FA_STATUS_EMPTY				= 0xFFFFFFFF,
	FA_STATUS_NOT_ACTIVE		= 0xFFFFFF00,
	FA_STATUS_ACTIVE			= 0xFFFF0000,
	FA_STATUS_DONE				= 0xFF000000,
	FA_STATUS_OLD				= 0x00000000
}tenuFlashAccessStatus;

typedef enum {
	/*
	 *	Special location ID values. If top bit is not set, the location ID is interpreted as flash address.
	 */
	MEM_ID_WINC_FLASH = 0x80000000,
	MEM_ID_WINC_APP_STORE,
	MEM_ID_WINC_ACTIVE,
	MEM_ID_WINC_INACTIVE
}tenuMemId;

typedef struct {
	uint32_t				u32Signature;
	tenuFlashAccessStatus	enuTransferStatus;
	uint16_t				u16AppId;
	uint8_t					u8AccessFlags;		// These correspond bitwise to application access options.
	uint8_t					u8ModeFlags;		// These are set internally, with reference to application mode options.
	tstrFlashAccessInfo		strDestinationInfo;
	tstrFlashAccessInfo		strSourceInfo;
	uint32_t				u32Size;
}tstrFlashAccess;
#define FA_SIG_SZ		4
#define FA_STA_OFS		4
#define FA_STA_SZ		4
#define FA_SIG_STA_SZ	(FA_SIG_SZ+FA_STA_SZ)
#define FA_PERM_SZ		(offsetof(tstrFlashAccess,strDestinationInfo))

typedef struct {
	tenuFlashAccessStatus	enuTransferStatus;
	uint32					u32Signature;
	uint32					u32DestinationAddr;
	uint32					u32SourceAddr;
	uint32					u32Size;
}tstrFlashAccessBackup;
#define FA_BACKUP_STA_SZ		4
#define FA_BACKUP_STORE_SZ		(2*sizeof(tstrFlashAccessBackup))
#define FA_BACKUP_STORE_OFFSET	(HOST_CONTROL_FLASH_OFFSET + HOST_CONTROL_FLASH_SZ - FA_BACKUP_STORE_SZ)

/**@}
*/

extern tstrFlashAccess gstrFlashAccess;

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
FUNCTION PROTOTYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
/** \defgroup FLASHFUNCTIONS Functions
*  @ingroup FLASHAPI
*/

/**@{*/
sint8 winc_flash_control_rd(uint32 u32LocationId, uint32 u32TotalSize, uint8 u8Flags, uint8 u8Ctl);
sint8 winc_flash_access_rd(uint8_t *pu8Buf, uint32_t u32BufSize, uint32_t u32DataOffset, uint32_t u32DataSize);
sint8 winc_flash_control(uint32 u32LocationId, uint32 u32TotalSize, uint8 u8Flags, uint8 u8Ctl);
sint8 winc_flash_access(uint8_t *pu8Buf, uint32_t u32BufSize, uint32_t u32DataOffset, uint32_t u32DataSize);
sint8 m2m_flash_access_image_get_target(uint8 *pu8Target);
sint8 m2m_flash_access_rootcert_get_size(tstrRootCertEntryHeader *pstrHdr, uint16 *pu16Size);
FlashAccessErr_t m2m_flash_access_rootcert(tenuFlashAccessMode enuMode, void *pvCertBuf, uint32 *pu32BufLen, uint32 *pu32Offset);
FlashAccessErr_t m2m_flash_access_run(void);


 /**@}*/
#endif /* __NM_FLASH_H__ */
