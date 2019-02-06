/**
 * \file
 *
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
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel
 * Support</a>
 */

#ifndef __IOT_MESSAGE_H__
#define __IOT_MESSAGE_H__
#include "cJSON.h"
#include "datapoint.h"

#define ATTR_COMMAND_NAME	"command"
#define ATTR_SUBCOMMAND_NAME	"subcommand"
#define ATTR_DEVNAME_NAME	"devLocation"//"devName" // WSN Demo
#define ATTR_DEVTYPE_NAME	"devType"
#define ATTR_DATATYPE_NAME	"dataType"
#define ATTR_VALUE_NAME	"value"
#define ATTR_MAC_ADDR_NAME	"macAddr"
#define ATTR_INFO_NAME	"info"
#define ATTR_UUID_NAME	"uuid"

#define CMD_SEARCH	"search"
#define CMD_GET	"get"
#define CMD_UPDATE	"update"
#define CMD_SEARCHRESP	"searchResp"
#define CMD_REPORTALLINFO	"reportAllInfo"
#define CMD_REPORTINFO	"reportInfo"
#define CMD_REPORTDISCONNECT	"reportDisconnect"
#define CMD_CONTROL	"control"
#define CMD_ADD_NODE	"addNode"
#define CMD_GET_3D_PLOT_DATA	"get3dPlotData"

#define TEMP_DATATYPE_NAME	"temp"
#define HUM_DATATYPE_NAME	"hum"
#define UV_DATATYPE_NAME	"uv"
#define PRESSURE_DATATYPE_NAME	"pressure"
#define LED1_DATATYPE_NAME	"led1"


#define MIWI_TEMP_DATATYPE_NAME "Temperature"
#define MIWI_BATTERY_DATATYPE_NAME "Battery"
#define MIWI_ROOMNAME_DATATYPE_NAME "Room Name"
#define MIWI_RSSI_DATATYPE_NAME "RSSI"
#define MIWI_NODEID_DATATYPE_NAME "nodeID"

#define ROTATION_W_DATATYPE_NAME	"r_w_cor"
#define ROTATION_X_DATATYPE_NAME	"r_x_cor"
#define ROTATION_Y_DATATYPE_NAME	"r_y_cor"
#define ROTATION_Z_DATATYPE_NAME	"r_z_cor"


typedef enum Iot_Msg_Command
{
	MSG_CMD_SEARCH = 0,
	MSG_CMD_GET = 1,
	MSG_CMD_UPDATE = 2,
	MSG_CMD_SEARCHRESP = 3,
	MSG_CMD_REPORTALLINFO = 4,
	MSG_CMD_REPORTINFO = 0,
	MSG_CMD_REPORTDISCONNECT = 5,
	MSG_CMD_CONTROL = 6,
	MSG_SUBCMD_ADDNODE = 7,
	MSG_SUBCMD_GET_3D_PLOT_DATA = 8,
	MSG_CMD_UNKNOWN 
}Iot_Msg_Command;

typedef enum {
	IOT_MSG_RC_NONE_ERROR = 0,
	IOT_MSG_RC_SUCCESS = 0,
	IOT_MSG_RC_FAIL= 1,
	IOT_MSG_RC_DECODE_FAIL= 2,
}Iot_Message_RC;

int iot_message_parser(char* json_msg, char* cmd, char* mac_addr, char* info);
int iot_message_get_info_count(char* json_msg);
Iot_Message_RC iot_message_parser_info_data(char* json_msg, int array_idx, char* dataType, int* value);
Iot_Message_RC iot_message_parser_addnode_cmd_msg(char* json_msg, char* node_mac_addr);
//Iot_Message_RC iot_message_parser_control_cmd_msg(char* json_msg);
Iot_Msg_Command iot_message_parser_cmd_type(char* json_msg);
cJSON* iot_message_searchResp(char* device_type, char* mac_addr);
cJSON* iot_message_searchResp_with_temp_uv(char* device_type, char* mac_addr, int temp, int uv );
cJSON* iot_message_reportAllInfo(char* device_type, char* mac_addr, int online_dev_num, NodeInfo endnode_info[]);
cJSON* iot_message_reportInfo(char* device_type, char* mac_addr, int report_data_num, NodeInfo data_info[]);
cJSON* iot_message_reportDisconnect(int report_dev_num, NodeInfo endnode_info[]);

#endif /*__WIFI_APP_H__*/
