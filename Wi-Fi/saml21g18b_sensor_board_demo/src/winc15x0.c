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
#include "driver/include/m2m_wifi.h"
#include "socket/include/socket.h"
#include "string.h"
#include "AWS_SDK/aws_iot_src/utils/aws_iot_log.h"
#include "AWS_SDK/aws_iot_src/utils/aws_iot_version.h"
#include "AWS_SDK/aws_iot_src/protocol/mqtt/aws_iot_mqtt_interface.h"
#include "winc15x0.h"
#include "iot_message.h"
#include "cJSON.h"
#include "cloud_wrapper.h"
//#include "button_handle.h"
#include "cmdset.h"
#include "gateway_prov.h"
#include "led.h"
#include "wearable.h"
#include "env_sensor.h"
#include "motion_sensor.h"
#include "nvm_handle.h"
#include "main.h"
#include "m2m_types.h"
#include "ecc_types.h"
#include "driver/include/m2m_ssl.h"
#include "atcacert/atcacert_client.h"
#include "atcacert/atcacert_host_hw.h"
#include "tls/atcatls.h"
#include "rtc.h"

//! Array of private key slots to rotate through the ECDH calculations
static uint16 g_ecdh_key_slot[] = {2};
char battery[] = "3.3V";
//! Index into the ECDH private key slots array
static uint32 g_ecdh_key_slot_index = 0;

uint8_t gMacaddr[M2M_MAC_ADDRES_LEN];
uint8 gDefaultSSID[M2M_MAX_SSID_LEN] = {0};
uint8 gAuthType = M2M_WIFI_SEC_INVALID;
uint8 gDefaultKey[M2M_MAX_PSK_LEN] = {0};
uint8 gUuid[AWS_COGNITO_UUID_LEN] = {0};
char gAwsMqttClientId[MQTT_CLIENT_ID_LEN]="40a23126739bb00d28b4e29f1bc00283bb01cecd";

char g_thing_name[129];
sint8	 gRSSI =0;
/** Wi-Fi status variable. */
bool gbConnectedWifi = false,receivedTime = false;

wifi_FSM_states wifi_states = WIFI_TASK_SWITCHING_TO_STA;

#define PUBLISH_BUTTON	SW0_PIN

int detSw0Sock = -1;	//detect sw0 socket

uint8 gu8WiFiMode = APP_STA;
static SOCKET provServerSocket = -1;
static uint8 gau8RxBuffer[SOCKET_BUFFER_MAX_LENGTH] = {0};
static uint8_t gAPEnabled = 0;

#define SOCK_TIMEOUT                   0x927C0 // 10min	
	
MQTTConnectParams connectParams;
MQTTSubscribeParams subParams;
MQTTMessageParams Msg;
MQTTPublishParams Params;

static ATCAIfaceCfg g_tls_crypto;


extern NodeInfo multiSpkDevice;
extern uint16_t battery_mV;

wifi_nvm_data_t wifi_nvm_data ={0};


#define WIFI_UPDATE_DEVICE_STATUS_TIME			(2000)




static void EnvSensorCallbackHandler(environment_data_t sensor_data, unsigned char flag)
{
	cJSON* item;
	int tempC = 0;
	
	sprintf(battery, "%d.%dV",(battery_mV/1000),(battery_mV/100)%10);
	printf("battery level = %s\r\n",battery);

	
	tempC = (sensor_data.temperature) * 0.01;
	tempC = 32+ (tempC * 1.8);
	printf("Temperature = %d\n",tempC);

	item = iot_message_reportInfo(DEVICE_NAME, tempC, battery, gRSSI);
	cloud_mqtt_publish(gPublish_Channel,item);
	
	cJSON_Delete(item);

	socketDeinit();
	m2m_wifi_deinit(NULL);
	nm_bsp_deinit();

	rtc_init();
	system_sleep();
}


