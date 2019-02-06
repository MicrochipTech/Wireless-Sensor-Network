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
 #include "framework/miwi/miwi_api.h"
 #include "sio2host.h"
 #include "extint_callback.h"
 #include "config/button.h"
 #include "config/timer.h"
 #include "config/console.h"

#include <asf.h>
#include "bsp/include/nm_bsp_samd21_app.h"
#include "driver/include/m2m_wifi.h"
#include "driver/include/m2m_periph.h"
#include "main.h"
#include "m2m_ble.h"
#include "atmel_ble_api/include/at_ble_api.h"
#include "wifi_prov.h"

#include "utils/ble_utils.h"

#include "asf.h"
#include "sio2host.h"
//#include "wsndemo.h"
//#include "miwi_api.h"

#include "asf.h"
#include "main.h"
#include <string.h>
#include "common/include/nm_common.h"
#include "driver/source/nmspi.h"
#include "bus_wrapper/include/nm_bus_wrapper.h"
#include "driver/include/m2m_wifi.h"
#include "socket/include/socket.h"
#include "extint_callback.h"
#include "cloud_wrapper.h"
#include "driver/include/m2m_wifi.h"
#include "socket/include/socket.h"
#include "winc15x0.h"
#include "cloud_wrapper.h"
#include "iot_message.h"
#include "rtc.h"

#include <asf.h>
#include "conf_board.h"
#include "driver/include/m2m_wifi.h"
#include "socket/include/socket.h"
#include "main.h"
#include "winc15x0.h"
#include "cloud_wrapper.h"
#include "iot_message.h"
#include "framework/miwi/miwi_api.h"
#include "sio2host.h"
#include "extint_callback.h"
#include "config/button.h"
#include "config/timer.h"
 
// Global Variables
int miwiNodeTemp, miwiNodeBatteryStatus;
char miwiNodeLocation[30];
int miwiNodeNum;
bool miwiNewDataArrived = true;
int miwiNodeRssi;
int cloudConnecting =0;


/** Index of scan list to request scan result. */
static uint8_t scan_request_index = 0;
/** Number of APs found. */
static uint8_t num_founded_ap = 0;

/************************** DEFINITIONS **********************************/
#if (BOARD == SAMR21ZLL_EK)
#define NVM_UID_ADDRESS   ((volatile uint16_t *)(0x00804008U))
#endif

/************************** PROTOTYPES **********************************/
void ReadMacAddress(void);



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
/*********************************************************************
* Function:         void main(void)
*
* PreCondition:     none
*
* Input:		    none
*
* Output:		    none
*
* Side Effects:	    none
*
* Overview:		    This is the main function that runs the simple 
*                   example demo. The purpose of this example is to
*                   demonstrate the simple application programming
*                   interface for the MiWi(TM) Development 
*                   Environment. By virtually total of less than 30 
*                   lines of code, we can develop a complete 
*                   application using MiApp interface. The 
*                   application will first try to establish a
*                   link with another device and then process the 
*                   received information as well as transmit its own 
*                   information.
*                   MiWi(TM) DE also support a set of rich 
*                   features. Example code FeatureExample will
*                   demonstrate how to implement the rich features 
*                   through MiApp programming interfaces.
*
* Note:			    
**********************************************************************/
uint8_t Slot = 0;
uint8_t sendIndex = 0;
uint8_t statusOfUnicast = false;
uint8_t newDataToForward = false;

uint8_t buffer[128];
uint8_t appPayloadSize = 0;
uint8_t *RxAddress;
uint8_t savePreviousPacket[128];


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
void toggleLED(uint8_t val){
	uint8_t timer = 10;
	while(timer--)
	{
		LED_Toggle(val);
		delay_ms(100);
	}
}
char str[16];
uint8_t count10Secs;

#define STRING_HEADER "-- Wifi BLE Provisioning + AWS WSN Demo --\r\n" \
"-- "BOARD_NAME" --\r\n" \
"-- Compiled: "__DATE__" "__TIME__" --\r\n"

