/*********************************************************************
 *                                                                    
 * Software License Agreement                                         
 *                                                                    
 * Copyright © 2016-2017 Microchip Technology Inc.  All rights reserved.
 *
 * Microchip licenses to you the right to use, modify, copy and distribute 
 * Software only when embedded on a Microchip microcontroller or digital 
 * signal controller and used with a Microchip radio frequency transceiver, 
 * which are integrated into your product or third party product (pursuant 
 * to the terms in the accompanying license agreement).   
 *
 * You should refer to the license agreement accompanying this Software for 
 * additional information regarding your rights and obligations.
 *
 * SOFTWARE AND DOCUMENTATION ARE PROVIDED ?AS IS? WITHOUT WARRANTY OF ANY 
 * KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A 
 * PARTICULAR PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE 
 * LIABLE OR OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, 
 * CONTRIBUTION, BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY 
 * DIRECT OR INDIRECT DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO 
 * ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, 
 * LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF SUBSTITUTE GOODS, 
 * TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT 
 * NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.             
 *                                                                    
 *********************************************************************/
/**
 *
 * Copyright (c) 2016-2017 Atmel Corporation. All rights reserved.
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
 *    Atmel microcontroller product.
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
 * Copyright (c) 2016-2017 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */
 #include "asf.h"
 #include "rtc.h"
 #include "framework/miwi/miwi_api.h"
 #include "sio2host.h"
 #include "extint_callback.h"
 #include "config/button.h"
 #include "config/timer.h"
 #include "config/console.h"
 #include "rtc.h"


const uint8_t MiWi_Data[6][21] =
{
	{0x20,0xB2,0x20,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0xB2,0x20,0x20,0x20,0x20,0x20,0xB2,0x20,0xB2,0x0D,0x0A},
	{0xB2,0x20,0xB2,0x20,0xB2,0x20,0xB2,0x20,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0x20,0x0D,0x0A},
	{0xB2,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0xB2,0x20,0xB2,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0xB2,0x0D,0x0A},
	{0xB2,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0xB2,0x20,0x20,0xB2,0x20,0xB2,0x20,0xB2,0x20,0x20,0xB2,0x0D,0x0A},
	{0xB2,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0xB2,0x20,0x20,0x20,0xB2,0x20,0xB2,0x20,0x20,0x20,0xB2,0x0D,0x0A},
	{0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x0D,0x0A}
};

const uint8_t DE[6][11] =
{
	{0xB2,0xB2,0xB2,0x20,0x20,0xB2,0xB2,0xB2,0xB2,0x0D,0x0A},
	{0xB2,0x20,0x20,0xB2,0x20,0xB2,0x20,0x20,0x20,0x0D,0x0A},
	{0xB2,0x20,0x20,0xB2,0x20,0xB2,0xB2,0xB2,0xB2,0x0D,0x0A},
	{0xB2,0x20,0x20,0xB2,0x20,0xB2,0x20,0x20,0x20,0x0D,0x0A},
	{0xB2,0xB2,0xB2,0x20,0x20,0xB2,0xB2,0xB2,0xB2,0x0D,0x0A},
	{0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x0D,0x0A}
};
// Watch Dog Timer Support
//! [setup]
static void watchdog_early_warning_callback(void)
{
	port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE);
}
static void configure_wdt(void)
{
	/* Create a new configuration structure for the Watchdog settings and fill
		* with the default module settings. */
	//! [setup_1]
	struct wdt_conf config_wdt;
	//! [setup_1]
	//! [setup_2]
	wdt_get_config_defaults(&config_wdt);
	//! [setup_2]

	/* Set the Watchdog configuration settings */
	//! [setup_3]
	config_wdt.always_on            = false;
#if !((SAML21) || (SAMC21) || (SAML22) || (SAMR30) || (SAMR34))
	config_wdt.clock_source         = GCLK_GENERATOR_1;
#endif
	config_wdt.timeout_period       = WDT_PERIOD_16384CLK;
	config_wdt.early_warning_period = WDT_PERIOD_8192CLK;
	//! [setup_3]

	/* Initialize and enable the Watchdog with the user settings */
	//! [setup_4]
	wdt_set_config(&config_wdt);
	//! [setup_4]
}
 static void configure_wdt_callbacks(void)
{
	//! [setup_5]
	wdt_register_callback(watchdog_early_warning_callback,
		WDT_CALLBACK_EARLY_WARNING);
	//! [setup_5]

	//! [setup_6]
	wdt_enable_callback(WDT_CALLBACK_EARLY_WARNING);
	//! [setup_6]
}

