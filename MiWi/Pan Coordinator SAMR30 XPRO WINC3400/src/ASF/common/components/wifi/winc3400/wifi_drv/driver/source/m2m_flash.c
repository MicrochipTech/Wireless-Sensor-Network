/**
 *
 * \file  m2m_flash.c
 *
 * \brief WINC Flash Interface.
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

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
INCLUDES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#include "driver/include/m2m_flash.h"
#include "driver/source/nmflash.h"
#include "driver/source/m2m_hif.h"
#include "driver/include/m2m_wifi.h"
#include "spi_flash/include/spi_flash.h"

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
GLOBALS
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
FUNCTIONS
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

static FlashAccessErr_t init_access(tstrFlashAccess *pstrFlashAccess)
{
	FlashAccessErr_t	ret = FA_RETURN_OK;
	sint8				status = M2M_ERR_FAIL;

	ret |= FA_RETURN_FLAG_INFO_WINC_RESET;
	status = m2m_wifi_reinit_hold();
	if (status == M2M_SUCCESS)
	{
		status = spi_flash_read((uint8*)pstrFlashAccess, HOST_CONTROL_FLASH_OFFSET, FA_SIG_STA_SZ);
		if (status == M2M_SUCCESS)
		{
			if ((pstrFlashAccess->u32Signature != FA_SIGNATURE) || (pstrFlashAccess->enuTransferStatus != FA_STATUS_EMPTY))
				status = spi_flash_erase(HOST_CONTROL_FLASH_OFFSET, HOST_CONTROL_FLASH_SZ);
		}
	}
	if (status == M2M_SUCCESS)
	{
		pstrFlashAccess->u32Signature = FA_SIGNATURE;
		pstrFlashAccess->enuTransferStatus = FA_STATUS_NOT_ACTIVE;
		status = spi_flash_write((uint8*)pstrFlashAccess, HOST_CONTROL_FLASH_OFFSET, FA_SIG_STA_SZ);
	}
	if (status != M2M_SUCCESS)
		ret |= FA_RETURN_FLAG_ERR | FA_RETURN_FLAG_ERR_WINC_ACCESS;
	return ret;
}
static FlashAccessErr_t commit_access(tstrFlashAccess *pstrFlashAccess)
{
	FlashAccessErr_t	ret = FA_RETURN_OK;
	sint8				status = M2M_ERR_FAIL;

	if (pstrFlashAccess == NULL)
		goto ERR;
	/*
	 *	To begin with, flash is unchanged. Later, when first flash erase/write occurs, this flag
	 *	will be cleared.
	 */
	pstrFlashAccess->u8ModeFlags |= FA_MODE_FLAGS_UNCHANGED;

	if (pstrFlashAccess->u32Size > 0)
	{
		/* Check legality of source and destination fields */
		if (pstrFlashAccess->strDestinationInfo.pfCtlFunction == NULL)
			goto ERR;
		if (pstrFlashAccess->strDestinationInfo.pfFunction == NULL)
			goto ERR;
		if (pstrFlashAccess->strSourceInfo.pfCtlFunction == NULL)
			goto ERR;
		if (pstrFlashAccess->strSourceInfo.pfFunction == NULL)
			goto ERR;
		if (pstrFlashAccess->strDestinationInfo.pfCtlFunction == pstrFlashAccess->strSourceInfo.pfCtlFunction)
			goto ERR;
		if (pstrFlashAccess->strDestinationInfo.pfFunction == pstrFlashAccess->strSourceInfo.pfFunction)
			goto ERR;
		if (pstrFlashAccess->u32Size > pstrFlashAccess->strDestinationInfo.u32LocationSize)
			goto ERR;
		if (pstrFlashAccess->u32Size > pstrFlashAccess->strSourceInfo.u32LocationSize)
			goto ERR;

		if (pstrFlashAccess->u8AccessFlags & FA_ACCESS_OPTION_KEEP_SURROUNDING)
		{
			uint32 tmp = pstrFlashAccess->strDestinationInfo.u32AlignmentSize;
			/* We cannot support erase block sizes greater than our buffer length. */
			if (tmp > FA_SECTOR_SIZE)
			{
				ret |= FA_RETURN_FLAG_ERR_PARAM;
				goto ERR;
			}
			/* Erase block size must be a power of 2. */
			if (tmp & (tmp-1))
			{
				ret |= FA_RETURN_FLAG_ERR_PARAM;
				goto ERR;
			}
			/* 0 or 1 is legal, but then no such thing as a partial sector. */
			if (tmp < 2)
				pstrFlashAccess->u8AccessFlags &= ~FA_ACCESS_OPTION_KEEP_SURROUNDING;
		}
	}

	pstrFlashAccess->enuTransferStatus = FA_STATUS_ACTIVE;
	m2m_memcpy((uint8*)&gstrFlashAccess, (uint8*)pstrFlashAccess, sizeof(tstrFlashAccess));
	ret |= FA_RETURN_FLAG_INFO_SAVED;

	status = spi_flash_write((uint8*)&gstrFlashAccess + FA_SIG_STA_SZ, HOST_CONTROL_FLASH_OFFSET + FA_SIG_STA_SZ, FA_PERM_SZ - FA_SIG_STA_SZ);
	if (status == M2M_SUCCESS)
	{
		status = spi_flash_write((uint8*)&gstrFlashAccess + FA_STA_OFS, HOST_CONTROL_FLASH_OFFSET + FA_STA_OFS, FA_STA_SZ);
		if (status == M2M_SUCCESS)
			ret |= m2m_flash_access_run();
	}
	if (status != M2M_SUCCESS)
	{
		ret |= FA_RETURN_FLAG_ERR_WINC_ACCESS;
		goto ERR;
	}
	goto DONE;
