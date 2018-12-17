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
#include <asf.h>
#include "main.h"
#include "nvm_handle.h"


#define FLASH_NVDS_SIZE		64
#define TEST_PAGE_ADDR     (FLASH_NB_OF_PAGES - 16) * NVMCTRL_PAGE_SIZE

uint8_t NVDS_Data[FLASH_NVDS_SIZE] = {0};
	
int nvds_erase(void)//sector erase
{
	uint32_t delay = 0;
	uint32_t i;
	uint32_t len_buf=FLASH_PAGE_SIZE;
	int ret;
	//uint8_t read_buf[len_buf];
	//uint8_t write_buf[len_buf];

	DBG_LOG("nvds_erase start\r\n");

	
	memset(NVDS_Data,0xff,sizeof(NVDS_Data));
	for(i=0;i<16;i++)
	{
		nvm_erase_row((unsigned long)(TEST_PAGE_ADDR+i*NVMCTRL_PAGE_SIZE));
		ret = nvm_write_buffer((unsigned long)(TEST_PAGE_ADDR+i*NVMCTRL_PAGE_SIZE),
		(uint8_t *)&NVDS_Data[i*NVMCTRL_PAGE_SIZE],
		NVMCTRL_PAGE_SIZE);
	}
}


int nvm_init(void)
{
	struct nvm_config config;
	enum status_code status;

	/* Get the default configuration */
	nvm_get_config_defaults(&config);

	/* Enable automatic page write mode */
	config.manual_page_write = false;

	/* Set the NVM configuration */
	status = nvm_set_config(&config);


	DBG_LOG("status = %d", status);

	return status;
}



	
void nvm_store_temperature(int16_t temp, int16_t count)	
{
	enum status_code error_code;
	
	memcpy(NVDS_Data,(uint8_t *)&temp,sizeof(int16_t));
	memcpy(NVDS_Data+2,(uint8_t *)&count,sizeof(int16_t));
	
	error_code = nvm_erase_row((unsigned long)(TEST_PAGE_ADDR));
	error_code = nvm_write_buffer((unsigned long)(TEST_PAGE_ADDR),
	(uint8_t *)NVDS_Data,
	NVMCTRL_PAGE_SIZE);
}

	
int16_t nvm_get_temperature(void)	
{
	int16_t temp;
	enum status_code error_code;
	struct nvm_parameters parameters;
	
	nvm_get_parameters(&parameters);

	error_code = nvm_read_buffer((unsigned long)(TEST_PAGE_ADDR),
	(uint8_t *)NVDS_Data,
	NVMCTRL_PAGE_SIZE);

	memcpy((uint8_t *)&temp,NVDS_Data,sizeof(int16_t));	
	return temp;
}

int16_t nvm_get_skip_count(void)	
{
	int16_t skipCount;
	enum status_code error_code;
	struct nvm_parameters parameters;
	
	nvm_get_parameters(&parameters);

	error_code = nvm_read_buffer((unsigned long)(TEST_PAGE_ADDR),
	(uint8_t *)NVDS_Data,
	NVMCTRL_PAGE_SIZE);

	memcpy((uint8_t *)&skipCount,NVDS_Data+2,sizeof(int16_t));	
	return skipCount;
}