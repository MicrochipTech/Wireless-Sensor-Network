/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to system_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include <asf.h>
#include "ecc_provision.h"

#include "conf_console.h"
#include "usart.h"
#include "m2m_wifi.h"

static struct usart_module cdc_uart_module;

/**
 *  Configure console.
 */
void serial_console_init(void)
{
 	struct usart_config usart_conf;

	usart_get_config_defaults(&usart_conf);
	usart_conf.mux_setting = CONF_STDIO_MUX_SETTING;
	usart_conf.pinmux_pad0 = CONF_STDIO_PINMUX_PAD0;
	usart_conf.pinmux_pad1 = CONF_STDIO_PINMUX_PAD1;
	usart_conf.pinmux_pad2 = CONF_STDIO_PINMUX_PAD2;
	usart_conf.pinmux_pad3 = CONF_STDIO_PINMUX_PAD3;
	usart_conf.baudrate    = CONF_STDIO_BAUDRATE;

	stdio_serial_init(&cdc_uart_module, CONF_STDIO_USART_MODULE, &usart_conf);
	usart_enable(&cdc_uart_module);
}

int main (void)
{
	ATCA_STATUS status = ATCA_SUCCESS;
			
	/* system clock initialization */
	system_init();

	/* Initialize serial console for debugging */
	serial_console_init();

	/* delay routine initialization */
	delay_init();
	
	printf("Provision ECC608 crypto device\r\n");
	printf("cpu_freq=%d\r\n",(int)system_cpu_clock_get_hz());

	/*
		To use I2C  to provision ECC608
		make sure ATCA_HAL_I2C is defined in the project
	*/

    /*
		 Step 1: Initialize the CryptoAuthLib library:
		 
		 Enable the code below, it's initializes the library and query basic info.
		 build and check that your device is alive and well configured.
	*/
#if 1
	status = cryptoauthlib_init();
	if (status == ATCA_SUCCESS)
	{
		printf("The ATECC508A device is configured and locked.\r\n");
	}
#endif	
	
	/*
	 Step 2: Generate a new Private key and get the corresponding public key:
	 
		1. The current configurations of ATECC608 sets SLOT_0 for an ECDSA Key.
		2. Each time the code below runs, it will generate a new private and public key in slot 0.
		3. When the slot is locked, only then you can't change the slot private key.
		4. Enable the code below and the Public Key will be on the terminal. 
		5. The private key will remain secret.
	 */
#if 1
    status = genKeyPair();
    if (status != ATCA_SUCCESS)
    {
	    printf("Failed to get the ECC pub Key.\r\n");
    }	
#endif

	/* 
		Step 3: Generate a CSR for the WINC device certificate
			1. Use the key pair from step 2 to generate a Certificate signing request.
			2. The public key and basic company info is added to the CSR and then signed by the private key in slot 0
			3. Enable the code below, The generated CSR (in DER format) will be printed on the console.
	*/
#if 1
	status = genDeviceCSR();
	if (status != ATCA_SUCCESS)
	{
		printf("Failed to get the ECC CSR.\r\n");
	}
#endif


	/* 
		Step 4: Download Device & Signer CA certificates to WINC1500
			1. Enable the code below to put WINC in FW download mode .
			2. The public key and basic company info is added to the CSR and then signed by the private key in slot 0
			3. Enable the code below, The generated CSR (in DER format) will be printed on the console.
	*/
#if 1
	/* Initialize BSP and Bus Interface */
	if(0 !=nm_bsp_init())
	{
		printf("nm_bsp_init.\r\n");
		while(1);
	}
	
		printf("WINC is in download mode, now go and run the script.\r\n");
	
	while(1);
#endif
}
