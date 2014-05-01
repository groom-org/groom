#ifndef GROOM_COM_H_
#define GROOM_COM_H_

//!! This protocol should be used in all uP.
#define HB_ALPHA '1'
#define HB_BETA '2'
#define READ_ALPHA '3'
#define READ_BETA '4'
#define SEND_ALPHA '5'
#define SEND_BETA '6'
#define ACK 'A'
#define ACTIVE_RESPONSE 'R'
#define ACTIVE_RESPONSE_MOTION 'r'
#define DEFAULT '0' 
#define COMMAND_MODE 0
#define TRANSMIT_MODE 1

// For commands to BETA
#define HEAT_ON     'H'
#define HEAT_OFF    'h'
#define COOL_ON     'C'
#define COOL_OFF    'c'
#define FAN_ON      'F'
#define FAN_OFF     'f'
#define BLINDS_UP   'U'
#define BLINDS_DOWN 'D'
#define LIGHTS_FULL 'L'
#define LIGHTS_HALF 'i'
#define LIGHTS_OFF  'l'

//request data
char* com_requestdata(char DeviceDATAID);

/* heartbeat function */
uint8_t com_heartbeat(char DeviceID);

//send data
uint8_t com_senddata(char DeviceSendID, char * data_send);


#endif /* GROOM_COM_H_ */