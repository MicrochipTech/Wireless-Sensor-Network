#ifndef _DATAPOINT_H_
#define _DATAPOINT_H_

#ifdef __cplusplus
extern "C" {
#endif

__packed typedef struct{ 
	char dataType[30];
	int value;
}NodeInfo;


#define DevName						"DevName"
#define DevTypeAttr					"DevType"
#define MacAddrAttr					"macAddr"


#ifdef __cplusplus
}
#endif

#endif