#define APP_STATE_IDLE								0
#define APP_STATE_WAITING_FOR_WIFI_DISCONNECTION	1
#define APP_STATE_PROVISIONING						2
#define APP_STATE_WAITING_FOR_WIFI_CONNECTION		3
#define APP_STATE_COMPLETED							4

static volatile uint8 gu8WiFiConnectionState = M2M_WIFI_UNDEF;
/*static volatile uint8 gu8BtnEvent;*/
static uint8 gu8ScanIndex;
static at_ble_event_parameter_t gu8BleParam __aligned(4);

static void app_wifi_init(tpfAppWifiCb wifi_cb_func)
{
	tstrWifiInitParam param;
	uint32 pinmask = (
	M2M_PERIPH_PULLUP_DIS_HOST_WAKEUP|
	M2M_PERIPH_PULLUP_DIS_SD_CMD_SPI_SCK|
	M2M_PERIPH_PULLUP_DIS_SD_DAT0_SPI_TXD);

	sint8 ret;

	uint8 mac_addr[6];
	uint8 u8IsMacAddrValid;
	sint8 deviceName[] = M2M_DEVICE_NAME;

	#ifdef _STATIC_PS_
	nm_bsp_register_wake_isr(wake_cb, PS_SLEEP_TIME_MS);
	#endif
	
	m2m_memset((uint8*)&param, 0, sizeof(param));
	param.pfAppWifiCb = wifi_cb_func;
	#ifdef ETH_MODE
	param.strEthInitParam.pfAppEthCb = ethernet_demo_cb;
	param.strEthInitParam.au8ethRcvBuf = gau8ethRcvBuf;
	param.strEthInitParam.u16ethRcvBufSize = sizeof(gau8ethRcvBuf);
	#endif
	ret = m2m_ble_wifi_init(&param);

	if (M2M_SUCCESS != ret)
	{
		M2M_ERR("Driver Init Failed <%d>\n",ret);
		M2M_ERR("Reseting\n");
		// Catastrophe - problem with booting. Nothing but to try and reset
		system_reset();

		while (1)
		{
		}
	}
	
	m2m_periph_pullup_ctrl(pinmask, 0);
	
	m2m_wifi_get_otp_mac_address(mac_addr, &u8IsMacAddrValid);
	if (!u8IsMacAddrValid) {
		uint8 DEFAULT_MAC[] = MAC_ADDRESS;
		M2M_INFO("Default MAC\n");
		m2m_wifi_set_mac_address(DEFAULT_MAC);
		} else {
		M2M_INFO("OTP MAC\n");
	}
	m2m_wifi_get_mac_address(mac_addr);
	M2M_INFO("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
	mac_addr[0],mac_addr[1],mac_addr[2],
	mac_addr[3],mac_addr[4],mac_addr[5]);
	

	/* Name must be in the format WINC3400_00:00 */
	{
		#define HEX2ASCII(x) (((x)>=10)? (((x)-10)+'A') : ((x)+'0'))

		uint16 len;
		len = m2m_strlen((uint8_t *) deviceName);
		if (len >= 5) {
			deviceName[len-1] = HEX2ASCII((mac_addr[5] >> 0) & 0x0f);
			deviceName[len-2] = HEX2ASCII((mac_addr[5] >> 4) & 0x0f);
			deviceName[len-4] = HEX2ASCII((mac_addr[4] >> 0) & 0x0f);
			deviceName[len-5] = HEX2ASCII((mac_addr[4] >> 4) & 0x0f);
		}
	}
	m2m_wifi_set_device_name((uint8*)deviceName, (uint8)m2m_strlen((uint8*)deviceName));

	#ifdef _DYNAMIC_PS_
	{
		tstrM2mLsnInt strM2mLsnInt;
		M2M_INFO("M2M_PS_DEEP_AUTOMATIC\n");
		m2m_wifi_set_sleep_mode(M2M_PS_DEEP_AUTOMATIC, 1);
		strM2mLsnInt.u16LsnInt = M2M_LISTEN_INTERVAL;
		m2m_wifi_set_lsn_int(&strM2mLsnInt);
	}
	#elif (defined _STATIC_PS_)
	M2M_INFO("M2M_PS_MANUAL\n");
	m2m_wifi_set_sleep_mode(M2M_PS_MANUAL, 1);
	#else
	M2M_INFO("M2M_NO_PS\n");
	m2m_wifi_set_sleep_mode(M2M_NO_PS, 1);
	#endif
}

