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
#include "iot_message.h"
#include "cJSON.h"
#include "datapoint.h"
#include "string.h"
#include "main.h"

extern uint8 gUuid[AWS_COGNITO_UUID_LEN];

int iot_message_get_info_count(char* json_msg)
{
	cJSON *json;
	cJSON *json_command;
	cJSON *json_info_array;
	int info_cnt;
	
	json=cJSON_Parse(json_msg);
		
	if (!json) {
		printf("Error when decode json: [%s]\n",cJSON_GetErrorPtr());
		return 0;
	}
	

	json_command = cJSON_GetObjectItem(json,ATTR_COMMAND_NAME);
	printf("DBG command = %s\n", json_command->valuestring);
	
	if (0 == strcmp(json_command->valuestring,CMD_UPDATE))
	{
		json_info_array = cJSON_GetObjectItem(json,ATTR_INFO_NAME);
		info_cnt = cJSON_GetArraySize(json_info_array);
		
	}
	else 
		info_cnt = 0;
	
	cJSON_Delete(json);	
	
	return info_cnt;
}


Iot_Message_RC iot_message_parser_info_data(char* json_msg, int array_idx, char* dataType, int* value)
{
	cJSON *json;
	cJSON *json_command;
	cJSON *json_info_array;
	cJSON *json_info;
	cJSON *json_data_type;
	
	Iot_Message_RC ret;
	
	json=cJSON_Parse(json_msg);
	
	if (!json) {
		printf("Error when decode json: [%s]\n",cJSON_GetErrorPtr());
		return IOT_MSG_RC_DECODE_FAIL;
	}
	

	json_command = cJSON_GetObjectItem(json,ATTR_COMMAND_NAME);
	printf("DBG command = %s\n", json_command->valuestring);
	
	if (0 == strcmp(json_command->valuestring,CMD_UPDATE))
	{
		json_info_array = cJSON_GetObjectItem(json,ATTR_INFO_NAME);
		int j = cJSON_GetArraySize(json_info_array);
		if (array_idx >= j)
		{
			cJSON_Delete(json);
			return IOT_MSG_RC_FAIL;
		}
		json_info = cJSON_GetArrayItem(json_info_array, array_idx);
		json_data_type = cJSON_GetObjectItem(json_info, ATTR_DATATYPE_NAME);
		//json_data_value = cJSON_GetObjectItem(json_info, ATTR_VALUE_NAME);
		//int data_value = cJSON_GetObjectItem(json_info, ATTR_VALUE_NAME)->valueint;
		*value = cJSON_GetObjectItem(json_info, ATTR_VALUE_NAME)->valueint;
		printf("DBG temp = %d\r\n", *value);
		
		strcpy(dataType, json_data_type->valuestring);
		ret = IOT_MSG_RC_SUCCESS;
	}
	else
		ret = IOT_MSG_RC_DECODE_FAIL;
	
	cJSON_Delete(json);
	
	return ret;
	
}



Iot_Message_RC iot_message_parser_addnode_cmd_msg(char* json_msg, char* node_mac_addr)
{
	cJSON *json;
	cJSON *json_mac_addr;
	
	json=cJSON_Parse(json_msg);
	
	if (!json) {
		printf("Error when decode json: [%s]\n",cJSON_GetErrorPtr());
		return IOT_MSG_RC_DECODE_FAIL;
	}
	
	json_mac_addr = cJSON_GetObjectItem(json,ATTR_MAC_ADDR_NAME);
	printf("Add Node [%s]\n",json_mac_addr->valuestring);
		
	sscanf(json_mac_addr->valuestring,"%02x%02x%02x%02x%02x%02x",&node_mac_addr[0],&node_mac_addr[1], &node_mac_addr[2],&node_mac_addr[3],&node_mac_addr[4],&node_mac_addr[5]);
	//printf("Add Node [%x,%x,%x,%x,%x,%x ]\n",node_mac_addr[0], node_mac_addr[1], node_mac_addr[2],node_mac_addr[3],node_mac_addr[4],node_mac_addr[5]);
	
	cJSON_Delete(json);	
	
	return IOT_MSG_RC_SUCCESS;
}