void MQTTSubscribeCBCallbackHandler(int topic_len, char* topic_name, int payload_len, char* payload)
{
	printf("%s In\n", __func__);
	printf("%.*s\t%.*s",
	topic_len, topic_name, payload_len, payload);
	printf("\n\r");
	

	cJSON* item=NULL;
	char data_type[30];
	int data_value;

	
	Iot_Msg_Command cmd = iot_message_parser_cmd_type(payload);
	if (cmd == MSG_CMD_UPDATE)
	{
		int info_cnt = iot_message_get_info_count(payload);
		
		for (int i=0; i<info_cnt; i++)
		{
			iot_message_parser_info_data(payload, i, data_type, &data_value);
			printf("info --- dataType: %s, val: %d\n", data_type, data_value);
			
			switch (data_value)
			{
				case 0:
					led_ctrl_set_color(LED_COLOR_BLUE, LED_MODE_NONE);
					break;
				case 1:
					led_ctrl_set_color(LED_COLOR_GREEN, LED_MODE_NONE);
					break;
				case 2:
					led_ctrl_set_color(LED_COLOR_YELLOW, LED_MODE_NONE);
					break;
					
			}
			
			return;
		}
	}
	else if (cmd == MSG_CMD_SEARCH)
	{
		
		//item = iot_message_searchResp(DEVICE_TYPE,gAwsMqttClientId);
		environment_data_t env_data;
		get_env_sensor_data_for_display(&env_data);
		DBG_LOG("DBG: temperature = %d, humidity = %d, uv = %d, pressure = %d\r\n", env_data.temperature, env_data.humidity, env_data.uv, env_data.pressure);
		
		item = iot_message_searchResp_with_temp_uv(DEVICE_TYPE,gAwsMqttClientId, env_data.temperature, env_data.uv);
		cloud_mqtt_publish(gSearchResp_Channel,item);
	}
	else if (cmd == MSG_CMD_GET)
	{
		environment_data_t env_data;
		get_env_sensor_data_for_display(&env_data);
		
		DBG_LOG("DBG: temperature = %d, humidity = %d, uv = %d, pressure = %d\r\n", env_data.temperature, env_data.humidity, env_data.uv, env_data.pressure);
		
		NodeInfo info[5];
		
		strcpy(info[0].dataType,TEMP_DATATYPE_NAME);
		info[0].value = (int) env_data.temperature;
		
		strcpy(info[1].dataType,HUM_DATATYPE_NAME);
		info[1].value = (int) env_data.humidity;
		
		strcpy(info[2].dataType,UV_DATATYPE_NAME);
		info[2].value = (int) env_data.uv;
		
		strcpy(info[3].dataType,PRESSURE_DATATYPE_NAME);
		info[3].value = (int) env_data.pressure;
		
		strcpy(info[4].dataType,LED1_DATATYPE_NAME);
		Led_Color color = led_ctrl_get_color();
		if (color == LED_COLOR_YELLOW)	//align with the mobile APP option number, yellow is option number 2, blue is 0 and green is 1
			color = 2;	
		info[4].value = color;
		
		
		item = iot_message_reportAllInfo(DEVICE_TYPE, gAwsMqttClientId, 5, info);
		cloud_mqtt_publish(gPublish_Channel,item);
		
	}
	else if (cmd == MSG_SUBCMD_GET_3D_PLOT_DATA)
	{
		set_motion_sensor_update_timer(5);
	}
	
	if (item!=NULL)
		cJSON_Delete(item);
	
	return 0;
}

static void set_dev_param_to_mac(uint8 *param, uint8 *addr, uint8_t offset)
{
	/* Name must be in the format AtmelSmartPlug000000 */
	uint16 len;

	len = m2m_strlen(param);
	if (len >= offset) {
		param[len - 1] = HEX2ASCII((addr[5] >> 0) & 0x0f);
		param[len - 2] = HEX2ASCII((addr[5] >> 4) & 0x0f);
		param[len - 3] = HEX2ASCII((addr[4] >> 0) & 0x0f);
		param[len - 4] = HEX2ASCII((addr[4] >> 4) & 0x0f);
		param[len - 5] = HEX2ASCII((addr[3] >> 0) & 0x0f);
		param[len - 6] = HEX2ASCII((addr[3] >> 4) & 0x0f);
	}
}

