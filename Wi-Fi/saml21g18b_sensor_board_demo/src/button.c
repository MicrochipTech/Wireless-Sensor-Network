
/*- Includes ---------------------------------------------------------------*/

#include <asf.h>
#include "stdio.h"
#include "string.h"
#include "button.h"
//#include "nvm_api.h"
#include "winc15x0.h"
#include "conf_board.h"
#include "main.h"
#include "nvm_handle.h"
#include "led.h"

extern wifi_nvm_data_t wifi_nvm_data;
extern uint8 gMacaddr[M2M_MAC_ADDRES_LEN];
extern uint8 gDefaultSSID[M2M_MAX_SSID_LEN];
extern uint8 gAuthType;
extern uint8 gDefaultKey[M2M_MAX_PSK_LEN];
extern uint8 gUuid[AWS_COGNITO_UUID_LEN];


void initialise_button(void)
{
	
	/* Set buttons as inputs */
	struct port_config config_port_pin;
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction = PORT_PIN_DIR_INPUT;
	config_port_pin.input_pull = PORT_PIN_PULL_DOWN;
	port_pin_set_config(SW1_PIN, &config_port_pin);
	port_pin_set_config(SW2_PIN, &config_port_pin);
	port_pin_set_config(SW3_PIN, &config_port_pin);

}

void buttonInitCheck()
{
	
	if(SW2_ACTIVE == port_pin_get_input_level(SW2_PIN)){	// Enter WINC1500 FW programming mode
		
		led_ctrl_set_color(LED_COLOR_GREEN, LED_MODE_BLINK_NORMAL);
		while(1) {
			
		}
	}
	
	if(SW1_ACTIVE == port_pin_get_input_level(SW1_PIN)){
		setWiFiStates(WIFI_TASK_SWITCH_TO_AP);
		printf("Set as AP mode\r\n");
		
		//memset((uint32_t*)&whiteListDevices,0,sizeof(whiteListDevices));
		//nv_flash_write(WHITE_LIST_PAGE_ADDRESS,WHITE_LIST_PAGE,(uint32_t*)(&whiteListDevices),sizeof(whiteListDevices));
		
		//printf("Reset Config page\r\n");
		//memset((uint32_t*)&wifi_nvm_data,0,sizeof(wifi_nvm_data));
		//nv_flash_write(CONFIG_PAGE_ADDRESS,CONFIG_PAGE,(uint32_t*)(&wifi_nvm_data),sizeof(wifi_nvm_data));
		
	}
	else{		
		
		//nv_flash_read(WHITE_LIST_PAGE_ADDRESS,WHITE_LIST_PAGE,(uint32_t*)(&whiteListDevices),sizeof(whiteListDevices));
		//if(whiteListDevices.whiteListNum > WHITE_LIST_NUM_MAX)
		//{
		//	memset((uint32_t*)&whiteListDevices,0,sizeof(whiteListDevices));
		//	nv_flash_write(WHITE_LIST_PAGE_ADDRESS,WHITE_LIST_PAGE,(uint32_t*)(&whiteListDevices),sizeof(whiteListDevices));
		//}
		#if 1
		printf("Read config page\r\n");
		nvm_get_config_data(&wifi_nvm_data);
		//nv_flash_read(CONFIG_PAGE_ADDRESS,CONFIG_PAGE,(uint32_t*)(&wifi_nvm_data),sizeof(wifi_nvm_data));
		
		if (wifi_nvm_data.ssid[0] != 0xFF && wifi_nvm_data.ssid[0] != 0)
		{
			
			printf("ssid=%s, ssidlen=%d\r\n", wifi_nvm_data.ssid, wifi_nvm_data.ssidlen);
			memcpy(gDefaultSSID, wifi_nvm_data.ssid, wifi_nvm_data.ssidlen);
			printf("key=%s, keylen=%d\r\n", wifi_nvm_data.key, wifi_nvm_data.keylen);
			memcpy(gDefaultKey, wifi_nvm_data.key, wifi_nvm_data.keylen);
			printf("gAuthType=%d\r\n", wifi_nvm_data.authtype);
			gAuthType = wifi_nvm_data.authtype;
		
			printf("gUuid=%s, len=%d\r\n", wifi_nvm_data.uuid, strlen((const char*) wifi_nvm_data.uuid));
			memcpy(gUuid, wifi_nvm_data.uuid, strlen((const char*) wifi_nvm_data.uuid));
		}
		#endif
	}
}
void buttonTaskInit()
{
	return;	
}
void buttonTaskExecute(uint32 tick)
{
	static uint32 pre_tick = 0;
	uint32 press_time = 0;
	static uint32 idx_5s = 1;
	
	bool pin_lvl = port_pin_get_output_level(SW1_PIN);
	
	if(SW1_ACTIVE == pin_lvl){
		for (int i=0; i<MAX_CB_INDEX; i++)
		{
			if (button_detect_cb[i]!=NULL)
			button_detect_cb[i]();
		}
	}
	if(SW1_ACTIVE == pin_lvl && pre_tick == 0){
		pre_tick = tick;
		
	}
	else if(SW1_ACTIVE == pin_lvl && pre_tick != 0){
		if (tick > pre_tick)
		press_time = tick - pre_tick;
		
		if (press_time >= idx_5s*TIMEOUT_COUNTER_5S)
		{
			idx_5s++;
			for (int i=0; i<MAX_CB_INDEX; i++)
			{
				if (button_5s_timeout_cb[i]!=NULL)
					button_5s_timeout_cb[i]();
			}
		}
	}
	else
	{
		pre_tick = 0;
		idx_5s = 0;
	}
}

int regButtonPressDetectCallback(void* cb)
{
	for (int i=0; i<MAX_CB_INDEX; i++)
	{
		if (button_detect_cb[i]==NULL)
		{
			button_detect_cb[i] = cb;
			return i;
		}
	}
	
	printf("[%s] No quota...\n", __func__);
	return -1;
}
int unRegButtonPressDetectCallback(int sock)
{
	if (button_detect_cb[sock]!=NULL)
	{
			button_detect_cb[sock] = NULL;
			return 0;
	}
	else
		printf("[%s] Cannot find the related cb..\n", __func__);
	
	return -1;
}

int regButtonPress5sTimeoutCallback(void* cb)
{
	for (int i=0; i<MAX_CB_INDEX; i++)
	{
		if (button_5s_timeout_cb[i]==NULL)
		{
			button_5s_timeout_cb[i] = cb;
			return i;
		}
	}
	
	printf("[%s] No quota...\n", __func__);
	return -1;
}
int unRegButtonPress5sTimeoutCallback(int sock)
{
	
	if (button_5s_timeout_cb[sock]!=NULL)
	{
		button_5s_timeout_cb[sock] = NULL;
		return 0;
	}
	else
	printf("[%s] Cannot find the related cb..\n", __func__);
	return -1;
}