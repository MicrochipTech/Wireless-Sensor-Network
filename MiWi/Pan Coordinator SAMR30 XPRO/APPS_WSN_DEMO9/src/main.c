/**
* \file  main.c
*
* \brief Main file for WSN Demo Example on MiWi Mesh.
*
* Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries. 
*
* \asf_license_start
*
* \page License
*
* Subject to your compliance with these terms, you may use Microchip
* software and any derivatives exclusively with Microchip products. 
* It is your responsibility to comply with third party license terms applicable 
* to your use of third party software (including open source software) that 
* may accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, 
* WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, 
* INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, 
* AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE 
* LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL 
* LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE 
* SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE 
* POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT 
* ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY 
* RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*
* \asf_license_stop
*
*/
/*
* Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
*/

/**
* \mainpage
* \section preface Preface
* This is the reference manual for the WSN Demo Application
* The WSNDemo application implements a typical wireless sensor network scenario,
* in which one central node collects the data from a network of sensors and
* passes this data over a serial connection for further processing.
* In the case of the WSNDemo this processing is performed by the WSNMonitor PC
* application. The MiWi™ Quick Start Guide  provides a detailed description
* of the WSNDemo application scenario, and instructions on how to use WSNMonitor.
* <P>• Device types (PAN Coordinator, Coordinator and End Device) and its address in 
* MiWi™ Mesh network is displayed on the nodes.</P>
* <P>• The value of the extended address field is set equal to the value of the
* short address field.</P>
* <P>• For all frames, the LQI and RSSI fields are filled in by the coordinator
* with the values of LQI and RSSI from the received frame. This means that nodes
* that are not connected to the coordinator directly will have the same values
* as the last node on the route to the coordinator.</P>
* <P>• Sensor data values are generated randomly on all platforms.</P>
* <P>• Sending data to the nodes can be triggered when the light button on the 
* node is clicked. This also blinks the LED in node.
* </P>
*/


/************************ HEADERS ****************************************/
#include "asf.h"
#include "sio2host.h"
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
 
int miwiNodeTemp; // used to store temperature of room
// Although the SAMR30 XPRO supports Voltage measurement, for this demo we send 
//constant node voltage. We have not implemented measurement of voltage on SAMR30 XPRO.
int miwiNodeBatteryStatus; // used to store battery status of node
// we used a predefined set of node numbers that belong to room names/locations
int miwiNodeNum; // Node Number represents the room/location of the node
char miwiNodeLocation[30]; // Convert Node num to string 
bool miwiNewDataArrived = true; // flag denoting new data arrival
int miwiNodeRssi; // used to store Received RSSI
char str[16]; // used for printing console output


// WIFI Stuff
#define STRING_EOL    "\r\n"
#define STRING_HEADER "-- WINC1500 AP scan example --"STRING_EOL \
"-- "BOARD_NAME " --"STRING_EOL	\
"-- Compiled: "__DATE__ " "__TIME__ " --"STRING_EOL


/** Index of scan list to request scan result. */
static uint8_t scan_request_index = 0;
/** Number of APs found. */
static uint8_t num_founded_ap = 0;
int cloudConnecting =0;

/************************** DEFINITIONS **********************************/
#if (BOARD == SAMR21ZLL_EK)
#define NVM_UID_ADDRESS   ((volatile uint16_t *)(0x00804008U))
#endif

// Call back to handle WINC1500 connection to WiFi Access point
static void wifi_cb(uint8_t u8MsgType, void *pvMsg)
{
	switch (u8MsgType) {
	case M2M_WIFI_RESP_SCAN_DONE:
	{
		tstrM2mScanDone *pstrInfo = (tstrM2mScanDone *)pvMsg;
		scan_request_index = 0;
		if (pstrInfo->u8NumofCh >= 1) {
			m2m_wifi_req_scan_result(scan_request_index);
			scan_request_index++;
		} else {
			m2m_wifi_request_scan(M2M_WIFI_CH_ALL);
		}

		break;
	}

	case M2M_WIFI_RESP_SCAN_RESULT:
	{
		tstrM2mWifiscanResult *pstrScanResult = (tstrM2mWifiscanResult *)pvMsg;
		uint16_t demo_ssid_len;
		uint16_t scan_ssid_len = strlen((const char *)pstrScanResult->au8SSID);

		/* display founded AP. */
		printf("[%d] SSID:%s\r\n", scan_request_index, pstrScanResult->au8SSID);

		num_founded_ap = m2m_wifi_get_num_ap_found();
		if (scan_ssid_len) {
			/* check same SSID. */
			demo_ssid_len = strlen((const char *)MAIN_WLAN_SSID);
			if
			(
				(demo_ssid_len == scan_ssid_len) &&
				(!memcmp(pstrScanResult->au8SSID, (uint8_t *)MAIN_WLAN_SSID, demo_ssid_len))
			) {
				/* A scan result matches an entry in the preferred AP List.
				 * Initiate a connection request.
				 */
				printf("Found %s \r\n", MAIN_WLAN_SSID);
				m2m_wifi_connect((char *)MAIN_WLAN_SSID,
						sizeof(MAIN_WLAN_SSID),
						MAIN_WLAN_AUTH,
						(void *)MAIN_WLAN_PSK,
						M2M_WIFI_CH_ALL);
				break;
			}
		}

		if (scan_request_index < num_founded_ap) {
			m2m_wifi_req_scan_result(scan_request_index);
			scan_request_index++;
		} else {
			printf("can not find AP %s\r\n", MAIN_WLAN_SSID);
			m2m_wifi_request_scan(M2M_WIFI_CH_ALL);
		}

		break;
	}

	case M2M_WIFI_RESP_CON_STATE_CHANGED:
	{
		tstrM2mWifiStateChanged *pstrWifiState = (tstrM2mWifiStateChanged *)pvMsg;
		if (pstrWifiState->u8CurrState == M2M_WIFI_CONNECTED) {
			m2m_wifi_request_dhcp_client();
		} else if (pstrWifiState->u8CurrState == M2M_WIFI_DISCONNECTED) {
			printf("Wi-Fi disconnected\r\n");

			/* Request scan. */
			m2m_wifi_request_scan(M2M_WIFI_CH_ALL);
		}

		break;
	}

	case M2M_WIFI_REQ_DHCP_CONF:
	{
		uint8_t *pu8IPAddress = (uint8_t *)pvMsg;
		printf("Wi-Fi connected\r\n");
		printf("Wi-Fi IP is %u.%u.%u.%u\r\n",
				pu8IPAddress[0], pu8IPAddress[1], pu8IPAddress[2], pu8IPAddress[3]);
		break;
	}

	default:
	{
		break;
	}
	}
}