Iot_Msg_Command iot_message_parser_cmd_type(char* json_msg)
{
	cJSON *json;
	cJSON *json_command, *json_subcommand;
	Iot_Msg_Command cmd;
	
	json=cJSON_Parse(json_msg);
	
	if (!json) {
		printf("Error when decode json: [%s]\n",cJSON_GetErrorPtr());
		return MSG_CMD_UNKNOWN;
	}
	
	json_command = cJSON_GetObjectItem(json,ATTR_COMMAND_NAME);
	printf("DBG command = %s\n", json_command->valuestring);
	
	
	if (0 == strcmp(json_command->valuestring,CMD_UPDATE))
		cmd = MSG_CMD_UPDATE;
	else if (0 == strcmp(json_command->valuestring,CMD_SEARCH))
		cmd = MSG_CMD_SEARCH;	
	else if (0 == strcmp(json_command->valuestring,CMD_GET))
		cmd = MSG_CMD_GET;
	else if (0 == strcmp(json_command->valuestring,CMD_SEARCHRESP))
		cmd = MSG_CMD_SEARCHRESP;
	else if (0 == strcmp(json_command->valuestring,CMD_REPORTALLINFO))
		cmd = MSG_CMD_REPORTALLINFO;
	else if (0 == strcmp(json_command->valuestring,CMD_REPORTINFO))
		cmd = MSG_CMD_REPORTINFO;
	else if (0 == strcmp(json_command->valuestring,CMD_REPORTDISCONNECT))	
		cmd = MSG_CMD_REPORTDISCONNECT;
	else if (0 == strcmp(json_command->valuestring,CMD_CONTROL))
	{
		json_subcommand = cJSON_GetObjectItem(json,ATTR_SUBCOMMAND_NAME);
		if (0 == strcmp(json_subcommand->valuestring,CMD_ADD_NODE))
			cmd = MSG_SUBCMD_ADDNODE;
		else if (0 == strcmp(json_subcommand->valuestring,CMD_GET_3D_PLOT_DATA))
			cmd = MSG_SUBCMD_GET_3D_PLOT_DATA;
	}
	else 
		cmd = MSG_CMD_UNKNOWN;
	
	cJSON_Delete(json);	
	
	return cmd;
}		


int iot_message_parser(char* json_msg, char* cmd, char* mac_addr, char* info)
{
	cJSON *json;
	cJSON *json_command;
	cJSON *json_mac_addr;
	cJSON *json_endnode_info;
	cJSON *json_info;
	char *out = NULL;
	
	json=cJSON_Parse(json_msg);
	
	if (!json) {
		printf("Error when decode json: [%s]\n",cJSON_GetErrorPtr());
		return 1;
	}
	
	out = cJSON_Print(json);
	//cJSON_Delete(json);
	printf("decode-------\n%s\n", out);
	free(out);

	json_command = cJSON_GetObjectItem(json,ATTR_COMMAND_NAME);
	printf("DBG command = %s\n", json_command->valuestring);
	strcpy(cmd, json_command->valuestring);
	
	if (0 == strcmp(json_command->valuestring,CMD_UPDATE))
	{
		json_endnode_info = cJSON_GetObjectItem(json,ATTR_INFO_NAME);
		int j = cJSON_GetArraySize(json_endnode_info);	
		printf("j= %d\n",j);
		json_mac_addr = cJSON_GetArrayItem(json_endnode_info, 0);
		json_info = cJSON_GetObjectItem(json_mac_addr, ATTR_MAC_ADDR_NAME);
		
		strcpy(mac_addr, json_mac_addr->valuestring);
		strcpy(info, json_info->valuestring);
		
	}
	printf("DBG cmd = %s, mac = %s, info = %s\n", json_command->valuestring, json_info->valuestring, json_info->valuestring);
	
	cJSON_Delete(json);	
	return 0;
}

cJSON* iot_message_searchResp_with_temp_uv(char* device_type, char* mac_addr, int temp, int uv )
{
	cJSON *json2CloudData;
	cJSON *json2CloudNodeData;
	cJSON *json2CloudDataArray1;
	
	json2CloudData=cJSON_CreateObject();
	

	if(json2CloudData == NULL)
	{
		printf("json2CloudData malloc fail");
		while(1);
	}
	
	cJSON_AddStringToObject(json2CloudData, ATTR_DEVNAME_NAME, DEVICE_NAME);
	cJSON_AddStringToObject(json2CloudData, ATTR_COMMAND_NAME, CMD_SEARCHRESP);
	cJSON_AddStringToObject(json2CloudData, ATTR_DEVTYPE_NAME, device_type);
	cJSON_AddStringToObject(json2CloudData, ATTR_MAC_ADDR_NAME, mac_addr);
	
	json2CloudDataArray1=cJSON_CreateArray();
	json2CloudNodeData=cJSON_CreateObject();
	
	cJSON_AddStringToObject(json2CloudNodeData, "dataType", TEMP_DATATYPE_NAME);
	cJSON_AddNumberToObject(json2CloudNodeData, "value", temp);
	cJSON_AddItemToArray(json2CloudDataArray1, json2CloudNodeData);
	
	json2CloudNodeData=cJSON_CreateObject();
	cJSON_AddStringToObject(json2CloudNodeData, "dataType", UV_DATATYPE_NAME);
	cJSON_AddNumberToObject(json2CloudNodeData, "value", uv);
	cJSON_AddItemToArray(json2CloudDataArray1, json2CloudNodeData);
	
	cJSON_AddItemToObject(json2CloudData, "info", json2CloudDataArray1);
	return json2CloudData;
}