ERR:
	ret |= FA_RETURN_FLAG_ERR;
DONE:
	return ret;
}

FlashAccessErr_t m2m_flash_access_image(uint8 u8ModeOptions, uint8 u8AccessOptions, uint16 u16Id, tstrFlashAccessInfo *pstrSourceInfo)
{
	FlashAccessErr_t	ret = FA_RETURN_OK;
	tstrFlashAccess		strFlashAccess;

	/* Check input parameters. */
	if (pstrSourceInfo == NULL)
	{
		ret |= FA_RETURN_FLAG_ERR_PARAM;
		goto ERR;
	}
	if (u8ModeOptions & FA_UPDATEIMAGE_OPTION_UPDATE)
		if (pstrSourceInfo->u32LocationSize > OTA_IMAGE_SIZE)
		{
			ret |= FA_RETURN_FLAG_ERR_PARAM | FA_RETURN_FLAG_ERR_WINC_SZ;
			goto ERR;
		}

	/* Set parameters for whole transfer, according to enuMode. */
	m2m_memset((uint8*)&strFlashAccess, 0, sizeof(tstrFlashAccess));

	strFlashAccess.u16AppId = u16Id;

	m2m_memcpy((uint8*)&strFlashAccess.strSourceInfo, (uint8*)pstrSourceInfo, sizeof(tstrFlashAccessInfo));
	strFlashAccess.u32Size = pstrSourceInfo->u32LocationSize;

	if (!(u8ModeOptions & FA_UPDATEIMAGE_OPTION_UPDATE))
		strFlashAccess.u32Size = 0;
	if (u8ModeOptions & FA_UPDATEIMAGE_OPTION_VALIDATE)
		strFlashAccess.u8ModeFlags |= FA_MODE_FLAGS_CS_VALIDATE_IMAGE;
	if (u8ModeOptions & FA_UPDATEIMAGE_OPTION_SWITCH)
		strFlashAccess.u8ModeFlags |= FA_MODE_FLAGS_CS_SWITCH;
	strFlashAccess.u8ModeFlags |= FA_MODE_FLAGS_CS;

	strFlashAccess.strDestinationInfo.pfCtlFunction = winc_flash_control;
	strFlashAccess.strDestinationInfo.pfFunction = winc_flash_access;
	strFlashAccess.strDestinationInfo.u32LocationId = MEM_ID_WINC_INACTIVE;
	strFlashAccess.strDestinationInfo.u32LocationSize = OTA_IMAGE_SIZE;
	strFlashAccess.strDestinationInfo.u32AlignmentSize = FLASH_SECTOR_SZ;

	strFlashAccess.u8AccessFlags = u8AccessOptions & FA_ACCESS_WINC_MASK;
	strFlashAccess.u8AccessFlags |= FA_ACCESS_OPTION_ERASE_FIRST;

	ret |= init_access(&strFlashAccess);
	if (!(ret & FA_RETURN_FLAG_ERR))
	{
		if (strFlashAccess.u8ModeFlags & FA_MODE_FLAGS_CS_SWITCH)
		{
			uint8 target = 0;
			if (m2m_flash_access_image_get_target(&target) != M2M_SUCCESS)
			{
				ret |= FA_RETURN_FLAG_ERR_WINC_ACCESS;
				goto ERR;
			}
			if (target > 0)
				strFlashAccess.u8ModeFlags |= FA_MODE_FLAGS_CS_SWITCH_TARGET;
		}
		ret |= commit_access(&strFlashAccess);
	}
	goto DONE;
ERR:
	ret |= FA_RETURN_FLAG_ERR;
DONE:
	M2M_INFO("FAState:%04x\n", ret);
	return ret;
}

