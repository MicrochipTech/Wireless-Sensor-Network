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



/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
INCLUDES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#include <stdbool.h>
#include "driver/include/m2m_flash.h"
#include "driver/source/nmflash.h"
#include "spi_flash/include/spi_flash.h"

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
GLOBALS
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
tstrFlashAccess			gstrFlashAccess = {0};
static uint8_t			gau8Buff[FA_SECTOR_SIZE];
static uint32			gu32CurrentAddr = 0;
static uint8			gu8Flags = 0;


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
FUNCTIONS
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

static sint8 winc_flash_compare(uint8_t *pu8Buf, uint32_t u32Offset, uint32_t u32Size)
{
	sint8 ret = M2M_SUCCESS;
	uint8 buf[128];
	uint32 offset = 0;

	while (offset < u32Size)
	{
		uint32 chunk_sz = sizeof(buf);
		if (chunk_sz > u32Size - offset)
			chunk_sz = u32Size - offset;
		ret = spi_flash_read(buf, u32Offset + offset, chunk_sz);
		if (ret != M2M_SUCCESS)
			break;
		ret = m2m_memcmp(buf, pu8Buf + offset, chunk_sz);
		if (ret != 0)
			break;
		offset += chunk_sz;
	}
	return ret;
}

static uint8 crc7(uint8 crc, const uint8 *buff, uint16 len)
{
	uint8 reg = crc;
	uint16 i;
	for(i = 0; i < len; i++)
	{
		uint16 g;
		for(g = 0; g < 8; g++)
		{
			uint8 inv = (((buff[i] << g) & 0x80) >> 7) ^ ((reg >> 6) & 1);
			reg = ((reg << 1) & 0x7f) ^ (9 * inv);
		}
	}
	return reg;
}
static sint8 update_control_sector(tstrOtaControlSec *pstrControlSec)
{
	sint8 ret = M2M_ERR_FAIL;

	ret = spi_flash_erase(M2M_BACKUP_FLASH_OFFSET, M2M_BACKUP_FLASH_SZ);
	if (ret == M2M_SUCCESS)
	{
		pstrControlSec->u32OtaSequenceNumber++;
		pstrControlSec->u32OtaControlSecCrc = crc7(0x7f, (uint8*)pstrControlSec, sizeof(tstrOtaControlSec) - 4);
		ret = spi_flash_write((uint8*)pstrControlSec, M2M_BACKUP_FLASH_OFFSET, sizeof(tstrOtaControlSec));
		if (ret == M2M_SUCCESS)
		{
			ret = winc_flash_compare((uint8*)pstrControlSec, M2M_BACKUP_FLASH_OFFSET, sizeof(tstrOtaControlSec));
			if (ret == M2M_SUCCESS)
			{
				ret = spi_flash_erase(M2M_CONTROL_FLASH_OFFSET, M2M_CONTROL_FLASH_SZ);
				if (ret == M2M_SUCCESS)
				{
					pstrControlSec->u32OtaSequenceNumber++;
					pstrControlSec->u32OtaControlSecCrc = crc7(0x7f, (uint8*)pstrControlSec, sizeof(tstrOtaControlSec) - 4);
					ret = spi_flash_write((uint8*)pstrControlSec, M2M_CONTROL_FLASH_OFFSET, sizeof(tstrOtaControlSec));
					if (ret == M2M_SUCCESS)
						ret = winc_flash_compare((uint8*)pstrControlSec, M2M_CONTROL_FLASH_OFFSET, sizeof(tstrOtaControlSec));
				}
			}
		}
	}
	return ret;
}
static sint8 access_control_sector(tenuCSOp enuOp, uint32 *param)
{
	static	tstrOtaControlSec strControlSec = {0};
	sint8	s8Ret = M2M_SUCCESS;
	uint8	bUpdate = false;

	if ((enuOp != CS_INITIALIZE) && (strControlSec.u32OtaMagicValue != OTA_MAGIC_VALUE))
	{
		if (param != NULL)
			*param = 0;
		return M2M_ERR_FAIL;
	}

	switch (enuOp)
	{
	case CS_INITIALIZE:
		s8Ret = spi_flash_read((uint8*)&strControlSec, M2M_CONTROL_FLASH_OFFSET, sizeof(tstrOtaControlSec));
		if ((s8Ret != M2M_SUCCESS) || (strControlSec.u32OtaMagicValue != OTA_MAGIC_VALUE))
		{
			s8Ret = spi_flash_read((uint8*)&strControlSec, M2M_BACKUP_FLASH_OFFSET, sizeof(tstrOtaControlSec));
			if ((s8Ret == M2M_SUCCESS) && (strControlSec.u32OtaMagicValue == OTA_MAGIC_VALUE))
			{
				/*
				 *	Reinstate the control sector from backup.
				 */
				s8Ret = spi_flash_erase(M2M_CONTROL_FLASH_OFFSET, M2M_CONTROL_FLASH_SZ);
				if (s8Ret == M2M_SUCCESS)
				{
					s8Ret = spi_flash_write((uint8*)&strControlSec, M2M_CONTROL_FLASH_OFFSET, sizeof(tstrOtaControlSec));
					if (s8Ret == M2M_SUCCESS)
						s8Ret = winc_flash_compare((uint8*)&strControlSec, M2M_CONTROL_FLASH_OFFSET, sizeof(tstrOtaControlSec));
				}
			}
		}
		if (strControlSec.u32OtaMagicValue != OTA_MAGIC_VALUE)
			s8Ret = M2M_ERR_FAIL;
		break;
	case CS_INVALIDATE_RB:
		// Update trashes the backup sector, so we need to avoid unnecessary updates.
		if (strControlSec.u32OtaRollbackImageValidStatus != OTA_STATUS_INVALID)
		{
			strControlSec.u32OtaRollbackImageValidStatus = OTA_STATUS_INVALID;
			bUpdate = true;
		}
		break;
	case CS_VALIDATE_RB:
		// Update trashes the backup sector, so we need to avoid unnecessary updates.
		if (strControlSec.u32OtaRollbackImageValidStatus != OTA_STATUS_VALID)
		{
			strControlSec.u32OtaRollbackImageValidStatus = OTA_STATUS_VALID;
			bUpdate = true;
		}
		break;
	case CS_VALIDATE_SWITCH:
		strControlSec.u32OtaRollbackImageValidStatus = OTA_STATUS_VALID;
		// intentional fallthrough.
	case CS_SWITCH:
		if (strControlSec.u32OtaRollbackImageValidStatus == OTA_STATUS_VALID)
		{
			uint32 tmp = strControlSec.u32OtaCurrentworkingImagOffset;
			strControlSec.u32OtaCurrentworkingImagOffset = strControlSec.u32OtaRollbackImageOffset;
			strControlSec.u32OtaRollbackImageOffset = tmp;
			bUpdate = true;
		}
		else
			s8Ret = M2M_ERR_FAIL;
		break;
	case CS_GET_ACTIVE:
		if (param == NULL)
			s8Ret = M2M_ERR_FAIL;
		else
			*param = strControlSec.u32OtaCurrentworkingImagOffset;
		break;
	case CS_GET_INACTIVE:
		if (param == NULL)
			s8Ret = M2M_ERR_FAIL;
		else
			*param = strControlSec.u32OtaRollbackImageOffset;
		break;
	case CS_DEINITIALIZE:
		m2m_memset((uint8*)&strControlSec, 0, sizeof(tstrOtaControlSec));
		break;
	default:
		s8Ret = M2M_ERR_FAIL;
	}
	if (bUpdate)
	{
		s8Ret = update_control_sector(&strControlSec);
		M2M_INFO("CS update:%d %s\n", enuOp, (s8Ret==M2M_SUCCESS)?"":"Failed");
	}
	return s8Ret;
}

