
#include "main.h"




void revers_string(char *str);


int main(int argc , char *argv[])
{
    char buf[BUFLEN];
    int c, read_size;
    
    unsigned long new_count_client = 1;
    struct st_control_sock *ptr_control_sock;
    struct pollfd *ptr_sock_fds;
    ptr_control_sock = malloc((new_count_client + 0)*sizeof(struct st_control_sock));
    ptr_sock_fds = malloc((new_count_client + 3)*sizeof(struct pollfd));
    
    unsigned long count_client = 0;
    char f_start = 0;
    
    int maxfdp;
    /*
    for(unsigned long i = 0; i < MAX_SOCKET_CLIENT; ++i)
    {
        control_sock[i].f_aktiv = 0;
        sock_fds[i].fd = -1;
    }
    */
    
    //create a TCP socket
    if (((ptr_control_sock->sockfd) = socket(AF_INET, SOCK_STREAM, 0)) == -1){perror("can't open socket");return (-1);}
    puts("Socket created");
    /////////////////////////////////////////////////////
    // zero out the structure
    memset((char *) &(ptr_control_sock->sock), 0, sizeof(struct st_control_sock));
    // Prepare the sockaddr_in structure
    (ptr_control_sock->sock.sin_family) = AF_INET;
    (ptr_control_sock->sock.sin_addr.s_addr) = htonl(INADDR_ANY);
    (ptr_control_sock->sock.sin_port) = htons(PORT);
    /////////////////////////////////////////////////////

    //Bind
    if( bind((ptr_control_sock->sockfd),(struct sockaddr *)&(ptr_control_sock->sock) , sizeof(struct sockaddr_in)) < 0){perror("bind failed. Error");return (-1);}//print the error message
    puts("bind done");
    
    //Listen for connect with client
    listen((ptr_control_sock->sockfd) , 3);
    
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    
    (ptr_control_sock->f_aktiv) = 1;
    
    // of sock server we wait input/output data
    (ptr_sock_fds->fd) = (ptr_control_sock->sockfd);
    (ptr_sock_fds->events) = POLLIN | POLLOUT;
    ptr_sock_fds->revents = 0;         // reset event
    
    maxfdp = (ptr_control_sock->sockfd) + 1;
    puts("Begin cicle\n");
    
    
//    ((ptr_sock_fds + 1)->fd) = (-1);
//    while(1);
    
    while(1)
    {
        
        // wait of 10 sec
        int ret = poll( ptr_sock_fds, maxfdp, 10000 );
        
        // check vizov
        if ( ret == -1 ){printf("eror poll\n");}// error
        else if ( ret == 0 ){printf("taim-out, event not make poll. Count client: %ld\n", count_client);}// taim-out, event not make
        else
        {
            
            // view, event revents reinit structur
            if ( ptr_sock_fds->revents & POLLIN )
            {
                ptr_sock_fds->revents = 0;         // reset event
//                printf("reseiv data\n");    // make data of sock_serv
                //////////////////////////////////////////////////////////////////////////////////
                
                ++new_count_client;
                
                printf("realloc 1\n");
                ptr_control_sock = realloc((char *)(&(*ptr_control_sock)), (new_count_client + 0)*sizeof(struct st_control_sock));
                printf("realloc 2\n");
                ptr_sock_fds = realloc((char *)ptr_sock_fds, (new_count_client + 3)*sizeof(struct pollfd));
                
                //accept connection from an incoming client
                (ptr_control_sock + new_count_client - 1)->f_aktiv = 1;
		(ptr_control_sock + new_count_client - 1)->sockfd = accept(ptr_control_sock->sockfd, (struct sockaddr *)&(ptr_control_sock + new_count_client - 1)->sock, (socklen_t*)&c);
		if ((ptr_control_sock + new_count_client - 1)->sockfd < 0){perror("accept failed");continue;/*return (-1);*/}  // continied accept
		puts("Connection accepted");
                
                (ptr_sock_fds + new_count_client - 1)->fd = (ptr_control_sock + new_count_client - 1)->sockfd;
                (ptr_sock_fds + new_count_client - 1)->events = POLLIN /*| POLLOUT | POLLRDHUP*/;
			
		if(maxfdp < ((ptr_control_sock + new_count_client - 1)->sockfd + 1))
		{maxfdp = (ptr_control_sock + new_count_client - 1)->sockfd + 1;}
		
		f_start = 1;
                //////////////////////////////////////////////////////////////////////////////////
                
            }
            
            if ( ptr_sock_fds->revents & POLLOUT )
            {
                ptr_sock_fds->revents = 0;
                // make transeiv data of sock2
                printf("transmit data\n");
            }
            
            
            for(unsigned long i = 1; i < new_count_client; ++i)
            {
                
                if ((ptr_sock_fds + i)->revents & POLLIN)
                {
                    (ptr_sock_fds + i)->revents = 0;         // reset event
                    
                    //try to receive some data, this is a blocking call
                    if ((read_size = recv((ptr_control_sock + i)->sockfd, buf, BUFLEN, 0)) == -1){printf("i = %ld\n", i);perror("can't recv()");return (-1);}
                    if(read_size == 0)
                    {
                        puts("Client disconnected");
                        (ptr_control_sock + i)->f_aktiv = 0;
                        (ptr_sock_fds + i)->fd = -1;
                        close((ptr_control_sock + i)->sockfd);    //close socket
//                        exit(EXIT_SUCCESS);        //exit process
                    }
                    else
                    {
                        buf[read_size] = 0;
                        
                        //print details of the client/peer and the data received
                        printf("Received packet from %s:%d\n", inet_ntoa((ptr_control_sock + i)->sock.sin_addr), ntohs((ptr_control_sock + i)->sock.sin_port));
                        printf("Data: %s\n" , buf);
                        
                        revers_string(buf);
                        
                        //Send the message back to client
                        write((ptr_control_sock + i)->sockfd , buf , strlen(buf));
                    }
                }
            }
            
            
            count_client = 0;
            for(unsigned long i = 1; i < new_count_client; ++i)
            {
                if(1 == (ptr_control_sock + i)->f_aktiv)
                {++count_client;}
            }
            printf("Count client: %ld\n", count_client);
            
            if((1 == f_start) && (0 == count_client))
            {
                break;
            }
            
        }
    }
    
    

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
    close(ptr_control_sock->sockfd);    //close socket
    printf("The End\n");
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

	
    return 0;
}


void revers_string(char *str)
{

	char temp_str[BUFLEN];
	int i = 0;
	int j = 0;
	while(0 != (*(str + i))){++i;}
	while(0 != i)
	{
	    --i;
	    temp_str[j] = (*(str + i));
	    ++j;
	}
	temp_str[j] = 0;
	strncpy (str, temp_str,BUFLEN);
}