FlashAccessErr_t m2m_flash_access_item(tenuFlashAccessMode enuMode, uint8 u8ModeOptions, uint8 u8AccessOptions, uint16 u16Id, tstrFlashAccessInfo *pstrInfo)
{
	FlashAccessErr_t			ret = FA_RETURN_OK;
	sint8						status = M2M_SUCCESS;
	tstrFlashAccess				strFlashAccess;
	tstrRootCertEntryHeader		strRootCertEntry;
	uint16						u16EntrySz = 0;

	/* Host control of root certificates is not compatible with old firmware. */
	if (hif_check_code(HIFCODE_ROOTCERT_ACCESS_V1 >> 8, HIFCODE_ROOTCERT_ACCESS_V1 & 0xFF) != M2M_SUCCESS)
		goto ERR;

	/* Check input parameters. */
	if ((pstrInfo == NULL) && (enuMode != FA_PRUNE_ROOTCERT))
	{
		ret |= FA_RETURN_FLAG_ERR_PARAM;
		goto ERR;
	}
	switch (enuMode)
	{
	case FA_ADD_ROOTCERT:
		// Read the header and check source size is sufficient for other parameters provided.
		status = pstrInfo->pfCtlFunction(pstrInfo->u32LocationId, sizeof(strRootCertEntry), FA_FN_FLAGS_READ, FA_FN_CTL_INITIALIZE);
		if (status == M2M_SUCCESS)
		{
			status = pstrInfo->pfFunction((uint8*)&strRootCertEntry, sizeof(strRootCertEntry), 0, sizeof(strRootCertEntry));
			//pstrSourceInfo->pfCtlFunction(pstrSourceInfo->u32LocationId, 0, 0, FA_FN_CTL_COMPLETE);
		}
		if (status != M2M_SUCCESS)
		{
			ret |= FA_RETURN_FLAG_ERR_LOCAL_ACCESS;
			goto ERR;
		}
		status = m2m_flash_access_rootcert_get_size(&strRootCertEntry, &u16EntrySz);
		if ((status != M2M_SUCCESS) || (pstrInfo->u32LocationSize < u16EntrySz))
		{
			ret |= FA_RETURN_FLAG_ERR_PARAM;
			goto ERR;
		}
		break;
	case FA_READ_ROOTCERT:
	case FA_REMOVE_ROOTCERT:
		// Read the identifier.
		status = pstrInfo->pfCtlFunction(pstrInfo->u32LocationId, sizeof(strRootCertEntry.au8SHA1NameHash), FA_FN_FLAGS_READ, FA_FN_CTL_INITIALIZE);
		if (status == M2M_SUCCESS)
		{
			status = pstrInfo->pfFunction((uint8*)&strRootCertEntry, sizeof(strRootCertEntry), 0, sizeof(strRootCertEntry.au8SHA1NameHash));
			//pstrInfo->pfCtlFunction(pstrInfo->u32LocationId, 0, 0, FA_FN_CTL_COMPLETE);
		}
		if (status != M2M_SUCCESS)
		{
			ret |= FA_RETURN_FLAG_ERR_LOCAL_ACCESS;
			goto ERR;
		}
		break;
	case FA_READIDX_ROOTCERT:
		// In this mode we just want to check any non-empty entry. Set the identifier to all 0 to help.
		m2m_memset((uint8*)&strRootCertEntry.au8SHA1NameHash, 0, sizeof(strRootCertEntry.au8SHA1NameHash));
		// Hack strRootCertEntry to carry the index from u8ModeOptions.
		*(uint8*)&strRootCertEntry.strExpDate = u8ModeOptions;
		break;
	case FA_PRUNE_ROOTCERT:
		// In this mode we just want to check any non-empty entry. Set the identifier to all 0 to help.
		m2m_memset((uint8*)&strRootCertEntry.au8SHA1NameHash, 0, sizeof(strRootCertEntry.au8SHA1NameHash));
		break;
	default:
		/* No other item modes supported. */
		ret |= FA_RETURN_FLAG_ERR_PARAM;
		goto ERR;
		break;
	}

	/* Set parameters for whole transfer, according to enuMode. */
	m2m_memset((uint8*)&strFlashAccess, 0, sizeof(tstrFlashAccess));

	strFlashAccess.u16AppId = u16Id;
	strFlashAccess.u8AccessFlags = u8AccessOptions;

	switch (enuMode)
	{
	case FA_ADD_ROOTCERT:
		strFlashAccess.u8AccessFlags &= FA_ACCESS_WINC_MASK;
		m2m_memcpy((uint8*)&strFlashAccess.strSourceInfo, (uint8*)pstrInfo, sizeof(tstrFlashAccessInfo));
		strFlashAccess.strDestinationInfo.pfCtlFunction = winc_flash_control;
		strFlashAccess.strDestinationInfo.pfFunction = winc_flash_access;
		strFlashAccess.strDestinationInfo.u32AlignmentSize = FLASH_SECTOR_SZ;
		strFlashAccess.u32Size = u16EntrySz;
		break;
	case FA_READ_ROOTCERT:
	case FA_READIDX_ROOTCERT:
		m2m_memcpy((uint8*)&strFlashAccess.strDestinationInfo, (uint8*)pstrInfo, sizeof(tstrFlashAccessInfo));
		strFlashAccess.strSourceInfo.pfCtlFunction = winc_flash_control;
		strFlashAccess.strSourceInfo.pfFunction = winc_flash_access;
		// We don't know the size yet. Set it to maximum. It will get decreased later by m2m_flash_access_rootcert().
		strFlashAccess.u32Size = pstrInfo->u32LocationSize;
	case FA_REMOVE_ROOTCERT:
		break;
	case FA_PRUNE_ROOTCERT:
		break;
	default:
		/* No other item modes supported. */
		ret |= FA_RETURN_FLAG_ERR_PARAM;
		goto ERR;
		break;
	}

	ret |= init_access(&strFlashAccess);
	if (!(ret & FA_RETURN_FLAG_ERR))
	{
		/* Now we can access the items in flash. */
		switch (enuMode)
		{
		case FA_ADD_ROOTCERT:
			// Prepare for add.
			ret |= m2m_flash_access_rootcert(enuMode, &strRootCertEntry, &pstrInfo->u32LocationSize, &strFlashAccess.strDestinationInfo.u32LocationId);
			strFlashAccess.strDestinationInfo.u32LocationSize = strFlashAccess.u32Size;
			break;
		case FA_READ_ROOTCERT:
			// Prepare for read.
			ret |= m2m_flash_access_rootcert(enuMode, &strRootCertEntry, &strFlashAccess.u32Size, &strFlashAccess.strSourceInfo.u32LocationId);
			strFlashAccess.strSourceInfo.u32LocationSize = strFlashAccess.u32Size;
			break;
		case FA_READIDX_ROOTCERT:
			// Prepare for read.
			ret |= m2m_flash_access_rootcert(enuMode, &strRootCertEntry, &strFlashAccess.u32Size, &strFlashAccess.strSourceInfo.u32LocationId);
			strFlashAccess.strSourceInfo.u32LocationSize = strFlashAccess.u32Size;
			break;
		case FA_REMOVE_ROOTCERT:
			// Do remove.
			ret |= m2m_flash_access_rootcert(enuMode, &strRootCertEntry, &pstrInfo->u32LocationSize, NULL);
			break;
		case FA_PRUNE_ROOTCERT:
			// Prepare for prune.
			{
				uint32 tmp = sizeof(strRootCertEntry);
				ret |= m2m_flash_access_rootcert(enuMode, &strRootCertEntry, &tmp, &strFlashAccess.strDestinationInfo.u32LocationId);
				if (!(ret & FA_RETURN_FLAG_ERR))
				{
					// Set up for copy from backup.
					uint32					u32BackupAddr = FA_BACKUP_STORE_OFFSET;
					uint32					u32Status = 0;
					tstrFlashAccessBackup	strFlashAccessBackup = {FA_STATUS_NOT_ACTIVE,
																	FA_SIGNATURE,
																	M2M_TLS_ROOTCER_FLASH_OFFSET,
																	M2M_BACKUP_FLASH_OFFSET,
																	M2M_TLS_ROOTCER_FLASH_SZ};

					status = spi_flash_read((uint8*)&u32Status, u32BackupAddr, sizeof(u32Status));
					if ((status != M2M_SUCCESS) || (u32Status != FA_STATUS_EMPTY))
					{
						u32BackupAddr += sizeof(tstrFlashAccessBackup);
						status = spi_flash_read((uint8*)&u32Status, u32BackupAddr, sizeof(u32Status));
					}
					if ((status != M2M_SUCCESS) || (u32Status != FA_STATUS_EMPTY))
					{
						ret |= FA_RETURN_FLAG_ERR_WINC_ACCESS;
						goto ERR;
					}
					status = spi_flash_write((uint8*)&strFlashAccessBackup.enuTransferStatus, u32BackupAddr, FA_BACKUP_STA_SZ);
					if (status == M2M_SUCCESS)
					{
						status = spi_flash_write((uint8*)&strFlashAccessBackup + FA_BACKUP_STA_SZ, u32BackupAddr + FA_BACKUP_STA_SZ, sizeof(strFlashAccessBackup) - FA_BACKUP_STA_SZ);
						if (status == M2M_SUCCESS)
						{
							u32Status = FA_STATUS_ACTIVE;
							status = spi_flash_write((uint8*)&u32Status, u32BackupAddr, FA_BACKUP_STA_SZ);
						}
					}
					if (status != M2M_SUCCESS)
					{
						ret |= FA_RETURN_FLAG_ERR_WINC_ACCESS;
						goto ERR;
					}
					// Now kick off the backup.
					strFlashAccess.u8ModeFlags |= FA_MODE_FLAGS_CHECK_BACKUP;
				}
			}
			break;
		default:
			/* No other item modes supported. */
			ret |= FA_RETURN_FLAG_ERR_PARAM;
			goto ERR;
			break;
		}
		if (!(ret & FA_RETURN_FLAG_ERR))
		{
			/* Commit_access() here. For Remove modes it is not needed. */
			if (enuMode != FA_REMOVE_ROOTCERT)
				ret |= commit_access(&strFlashAccess);
		}
	}
	goto DONE;
ERR:
	ret |= FA_RETURN_FLAG_ERR;
DONE:
	M2M_INFO("FAState:%04x\n", ret);
	return ret;
}

