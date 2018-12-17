/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel
 * Support</a>
 */

#ifndef __BUTTON_H__
#define __BUTTON_H__


#define TIMEOUT_COUNTER_5S		(5*1000)		//5seconds
#define MAX_CB_INDEX			3
//typedef void *(button_5s_timeout_cb[MAX_CB_INDEX])();
void (*button_5s_timeout_cb[MAX_CB_INDEX]) (void);
void (*button_detect_cb[MAX_CB_INDEX]) (void);
//const button_5s_timeout_cb **button_5s_timeout_cb_ptr = NULL;
//const button_5s_timeout_cb *button_5s_timeout_cb_ptr[MAX_CB_INDEX];

void initialise_button(void);
void buttonInitCheck(void);
void buttonTaskInit(void);
void buttonTaskExecute(uint32_t tick);
int regButtonPressDetectCallback(void* cb);
int unRegButtonPressDetectCallback(int sock);
int regButtonPress5sTimeoutCallback(void* cb);
int unRegButtonPress5sTimeoutCallback(int sock);


#endif /*__BUTTON_H__*/