cJSON* iot_message_searchResp(char* device_type, char* mac_addr)
{
	cJSON *json2CloudData;
	
	json2CloudData=cJSON_CreateObject();
	
	if(json2CloudData == NULL)
	{
		printf("json2CloudData malloc fail");
		while(1);
	}
	
	cJSON_AddStringToObject(json2CloudData, ATTR_DEVNAME_NAME, DEVICE_NAME);
	cJSON_AddStringToObject(json2CloudData, ATTR_COMMAND_NAME, CMD_SEARCHRESP);
	cJSON_AddStringToObject(json2CloudData, ATTR_DEVTYPE_NAME, device_type);
	cJSON_AddStringToObject(json2CloudData, ATTR_MAC_ADDR_NAME, mac_addr);
	
	
	return json2CloudData;
}


cJSON* iot_message_reportAllInfo(char* device_type, char* mac_addr, int num_of_data, NodeInfo data[])
{
	cJSON *json2CloudData;
	cJSON *json2CloudNodeData;
	cJSON *json2CloudDataArray1;
	
	json2CloudData=cJSON_CreateObject();
	//json2CloudNodeData=cJSON_CreateObject();
	json2CloudDataArray1=cJSON_CreateArray();

	if((json2CloudData == NULL) || (json2CloudDataArray1 == NULL))
	{
		printf("json2CloudData malloc fail");
		while(1);
	}
	
	cJSON_AddStringToObject(json2CloudData, ATTR_DEVNAME_NAME, DEVICE_NAME);
	cJSON_AddStringToObject(json2CloudData, ATTR_UUID_NAME, gUuid);
	cJSON_AddStringToObject(json2CloudData, ATTR_COMMAND_NAME, CMD_REPORTALLINFO);
	cJSON_AddStringToObject(json2CloudData, ATTR_DEVTYPE_NAME, device_type);
	cJSON_AddStringToObject(json2CloudData, ATTR_MAC_ADDR_NAME, mac_addr);

	for (int i=0; i<num_of_data; i++)
	{
		json2CloudNodeData=cJSON_CreateObject();
		
		cJSON_AddStringToObject(json2CloudNodeData, "dataType", data[i].dataType);
		cJSON_AddNumberToObject(json2CloudNodeData, "value", data[i].value);
	
		cJSON_AddItemToArray(json2CloudDataArray1, json2CloudNodeData);
		
	}
	
	cJSON_AddItemToObject(json2CloudData, "info", json2CloudDataArray1);
	
	return json2CloudData;

}
extern char battery[];

cJSON* iot_message_reportInfo(char* device_name, int temp, char* batteryV, int rssi)
{
	cJSON *json2CloudData;
	cJSON *json2CloudNodeData;
	cJSON *json2CloudDataArray1;
	
	json2CloudData=cJSON_CreateObject();
	json2CloudDataArray1=cJSON_CreateArray();

	if((json2CloudData == NULL) || (json2CloudDataArray1 == NULL))
	{
		printf("json2CloudData malloc fail");
		while(1);
	}


		
	cJSON_AddStringToObject(json2CloudData, ATTR_BATTERY, batteryV);
	cJSON_AddStringToObject(json2CloudData, ATTR_NODE_ID, device_name);
	cJSON_AddNumberToObject(json2CloudData, ATTR_TEMP, temp);
	cJSON_AddNumberToObject(json2CloudData, ATTR_RSSI, rssi);

	
	return json2CloudData;
}


cJSON* iot_message_reportDisconnect(int report_dev_num, NodeInfo endnode_info[])
{
	cJSON *json2CloudData;
	cJSON *json2CloudNodeData;
	cJSON *json2CloudDataArray1;
	printf("%s In\n", __func__);
	json2CloudData=cJSON_CreateObject();
	json2CloudDataArray1=cJSON_CreateArray();

	if((json2CloudData == NULL) || (json2CloudDataArray1 == NULL))
	{
		printf("json2CloudData malloc fail");
		while(1);
	}
	
	cJSON_AddStringToObject(json2CloudData, "command", CMD_REPORTDISCONNECT);



	for (int i=0; i<report_dev_num; i++)
	{
		json2CloudNodeData=cJSON_CreateObject();
		
		//cJSON_AddStringToObject(json2CloudNodeData, "devName", endnode_info[i].dev_name);
		
		cJSON_AddItemToArray(json2CloudDataArray1, json2CloudNodeData);
	}
	
	cJSON_AddItemToObject(json2CloudData, "endNodeInfo", json2CloudDataArray1);
	
	return json2CloudData;
}