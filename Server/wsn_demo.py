import json as jasons
import unicodedata
import redis
import copy
import csv
from flask import Flask, jsonify,  render_template
from flask_json import FlaskJSON, as_json_p
from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
from string import Template
from random import randint
from flask_mqtt import Mqtt
from operator import methodcaller
from threading import Lock
from redisworks import Root
from os import getpid
from datetime import datetime


capitalize = methodcaller('capitalize')


##### Flask APP #####
app = Flask(__name__)
json = FlaskJSON(app)

db = redis.Redis('localhost')
redisDB = Root()

app.config['JSON_ADD_STATUS'] = False
app.config['JSON_JSONP_OPTIONAL'] = True
##################################################

################Init Lock################
lock = Lock()
##########################################



################Init Nodes################
#our Rooms database
USMastersNodeLocation = {
    "Node1": "Desert Suite 1",
    "Node2": "Desert Suite 2",
    "Node3": "Desert Suite 3",
    "Node4": "Desert Suite 4",
    "Node5": "Desert Suite 5",
    "Node6": "Desert Suite 6",
    "Node7": "Desert Suite 7",
    "Node8": "Desert Suite 8",
    "Node9": "Pinnacle Peak 1",
    "Node10": "Pinnacle Peak 2",
    "Node11": "Pinnacle Peak 3",
    "Node12": "Wildflower A",
    "Node13": "Wildflower B",
    "Node14": "Wildflower C",
    "Node15": "Grand Canyon 1",
    "Node16": "Grand Canyon 2",
    "Node17": "Grand Canyon 3",
    "Node18": "Grand Canyon 4",
    "Node19": "Grand Canyon 5",
    "Node20": "Grand Canyon 9",
    "Node21": "Grand Canyon 10",
    "Node22": "Grand Canyon 11",
    "Node23": "Grand Canyon 12",
    "Node24": "Grand Sonoran A",
    "Node25": "Grand Sonoran B",
    "Node26": "Grand Sonoran C",
    "Node27": "Grand Sonoran D",
    "Node28": "Grand Sonoran H",
    "Node29": "Grand Sonoran I",
    "Node30": "Grand Sonoran J",
    "Node31": "Grand Sonoran K",
    "Node32": "ATE / Grand Canyon 6",
    "Node33": "Cyber Cafe / Grand Sonoran G",
    "Node34": "Grand Saguaro East/West",
    "Node35": "Golf course"
}

CES19NodeLocation = {
    "Node4": "Connected Home Display",
    "Node6": "Microchip welcome booth",
    "Node17": "Connected Factory Display",
    "Node19": "Connected Car Display",
    "Node32": "Connected Factory Demo",
}

IndiaMastersNodeLocation = {
    "Node1": "Beaumont",
    "Node2": "Orchid",
    "Node3": "Utsav",
    "Node4": "Dominion",
    "Node5": "Senate",
    "Node6": "Coronet 2",
    "Node7": "Embassy",
    "Node8": "Board Room",
    "Node9": "Ask The Expert",
}

WiFiNodes = {}
MiWiNodes = {}
LoRaNodes = {}

def init_nodes():
    for NodeID in USMastersNodeLocation:
        WiFiNodes[NodeID] = {
                    "roomName": USMastersNodeLocation[NodeID], 
                    "nodeID": NodeID,
                    "Temperature": 70+ randint(-3,3),
                    "RSSI": -60 + randint(-20,5),
                    "Battery": "3.3V"
                }
        MiWiNodes[NodeID] = {
                    "roomName": USMastersNodeLocation[NodeID], 
                    "nodeID": NodeID,
                    "Temperature": 70+ randint(-3,3),
                    "RSSI": 190+ randint(-10,10),
                    "Battery": "3.3V"
                }
        LoRaNodes[NodeID] = {
                    "roomName": USMastersNodeLocation[NodeID], 
                    "nodeID": NodeID,
                    "Temperature": 70+ randint(-3,3),
                    "RSSI": -30+ randint(-40,0),
                    "Battery": "3.3V"
                }

Nodes = {
    "Wi-Fi" :WiFiNodes,
    "MiWi" : MiWiNodes,
    "LoRa" : LoRaNodes
}
##################################################

##### Nodes data #####



