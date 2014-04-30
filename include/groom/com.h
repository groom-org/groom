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

//request data
char* com_requestdata(char DeviceDATAID);

/* heartbeat function */
uint8_t com_heartbeat(char DeviceID);

//send data
uint8_t com_senddata(char DeviceSendID, char * data_send);


#endif /* GROOM_COM_H_ */