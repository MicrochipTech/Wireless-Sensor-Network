/**
* \file
*
* \brief Multi-Role/Multi-Connect Application
*
* Copyright (c) 2016 Atmel Corporation. All rights reserved.
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
* 4. This software may only be redistributed and used in connection with an
*    Atmel micro controller product.
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

/*
* Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel
*Support</a>
*/

/**
* \mainpage
* \section preface Preface
* This is the reference manual for the Multi-Role/Multi-Connect Application
*/
/*- Includes ---------------------------------------------------------------*/


#include "atcacert/atcacert_client.h"
#include "atcacert/atcacert_host_hw.h"
#include "atcacert/atcacert_def.h"
#include "basic/atca_basic.h"
#include "ecc_provision.h"


const uint8_t g_csr_template_3_device[] = {
	0x30, 0x81, 0xfb, 0x30, 0x81, 0xa2, 0x02, 0x01,  0x00, 0x30, 0x2f, 0x31, 0x14, 0x30, 0x12, 0x06,
	0x03, 0x55, 0x04, 0x0a, 0x0c, 0x0b, 0x45, 0x78,  0x61, 0x6d, 0x70, 0x6c, 0x65, 0x20, 0x49, 0x6e,
	0x63, 0x31, 0x17, 0x30, 0x15, 0x06, 0x03, 0x55,  0x04, 0x03, 0x0c, 0x0e, 0x45, 0x78, 0x61, 0x6d,
	0x70, 0x6c, 0x65, 0x20, 0x44, 0x65, 0x76, 0x69,  0x63, 0x65, 0x30, 0x59, 0x30, 0x13, 0x06, 0x07,
	0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02, 0x01, 0x06,  0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x03, 0x01,
	0x07, 0x03, 0x42, 0x00, 0x04, 0xd8, 0x70, 0xa4,  0xdf, 0x98, 0xb4, 0x6a, 0x93, 0x2b, 0xf7, 0x40,
	0x39, 0x86, 0x0f, 0xed, 0xd6, 0x69, 0x03, 0x6a,  0xe7, 0xe4, 0x84, 0x9f, 0xfc, 0xfb, 0x61, 0x50,
	0x63, 0x21, 0x95, 0xa8, 0x91, 0x2c, 0x98, 0x04,  0x0e, 0x9c, 0x2f, 0x03, 0xe1, 0xe4, 0x2e, 0xc7,
	0x93, 0x8c, 0x6b, 0xf4, 0xfb, 0x98, 0x4c, 0x50,  0xdb, 0x51, 0xa3, 0xee, 0x04, 0x1b, 0x55, 0xf0,
	0x60, 0x63, 0xeb, 0x46, 0x90, 0xa0, 0x11, 0x30,  0x0f, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7,
	0x0d, 0x01, 0x09, 0x0e, 0x31, 0x02, 0x30, 0x00,  0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce,
	0x3d, 0x04, 0x03, 0x02, 0x03, 0x48, 0x00, 0x30,  0x45, 0x02, 0x20, 0x26, 0xab, 0x8a, 0x4f, 0x71,
	0x2c, 0xf9, 0xbb, 0x4f, 0xfa, 0xa4, 0xcd, 0x01,  0x48, 0xf1, 0xdf, 0x9c, 0xdc, 0xff, 0xa0, 0xff,
	0x53, 0x8f, 0x35, 0x8d, 0xd4, 0x3d, 0x49, 0xc0,  0x72, 0xf5, 0x0a, 0x02, 0x21, 0x00, 0xa5, 0x9d,
	0xb4, 0x11, 0x4b, 0xa1, 0x65, 0x7c, 0xbb, 0x48,  0xcf, 0x6d, 0xf6, 0xd0, 0x6a, 0x41, 0x00, 0x96,
	0xe1, 0xe2, 0x79, 0x73, 0xdb, 0xf7, 0x97, 0x80,  0x41, 0x9b, 0x35, 0x01, 0x88, 0x5e
};
// Default AWS config for the ECCx08A.  The first 16 bytes are device specific and are not copied
//
uint8_t aws_config[] = {
	
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,   0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	AWS_ECCx08A_I2C_ADDRESS , 0x00, 0xAA, 0x00, 0x8F, 0x20, 0xC4, 0x44,   0x87, 0x20, 0x87, 0x20, 0x8F, 0x0F, 0xC4, 0x36,
	0x9F, 0x0F, 0x82, 0x20, 0x0F, 0x0F, 0xC4, 0x44,   0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x0F, 0x0F, 0x0F, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF,   0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x33, 0x00, 0x1C, 0x00, 0x13, 0x00, 0x13, 0x00,   0x7C, 0x00, 0x1C, 0x00, 0x3C, 0x00, 0x33, 0x00,
	0x3C, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x30, 0x00,   0x3C, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x30, 0x00
};
//! Mutable device description
ATCAIfaceCfg      g_crypto_device;
const atcacert_def_t g_csr_def_3_device = {
	.type                   = CERTTYPE_X509,
	.template_id            = 3,
	.chain_id               = 0,
	.private_key_slot       = 0,
	.sn_source              = SNSRC_PUB_KEY_HASH,
	.cert_sn_dev_loc        = {
		.zone      = DEVZONE_NONE,
		.slot      = 0,
		.is_genkey = 0,
		.offset    = 0,
		.count     = 0
	},
	.issue_date_format      = DATEFMT_RFC5280_UTC,
	.expire_date_format     = DATEFMT_RFC5280_UTC,
	.tbs_cert_loc           = {
		.offset = 3,
		.count  = 165
	},
	.expire_years           = 0,
	.public_key_dev_loc     = {
		.zone      = DEVZONE_NONE,
		.slot      = 0,
		.is_genkey = 1,
		.offset    = 0,
		.count     = 64
	},
	.comp_cert_dev_loc      = {
		.zone      = DEVZONE_NONE,
		.slot      = 0,
		.is_genkey = 0,
		.offset    = 0,
		.count     = 0
	},
	.std_cert_elements      = {
		{ // STDCERT_PUBLIC_KEY
			.offset = 85,
			.count  = 64
		},
		{ // STDCERT_SIGNATURE
			.offset = 180,
			.count  = 74
		},
		{ // STDCERT_ISSUE_DATE
			.offset = 0,
			.count  = 0
		},
		{ // STDCERT_EXPIRE_DATE
			.offset = 0,
			.count  = 0
		},
		{ // STDCERT_SIGNER_ID
			.offset = 0,
			.count  = 0
		},
		{ // STDCERT_CERT_SN
			.offset = 0,
			.count  = 0
		},
		{ // STDCERT_AUTH_KEY_ID
			.offset = 0,
			.count  = 0
		},
		{ // STDCERT_SUBJ_KEY_ID
			.offset = 0,
			.count  = 0
		}
	},
	.cert_elements          = NULL,
	.cert_elements_count    = 0,
	.cert_template          = g_csr_template_3_device,
	.cert_template_size     = sizeof(g_csr_template_3_device)
};