static void print_reset_causes(void)
{
	enum system_reset_cause rcause = system_get_reset_cause();
	printf("\r\nLast reset cause: ");
	if(rcause & (1 << 6)) {
		printf("System Reset Request\r\n");
	}
	if(rcause & (1 << 5)) {
		printf("Watchdog Reset\r\n");
	}
	if(rcause & (1 << 4)) {
		printf("External Reset\r\n");
	}
	if(rcause & (1 << 2)) {
		printf("Brown Out 33 Detector Reset\r\n");
	}
	if(rcause & (1 << 1)) {
		printf("Brown Out 12 Detector Reset\r\n");
	}
	if(rcause & (1 << 0)) {
		printf("Power-On Reset\r\n");
	}
}

void toggleLED(uint8_t val){
	uint8_t timer = 10;
	while(timer--)
	{
		LED_Toggle(val);
		delay_ms(100);
	}
}
int main(void)
{	
	uint8_t i , button_press;
	uint8_t connection_index = 0xFF;
	// Hardware Init
	system_init();
	delay_init();
	// UART Init for Console
	sio2host_init();
	// Timer Init for Stack Related Stuff
	configure_tc();
	// RTC Init for wake up from Backup Sleep Mode
	rtc_init();

	/*******************************************************************/
	// Initialize Microchip proprietary protocol. Which protocol to use
	// depends on the configuration in ConfigApp.h
	/*******************************************************************/
	/*******************************************************************/
	// Function MiApp_ProtocolInit initialize the protocol stack. The
	// only input parameter indicates if previous network configuration
	// should be restored. In this simple example, we assume that the
	// network starts from scratch.
	/*******************************************************************/
	MiApp_ProtocolInit(false);
	// Set the Channel 
	MiApp_SetChannel(APP_CHANNEL_SEL);
	/*******************************************************************/
	// Function MiApp_ConnectionMode defines the connection mode. The
	// possible connection modes are:
	//  ENABLE_ALL_CONN:    Enable all kinds of connection
	//  ENABLE_PREV_CONN:   Only allow connection already exists in
	//                      connection table
	//  ENABL_ACTIVE_SCAN_RSP:  Allow response to Active scan
	//  DISABLE_ALL_CONN:   Disable all connections.
	/*******************************************************************/
	MiApp_ConnectionMode(ENABLE_ALL_CONN);
	// Enable Transceiver Interrupt 
	ENABLE_TRX_IRQ();
	// Enable All cpu interrupts 
	cpu_irq_enable();
	printf("\033[2J\r\n ");
	print_reset_causes();
	while(connection_index == 0xFF)
	{
		toggleLED(LED0);
		connection_index  = MiApp_EstablishConnection(0xFF, CONN_MODE_DIRECT);
		printf("Waiting to Join\r\n");
	}
	printf("Total Connections:%d\r\n", Total_Connections());
	#if defined(ED)
		printf("joined as a End Device....\r\n");
		at30tse_init();
	#endif


	/*******************************************************************/
	// Display current opertion on LCD of demo board, if applicable
	/*******************************************************************/
	if( connection_index  != 0xFF )
	{
		LED_On(LED0);
	}
	// Configure watchdog for handling failures. reset will be initiated if watchdog count is not reset
	configure_wdt();
	// Configure watchdog call back
	configure_wdt_callbacks();

	while (1) {

		wdt_reset_count();
		// Data Packet Available
		if (MiApp_MessageAvailable())
		{
			/*******************************************************************/
			// Function MiApp_DiscardMessage is used to release the current
			//  received packet.
			// After calling this function, the stack can start to process the
			//  next received frame
			/*******************************************************************/
			MiApp_DiscardMessage();

		}
		#if defined(ED)
			MiApp_FlushTx();
			LED_Toggle(LED0);
			// Tx Buffer User Data
			MiApp_WriteData(NodeID);
			MiApp_WriteData( at30tse_read_temperature()*1.8+32);
			//This function unicasts a message in the TxBuffer to the first connected peer device
			// indexed at 0 in connection table
			MiApp_BroadcastPacket(true); // Send Packet to Parent Device
			delay_ms(50);
			PHY_Sleep();
			setSleepPeriod(60); // 65 = 16 min
		#endif
	}
}