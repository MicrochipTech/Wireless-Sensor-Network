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
#include "main.h"
#include "socket/include/socket.h"
#include "string.h"
#include "AWS_SDK/aws_iot_src/utils/aws_iot_log.h"
#include "AWS_SDK/aws_iot_src/utils/aws_iot_version.h"
#include "AWS_SDK/aws_iot_src/protocol/mqtt/aws_iot_mqtt_interface.h"
#include "winc15x0.h"
#include "cJSON.h"
#include "cloud_wrapper.h"
#include "iot_message.h"
#include "aws_iot_config.h"

/**
 * @brief Default MQTT HOST URL is pulled from the aws_iot_config.h
 */
char HostAddress[255] = AWS_IOT_MQTT_HOST;

/**
 * @brief Default MQTT port is pulled from the aws_iot_config.h
 */
uint32_t port = AWS_IOT_MQTT_PORT;


MQTTConnectParams connectParams;
MQTTSubscribeParams subParams;
MQTTMessageParams Msg;
MQTTPublishParams Params;




static bool toggle = false;

static void (*mqtt_subscribe_cb)(int topic_len, char* topic_name, int payload_len, char* payload);


static void disconnectCallbackHandler(void) {
	printf("MQTT Disconnect");
	IoT_Error_t rc = NONE_ERROR;
	if(aws_iot_is_autoreconnect_enabled()){
		printf("Auto Reconnect is enabled, Reconnecting attempt will start now");
		}else{
		printf("Auto Reconnect not enabled. Starting manual reconnect...");
		rc = aws_iot_mqtt_attempt_reconnect();
		if(RECONNECT_SUCCESSFUL == rc){
			printf("Manual Reconnect Successful");
			}else{
			printf("Manual Reconnect Failed - %d", rc);
		}
	}
}

static int32_t MQTTcallbackHandler(MQTTCallbackParams params) {

	printf("Subscribe callback");
	printf("%.*s\t%.*s",
	(int)params.TopicNameLen, params.pTopicName,
	(int)params.MessageParams.PayloadLen, (char*)params.MessageParams.pPayload);
	printf("\n\r");
	
	mqtt_subscribe_cb((int)params.TopicNameLen, params.pTopicName, (int)params.MessageParams.PayloadLen, (char*)params.MessageParams.pPayload);
	return 0;
}


static void jsonMessagePublish(char* channel, cJSON *message)
{
	IoT_Error_t rc = NONE_ERROR;
	char* out;
	out = cJSON_Print(message);
	Msg.qos = QOS_0;
	Msg.PayloadLen = strlen(out) + 1;
	Msg.pPayload = (void *)out ;
	Params.pTopic = (char*)channel;
	Params.MessageParams = Msg;
	rc = aws_iot_mqtt_publish(&Params);
	if (rc != NONE_ERROR)
	printf("aws_iot_mqtt_publish() error, rc = %d\n", rc);
	free(out);
	
}

Cloud_RC cloud_connect()
{
	IoT_Error_t rc = NONE_ERROR;
	
	gethostbyname((uint8_t *)HostAddress);
	
	connectParams = MQTTConnectParamsDefault;
	subParams = MQTTSubscribeParamsDefault;
	Msg = MQTTMessageParamsDefault;
	Params = MQTTPublishParamsDefault;
			
	connectParams.KeepAliveInterval_sec = 10;
	connectParams.isCleansession = true;
	connectParams.MQTTVersion = MQTT_3_1_1;
	connectParams.pClientID = gAwsMqttClientId;
	connectParams.pHostURL = HostAddress;
	connectParams.port = port;
	connectParams.isWillMsgPresent = false;
	connectParams.pRootCALocation = NULL;
	connectParams.pDeviceCertLocation = NULL;
	connectParams.pDevicePrivateKeyLocation = NULL;
	connectParams.mqttCommandTimeout_ms = 5000;
	connectParams.tlsHandshakeTimeout_ms = 5000;
	connectParams.isSSLHostnameVerify = true; // ensure this is set to true for production
	connectParams.disconnectHandler = disconnectCallbackHandler;
			
	if(gbConnectedWifi && receivedTime)
	{
		//sslEnableCertExpirationCheck(0);
		printf("Connecting...");
		rc = aws_iot_mqtt_connect(&connectParams);
		if (NONE_ERROR != rc) {
			printf("Error(%d) connecting to %s:%d", rc, connectParams.pHostURL, connectParams.port);
			return rc;
		}
		/*
		* Enable Auto Reconnect functionality. Minimum and Maximum time of Exponential backoff are set in aws_iot_config.h
		*  #AWS_IOT_MQTT_MIN_RECONNECT_WAIT_INTERVAL
		*  #AWS_IOT_MQTT_MAX_RECONNECT_WAIT_INTERVAL
		*/
		rc = aws_iot_mqtt_autoreconnect_set_status(true);
		if (NONE_ERROR != rc) {
			printf("Unable to set Auto Reconnect to true - %d", rc);
			return rc;
		}
		
		return rc;

	}
	return rc;
}

Cloud_RC cloud_mqtt_yield(int timeout)
{
	IoT_Error_t rc = NONE_ERROR;
	rc = aws_iot_mqtt_yield(timeout);
	return rc;
}

Cloud_RC cloud_mqtt_publish(char* channel, void* message)
{
	IoT_Error_t rc = NONE_ERROR;
	jsonMessagePublish(channel, message);
	
	return rc;
	
}

Cloud_RC cloud_mqtt_subscribe(char* channel, void* cb)
{
	IoT_Error_t rc = NONE_ERROR;
	
	mqtt_subscribe_cb = cb;
	
	subParams.mHandler = MQTTcallbackHandler;
	subParams.pTopic = (char*)channel;
	subParams.qos = QOS_0;

	if (NONE_ERROR == rc) {
		printf("Subscribing...");
		rc = aws_iot_mqtt_subscribe(&subParams);
		if (NONE_ERROR != rc) {
			printf("Error subscribing %d\r\n",rc);
			return rc;
		}
	}
	printf("Subscription success\r\n");
	
	return rc;
	
}

Cloud_RC cloud_create_topic(char* full_path, char* device_type, char* device_id, char* topic_name)
{
	IoT_Error_t rc = NONE_ERROR;
	
	//sprintf(full_path, "%s/%s/%s\0", device_type, device_id, topic_name);
	sprintf(full_path, "/Microchip/WSN_Demo/WiFi\0");
	return rc;
	
}

Cloud_RC cloud_create_search_topic(char* full_path, char* acct_uuid, char* topic_name)
{
	IoT_Error_t rc = NONE_ERROR;
	sprintf(full_path, "winc1500Iot/%s/%s\0",acct_uuid, topic_name);
	return rc;
	
}