static void timer_cb(void)
{
	printf(".");
}

static void app_wifi_handle_event(uint8 u8MsgType, void * pvMsg)
{
	if (u8MsgType == M2M_WIFI_RESP_CON_STATE_CHANGED){
		tstrM2mWifiStateChanged *pstrWifiState = (tstrM2mWifiStateChanged*) pvMsg;

		printf("\r\nWifi State :: %s ::\r\n", pstrWifiState->u8CurrState ? "CONNECTED" : "DISCONNECTED");
		if(pstrWifiState->u8CurrState == M2M_WIFI_DISCONNECTED){
			gu8WiFiConnectionState = M2M_WIFI_DISCONNECTED;
		}
	}
	else if (u8MsgType == M2M_WIFI_REQ_DHCP_CONF){
		tstrM2MIPConfig* pstrM2MIpConfig = (tstrM2MIPConfig*) pvMsg;
		uint8 *pu8IPAddress = (uint8*) &pstrM2MIpConfig->u32StaticIP;
		
		printf("DHCP IP Address :: %u.%u.%u.%u ::\r\n",
		pu8IPAddress[0], pu8IPAddress[1], pu8IPAddress[2], pu8IPAddress[3]);
		printf("WiFi Connected\r\n");

		gu8WiFiConnectionState = M2M_WIFI_CONNECTED;
		
	}
	else if (u8MsgType == M2M_WIFI_RESP_SCAN_DONE){
		tstrM2mScanDone *pstrInfo = (tstrM2mScanDone*) pvMsg;

		if (gu8WiFiConnectionState != M2M_WIFI_CONNECTED){
			gu8ScanIndex = 0;
			
			if (pstrInfo->u8NumofCh >= 1){
				m2m_wifi_req_scan_result(gu8ScanIndex);
				gu8ScanIndex++;
			}
		}
	}
	else if (u8MsgType == M2M_WIFI_RESP_SCAN_RESULT){
		uint8 u8NumFoundAPs = m2m_wifi_get_num_ap_found();

		if (gu8WiFiConnectionState != M2M_WIFI_CONNECTED){
			tstrM2mWifiscanResult *pstrScanResult = (tstrM2mWifiscanResult*) pvMsg;

			ble_prov_scan_result(pstrScanResult, u8NumFoundAPs-gu8ScanIndex);
			if(gu8ScanIndex < u8NumFoundAPs){
				m2m_wifi_req_scan_result(gu8ScanIndex);
				gu8ScanIndex++;
			}
		}
	}
}
credentials cred;
// This is an example of using onchip_profile, ble_prov API.
void app_ble_wifi_provisioning(void)
{
	uint8_t app_state = APP_STATE_IDLE;
	uint8_t wifi_con_state = M2M_WIFI_UNDEF;
	/*uint8_t btn_event;*/
	at_ble_events_t ble_event;

	// Initialize BLE stack on 3400.
	m2m_ble_init();
	m2m_wifi_req_unrestrict_ble();
	ble_prov_init((uint8_t *)"Mystery123");  // WiFi Prov
	printf("\r\nOpen MBD app and provision Wi-Fi\r\n");
	printf("\r\nBLE pairing pin: 123456\r\n");

	while (app_state != APP_STATE_COMPLETED)
	{
		if (m2m_ble_event_get(&ble_event, &gu8BleParam) == AT_BLE_SUCCESS)
		{
			ble_prov_process_event(ble_event, &gu8BleParam);
		}
		
		if (wifi_con_state != gu8WiFiConnectionState)
		{
			if (gu8WiFiConnectionState != M2M_WIFI_UNDEF)
			{
				ble_prov_wifi_con_update(
				gu8WiFiConnectionState ? WIFIPROV_CON_STATE_CONNECTED:
				WIFIPROV_CON_STATE_DISCONNECTED);
			}
			wifi_con_state = gu8WiFiConnectionState;
		}

		switch (app_state)
		{
			case APP_STATE_IDLE:
			{
				if (wifi_con_state == M2M_WIFI_CONNECTED)
				{
					m2m_wifi_disconnect();
					app_state = APP_STATE_WAITING_FOR_WIFI_DISCONNECTION;
				}
				else
				{
					gu8WiFiConnectionState = M2M_WIFI_UNDEF;
					if (ble_prov_start() == AT_BLE_SUCCESS)
					{
						app_state = APP_STATE_PROVISIONING;
					}
				}
				break;
			}
			case APP_STATE_WAITING_FOR_WIFI_DISCONNECTION:
			{
				if (wifi_con_state == M2M_WIFI_DISCONNECTED)
				{
					if (ble_prov_start() == AT_BLE_SUCCESS)
					{
						app_state = APP_STATE_PROVISIONING;
						wifi_con_state = M2M_WIFI_UNDEF;
						gu8WiFiConnectionState = M2M_WIFI_UNDEF;
					}
				}
				break;
			}
			case APP_STATE_PROVISIONING:
			{
				// BLE requests are handled in ble_prv framework.
				// The application layer handles scan_result (handle ble_prov_scan_result)
				// Here we check if process has been completed.
				switch (ble_prov_get_provision_state())
				{
					case BLE_PROV_STATE_SUCCESS:
					{

						printf("Provisioning data received\r\n");
						if (ble_prov_get_credentials(&cred) == CREDENTIALS_VALID)
						{
							//printf("Connecting to %s ",(char *)mycred.ssid);
							printf("Received WiFi - Credentials SSID and Password!!!\r\n");
							//nm_bsp_start_timer(timer_cb, 1000);
							//m2m_wifi_connect((char *)mycred.ssid, mycred.ssid_length,
							//mycred.sec_type, mycred.passphrase, M2M_WIFI_CH_ALL);
							ble_prov_wifi_con_update(WIFIPROV_CON_STATE_CONNECTING);
							app_state = APP_STATE_COMPLETED;//APP_STATE_WAITING_FOR_WIFI_CONNECTION;
							printf("Provisioning Completed\r\n");
							ble_prov_wifi_con_update(WIFIPROV_CON_STATE_CONNECTED);
							app_state = APP_STATE_COMPLETED;
							wifi_con_state = M2M_WIFI_UNDEF;
							ble_prov_stop();
							//Re-init the BLE to put it into a default, known state.
							m2m_ble_init();
							//Now we have finished provisioning, we can place BLE in restricted mode to save power
							m2m_wifi_req_restrict_ble();
						}
						else
						{
							ble_prov_stop();
							app_state = APP_STATE_IDLE;
						}
						break;
					}
					case BLE_PROV_STATE_FAILED:
					{
						ble_prov_stop();
						app_state = APP_STATE_IDLE;
						break;
					}
				}
				break;
			}
			case APP_STATE_WAITING_FOR_WIFI_CONNECTION:
			{
				if (wifi_con_state == M2M_WIFI_CONNECTED)
				{
					nm_bsp_stop_timer();
					printf("Provisioning Completed\r\n");
					ble_prov_wifi_con_update(WIFIPROV_CON_STATE_CONNECTED);
					app_state = APP_STATE_COMPLETED;
					wifi_con_state = M2M_WIFI_UNDEF;
					ble_prov_stop();
					//Re-init the BLE to put it into a default, known state.
					m2m_ble_init();
					//Now we have finished provisioning, we can place BLE in restricted mode to save power
					m2m_wifi_req_restrict_ble();
				}
				if (wifi_con_state == M2M_WIFI_DISCONNECTED)
				{
					nm_bsp_stop_timer();
					printf("WiFi Connect failed.\r\n");
					ble_prov_stop();
					ble_prov_wifi_con_update(WIFIPROV_CON_STATE_DISCONNECTED);
					app_state = APP_STATE_IDLE;
					wifi_con_state = M2M_WIFI_UNDEF;
				}
				break;
			}
			case APP_STATE_COMPLETED:
			{

				break;
			}
		}
	}
}
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