###################################
###### Files to store data ########
###################################
wifiFile = open('/home/c43071/WSN/wifiData.csv', 'a')  
miwiFile = open('/home/c43071/WSN/miwiData.csv', 'a') 
loraFile = open('/home/c43071/WSN/loraData.csv', 'a') 
WiFiWriter = csv.writer(wifiFile)
MiWiWriter = csv.writer(miwiFile)
LoRaWriter = csv.writer(loraFile)


##### MQTT AWS & TTN clients #####

################AWS################
# For certificate based connection
myMQTTClient = AWSIoTMQTTClient("WSNClientID")

# For TLS mutual authentication with TLS ALPN extension
myMQTTClient.configureEndpoint("a3adakhi3icyv9.iot.us-west-2.amazonaws.com", 443)
myMQTTClient.configureCredentials("/home/c43071/WSN/VeriSign.pem", "/home/c43071/WSN/WSN_BE_private.pem", "/home/c43071/WSN/WSN_BE_certificate.pem")
myMQTTClient.configureOfflinePublishQueueing(-1)  # Infinite offline Publish queueing
myMQTTClient.configureDrainingFrequency(2)  # Draining: 2 Hz
myMQTTClient.configureConnectDisconnectTimeout(10)  # 10 sec
myMQTTClient.configureMQTTOperationTimeout(5)  # 5 sec
myMQTTClient.connect()
##################################################






################### AWS Node update###############################
#Custom MQTT message callback
def WiFiCallback(client, userdata, message):
    global WiFiWriter
    with app.app_context(), lock:
        redisDB.flush()
        Nodes = redisDB.Nodes
        print("Received a new WiFi message: ")
        print(message.payload)
        print("from topic: ")
        print(message.topic)
        print("--------------\n\n")
        dicMessage =  jasons.loads(message.payload[:(len(message.payload )- 1)])
        dicMessage["nodeID"] = dicMessage["nodeID"].encode('ascii','ignore')
        dicMessage["Battery"] = dicMessage["Battery"].encode('ascii','ignore')
        print(dicMessage)
        Nodes["Wi-Fi"][dicMessage["nodeID"]]["Temperature"]= dicMessage["Temperature"]
        Nodes["Wi-Fi"][dicMessage["nodeID"]]["RSSI"]= dicMessage["RSSI"]
        Nodes["Wi-Fi"][dicMessage["nodeID"]]["Battery"]= dicMessage["Battery"]
        print (Nodes["Wi-Fi"][dicMessage["nodeID"]])
        Nodes2 = copy.deepcopy(Nodes)
        redisDB.Nodes = Nodes2
        #write data into csv
        data = []
        data.append(str(datetime.now())) #add time
        data.append(dicMessage["nodeID"]) #add nodeID
        data.append(dicMessage["Temperature"]) #add temp
        data.append(dicMessage["RSSI"]) #add rssi
        data.append(dicMessage["Battery"]) #add battery
        WiFiWriter.writerows([data])


def MiWiCallback(client, userdata, message):
    global MiWiWriter
    with app.app_context(), lock:
        redisDB.flush()
        Nodes = redisDB.Nodes
        print("Received a new MiWi message: ")
        print(message.payload)
        print("from topic: ")
        print(message.topic)
        print("--------------\n\n")
        #print(len(message.payload ))
        dicMessage =  jasons.loads(message.payload[:(len(message.payload )- 1)])
        #dicMessage =  jasons.loads(message.payload[:65])
        dicMessage["nodeID"] = dicMessage["nodeID"].encode('ascii','ignore')
        dicMessage["Battery"] = dicMessage["Battery"].encode('ascii','ignore')
        print(dicMessage)
        Nodes["MiWi"][dicMessage["nodeID"]]["Temperature"]= dicMessage["Temperature"]
        Nodes["MiWi"][dicMessage["nodeID"]]["RSSI"]= dicMessage["RSSI"]
        Nodes["MiWi"][dicMessage["nodeID"]]["Battery"]= dicMessage["Battery"]
        print (Nodes["MiWi"][dicMessage["nodeID"]])
        Nodes2 = copy.deepcopy(Nodes)
        redisDB.Nodes = Nodes2
        #write data into csv
        data = []
        data.append(str(datetime.now())) #add time
        data.append(dicMessage["nodeID"]) #add nodeID
        data.append(dicMessage["Temperature"]) #add temp
        data.append(dicMessage["RSSI"]) #add rssi
        data.append(dicMessage["Battery"]) #add battery
        MiWiWriter.writerows([data])