sint8 winc_flash_control(uint32 u32LocationId, uint32 u32TotalSize, uint8 u8Flags, tenuFlashAccessFnCtl enuCtl)
{
	switch (enuCtl)
	{
	case FA_FN_CTL_INITIALIZE:
		gu8Flags = u8Flags;
		switch (u32LocationId)
		{
		case MEM_ID_WINC_FLASH:
			gu32CurrentAddr = 0;
			break;
		case MEM_ID_WINC_ACTIVE:
			if (access_control_sector(CS_GET_ACTIVE, &gu32CurrentAddr) != M2M_SUCCESS)
				goto ERR;
			break;
		case MEM_ID_WINC_INACTIVE:
			if (access_control_sector(CS_GET_INACTIVE, &gu32CurrentAddr) != M2M_SUCCESS)
				goto ERR;
			/*	If we're about to write to the inactive partition, mark it as invalid. */
			if (access_control_sector(CS_INVALIDATE_RB, NULL) != M2M_SUCCESS)
				goto ERR;
			break;
		case MEM_ID_WINC_APP_STORE:
			gu32CurrentAddr = M2M_OTA_IMAGE2_OFFSET + OTA_IMAGE_SIZE;
			break;
		default:
			gu32CurrentAddr = u32LocationId;
			break;
		}
		break;
	case FA_FN_CTL_TERMINATE:
	case FA_FN_CTL_COMPLETE:
		gu8Flags = 0;
		gu32CurrentAddr = 0;
		break;
	case FA_FN_CTL_CHECK_BACKUP:
		{
			uint32					u32BackupAddr = FA_BACKUP_STORE_OFFSET;
			tstrFlashAccessBackup	strFlashAccessBackup;
			tenuFlashAccessStatus	enuStatus = FA_STATUS_DONE;

			while (u32BackupAddr < FA_BACKUP_STORE_OFFSET + FA_BACKUP_STORE_SZ)
			{
				sint8 status = spi_flash_read((uint8*)&strFlashAccessBackup, u32BackupAddr, sizeof(tstrFlashAccessBackup));
				if ((status == M2M_SUCCESS) && (strFlashAccessBackup.enuTransferStatus == FA_STATUS_ACTIVE))
				{
					status = spi_flash_read(gau8Buff, strFlashAccessBackup.u32SourceAddr, strFlashAccessBackup.u32Size);
					if (status == M2M_SUCCESS)
					{
						status = spi_flash_erase(strFlashAccessBackup.u32DestinationAddr, strFlashAccessBackup.u32Size);
						if (status == M2M_SUCCESS)
						{
							status =spi_flash_write(gau8Buff, strFlashAccessBackup.u32DestinationAddr, strFlashAccessBackup.u32Size);
							if (status == M2M_SUCCESS)
								status =spi_flash_write((uint8*)&enuStatus, u32BackupAddr, FA_BACKUP_STA_SZ);
						}
					}
				}
				if (status != M2M_SUCCESS)
					goto ERR;
				u32BackupAddr += sizeof(tstrFlashAccessBackup);
			}
		}
		break;
	}
	return M2M_SUCCESS;
ERR:
	return M2M_ERR_FAIL;
}
sint8 winc_flash_access(uint8_t *pu8Buf, uint32_t u32BufSize, uint32_t u32DataOffset, uint32_t u32DataSize)
{
	sint8 s8Ret = M2M_SUCCESS;
	if (gu8Flags & FA_FN_FLAGS_COMPARE_BEFORE)
	{
		// If contents match already, return success
		if (winc_flash_compare(pu8Buf + u32DataOffset, gu32CurrentAddr, u32DataSize) == 0)
			goto END;
	}
	if (gu8Flags & FA_FN_FLAGS_READ_SURROUNDING)
	{
		// Not implemented yet. Not required by any currently supported mode.
	}
	if (gu8Flags & FA_FN_FLAGS_BACKUP)
	{
		// Not implemented yet. Not required by any currently supported mode.
	}
	if (gu8Flags & FA_FN_FLAGS_ERASE)
	{
		s8Ret = spi_flash_erase(gu32CurrentAddr, u32DataSize);
		if (s8Ret != M2M_SUCCESS)
			goto END;
	}
	if (gu8Flags & FA_FN_FLAGS_WRITE)
	{
		s8Ret = spi_flash_write(pu8Buf + u32DataOffset, gu32CurrentAddr, u32DataSize);
		if (s8Ret != M2M_SUCCESS)
			goto END;
	}
	if (gu8Flags & FA_FN_FLAGS_COMPARE_AFTER)
	{
		// If contents do not match, return failure
		s8Ret = winc_flash_compare(pu8Buf + u32DataOffset, gu32CurrentAddr, u32DataSize);
		if (s8Ret != M2M_SUCCESS)
			goto END;
	}
	if (gu8Flags & FA_FN_FLAGS_READ)
	{
		s8Ret = spi_flash_read(pu8Buf + u32DataOffset, gu32CurrentAddr, u32DataSize);
		if (s8Ret != M2M_SUCCESS)
			goto END;
	}
END:
	gu32CurrentAddr += u32DataSize;
	return s8Ret;
}