FlashAccessErr_t m2m_flash_access_item_remove(uint16 u16Id, void* pvItem, uint32 u32ItemLen)
{
	FlashAccessErr_t			ret = FA_RETURN_OK;
	tstrFlashAccess				strFlashAccess;
	tstrRootCertEntryHeader		*pstrRootCertEntry = (tstrRootCertEntryHeader *)pvItem;

	/* Host control of root certificates is not compatible with old firmware. */
	if (hif_check_code(HIFCODE_ROOTCERT_ACCESS_V1 >> 8, HIFCODE_ROOTCERT_ACCESS_V1 & 0xFF) != M2M_SUCCESS)
		goto ERR;

	/* Check input parameters. */
	if (pvItem == NULL)
	{
		ret |= FA_RETURN_FLAG_ERR_PARAM;
		goto ERR;
	}
	// Check buffer length is sufficient for identifier.
	if (sizeof(pstrRootCertEntry->au8SHA1NameHash) > u32ItemLen)
	{
		ret |= FA_RETURN_FLAG_ERR_PARAM;
		goto ERR;
	}

	/* We won't be using strFlashAccess, but still need to call init_access to get the WINC ready. */
	m2m_memset((uint8*)&strFlashAccess, 0, sizeof(tstrFlashAccess));

	ret |= init_access(&strFlashAccess);
	if (!(ret & FA_RETURN_FLAG_ERR))
	{
		uint32	u32Offset = 0;

		/* Now we can access the items in flash. */
		// Remove rootcert.
		ret |= m2m_flash_access_rootcert(FA_REMOVE_ROOTCERT, pvItem, &u32ItemLen, &u32Offset);
	}
	goto DONE;
ERR:
	ret |= FA_RETURN_FLAG_ERR;
DONE:
	M2M_INFO("FAState:%04x\n", ret);
	return ret;
}