#define AWS_ECC508A_I2C_ADDRESS  (uint8_t)(0xB0)  //! AWS ECC508A device I2C address


uint8_t public_key[ATCA_PUB_KEY_SIZE];
//local function prototypes
ATCA_STATUS configure_device(uint8_t new_device_address);

ATCA_STATUS detect_crypto_device()
{
	ATCA_STATUS  status;
	static bool attachedDevices[3];    //array to keep track of the devices detected
	uint8_t cur_config[ATCA_ECC_CONFIG_SIZE];

	// do device detection
	memset(attachedDevices, 0, sizeof(attachedDevices));
	
	g_crypto_device = cfg_ateccx08a_i2c_default;
	g_crypto_device.atcai2c.slave_address = ECCx08A_DEFAULT_ADDRESS;

	// detect any devices connected with factory default address, ECCx08A_DEFAULT_ADDRESS
	status = atcab_init(&g_crypto_device);
	if(status != ATCA_SUCCESS)
	return status;
	
	status = atcab_read_config_zone(cur_config);
	if(status == ATCA_RX_CRC_ERROR)
	{
		// corrupted data received.  A likely cause is that there are multiple devices with the same address attached.
		// firmware bug in WINC will cause a CRC Error on the default address if it's connected
		attachedDevices[DEV_INVALID] = true;
	}
	else if(status == ATCA_TOO_MANY_COMM_RETRIES)
	// no device found
	attachedDevices[DEV_UNCONF] = false;
	else if(status == ATCA_SUCCESS)
	// a single unconfigured device was found at ECCx08A_DEFAULT_ADDRESS
	attachedDevices[DEV_UNCONF] = true;
	else
	// other error
	return status;


	// try to communicate with AWS_ECCx08A_I2C_ADDRESS
	g_crypto_device.atcai2c.slave_address = AWS_ECCx08A_I2C_ADDRESS;
	atcab_init(&g_crypto_device);
	status = atcab_read_config_zone(cur_config);
	
	if(status == ATCA_TOO_MANY_COMM_RETRIES)
	// no devices found
	attachedDevices[DEV_CRYPTO] = false;
	else if(status == ATCA_SUCCESS)
	// device was found and able to be read from, assuming WINC-based crypto device
	attachedDevices[DEV_CRYPTO] = true;
	else
	// other error
	return status;
	
	// device detection completed, now do appropriate configuration...
	
	if(attachedDevices[DEV_CRYPTO])
	// pre-configured crypto device found, proceed with demo
	return ATCA_SUCCESS;
	else if(attachedDevices[DEV_INVALID])
	// invalid data found, probably because multiple devices connected
	return ATCA_RX_CRC_ERROR;
	else if(attachedDevices[DEV_UNCONF])
	// found unconfigured crypto device
	return ATCA_GEN_FAIL;
	else
	// no crypto devices were found
	return ATCA_NO_DEVICES;
}