sint8 m2m_flash_access_image_get_target(uint8 *pu8Target)
{
	sint8	s8Ret = M2M_ERR_FAIL;
	uint32	u32OffsetActive = 0;
	uint32	u32OffsetInactive = 0;

	s8Ret = access_control_sector(CS_INITIALIZE, NULL);
	if (s8Ret == M2M_SUCCESS)
	{
		s8Ret = access_control_sector(CS_GET_ACTIVE, &u32OffsetActive);
		if (s8Ret == M2M_SUCCESS)
		{
			s8Ret = access_control_sector(CS_GET_INACTIVE, &u32OffsetInactive);
			if (s8Ret == M2M_SUCCESS)
				*pu8Target = (u32OffsetInactive > u32OffsetActive) ? 1 : 0;
		}
		access_control_sector(CS_DEINITIALIZE, NULL);
	}
	return s8Ret;
}
sint8 m2m_flash_access_rootcert_get_size(tstrRootCertEntryHeader *pstrHdr, uint16 *pu16Size)
{
	sint8 s8Ret = M2M_ERR_FAIL;
	if ((pstrHdr == NULL) || (pu16Size == NULL))
		goto ERR;

	/* Set default size out to maximum. */
	*pu16Size = 0xFFFF;
	switch (pstrHdr->strPubKey.u32PubKeyType)
	{
	case ROOT_CERT_PUBKEY_RSA:
		if (pstrHdr->strPubKey.strRsaKeyInfo.u16NSz > M2M_TLS_ROOTCER_FLASH_SZ)
			goto ERR;
		if (pstrHdr->strPubKey.strRsaKeyInfo.u16ESz > M2M_TLS_ROOTCER_FLASH_SZ)
			goto ERR;
		*pu16Size = sizeof(tstrRootCertEntryHeader) + ((pstrHdr->strPubKey.strRsaKeyInfo.u16NSz + 0x3) & ~0x3) + ((pstrHdr->strPubKey.strRsaKeyInfo.u16ESz + 0x3) & ~0x3);
		s8Ret = M2M_SUCCESS;
		break;
	case ROOT_CERT_PUBKEY_ECDSA:
		if (pstrHdr->strPubKey.strEcsdaKeyInfo.u16KeySz > M2M_TLS_ROOTCER_FLASH_SZ)
			goto ERR;
		*pu16Size = sizeof(tstrRootCertEntryHeader) + ((pstrHdr->strPubKey.strEcsdaKeyInfo.u16KeySz + 0x3) & ~0x3) * 2;
		s8Ret = M2M_SUCCESS;
		break;
	case 0xFFFFFFFF:
		// Invalid. May indicate end of list. Fail with size set to 0.
		*pu16Size = 0;
		break;
	default:
		// Corrupt header.
		break;
	}
ERR:
	return s8Ret;
}
FlashAccessErr_t m2m_flash_access_rootcert(tenuFlashAccessMode enuMode, void *pvCertBuf, uint32 *pu32BufLen, uint32 *pu32Offset)
{
	FlashAccessErr_t		ret = FA_RETURN_OK;
	sint8					status = M2M_SUCCESS;
	uint8					au8RootCertSig[] = M2M_TLS_ROOTCER_FLASH_SIG;
	tstrRootCertEntryHeader	strEntryHeader;
	uint32					u32PruneOffset = M2M_BACKUP_FLASH_OFFSET;
	uint8					u8ReadIdx = 0;

	/* Sanity check input params. */
	if ((pvCertBuf == NULL) || (pu32BufLen == NULL) || (pu32Offset == NULL))
		goto ERR;
	if (*pu32BufLen < sizeof(strEntryHeader.au8SHA1NameHash))
		goto ERR;

	/* Assume sizeof(strEntryHeader) >= sizeof(au8RootCertSig) and temporarily use it to read signature. */
	status = spi_flash_read((uint8*)&strEntryHeader, M2M_TLS_ROOTCER_FLASH_OFFSET, sizeof(au8RootCertSig));
	if ((status != M2M_SUCCESS) || m2m_memcmp((uint8*)&strEntryHeader, au8RootCertSig, sizeof(au8RootCertSig)))
	{
		/*
		 *	Root certificate section is not initialized. We could try to initialize it
		 *	here, but for now just fail.
		 */
		ret |= FA_RETURN_FLAG_ERR_WINC_ACCESS;
		goto ERR;
	}
	if (enuMode == FA_PRUNE_ROOTCERT)
	{
		status = spi_flash_erase(M2M_BACKUP_FLASH_OFFSET, M2M_BACKUP_FLASH_SZ);
		if (status == M2M_SUCCESS)
			status = spi_flash_write((uint8*)&strEntryHeader, u32PruneOffset, ROOTCERT_SECTION_HEADER_SZ);
		if (status != M2M_SUCCESS)
		{
			ret |= FA_RETURN_FLAG_ERR_WINC_ACCESS;
			goto ERR;
		}
		u32PruneOffset += ROOTCERT_SECTION_HEADER_SZ;
	}

	if (enuMode == FA_READIDX_ROOTCERT)
		u8ReadIdx = *((uint8*)pvCertBuf + offsetof(tstrRootCertEntryHeader,strExpDate));

	*pu32Offset = M2M_TLS_ROOTCER_FLASH_OFFSET + ROOTCERT_SECTION_HEADER_SZ;
	while (*pu32Offset < M2M_TLS_ROOTCER_FLASH_OFFSET + M2M_TLS_ROOTCER_FLASH_SZ)
	{
		uint16	u16EntrySize = 0;
		status = spi_flash_read((uint8*)&strEntryHeader, *pu32Offset, sizeof(tstrRootCertEntryHeader));
		if (status != M2M_SUCCESS)
		{
			ret |= FA_RETURN_FLAG_ERR_WINC_ACCESS;
			goto ERR;
		}
		status = m2m_flash_access_rootcert_get_size(&strEntryHeader, &u16EntrySize);
		if (status != M2M_SUCCESS)
		{
			// Found the end of the list.
			if (enuMode == FA_ADD_ROOTCERT)
			{
				if (u16EntrySize == 0)
				{
					// End of the list is good. pu32Offset contains the address for adding a new entry.
					if ((*pu32Offset + *pu32BufLen) > (M2M_TLS_ROOTCER_FLASH_OFFSET + M2M_TLS_ROOTCER_FLASH_SZ))
					{
						ret |= FA_RETURN_FLAG_ERR_WINC_SZ;
						goto ERR;
					}
					/* Need to write PubKeyType first for safety. */
					ret |= FA_RETURN_FLAG_INFO_CHANGED;
					status = spi_flash_write((uint8*)&strEntryHeader.strPubKey.u32PubKeyType, *pu32Offset, sizeof(strEntryHeader.strPubKey.u32PubKeyType));
					if (status != M2M_SUCCESS)
					{
						ret |= FA_RETURN_FLAG_ERR_WINC_ACCESS;
						goto ERR;
					}
				}
				else
				{
					// End of the list is corrupt. We are effectively at the end of the flash section.
					ret |= FA_RETURN_FLAG_ERR_WINC_SZ;
					goto ERR;
				}
			}
			else if (enuMode == FA_PRUNE_ROOTCERT)
				*pu32Offset = M2M_TLS_ROOTCER_FLASH_OFFSET;
			else
			{
				// Reached the end of the list without finding the item we wanted (read or remove)
				ret |= FA_RETURN_FLAG_ERR_WINC_ITEM;
				goto ERR;
			}
			// We are done.
			goto DONE;
		}

		// If we are here we know that u32EntrySize is sane.

		switch (enuMode)
		{
		case FA_ADD_ROOTCERT:
			if (!m2m_memcmp(strEntryHeader.au8SHA1NameHash, pvCertBuf, sizeof(strEntryHeader.au8SHA1NameHash)))
			{
				// Found a match. Cannot add.
				ret |= FA_RETURN_FLAG_ERR_WINC_ITEM;
				goto ERR;
			}
			break;
		case FA_READ_ROOTCERT:
			if (!m2m_memcmp(strEntryHeader.au8SHA1NameHash, pvCertBuf, sizeof(strEntryHeader.au8SHA1NameHash)))
			{
				// Found a match. Update contents of pu32BufLen with actual size.
				if (u16EntrySize <= *pu32BufLen)
					*pu32BufLen = u16EntrySize;
				else
				{
					ret |= FA_RETURN_FLAG_ERR_WINC_SZ;
					goto ERR;
				}
				goto DONE;
			}
			break;
		case FA_REMOVE_ROOTCERT:
			if (!m2m_memcmp(strEntryHeader.au8SHA1NameHash, pvCertBuf, sizeof(strEntryHeader.au8SHA1NameHash)))
			{
				// Found a match. Remove it.
				ret |= FA_RETURN_FLAG_INFO_CHANGED;
				m2m_memset(strEntryHeader.au8SHA1NameHash, 0, sizeof(strEntryHeader.au8SHA1NameHash));
				status = spi_flash_write(strEntryHeader.au8SHA1NameHash, *pu32Offset, sizeof(strEntryHeader.au8SHA1NameHash));
				if (status != M2M_SUCCESS)
				{
					ret |= FA_RETURN_FLAG_ERR_WINC_ACCESS;
					goto ERR;
				}
				ret |= FA_RETURN_FLAG_COMPLETE;
				goto DONE;
			}
			break;
		case FA_READIDX_ROOTCERT:
			if (m2m_memcmp(strEntryHeader.au8SHA1NameHash, pvCertBuf, sizeof(strEntryHeader.au8SHA1NameHash)))
			{
				// Entry is not empty. Decrement index. Done if we reach 0.
				if (u8ReadIdx-- == 0)
				{
					// Update contents of pu32BufLen with actual size.
					if (u16EntrySize <= *pu32BufLen)
						*pu32BufLen = u16EntrySize;
					else
					{
						ret |= FA_RETURN_FLAG_ERR_WINC_SZ;
						goto ERR;
					}
					goto DONE;
				}
			}
			break;
		case FA_PRUNE_ROOTCERT:
			if (m2m_memcmp(strEntryHeader.au8SHA1NameHash, pvCertBuf, sizeof(strEntryHeader.au8SHA1NameHash)))
			{
				// Entry is not empty. Copy it to u32PruneOffset. Use gau8Buff.
				status = spi_flash_read(gau8Buff, *pu32Offset, u16EntrySize);
				if (status == M2M_SUCCESS)
					spi_flash_write(gau8Buff, u32PruneOffset, u16EntrySize);
				if (status != M2M_SUCCESS)
				{
					ret |= FA_RETURN_FLAG_ERR_WINC_ACCESS;
					goto ERR;
				}
				u32PruneOffset += u16EntrySize;
			}
			break;
		default:
			// No other item modes supported.
			goto ERR;
			break;
		}
		*pu32Offset += u16EntrySize;
	}
	// Reached the end of the flash section.
	switch (enuMode)
	{
	case FA_ADD_ROOTCERT:
		ret |= FA_RETURN_FLAG_ERR_WINC_SZ;
		goto ERR;
	case FA_READ_ROOTCERT:
	case FA_READIDX_ROOTCERT:
	case FA_REMOVE_ROOTCERT:
		ret |= FA_RETURN_FLAG_ERR_WINC_ITEM;
		goto ERR;
	case FA_PRUNE_ROOTCERT:
		*pu32Offset = M2M_TLS_ROOTCER_FLASH_OFFSET;
		goto DONE;
	default:
		// No other item modes supported.
		goto ERR;
	}
ERR:
	*pu32Offset = 0;
	ret |= FA_RETURN_FLAG_ERR;
DONE:
	return ret;
}