myMQTTClient.subscribe("/Microchip/WSN_Demo/WiFi", 1, WiFiCallback)
myMQTTClient.subscribe("/Microchip/WSN_Demo/MiWi", 1, MiWiCallback)
##################################################





########### MQTT for TTN - used for US Masters and CES #############

app.config['MQTT_BROKER_URL'] = 'us-west.thethings.network'
app.config['MQTT_BROKER_PORT'] = 1883
app.config['MQTT_USERNAME'] = 'jwmarriottdesertridge'
app.config['MQTT_PASSWORD'] = ''
app.config['MQTT_REFRESH_TIME'] = 1.0  # refresh time in seconds
mqtt = Mqtt(app)

@mqtt.on_connect()
def handle_connect(client, userdata, flags, rc):
    print ("MQTT connected!!!\r\n")
    mqtt.subscribe('jwmarriottdesertridge/devices/+/up')




@mqtt.on_message()
def handle_mqtt_message(client, userdata, message):
    global LoRaWriter
    with app.app_context(), lock:
        redisDB.flush()
        Nodes = redisDB.Nodes
        data = dict(
            topic=message.topic,
            payload=message.payload
        )
        print("Received a new LoRa message: ")
        print(data["payload"])
        print("from topic: ")
        print(data["topic"])
        print("--------------\n\n")
        dicMessage =  jasons.loads(data["payload"[:(len(data["payload"])- 1)]])
        dicMessage["RSSI"] = dicMessage["metadata"]["gateways"][0]['rssi']
        dicMessage["nodeID"] = capitalize(dicMessage["dev_id"].encode('ascii','ignore'))
        dicMessage["payLoad"] = dicMessage["payload_fields"]

        print("Node ID = ")
        print(dicMessage["nodeID"])
        print(", payload = ")
        print(dicMessage["payLoad"])
        #print(dicMessage)
        if 'temperature' in dicMessage["payLoad"]:
            Nodes["LoRa"][dicMessage["nodeID"]]["Temperature"]= dicMessage["payLoad"]["temperature"]
            Nodes["LoRa"][dicMessage["nodeID"]]["RSSI"]= dicMessage['RSSI']
            Nodes["LoRa"][dicMessage["nodeID"]]["Battery"]= dicMessage["payLoad"]["battery"]
            print (Nodes["LoRa"][dicMessage["nodeID"]])
            Nodes2 = copy.deepcopy(Nodes)
            redisDB.Nodes = Nodes2
            #write data into csv
            data = []
            data.append(str(datetime.now())) #add time
            data.append(dicMessage["nodeID"]) #add nodeID
            data.append(Nodes["LoRa"][dicMessage["nodeID"]]["Temperature"]) #add temp
            data.append(Nodes["LoRa"][dicMessage["nodeID"]]["RSSI"]) #add rssi
            data.append(Nodes["LoRa"][dicMessage["nodeID"]]["Battery"]) #add battery
            LoRaWriter.writerows([data])
##################################################







#Main page
@app.route("/")
def hello_world():
    return ("Hi there, this page will be update soon. stay tuned!")
    

#Gets all edge nodes data per technology type
def get_all_nodes_data(Network_Type, Conference):
    with lock:
        global USMastersNodeLocation
        global CES19NodeLocation
        global IndiaMastersNodeLocation
        redisDB.flush()
        Nodes = redisDB.Nodes
        message = []
        if Conference == "CES19":
            NodeList = CES19NodeLocation
        elif  Conference == "US Masters":
            NodeList = USMastersNodeLocation
        elif  Conference == "India Masters":
            NodeList = IndiaMastersNodeLocation
        for NodeID in NodeList:
            if not(((Network_Type == "Wi-Fi")or(Network_Type == "MiWi")) and (NodeList[NodeID]=="Golf course")):
                message.append( {
                        "roomName": NodeList[NodeID], 
                        "nodeID": NodeID,
                        "Temperature": int(Nodes[Network_Type][NodeID]["Temperature"]),
                        "RSSI": Nodes[Network_Type][NodeID]["RSSI"],
                        "Battery": Nodes[Network_Type][NodeID]["Battery"]
                    })
        resp = jsonify(message)
        resp.status_code = 200
        return resp