void printDevDetectStatus(ATCA_STATUS status)
{
	if(status == ATCA_NO_DEVICES)
	{
		// no device detected
		printf("The AWS IoT Zero Touch Demo ATECCx08A pre-config has not completed.\r\n");

		printf("No attached CryptoAuth board detected.\r\n");
		printf("Please check your hardware configuration.\r\n");
		printf("Stopping the AWS IoT demo.\r\n");
	}
	else if(status == ATCA_RX_CRC_ERROR)
	{
		// bad data received, likely because multiple crypto devices are on the same address
		printf("The AWS IoT Zero Touch Demo ATECCx08A pre-config has not completed.\r\n");

		printf("Unconfigured CryptoAuth board connected while WINC1500 connected.\r\n");
		printf("Please disconnect WINC1500 and restart the demo.\r\n");
		printf("Stopping the AWS IoT demo.\r\n");
	}
	else
	{
		// Other error, stop the demo

		// Set the current state
		printf("The AWS IoT Zero Touch Demo ATECCx08A pre-config has not completed.\r\n");

		printf("Unknown error trying to communicate with CryptoAuth board.\r\n");
		printf("Please check your hardware configuration.\r\n");
		printf("Stopping the AWS IoT demo.\r\n");
	}
}

ATCA_STATUS preconfigure_crypto_device()
{
	ATCA_STATUS  status;
	bool isLocked = false;
	uint8_t slots_to_genkey[] = {0, 2, 3, 7};
	
	// setup to talk to initailly unconfigured device
	g_crypto_device = cfg_ateccx08a_i2c_default;
	g_crypto_device.atcai2c.slave_address = ECCx08A_DEFAULT_ADDRESS;

	status = atcab_init(&g_crypto_device);
	if(status != ATCA_SUCCESS)
	return status;

	// start the configuration

	// configure as an AWS ECCx08...
	printf("Configuring CryptoAuth Board now...\r\n");
	
	// check to see if the Config Zone is already locked
	status = atcab_is_locked(ATCA_ZONE_CONFIG, &isLocked);
	if(status != ATCA_SUCCESS)
	return status;
	
	if(isLocked)
	{
		printf("Cannot configure Config Zone - Config Zone already locked.\r\n");
		return ATCA_CONFIG_ZONE_LOCKED;
	}
	
	// write the entire AWS config to the device
	status = atcab_write_config_zone(aws_config);
	if(status != ATCA_SUCCESS)
	return status;
	
	// lock the device in preparation for the actual demo
	status = atcab_lock_config_zone();
	if(status != ATCA_SUCCESS)
	return status;

	status = atcab_lock_data_zone();
	if(status != ATCA_SUCCESS)
	return status;
	
	
	// put the newly configured device to sleep to have the changes take effect.
	atcab_wakeup();
	atcab_sleep();
	
	// setup to talk to newly configured AWS device
	g_crypto_device.atcai2c.slave_address = AWS_ECCx08A_I2C_ADDRESS;

	status = atcab_init(&g_crypto_device);
	if(status != ATCA_SUCCESS)
	return status;
	
	// Generate private keys for slots
	for(uint16_t i=0; i < (sizeof(slots_to_genkey)/sizeof(slots_to_genkey[0])); i++)
	{
		status = atcab_genkey(slots_to_genkey[i], NULL);
		if(status != ATCA_SUCCESS)
		{
			printf("Could not generate key on slot.\r\n");
			return status;
		}
	}
	
	// all done
	return ATCA_SUCCESS;
}
static const char* bin2hex(const void* data, size_t data_size)
{
	static char buf[256];
	static char hex[] = "0123456789abcdef";
	const uint8_t* data8 = data;
	
	if (data_size*2 > sizeof(buf)-1)
	return "[buf too small]";
	
	for (size_t i = 0; i < data_size; i++)
	{
		buf[i*2 + 0] = hex[(*data8) >> 4];
		buf[i*2 + 1] = hex[(*data8) & 0xF];
		data8++;
	}
	buf[data_size*2] = 0;
	
	return buf;
}