static void close_app_socket(void)
{
	int8_t ret = M2M_SUCCESS;

	if (provServerSocket != -1) {
		ret = close(provServerSocket);
		printf("[AP] TCP server socket %d closed %d!\r\n", provServerSocket, ret);
		provServerSocket = -1;
	}
	return;
}

static void start_AP_app(void)
{
	struct sockaddr_in	addr;
	int ret = -1;
	
	/* TCP Server. */
	if(provServerSocket == -1) {
		if((provServerSocket = socket(AF_INET, SOCK_STREAM, 0)) >= 0) {
			// Initialize socket address structure.
			addr.sin_family      = AF_INET;
			addr.sin_port        = _htons(AP_TCP_SERVER_PORT);
			addr.sin_addr.s_addr = 0;

			if((ret = bind(provServerSocket, (struct sockaddr*)&addr, sizeof(addr))) == 0) {
				M2M_INFO("[AP] TCP socket bind success!\r\n");
			}
			else {
				M2M_INFO("[AP] Bind Failed. Error code = %d\r\n", ret);
				close(provServerSocket);
				M2M_INFO("[AP] TCP server socket %d closed!\r\n", provServerSocket);
				provServerSocket = -1;
			}
		}
		else {
			M2M_INFO("[AP] TCP Server Socket Creation Failed\r\n");
			return;
		}
	}
	else {
		accept(provServerSocket, NULL, 0);
	}
}


static void parse_cmd_frame(SOCKET sock, uint8 *provbuffer)
{
	uint16_t cmd_length = _get_u16(provbuffer + CMD_LEN_OFFSET) + SOF_LEN + CMDLNTH_LEN;
	if (cmd_length > MIN_CMD_LEN && cmd_length < SOCKET_BUFFER_MAX_LENGTH){
		//CRC32 checksum
		uint32_t chksum = 0;
		//To: Add the checksum
		//crc32_calculate(provbuffer, cmd_length, &chksum);
		//uint32_t rx_chksum = _get_u32(provbuffer + cmd_length);
		uint32_t rx_chksum = 0;
		if (chksum == rx_chksum) {
			//M2M_INFO("CRC pass\r\n");
			uint8_t cmd_id =  *(provbuffer + CMD_ID_OFFSET);
			cmd_resp_t cmd_response;
			memset(&cmd_response, 0, sizeof(cmd_response));
			// Parse IoT command
			parse_iot_cmd(sock, cmd_id, provbuffer + CMD_PARAM_OFFSET, &cmd_response);
			// Fill seq number
			cmd_response.data.base[CMD_SEQ_OFFSET] = *(provbuffer + CMD_SEQ_OFFSET);
			
			//To Do: Add checksum
			//crc32_calculate(cmd_response.data.base, cmd_response.length, &msg_check_sum);
			//memcpy(&cmd_response.data.base[cmd_response.length], &msg_check_sum, CHECKSUM_LEN);
			{
				DBG_LOG("send data, len = %d\r\n",cmd_response.length);
				//send(sock, (void *)cmd_response.data.base, \
				//cmd_response.length + CHECKSUM_LEN, 0);
				//if (cmd_response.length == 19)
				{
					DBG_LOG("send !!\r\n");
					send(sock, (void *)cmd_response.data.base, \
					cmd_response.length, 0);
				}
			}
			if (get_prov_state() == PROV_DONE) {
				nm_bsp_sleep(500);
				// Store WiFi information in NVM before connecting.
				wifi_nvm_data.ssidlen = strlen((char *)gDefaultSSID);
				printf("SSID len=%d\r\n", wifi_nvm_data.ssidlen);
				memcpy(wifi_nvm_data.ssid, gDefaultSSID, wifi_nvm_data.ssidlen);
				printf("SSID =%s\r\n", wifi_nvm_data.ssid);
				wifi_nvm_data.keylen = strlen((char *)gDefaultKey);
				memcpy(wifi_nvm_data.key, gDefaultKey, wifi_nvm_data.keylen);
				wifi_nvm_data.authtype = gAuthType;
				wifi_nvm_data.valid = 1;
				memcpy(wifi_nvm_data.uuid,gUuid,strlen(gUuid));
				
				//To Do:
				// Save wifi_nvm_data to flash
				nvm_store_config_data(wifi_nvm_data);
				///nv_flash_write(CONFIG_PAGE_ADDRESS,CONFIG_PAGE,(uint32_t*)(&wifi_nvm_data),sizeof(wifi_nvm_data));
				printf("Write config page\r\n");
				nvm_get_config_data(&wifi_nvm_data);
				printf("DBG SSID=%s\r\n", wifi_nvm_data.ssid);
			}
		}
		else {
			//send(get_session(session_num)->sock, (void *)"Checksum error!", strlen("Checksum error!"), 0);
		}
	}
	else {
		//send(get_session(session_num)->sock, (void *)"Msg length error!", strlen("Msg length error!"), 0);
	}
}



