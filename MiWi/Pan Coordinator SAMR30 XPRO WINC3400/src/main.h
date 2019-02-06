/*
 * main.h
 *
 * Created: 9/6/2017 4:44:45 PM
 *  Author: C17143
 */ 


#ifndef MAIN_H_
#define MAIN_H_

#include "stdint.h"
#include "stdbool.h"
#define DEFAULT_SSID				"DEMO_AP"
#define DEFAULT_AUTH				M2M_WIFI_SEC_WPA_PSK
#define	DEFAULT_KEY					"12345678"


#include "driver/include/m2m_wifi.h"
/** Wi-Fi Settings */
#define MAIN_WLAN_SSID        "NETGEAR36"//"MASTERS"
#define MAIN_WLAN_AUTH        M2M_WIFI_SEC_WPA_PSK /* < Security manner */
#define MAIN_WLAN_PSK         "sunnysquirrel791"//"microchip"

#ifdef DEBUG_SUPPORT
#define DBG_LOG		printf("\r\n");\
printf
#else
#define DBG_LOG		ALL_UNUSED
#endif

extern int miwiNodeTemp, miwiNodeBatteryStatus;
extern char miwiNodeLocation[30];
extern int miwiNodeNum;
extern int miwiNodeRssi;
extern bool miwiNewDataArrived;
#define FIRMWARE_MAJOR_VER	"1"
#define FIRMWARE_MINOR_VER	"2"


/** Wi-Fi Settings */
#define MAIN_WLAN_CHANNEL				(6)
#define MAIN_WIFI_M2M_PRODUCT_NAME        "NMCTemp"
#define MAIN_WIFI_M2M_SERVER_IP           0xc0a80164 //0xFFFFFFFF /* 255.255.255.255 */
#define MAIN_WIFI_M2M_SERVER_PORT         (6666)
#define MAIN_WIFI_M2M_REPORT_INTERVAL     (1000)

#define MAIN_WIFI_M2M_BUFFER_SIZE          1460


#define DEVICE_NAME		"living room"
#define DEVICE_TYPE		"wifiSensorBoard"//"MiWi-WiFi-Secure(ECC608)-Gateway"//

#define INIT_TEMP  73
#define INIT_BATTERY "3.0V"
#define INIT_RSSI -87
#define INIT_NODEID "registration"
#define AWS_COGNITO_UUID_LEN	60


typedef struct _wifi_nvm_data {
	uint8_t valid;
	uint8_t ssid[M2M_MAX_SSID_LEN];
	uint8_t ssidlen;
	uint8_t key[M2M_MAX_PSK_LEN];
	uint8_t keylen;
	uint8_t authtype;
	uint8_t uuid[AWS_COGNITO_UUID_LEN];
} wifi_nvm_data_t;


// to be deleted
#define TEMP_UPDATE_BIT	(1ul << 0)
#define HUM_UPDATE_BIT	(1ul << 1)
#define UV_UPDATE_BIT	(1ul << 2)
#define PRESSURE_UPDATE_BIT	(1ul << 3)

typedef struct environment_data
{
	int16_t temperature;
	uint16_t pressure;
	uint32_t uv;
	uint8_t humidity;
}environment_data_t;


typedef enum Led_Mode
{
	LED_MODE_NONE = 0,
	LED_MODE_TURN_OFF = 1,
	LED_MODE_TURN_ON = 2,
	LED_MODE_BLINK_NORMAL = 3,
	LED_MODE_BLINK_FAST = 4,
	LED_MODE_BLINK_SHORT = 5,
}Led_Mode;

typedef enum Led_Color
{
	LED_COLOR_BLUE = 0,
	LED_COLOR_GREEN = 1,
	LED_COLOR_RED = 2,
	LED_COLOR_YELLOW = 3,
	LED_COLOR_Magneta = 4,
	LED_COLOR_Cyan = 5,
	LED_COLOR_WHTIE = 6,
}Led_Color;
void initialise_led(void);
void led_ctrl_set_color(Led_Color color, Led_Mode mode);
void led_ctrl_set_mode(Led_Mode mode);
void led_ctrl_execute(void);
void toggleLED(uint8_t val);

typedef struct device_rotation_vector
{
	int16_t x;
	int16_t y;
	int16_t z;
	int16_t w;
	
}device_rotation_vector_t;
void env_sensor_data_init(void);
void get_env_sensor_data_for_display(environment_data_t *env_data);
void env_sensor_execute(void);
void register_env_sensor_udpate_callback_handler(void* cb);
void get_env_sensor_data_from_chip(environment_data_t *env_data);

Led_Color led_ctrl_get_color(void);

void motion_sensor_data_init(void);
void motion_sensor_execute(void);
void register_rotation_vector_udpate_callback_handler(void* cb);
void set_motion_sensor_update_timer(unsigned char time);


#define dev11 "8b5c5d3f439b821216639fd0a4077813887674ff";
#define dev10 "d45d27ccc0afb7c2bae8ca3ede2ae568e9a1feec";
#define dev9  "76ae1af626ebf0814d41bd1975c4b3a135715009";
#define dev8  "82443aa2ce28c18570facbfe625c7d483945af43";
#define dev7  "b2e86bc436d77f04ff1f500e2a526fd3ca2188a9";
#define dev6  "0ca2c8a955ff2a30755ee8580818a2ab9aeb529b"; // Configuration Failed
#define dev5  "7ce4950de1efbb08836aa24688bcd358704de2a4";
#define dev4  "fecd9d386932f1eda6061cf5ccc8404c34f0ccca";
#define dev3  "fbe24204bd3e0893d8649e1197f598d63420db49";
#define dev2  "17e028f0e5a68150c28288661689dae39473ba83";
#define dev1  "361556b4952fb297dc806b37a05cf3dc8aa8da64";
#define dev0  "7ffb6bd8c649730c63de58189a12705eafe6fabb";

#define gwid11  11
#define gwid10 10
#define gwid9  9
#define gwid8  8
#define gwid7  7
#define gwid6  6
#define gwid5  5
#define gwid4  4
#define gwid3  3
#define gwid2  2
#define gwid1  1
#define gwid0  0





//#define SETUP0
//#define SETUP1
//#define SETUP2
#define SETUP3
//#define SETUP4
//#define SETUP5
//#define SETUP6
//#define SETUP7
//#define SETUP8
//#define SETUP9
//#define SETUP10
//#define SETUP11

#endif /* MAIN_H_ */