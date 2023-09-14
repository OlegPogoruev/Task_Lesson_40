

#include <sys/types.h>
#include <sys/socket.h>

#include <unistd.h>
#include <arpa/inet.h>


#include <stdio.h>
#include <string.h>

///////////////////////////////////////////////////////

#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>


#include <fcntl.h>              /* Obtain O_* constant definitions */


#include <sys/poll.h>




#define  _GNU_SOURCE

///////////////////////////////////////////////////////



#define BUFLEN 2000	//Max length of buffer
#define PORT 8888	//The port on which to listen for incoming data




#define MAX_SOCKET_CLIENT 1024


#define MAX_LEN_NAME 32






/*
struct st_control_clients{
    char f_aktiv;
    
    int client_sockfd;
    struct sockaddr_in sock_client;
    struct pollfd client_fds[3];    // for events client
    
    char receiv_buf[BUFLEN];
    char transmit_buf[BUFLEN];
};
*/

struct st_control_sock{
    int sockfd;
    struct sockaddr_in sock;
    char f_aktiv;
    char zero_byte[11];
//    char receiv_buf[BUFLEN];
//    char transmit_buf[BUFLEN];
};

/*
struct st_array_pollfd{
    struct pollfd sock_fds;    // for events client
//    struct pollfd sock_fds[3];    // for events client
};
*/





