static void tcpsendresponse(SOCKET sock, tstrSocketRecvMsg *pstrRx)
{
	uint8 *provbuffer = pstrRx->pu8Buffer;
	
	if (provbuffer[0] == SOF_INDICATER) {
		// Plain text frame
		{
			parse_cmd_frame(sock, provbuffer);
		}
	}
	
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



/**
 * \brief Callback to get the Wi-Fi status update.
 *
 * \param[in] u8MsgType type of Wi-Fi notification. Possible types are:
 *  - [M2M_WIFI_RESP_CON_STATE_CHANGED](@ref M2M_WIFI_RESP_CON_STATE_CHANGED)
 *  - [M2M_WIFI_REQ_DHCP_CONF](@ref M2M_WIFI_REQ_DHCP_CONF)
 * \param[in] pvMsg A pointer to a buffer containing the notification parameters
 * (if any). It should be casted to the correct data type corresponding to the
 * notification type.
 */
static void wifi_cb(uint8_t u8MsgType, void *pvMsg)
{
	switch (u8MsgType) {
	case M2M_WIFI_RESP_CON_STATE_CHANGED:
	{
		tstrM2mWifiStateChanged *pstrWifiState = (tstrM2mWifiStateChanged *)pvMsg;
		if (pstrWifiState->u8CurrState == M2M_WIFI_CONNECTED) {
			printf("wifi_cb: M2M_WIFI_RESP_CON_STATE_CHANGED: CONNECTED\r\n");

			//get current RSSI
			m2m_wifi_req_curr_rssi();
			m2m_wifi_request_dhcp_client();
		} else if (pstrWifiState->u8CurrState == M2M_WIFI_DISCONNECTED) {
			printf("wifi_cb: M2M_WIFI_RESP_CON_STATE_CHANGED: DISCONNECTED\r\n");
			gbConnectedWifi = false;
			led_ctrl_set_color(LED_COLOR_RED, LED_MODE_TURN_ON);
			led_ctrl_set_mode(LED_MODE_TURN_ON);
			m2m_wifi_connect((char *)gDefaultSSID, strlen((char *)gDefaultSSID), 
			gAuthType, (char *)gDefaultKey, M2M_WIFI_CH_ALL);
		}

		break;
	}

	case M2M_WIFI_REQ_DHCP_CONF:
	{
		uint8_t *pu8IPAddress = (uint8_t *)pvMsg;
		/* Turn LED0 on to declare that IP address received. */
		printf("wifi_cb: M2M_WIFI_REQ_DHCP_CONF: IP is %u.%u.%u.%u\r\n",
				pu8IPAddress[0], pu8IPAddress[1], pu8IPAddress[2], pu8IPAddress[3]);
		gbConnectedWifi = true;

		led_ctrl_set_mode(LED_MODE_BLINK_FAST);
		/* Obtain the IP Address by network name */
		///gethostbyname((uint8_t *)HostAddress);
		if (gu8WiFiMode == APP_AP)
		{
			start_AP_app();
			wifi_states = WIFI_TASK_AP_CONNECTED;
		}
		else
			m2m_wifi_get_sytem_time();
		break;
	}
	
	case M2M_WIFI_RESP_GET_SYS_TIME:
	{
		printf("Received time\r\n");
		receivedTime = true;
		wifi_states = WIFI_TASK_CONNECT_CLOUD;
		break;
	}

	case M2M_WIFI_RESP_CURRENT_RSSI:
	{
		gRSSI = *((sint8*)pvMsg);
		M2M_INFO("Link RSSI = %d\n",gRSSI);

		break;

	}

	default:
	{
		break;
	}
	}
}

static sint8 ecdh_derive_client_shared_secret(tstrECPoint *server_public_key,
uint8 *ecdh_shared_secret,
tstrECPoint *client_public_Key)
{
	sint8 status = M2M_ERR_FAIL;
	
	if ((g_ecdh_key_slot_index < 0) ||
	(g_ecdh_key_slot_index >= (sizeof(g_ecdh_key_slot) / sizeof(g_ecdh_key_slot[0]))))
	{
		g_ecdh_key_slot_index = 0;
	}
	
	if (atcatls_create_key(g_ecdh_key_slot[g_ecdh_key_slot_index], client_public_Key->X) == ATCA_SUCCESS)
	{
		client_public_Key->u16Size = 32;
		if (atcab_ecdh(g_ecdh_key_slot[g_ecdh_key_slot_index], server_public_key->X,
		ecdh_shared_secret) == ATCA_SUCCESS)
		{
			status = M2M_SUCCESS;
			g_ecdh_key_slot_index++;
		}
	}
	
	return status;
}

static sint8 ecdh_derive_key_pair(tstrECPoint *server_public_key)
{
	sint8 status = M2M_ERR_FAIL;
	
	if ((g_ecdh_key_slot_index < 0) ||
	(g_ecdh_key_slot_index >= (sizeof(g_ecdh_key_slot) / sizeof(g_ecdh_key_slot[0]))))
	{
		g_ecdh_key_slot_index = 0;
	}

	if(atcatls_create_key(g_ecdh_key_slot[g_ecdh_key_slot_index], server_public_key->X) == ATCA_SUCCESS)
	{
		server_public_key->u16Size      = 32;
		server_public_key->u16PrivKeyID = g_ecdh_key_slot[g_ecdh_key_slot_index];

		g_ecdh_key_slot_index++;

		status = M2M_SUCCESS;
	}

	return status;
}

static sint8 ecdsa_process_sign_verify_request(uint32 number_of_signatures)
{
	sint8 status = M2M_ERR_FAIL;
	tstrECPoint	Key;
	uint32 index = 0;
	uint8 signature[80];
	uint8 hash[80] = {0};
	uint16 curve_type = 0;
	
	for(index = 0; index < number_of_signatures; index++)
	{
		status = m2m_ssl_retrieve_cert(&curve_type, hash, signature, &Key);
		if (status != M2M_SUCCESS)
		{
			M2M_ERR("m2m_ssl_retrieve_cert() failed with ret=%d", status);
			return status;
		}

		if(curve_type == EC_SECP256R1)
		{
			bool is_verified = false;
			status = atcatls_verify(hash, signature, Key.X, &is_verified);
			status = M2M_SUCCESS;
			break;
			if(status == ATCA_SUCCESS)
			{
				status = (is_verified == true) ? M2M_SUCCESS : M2M_ERR_FAIL;
				if(is_verified == false)
				{
					M2M_INFO("ECDSA SigVerif FAILED\n");
				}
			}
			else
			{
				status = M2M_ERR_FAIL;
			}
			
			if(status != M2M_SUCCESS)
			{
				m2m_ssl_stop_processing_certs();
				break;
			}
		}
	}
	return status;
}

static sint8 ecdsa_process_sign_gen_request(tstrEcdsaSignReqInfo *sign_request,
uint8 *signature,
uint16 *signature_size)
{
	sint8 status = M2M_ERR_FAIL;
	uint8 hash[32];
	
	status = m2m_ssl_retrieve_hash(hash, sign_request->u16HashSz);
	if (status != M2M_SUCCESS)
	{
		M2M_ERR("m2m_ssl_retrieve_hash() failed with ret=%d", status);
		return status;
	}

	if(sign_request->u16CurveType == EC_SECP256R1)
	{
		*signature_size = 64;
		status = atcatls_sign(DEVICE_KEY_SLOT, hash, signature);
	}

	return status;
}

static sint8 ecdh_derive_server_shared_secret(uint16 private_key_id,
tstrECPoint *client_public_key,
uint8 *ecdh_shared_secret)
{
	uint16 key_slot	= private_key_id;
	sint8 status = M2M_ERR_FAIL;
	uint8 atca_status = ATCA_STATUS_UNKNOWN;

	atca_status = atcab_ecdh(key_slot, client_public_key->X, ecdh_shared_secret);
	if(atca_status == ATCA_SUCCESS)
	{
		status = M2M_SUCCESS;
	}
	else
	{
		M2M_INFO("__SLOT = %u, Err = %X\n", key_slot, atca_status);
	}
	
	return status;
}

static void eccProcessREQ(tstrEccReqInfo *ecc_request)
{
	tstrEccReqInfo ecc_response;
	uint8 signature[80];
	uint16 response_data_size = 0;
	uint8 *response_data_buffer = NULL;
	
	ecc_response.u16Status = 1;

	switch (ecc_request->u16REQ)
	{
		case ECC_REQ_CLIENT_ECDH:
		ecc_response.u16Status = ecdh_derive_client_shared_secret(&(ecc_request->strEcdhREQ.strPubKey),
		ecc_response.strEcdhREQ.au8Key,
		&ecc_response.strEcdhREQ.strPubKey);
		break;

		case ECC_REQ_GEN_KEY:
		ecc_response.u16Status = ecdh_derive_key_pair(&ecc_response.strEcdhREQ.strPubKey);
		break;

		case ECC_REQ_SERVER_ECDH:
		ecc_response.u16Status = ecdh_derive_server_shared_secret(ecc_request->strEcdhREQ.strPubKey.u16PrivKeyID,
		&(ecc_request->strEcdhREQ.strPubKey),
		ecc_response.strEcdhREQ.au8Key);
		break;
		
		case ECC_REQ_SIGN_VERIFY:
		ecc_response.u16Status = ecdsa_process_sign_verify_request(ecc_request->strEcdsaVerifyREQ.u32nSig);
		break;
		
		case ECC_REQ_SIGN_GEN:
		ecc_response.u16Status = ecdsa_process_sign_gen_request(&(ecc_request->strEcdsaSignREQ), signature,
		&response_data_size);
		response_data_buffer = signature;
		break;
		
		default:
		// Do nothing
		break;
	}
	
	ecc_response.u16REQ      = ecc_request->u16REQ;
	ecc_response.u32UserData = ecc_request->u32UserData;
	ecc_response.u32SeqNo    = ecc_request->u32SeqNo;

	m2m_ssl_ecc_process_done();
	m2m_ssl_handshake_rsp(&ecc_response, response_data_buffer, response_data_size);
}



static void ssl_cb(uint8 u8MsgType, void * pvMsg)
{
	switch(u8MsgType)
	{
		case M2M_SSL_REQ_ECC:
		{
			tstrEccReqInfo *pstrEccREQ = (tstrEccReqInfo *)pvMsg;
			eccProcessREQ(pstrEccREQ);
		}
		break;
		case M2M_SSL_RESP_SET_CS_LIST:
		{
			tstrSslSetActiveCsList *pstrCsList = (tstrSslSetActiveCsList *)pvMsg;
			M2M_INFO("ActiveCS bitmap:%04x\n", pstrCsList->u32CsBMP);
		}
		break;
	}
}


void setWiFiStates(wifi_FSM_states state)
{
	wifi_states = state;
	return;
}

wifi_FSM_states getWiFiStates()
{
	return wifi_states;
}

wifi_mode getWiFiMode()
{
	return gu8WiFiMode;
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

		
        status = ATCA_SUCCESS;
    } while (false);    
    
    return status;
}

