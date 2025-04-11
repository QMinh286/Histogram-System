#ifndef CONSTANTS_H
#define CONSTANTS_H

/* Circular buffer size */
#define BUFFER_SIZE 256

#define CHAR_START 'A'
#define CHAR_END 'T'

#define SEM_KEY 0x1234

#define SHM_KEY 0x4321

#define DP1_SLEEP_TIME 2000000 
#define DP2_SLEEP_TIME 50000  

#define DP1_PROCESS "./bin/dp1"
#define DP2_PROCESS "../DP-2/bin/dp2"
#define DC_PROCESS "../DC/bin/dc"
#endif /* CONSTANTS_H */