// Prints this reason for reset of Demo
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

// In this demo we use Watch Dog timer to reset the demo incase of unexpected fault in application
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


int main ( void )
{
	uint8_t i , TxCount = 0 , button_press;
	uint8_t connection_index = 0xFF, TxSynCount2 = 0;
	tstrWifiInitParam param;
	//int8_t ret;
	Cloud_RC ret = CLOUD_RC_NONE_ERROR;
	rtc_init();
	delay_init();
	/* Initialize the board. */
	system_init();
	/* Initialize the UART console. */
	sio2host_init();
	// Configure Timer MiWi Stack uses this 
	configure_tc();

	/*******************************************************************/
	// MiWi Stack/Radio Init for channels, modulation etc
	MiApp_ProtocolInit(false);
	// Set the Channel
	MiApp_SetChannel(APP_CHANNEL_SEL);
	printf(STRING_HEADER);
	MiApp_ConnectionMode(ENABLE_ALL_CONN);
	// Enable Transceiver Interrupt
	ENABLE_TRX_IRQ();
	// Enable All cpu interrupts
	cpu_irq_enable();
	print_reset_causes();
	// MiWi API to establish a connection with Peer Node
	connection_index  = MiApp_EstablishConnection(0xFF, CONN_MODE_DIRECT);
	// MiWi API to enable the node as PAN Coordinator Type
	MiApp_StartConnection(START_CONN_DIRECT, 10, 0);
	LED_On(LED1);
	/* Initialize the BSP. */
	nm_bsp_init();
	DBG_LOG("Initializing WSN  Device\r\n");
	DBG_LOG("cpu_freq=%d\n",(int)system_cpu_clock_get_hz());
	// WiFi and ECC init
	wifiCryptoInit();
	// WatchDog Init
	configure_wdt();
	// WatchDog call back 
	configure_wdt_callbacks();
	while (1) {
		LED_On(LED0);
	// Handle all WiFi related events
	m2m_wifi_handle_events(NULL);
	// Handle the WatchDog reset count when no failure is happening 
	wdt_reset_count();
	// Handle received message from MiWi end device
	if (MiApp_MessageAvailable())
	{
		// if received a data packet toggle led
		LED_Toggle(LED1);
			#if defined (PC)
		#endif
		memset(miwiNodeLocation,0,sizeof(miwiNodeLocation));
		strcat(miwiNodeLocation, "Node");
		strcat (miwiNodeLocation,itoa(rxMessage.Payload[0], str , 10));
		printf("Node Id:%s\r\n",miwiNodeLocation);
		miwiNodeTemp = rxMessage.Payload[1];
		miwiNodeRssi = rxMessage.PacketRSSI;
		miwiNewDataArrived = true;
		// print received data on console
		printf("NodeId: %d, Temp:%d,  RSSI: %d\r\n",rxMessage.Payload[0], rxMessage.Payload[1], rxMessage.PacketRSSI);
		/*******************************************************************/
		// Function MiApp_DiscardMessage is used to release the current
		//  received packet.
		// After calling this function, the stack can start to process the
		//  next received frame
		/*******************************************************************/
		MiApp_DiscardMessage();

	}
	// Handle Connection to AWS Cloud
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
							// Toggle the Yellow LED on SAMR30 XPRO when connection to AWS is unsuccessful
							// WatchDog reset will happen when failing to connect to AWS
							LED_Toggle(LED0);
							delay_ms(100);
				}
			}

		}
	#endif

		/*
			 Step 2: Communicate with AWS IoT Core:
		 
				1. Code below subscribe to a topic in AWS IoT Core.
				2. The topic name is wifiSensorBoard/”your_thing_name”/dataControl
				3. After enabling the code, you can start publishing to the the above topic and receive the data here.
		*/	
		// We do not subscribe to any topic in this demo
		// We just publish data to AWS
		#if 0
			if(cloudConnecting == 2){
				cloudConnecting = 3;
				ret = cloud_mqtt_subscribe(gSubscribe_Channel, MQTTSubscribeCBCallbackHandler);
				if (ret == CLOUD_RC_SUCCESS)
				{
					printf("subscribed to : %s\n", gSubscribe_Channel);
				}
				else
				printf("subscribe MQTT channel fail... %s\r\n",gSubscribe_Channel);
			}
		#endif

		/*
			 Step 3: Communicate with AWS IoT Core:
		 
				1. Code below publish to a topic in AWS IoT Core.
				2. The topic name is wifiSensorBoard/”your_thing_name”/dataControl
				3. After enabling the code, you can start subscribe to the the above topic and receive the data on AWS IoT.
	*/
	// Once MiWi data has arrived from end device publishing this data to AWS is accomplished by the below code
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

	return 0;



}