int wifiCryptoInit(void)
{	
	tstrWifiInitParam param;
	int8_t ret;
	ATCA_STATUS atca_status;


	/* Initialize Wi-Fi parameters structure. */
	memset((uint8_t *)&param, 0, sizeof(tstrWifiInitParam));

	/* Initialize Wi-Fi driver with data and status callbacks. */
	param.pfAppWifiCb = wifi_cb;

	// Initialize the WINC1500 
	ret = m2m_wifi_init(&param);
	if (M2M_SUCCESS != ret) {
		printf("main: m2m_wifi_init call error!(%d)\r\n", ret);
		while (1) {
		}
	}
			m2m_wifi_set_sleep_mode(M2M_PS_DEEP_AUTOMATIC, 1);

	// Initialize the ECC608
	atca_status = cryptoauthlib_init();
	if (atca_status == ATCACERT_E_SUCCESS)
	{
		atca_status = eccPrintInfo();
	}
	
	// Initialize the WINC1500 SSL module
	ret = m2m_ssl_init(ssl_cb);
	if (ret != M2M_SUCCESS)
	{
		printf("main: m2m_ssl_init call error!(%d)\r\n", ret);
		while (1) {
		}
	}


	// Set the active WINC1500 TLS cipher suites to ECC only
	//ret = m2m_ssl_set_active_ciphersuites(SSL_ECC_ONLY_CIPHERS);
	ret = m2m_ssl_set_active_ciphersuites(SSL_NON_ECC_CIPHERS_AES_128);
	if (ret != M2M_SUCCESS)
	{
		printf("main: m2m_ssl_set_active_ciphersuites call error!(%d)\r\n", ret);
		while (1) {
		}
	}


	// Prepare the MQTT topic
	gAwsMqttClientId[20*2] = 0; // Add terminating null

	// Make the thing name the same as the MQTT client ID
	memcpy(g_thing_name, gAwsMqttClientId, min(sizeof(g_thing_name), sizeof(gAwsMqttClientId)));
	g_thing_name[sizeof(g_thing_name)-1] = 0; // Ensure a terminating null
	
	printf("Thing name = %s\r\n", g_thing_name);

	//Generate MQTT Topic
	cloud_create_topic(gSubscribe_Channel, DEVICE_TYPE, gAwsMqttClientId, SUBSCRIBE_TOPIC);
	cloud_create_topic(gPublish_Channel, DEVICE_TYPE, gAwsMqttClientId, PUBLISH_TOPIC);
	DBG_LOG("gSubscribe_Channel: %s\r\n", gSubscribe_Channel);
	DBG_LOG("gPublish_Channel: %s\r\n", gPublish_Channel);

	//Set Wi-Fi AP credintials
	memcpy(gDefaultSSID, MAIN_WLAN_SSID, strlen(MAIN_WLAN_SSID));
	memcpy(gDefaultKey, MAIN_WLAN_PSK, strlen(MAIN_WLAN_PSK));
	gAuthType = MAIN_WLAN_AUTH;
	gDefaultSSID[strlen(MAIN_WLAN_SSID)]=0;
	gDefaultKey[strlen(MAIN_WLAN_PSK)]=0;
	
	register_env_sensor_udpate_callback_handler(EnvSensorCallbackHandler);

	DBG_LOG("connecting AP, ssid = %s , pwd= %s\n", (char *)gDefaultSSID,(char *)gDefaultKey);
	m2m_wifi_connect((char *)gDefaultSSID, strlen(gDefaultSSID), gAuthType, (char *)gDefaultKey, M2M_WIFI_CH_ALL);
}