int main(void)
{	
	uint8_t i , TxCount = 0 , button_press;
	uint8_t connection_index = 0xFF, TxSynCount2 = 0;
	tstrWifiInitParam param;
	//int8_t ret;
	Cloud_RC ret = CLOUD_RC_NONE_ERROR;
	rtc_init();

	MIWI_TICK t1 , t2;
	// Hardware Init
	system_init();
	delay_init();
	sio2host_init();

	//configure_tc();
	print_reset_causes();
	/* Timer Used for MiWi Tick. */
	configure_tc();
	/* MiWi Protocol Init, Radio Init */
	MiApp_ProtocolInit(false);
	// Set the Channel
	MiApp_SetChannel(APP_CHANNEL_SEL);
	printf(STRING_HEADER);
	MiApp_ConnectionMode(ENABLE_ALL_CONN);
	// Enable Transceiver Interrupt
	ENABLE_TRX_IRQ();
	// Enable All cpu interrupts
	cpu_irq_enable();
	connection_index  = MiApp_EstablishConnection(0xFF, CONN_MODE_DIRECT);
	MiApp_StartConnection(START_CONN_DIRECT, 10, 0);
	LED_On(LED1);
	puts(STRING_HEADER);
	
	nm_bsp_init();
	DBG_LOG("Initializing WSN  Device\r\n");
	DBG_LOG("cpu_freq=%d\r\n",(int)system_cpu_clock_get_hz());
	nm_bsp_app_init();
	app_wifi_init(app_wifi_handle_event);
	app_ble_wifi_provisioning();
	wifiCryptoInit();
	
	while(1)
	{
		
		m2m_wifi_handle_events(NULL);
		if (MiApp_MessageAvailable())
		{
			// if received a data packet toggle led
			LED_Toggle(LED1);
			memset(miwiNodeLocation,0,sizeof(miwiNodeLocation));
			strcat(miwiNodeLocation, "Node");
			strcat (miwiNodeLocation,itoa(rxMessage.Payload[0], str , 10));
			printf("Node Id:%s\r\n",miwiNodeLocation);
			miwiNodeTemp = rxMessage.Payload[1];
			miwiNodeRssi = rxMessage.PacketRSSI;
			miwiNewDataArrived = true;
			printf("NodeId: %d, Temp:%d,  RSSI: %d\r\n",rxMessage.Payload[0], rxMessage.Payload[1], rxMessage.PacketRSSI);
			/*******************************************************************/
			// Function MiApp_DiscardMessage is used to release the current
			//  received packet.
			// After calling this function, the stack can start to process the
			//  next received frame
			/*******************************************************************/
			MiApp_DiscardMessage();

		}
		#if 1
			if(receivedTime && !cloudConnecting){
				cloudConnecting = 1;
			
				ret = cloud_connect();

				if (ret == CLOUD_RC_SUCCESS)
				{
					cloudConnecting = 3;
					printf("connected to AWS IoT Cloud ...\r\n");
				}
				else
				{
					printf("Cloud connect fail...\r\n");
					while(1)
					{
								LED_Toggle(LED0);
								delay_ms(100);
					}
				}

			}
		#endif

			/*
				 Step 2: Communicate with AWS IoT Core:
		 
					1. Code below publish to a topic in AWS IoT Core.
					2. The topic name is wifiSensorBoard/”your_thing_name”/dataControl
					3. After enabling the code, you can start subscribe to the the above topic and receive the data on AWS IoT.
		*/
		if (miwiNewDataArrived)
		{
			#if 1
				if(cloudConnecting == 3){
					miwiNewDataArrived = false;
					cloudConnecting = 3;
					env_sensor_execute();
				}
			#endif
		}

			cloud_mqtt_yield(100);

	}


}