static int eccPrintInfo(void)
{
	ATCA_STATUS atca_status;
	uint8_t sn[9];
	uint8_t dev_rev[4];

	atca_status = atcab_read_serial_number(sn);
	if (atca_status)
	{
		printf("atcab_read_serial_number() failed with ret=0x%08X\r\n", atca_status);
		return atca_status;
	}
	
	atca_status = atcab_info(dev_rev);
	if (atca_status)
	{
		printf("atcab_info() failed with ret=0x%08X\r\n", atca_status);
		return atca_status;
	}

	printf("Crypto Device:\r\n");
	printf("  SN:          %s\r\n", bin2hex(sn, sizeof(sn)));
	printf("  DevRev:      %s\r\n", bin2hex(dev_rev, sizeof(dev_rev)));

	return 0;
}

ATCA_STATUS read_ecc_confiuration(void)
{
	ATCA_STATUS ret = 0;
	uint8_t config64[64];
	
	// Read the first 64 bytes of the config zone to get the slot config at least
	ret = atcab_read_zone(ATCA_ZONE_CONFIG, 0, 0, 0, &config64[0], 32);
	if (ret != ATCA_SUCCESS) return ret;
	ret = atcab_read_zone(ATCA_ZONE_CONFIG, 0, 1, 0, &config64[32], 32);
	if (ret != ATCA_SUCCESS) return ret;
	
	for(int i =0; i<90;i++)
		printf("config[%d]=%x\r\n",i,config64[i]);
}

