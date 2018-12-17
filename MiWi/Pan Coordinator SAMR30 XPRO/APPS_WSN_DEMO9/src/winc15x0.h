/*
 * winc15x0.h
 *
 * Created: 7/1/2018 10:00:30 PM
 *  Author: C17143
 */ 


#ifndef WINC15X0_H_
#define WINC15X0_H_

#include "datapoint.h"
#include "socket/include/m2m_socket_host_if.h"

#define AWS_ECC508A_I2C_ADDRESS  (uint8_t)(0xB0)  //! AWS ECC508A device I2C address

#define SUBSCRIBE_TOPIC "dataControl"
#define PUBLISH_TOPIC	"dataReport"
#define SUBSCRIBE_SEARCH_TOPIC "search"
#define PUBLISH_SEARCH_RESP_TOPIC "searchResp"
#define DEV_SEARCH_SUBSCRIBE_TOPIC "winc1500Iot/search"

#define DEVICE_KEY_SLOT            (0)

#define HEX2ASCII(x) (((x) >= 10) ? (((x) - 10) + 'A') : ((x) + '0'))
#define MAIN_WLAN_SSID_OFFSET    14                     /* < Attach MAC addr after this offset in SSID */
#define AP_WLAN_SSID	"iGatewayF8F005F3659B"


#define AP_TCP_SERVER_PORT				8899
#define MQTT_CLIENT_ID_LEN				100
#define MQTT_CHANNEL_LEN				70 // 60


typedef enum
{
	APP_STA,
	APP_AP,
	APP_P2P,
} wifi_mode;

typedef enum
{
	WIFI_TASK_IDLE,
	WIFI_TASK_SWITCHING_TO_STA,
	WIFI_TASK_CONNECT_CLOUD,
	WIFI_TASK_MQTT_SUBSCRIBE,
	WIFI_TASK_MQTT_RUNNING,
	WIFI_TASK_CONNECT_CLOUD_FINISH,
	WIFI_TASK_SWITCH_TO_AP,
	WIFI_TASK_SWITCHING_TO_AP,
	WIFI_TASK_AP_CONNECTED,
	WIFI_TASK_STA_DISCONNECTED,

	WIFI_TASK_MAX
} wifi_FSM_states;


extern char gAwsMqttClientId[MQTT_CLIENT_ID_LEN];
char gSubscribe_Channel[MQTT_CHANNEL_LEN];
char gPublish_Channel[MQTT_CHANNEL_LEN];
char gSearch_Channel[MQTT_CHANNEL_LEN];
char gSearchResp_Channel[MQTT_CHANNEL_LEN];


int wifiCryptoInit(void);
int wifiTaskExecute(void);
void detectWiFiMode(void);
void setWiFiStates(wifi_FSM_states state);
wifi_FSM_states getWiFiStates(void);
wifi_mode getWiFiMode(void);


#endif /* WINC15X0_H_ */