FlashAccessErr_t m2m_flash_access_run(void)
{
	/*
	 *	Errors before start of first transfer will be reported as parameter errors.
	 *	This means the information is insufficient to allow us to begin.
	 */
	FlashAccessErr_t		ret = FA_RETURN_OK;
	sint8					status = M2M_ERR_FAIL;

	tstrFlashAccessInfo		*pstrWriteInfo = &gstrFlashAccess.strDestinationInfo;
	tstrFlashAccessInfo		*pstrReadInfo = &gstrFlashAccess.strSourceInfo;
	uint32_t				u32BytesTransferred = 0;
	uint32_t				u32BytesRemaining = gstrFlashAccess.u32Size;
	uint8					u8WriteFnFlags = FA_FN_FLAGS_WRITE;

	if (gstrFlashAccess.u8AccessFlags & FA_ACCESS_OPTION_COMPARE_BEFORE)
		u8WriteFnFlags |= FA_FN_FLAGS_COMPARE_BEFORE;
	if (gstrFlashAccess.u8AccessFlags & FA_ACCESS_OPTION_ERASE_FIRST)
	{
		u8WriteFnFlags |= FA_FN_FLAGS_ERASE;
		if (gstrFlashAccess.u8AccessFlags & FA_ACCESS_OPTION_KEEP_SURROUNDING)
		{
			u8WriteFnFlags |= FA_FN_FLAGS_READ_SURROUNDING;
			if (gstrFlashAccess.u8AccessFlags & FA_ACCESS_OPTION_USE_BACKUP)
				u8WriteFnFlags |= FA_FN_FLAGS_BACKUP;
		}
	}
	if (gstrFlashAccess.u8AccessFlags & FA_ACCESS_OPTION_COMPARE_AFTER)
		u8WriteFnFlags |= FA_FN_FLAGS_COMPARE_AFTER;

	if (gstrFlashAccess.u8ModeFlags & FA_MODE_FLAGS_CHECK_BACKUP)
	{
		status = winc_flash_control(0,0,0,FA_FN_CTL_CHECK_BACKUP);
		if (status != M2M_SUCCESS)
		{
			ret |= FA_RETURN_FLAG_ERR_WINC_ACCESS;
			goto ERR;
		}
	}
	/*
	 *	Initialize control sector. Even if we don't need to access it, this at
	 *	least ensures that the control sector is not relying on the flash backup sector.
	 */
	status = access_control_sector(CS_INITIALIZE, NULL);
	if (status != M2M_SUCCESS && (gstrFlashAccess.u8ModeFlags & FA_MODE_FLAGS_CS))
	{
		ret |= FA_RETURN_FLAG_ERR_WINC_ACCESS;
		goto ERR;
	}

	if (u32BytesRemaining > 0)
	{
		/* Prepare for read and write. */
		status = pstrReadInfo->pfCtlFunction(pstrReadInfo->u32LocationId, u32BytesRemaining, FA_FN_FLAGS_READ, FA_FN_CTL_INITIALIZE);
		if (status == M2M_SUCCESS)
			status = pstrWriteInfo->pfCtlFunction(pstrWriteInfo->u32LocationId, u32BytesRemaining, u8WriteFnFlags, FA_FN_CTL_INITIALIZE);
		if (status != M2M_SUCCESS)
		{
			ret |= FA_RETURN_FLAG_ERR_LOCAL_ACCESS;
			goto ERR;
		}
	}

	while (u32BytesRemaining > 0)
	{
		uint32	chunk_size = sizeof(gau8Buff);
		uint32	before_chunk_size = 0;

		if ((u32BytesTransferred == 0) && (pstrWriteInfo->u32AlignmentSize > 0))
		{
			before_chunk_size = pstrWriteInfo->u32StartAlignment & (pstrWriteInfo->u32AlignmentSize-1);
			chunk_size = pstrWriteInfo->u32AlignmentSize - before_chunk_size;
		}

		if (chunk_size > u32BytesRemaining)
			chunk_size = u32BytesRemaining;

		/* Read. */
		status = pstrReadInfo->pfFunction(gau8Buff, sizeof(gau8Buff), before_chunk_size, chunk_size);
		if (status != M2M_SUCCESS)
			break;

		if (gstrFlashAccess.u8ModeFlags & FA_MODE_FLAGS_UNCHANGED)
		{
			ret |= FA_RETURN_FLAG_INFO_CHANGED;
			gstrFlashAccess.u8ModeFlags &= ~FA_MODE_FLAGS_UNCHANGED;
			spi_flash_write((uint8*)&gstrFlashAccess, HOST_CONTROL_FLASH_OFFSET, FA_PERM_SZ);
		}

		/* Write. */
		status = pstrWriteInfo->pfFunction(gau8Buff, sizeof(gau8Buff), before_chunk_size, chunk_size);
		if (status != M2M_SUCCESS)
			break;

		u32BytesTransferred += chunk_size;
		u32BytesRemaining -= chunk_size;
	}

	if (gstrFlashAccess.u32Size > 0)
	{
		if (u32BytesRemaining > 0)
		{
			pstrReadInfo->pfCtlFunction(pstrReadInfo->u32LocationId, 0, 0, FA_FN_CTL_TERMINATE);
			pstrWriteInfo->pfCtlFunction(pstrWriteInfo->u32LocationId, 0, 0, FA_FN_CTL_TERMINATE);
			ret |= FA_RETURN_FLAG_ERR_LOCAL_ACCESS;
			goto ERR;
		}
		else
		{
			pstrReadInfo->pfCtlFunction(pstrReadInfo->u32LocationId, 0, 0, FA_FN_CTL_COMPLETE);
			pstrWriteInfo->pfCtlFunction(pstrWriteInfo->u32LocationId, 0, 0, FA_FN_CTL_COMPLETE);
		}
	}

	if (gstrFlashAccess.u8ModeFlags & (FA_MODE_FLAGS_CS_VALIDATE_IMAGE | FA_MODE_FLAGS_CS_SWITCH))
	{
		tenuCSOp enuOp;
		if (gstrFlashAccess.u8ModeFlags & FA_MODE_FLAGS_UNCHANGED)
		{
			ret |= FA_RETURN_FLAG_INFO_CHANGED;
			gstrFlashAccess.u8ModeFlags &= ~FA_MODE_FLAGS_UNCHANGED;
			spi_flash_write((uint8*)&gstrFlashAccess, HOST_CONTROL_FLASH_OFFSET, FA_PERM_SZ);
		}
		if (gstrFlashAccess.u8ModeFlags & FA_MODE_FLAGS_CS_VALIDATE_IMAGE)
		{
			if (gstrFlashAccess.u8ModeFlags & FA_MODE_FLAGS_CS_SWITCH)
				enuOp = CS_VALIDATE_SWITCH;
			else
				enuOp = CS_VALIDATE_RB;
		}
		else
			enuOp = CS_SWITCH;
		status = access_control_sector(enuOp, NULL);
		if (status != M2M_SUCCESS)
		{
			ret |= FA_RETURN_FLAG_ERR_WINC_ACCESS;
			goto ERR;
		}
	}
	access_control_sector(CS_DEINITIALIZE, NULL);

	ret |= FA_RETURN_FLAG_COMPLETE;
	gstrFlashAccess.enuTransferStatus = FA_STATUS_DONE;
	spi_flash_write((uint8*)&gstrFlashAccess, HOST_CONTROL_FLASH_OFFSET, FA_PERM_SZ);
	return ret;
ERR:
	ret |= FA_RETURN_FLAG_ERR;
	access_control_sector(CS_DEINITIALIZE, NULL);
	return ret;
}