/**
 * \brief Initializes the CryptoAuthLib library
 *
 * \return  The status of the CryptoAuthLib initialization
 *            ATCA_SUCCESS - Returned on a successful CryptoAuthLib initialization
 */
ATCA_STATUS cryptoauthlib_init(void)
{
    ATCA_STATUS status = ATCA_NO_DEVICES;
    bool device_locked = false;
    uint8_t revision[INFO_SIZE];
    
    // Initialize the CryptoAuthLib library
    cfg_ateccx08a_i2c_default.atcai2c.slave_address = AWS_ECC508A_I2C_ADDRESS;
    
    do 
    {
        status = atcab_init(&cfg_ateccx08a_i2c_default);
        if (status != ATCA_SUCCESS)
        {
            printf("The CryptoAuthLib library failed to initialize.");
            
            // Break the do/while loop
            break;
        }

        // Force the ATECC508A to sleep
        atcab_wakeup();
        atcab_sleep();
        
        // Check to make sure the ATECC508A Config zone is locked    
	    status = atcab_is_locked(LOCK_ZONE_CONFIG, &device_locked);
        if (status != ATCA_SUCCESS)
        {
            printf("The ATECC508A device is not configured correctly.");
            
            // Break the do/while loop
            break;
        }
		
		if(!device_locked)
        {
	        printf("The ATECC508A device data zone is no configured. \r\n");
	        // Break the do/while loop
	        break;
        }

		eccPrintInfo();
		
        status = ATCA_SUCCESS;
    } while (false);    
    
    return status;
}




ATCA_STATUS genPublicKey(void)
{
	
	ATCA_STATUS atca_status = ATCA_STATUS_UNKNOWN;
	
    // Get the ATECC508A device public key
    memset(&public_key[0], 0, sizeof(public_key));
    atca_status = atcab_genkey_base(GENKEY_MODE_PUBLIC, DEVICE_KEY_SLOT,
    NULL, public_key);
    if (atca_status == ATCA_SUCCESS)
    {
		printf("\r\npublic_key: Begin\r\n");
		for(int i =0; i<ATCA_PUB_KEY_SIZE;i++)
			printf("%x",public_key[i]);
		
		printf("\r\npublic_key: end\r\n");
    }
    else
    {
	    printf("failed to get ECC pub Key\r\n");
    }

    return atca_status;	
}


ATCA_STATUS genKeyPair(void)
{
    ATCA_STATUS atca_status = ATCA_STATUS_UNKNOWN;
	
    // Generate a new ATECC508A Device ECC-p256 key pair
    memset(&public_key[0], 0, sizeof(public_key));
    atca_status = atcab_genkey(DEVICE_KEY_SLOT, public_key);
    if (atca_status == ATCA_SUCCESS)
    {
		printf("\r\npublic_key: Begin\r\n");
		for(int i =0; i<ATCA_PUB_KEY_SIZE;i++)
			printf("%x",public_key[i]);
		
		printf("\r\npublic_key: end\r\n");
	}
	else
		printf("failed to generate key pair\r\n");
}

ATCA_STATUS genDeviceCSR(void)
{
    ATCA_STATUS atca_status = ATCA_STATUS_UNKNOWN;
    uint8_t csr_buffer[1500];
    size_t csr_buffer_length = 0;
        
	// Generate the AWS IoT device CSR
    csr_buffer_length = sizeof(csr_buffer);
    atca_status = atcacert_create_csr(&g_csr_def_3_device, csr_buffer, &csr_buffer_length);
        
    if (atca_status == ATCA_SUCCESS)
    {
		printf("\r\ncsr: Begin, length = %d\r\n",csr_buffer_length);
	    for(int i =0; i<csr_buffer_length;i++)
			printf("%02x",csr_buffer[i]);
		
		printf("\r\ncsr: end\r\n");
		
		atca_status = ATCA_SUCCESS;
    }
    else
		printf("failed to generate device CSR\r\n");
		
	return atca_status;
}