#Gets a single edge node data per technology type
def get_node_data(Network_Type, NodeID):
    with lock:
        global USMastersNodeLocation
        redisDB.flush()
        Nodes = redisDB.Nodes
        if NodeID in USMastersNodeLocation:
            message = {
                        "roomName": Nodes[Network_Type][NodeID]["roomName"], 
                        "nodeID": NodeID,
                        "Temperature": int(Nodes[Network_Type][NodeID]["Temperature"]),
                        "RSSI": Nodes[Network_Type][NodeID]["RSSI"],
                        "Battery": Nodes[Network_Type][NodeID]["Battery"]
                    }
            #print (NodeID)
            #print (Nodes[Network_Type][NodeID])
            resp = jsonify(message)
            resp.status_code = 200
            return resp
        else:
            
            return "Node doesn't exist"


###########################################
#########   Wi-Fi Endpoints    ############
###########################################


#WiFi all nodes sensor data endpoint for US Masters conference
@app.route("/WSN/Data/Wi-Fi/")
@as_json_p
def WiFi_all_Node_Data():
    return get_all_nodes_data("Wi-Fi", "US Masters")
    
#WiFi all nodes sensor data endpoint for CES19 conference
@app.route("/WSN/Data/CES19/Wi-Fi/")
@as_json_p
def WiFi_CES_all_Node_Data():
    return get_all_nodes_data("Wi-Fi", "CES19")

#WiFi all nodes sensor data endpoint for India Masters conference
@app.route("/WSN/Data/India/Wi-Fi/")
@as_json_p
def WiFi_IND_all_Node_Data():
    return get_all_nodes_data("Wi-Fi", "India Masters")

#WiFi individual sensor data endpoint 
@app.route("/WSN/Data/Wi-Fi/<NodeID>")
@as_json_p
def WiFi_Node_Data(NodeID):
    return get_node_data("Wi-Fi", NodeID)

    
###########################################
#########    MiWi Endpoints    ############
###########################################

#MiWi all nodes sensor data endpoint for US Masters conference
@app.route("/WSN/Data/MiWi/")
@as_json_p
def MiWi_all_Node_Data():
    return get_all_nodes_data("MiWi", "US Masters")

#MiWi all nodes sensor data endpoint for CES19 conference
@app.route("/WSN/Data/CES19/MiWi/")
@as_json_p
def MiWi_CES_all_Node_Data():
    return get_all_nodes_data("MiWi", "CES19")

#MiWi all nodes sensor data endpoint for India Masters conference
@app.route("/WSN/Data/India/MiWi/")
@as_json_p
def MiWi_IND_all_Node_Data():
    return get_all_nodes_data("MiWi", "India Masters")

#MiWi individual sensor data endpoint
@app.route("/WSN/Data/MiWi/<NodeID>")
@as_json_p
def MiWi_Node_Data(NodeID):
    return get_node_data("MiWi", NodeID)


###########################################
#########   LoRa  Endpoints    ############
###########################################

#LoRa all nodes sensor data endpoint for US Masters conference
@app.route("/WSN/Data/LoRa/")
@as_json_p
def LoRa_all_Node_Data():
    return get_all_nodes_data("LoRa", "US Masters")

#LoRa all nodes sensor data endpoint for CES19 conference
@app.route("/WSN/Data/CES19/LoRa/")
@as_json_p
def LoRa_CES_all_Node_Data():
    return get_all_nodes_data("LoRa", "CES19")
    
# LoRa all nodes sensor data endpoint for India Masters conference
# are directed at an external host at the moment. 
# http://35.226.82.124/WSN/Data/LoRa/

#LoRa individual sensor data endpoint
@app.route("/WSN/Data/LoRa/<NodeID>")
@as_json_p
def LoRa_Node_Data(NodeID):
    return get_node_data("LoRa", NodeID)

    
    
#@app.before_request
def init():
    print("Before Request!!!!!!!!!!!")
    init_nodes()
    redisDB.Nodes = Nodes


if __name__ == '__main__':
    init()
    app.run()