#ifndef CORE_DEFS_H_
#define CORE_DEFS_H_

#define CORE_COUNT 	24
const unsigned int	GPC_RESET_HIGH 			= 	1;
const unsigned int	GPC_RESET_LOW 			= 	0;
//Poll events
#define GPC_STATE POLLRDBAND
#define H2C_MQ_WRITABLE POLLOUT
#define C2H_MQ_READABLE POLLIN
#define WAIT_FOR_IDLE 2000000 //20000 sec
#endif
