#ifndef GROOM_COM_H_
#define GROOM_COM_H_

/* reserve char com '1' '2' '3' '4' 'R' 'A' */
char* com_requestdata(char DeviceDATAID);

/* heartbeat function */
uint8_t com_heartbeat(char DeviceID);

#endif /* GROOM_COM_H_ */