sint8 m2m_flash_access_init(tstrFlashAccessReturn *pstrRet)
{
	tstrFlashAccess	strSavedFlashAccess;

	m2m_memset((uint8*)&gstrFlashAccess, 0, sizeof(tstrFlashAccess));

	pstrRet->u16AppId = 0;
	pstrRet->u16Ret = FA_RETURN_FLAG_INFO_WINC_RESET;

	if (spi_flash_read((uint8*)&strSavedFlashAccess, HOST_CONTROL_FLASH_OFFSET, FA_PERM_SZ) != M2M_SUCCESS)
	{
		pstrRet->u16Ret |= FA_RETURN_FLAG_ERR_WINC_ACCESS;
		goto ERR;
	}
	if (strSavedFlashAccess.u32Signature == FA_SIGNATURE)
	{
		switch (strSavedFlashAccess.enuTransferStatus)
		{
		case FA_STATUS_EMPTY:
		case FA_STATUS_NOT_ACTIVE:
			break;
		case FA_STATUS_ACTIVE:
			if (strSavedFlashAccess.u8ModeFlags & FA_MODE_FLAGS_CS_SWITCH)
			{
				// Check to see if switch happened before we were interrupted. If so we had actually completed.
				uint8 target;
				if (m2m_flash_access_image_get_target(&target) == M2M_SUCCESS)
				{
					if ((target == 0) && (gstrFlashAccess.u8ModeFlags & FA_MODE_FLAGS_CS_SWITCH_TARGET))
						goto DONE;
					if ((target > 0) && !(gstrFlashAccess.u8ModeFlags & FA_MODE_FLAGS_CS_SWITCH_TARGET))
						goto DONE;
				}
			}
			pstrRet->u16AppId = strSavedFlashAccess.u16AppId;
			// We were interrupted. WINC backup recovery may be needed.
			winc_flash_control(0,0,0,FA_FN_CTL_CHECK_BACKUP);
			pstrRet->u16Ret |= FA_RETURN_FLAG_ERR | FA_RETURN_FLAG_ERR_INTERRUPTED | FA_RETURN_FLAG_INFO_SAVED;
			if (!(strSavedFlashAccess.u8ModeFlags & FA_MODE_FLAGS_UNCHANGED))
				pstrRet->u16Ret |= FA_RETURN_FLAG_INFO_CHANGED;
			if (strSavedFlashAccess.u8AccessFlags & FA_ACCESS_OPTION_USE_BACKUP)
				pstrRet->u16Ret |= FA_RETURN_FLAG_INFO_CHECK_BACKUP;
			break;
		case FA_STATUS_DONE:
DONE:
			pstrRet->u16AppId = strSavedFlashAccess.u16AppId;
			pstrRet->u16Ret |= FA_RETURN_FLAG_COMPLETE | FA_RETURN_FLAG_INFO_SAVED;
			if (!(strSavedFlashAccess.u8ModeFlags & FA_MODE_FLAGS_UNCHANGED))
				pstrRet->u16Ret |= FA_RETURN_FLAG_INFO_CHANGED;
			strSavedFlashAccess.enuTransferStatus = FA_STATUS_OLD;
			spi_flash_write((uint8*)&strSavedFlashAccess.enuTransferStatus, HOST_CONTROL_FLASH_OFFSET + FA_STA_OFS, FA_STA_SZ);
			break;
		case FA_STATUS_OLD:
			pstrRet->u16AppId = strSavedFlashAccess.u16AppId;
			break;
		}
	}
	return M2M_SUCCESS;
ERR:
	pstrRet->u16Ret |= FA_RETURN_FLAG_ERR;
	return M2M_ERR_FAIL;
}
sint8 m2m_flash_access_reset(void)
{
	sint8 s8Ret = spi_flash_erase(HOST_CONTROL_FLASH_OFFSET, HOST_CONTROL_FLASH_SZ);
	if (s8Ret == M2M_SUCCESS)
	{
		uint32 u32Signature = FA_SIGNATURE;
		s8Ret = spi_flash_write((uint8*)&u32Signature, HOST_CONTROL_FLASH_OFFSET, FA_SIG_SZ);
	}
	return s8Ret;
}
FlashAccessErr_t m2m_flash_access_retry(void)
{
	FlashAccessErr_t	ret = FA_RETURN_FLAG_INFO_WINC_RESET;

	if (gstrFlashAccess.u32Signature == FA_SIGNATURE)
	{
		if (gstrFlashAccess.enuTransferStatus == FA_STATUS_ACTIVE)
		{
			ret |= FA_RETURN_FLAG_INFO_SAVED;
			if (!(gstrFlashAccess.u8ModeFlags & FA_MODE_FLAGS_UNCHANGED))
				ret |= FA_RETURN_FLAG_INFO_CHANGED;
			ret |= m2m_flash_access_run();
			goto DONE;
		}
	}
	ret |= FA_RETURN_FLAG_ERR | FA_RETURN_FLAG_ERR_PARAM;
DONE:
	return ret;
}
