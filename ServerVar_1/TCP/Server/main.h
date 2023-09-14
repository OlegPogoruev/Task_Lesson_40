

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






