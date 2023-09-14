
#include <sys/types.h>
#include <sys/socket.h>

#include <unistd.h>
#include <arpa/inet.h>


#include <stdio.h>
#include <string.h>


#include <fcntl.h>              /* Obtain O_* constant definitions */




#define BUFLEN 2000	//Max length of buffer
#define PORT 8888	//The port on which to listen for incoming data


#define POOL_PROCESS 3

#define MAX_LEN_NAME 32
#define MAX_LEN_STRING 1024
/*
struct st_control_threads{
    char f_activ;
    char f_activ_aray[POOL_PROCESS];
    char f_start;
    int error;
    unsigned int count_process;
    int status;
};
*/
struct st_control_process{
    char f_activ;
    unsigned int num_process;
    
    int fd1[2]; // Used to store two ends of first pipe
    int fd2[2]; // Used to store two ends of second pipe
    
    unsigned short int port;
};

/*
#include <sys/types.h>
#include <sys/socket.h>

#include <unistd.h>
#include <arpa/inet.h>


#include <stdio.h>
#include <string.h>



#define BUFLEN 2000	//Max length of buffer
#define PORT 8888	//The port on which to listen for incoming data




#define MAX_LEN_NAME 32

struct st_control_threads{
    char f_start;
    int error;
    unsigned int count_process;
    int status;
};

*/




