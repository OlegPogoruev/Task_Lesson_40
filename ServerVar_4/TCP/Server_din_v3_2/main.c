
#include "main.h"




void revers_string(char *str);


int main(int argc , char *argv[])
{
    char buf[BUFLEN];
    int c, read_size;
    
    unsigned long new_count_client = 1;
    
    char* ptr_control_sock;
    char* ptr_sock_fds;
    
    struct st_control_sock *temp_control_sock;
    struct pollfd *temp_sock_fds;
    struct st_control_sock *server_control_sock;
    struct pollfd *server_sock_fds;
    
    ptr_control_sock = malloc((new_count_client + 0)*sizeof(struct st_control_sock));
    ptr_sock_fds = malloc((new_count_client + 3)*sizeof(struct pollfd));
    
    printf("sizeof struct st_control_sock = %ld\n", sizeof(struct st_control_sock));
    printf("sizeof struct pollfd = %ld\n", sizeof(struct pollfd));
    
    printf("sizeof struct sockaddr_in = %ld\n", sizeof(struct sockaddr_in));
    printf("sizeof struct int = %ld\n", sizeof(int));
    printf("sizeof struct char = %ld\n", sizeof(char));
//    while(1);
    
//    server_control_sock = temp_control_sock = (struct st_control_sock *)&(*ptr_control_sock);
    server_control_sock = temp_control_sock = (struct st_control_sock *)ptr_control_sock;
    server_sock_fds = temp_sock_fds = (struct pollfd *)ptr_sock_fds;
    
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
    if (((server_control_sock->sockfd) = socket(AF_INET, SOCK_STREAM, 0)) == -1){perror("can't open socket");return (-1);}
    puts("Socket created");
    /////////////////////////////////////////////////////
    // zero out the structure
    memset((char *) &(server_control_sock->sock), 0, sizeof(struct st_control_sock));
    // Prepare the sockaddr_in structure
    (server_control_sock->sock.sin_family) = AF_INET;
    (server_control_sock->sock.sin_addr.s_addr) = htonl(INADDR_ANY);
    (server_control_sock->sock.sin_port) = htons(PORT);
    /////////////////////////////////////////////////////
    
    
    //Bind
    if( bind((server_control_sock->sockfd),(struct sockaddr *)&(server_control_sock->sock) , sizeof(struct sockaddr_in)) < 0){perror("bind failed. Error");return (-1);}//print the error message
    puts("bind done");
    
    //Listen for connect with client
    listen((server_control_sock->sockfd) , 3);
    
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    
    (server_control_sock->f_aktiv) = 1;
    
    
    // of sock server we wait input/output data
    (server_sock_fds->fd) = (server_control_sock->sockfd);
    (server_sock_fds->events) = POLLIN | POLLOUT;
//    ptr_sock_fds->revents = 0;         // reset event
    
    maxfdp = (server_control_sock->sockfd) + 1;
    puts("Begin cicle\n");
    
    
//    ((temp_sock_fds + 1)->fd) = (-1);
//    while(1);
    
    while(1)
    {
        
        // wait of 10 sec
        int ret = poll( server_sock_fds, maxfdp, 10000 );
        
        // check vizov
        if ( ret == -1 ){printf("eror poll\n");}// error
        else if ( ret == 0 ){printf("taim-out, event not make poll. Count client: %ld\n", count_client);}// taim-out, event not make
        else
        {
            
            // view, event revents reinit structur
            if ( server_sock_fds->revents & POLLIN )
            {
                server_sock_fds->revents = 0;         // reset event
//                printf("reseiv data\n");    // make data of sock_serv
                //////////////////////////////////////////////////////////////////////////////////
                
                ++new_count_client;
                
                printf("realloc 1\n");
                ptr_control_sock = realloc(ptr_control_sock, (new_count_client + 0)*(sizeof(struct st_control_sock)));
                printf("realloc 2\n");
                ptr_sock_fds = realloc(ptr_sock_fds, (new_count_client + 3)*sizeof(struct pollfd));
                server_control_sock = (struct st_control_sock *)ptr_control_sock;
                server_sock_fds = (struct pollfd *)ptr_sock_fds;
                temp_control_sock = (struct st_control_sock *)(ptr_control_sock + (new_count_client - 1)*sizeof(struct st_control_sock));
                temp_sock_fds = (struct pollfd *)(ptr_sock_fds + (new_count_client - 1)*sizeof(struct pollfd));
                
                //accept connection from an incoming client
                temp_control_sock->f_aktiv = 1;
		temp_control_sock->sockfd = accept(server_control_sock->sockfd, (struct sockaddr *)&(temp_control_sock->sock), (socklen_t*)&c);
		if ((temp_control_sock->sockfd) < 0){perror("accept failed");continue;/*return (-1);*/}  // continied accept
		puts("Connection accepted");
                
                temp_sock_fds->fd = temp_control_sock->sockfd;
                temp_sock_fds->events = POLLIN /*| POLLOUT | POLLRDHUP*/;
			
		if(maxfdp < ((temp_control_sock->sockfd) + 1))
		{maxfdp = (temp_control_sock->sockfd) + 1;}
		
		f_start = 1;
                //////////////////////////////////////////////////////////////////////////////////
                
            }
            
            if ( server_sock_fds->revents & POLLOUT )
            {
                server_sock_fds->revents = 0;
                // make transeiv data of sock2
                printf("transmit data\n");
            }
            
            
            for(unsigned long i = 1; i < new_count_client; ++i)
            {
                temp_control_sock = (struct st_control_sock *)(ptr_control_sock + i*sizeof(struct st_control_sock));
                temp_sock_fds = (struct pollfd *)(ptr_sock_fds + i*sizeof(struct pollfd));
                if ((temp_sock_fds->revents) & POLLIN)
                {
                    temp_sock_fds->revents = 0;         // reset event
                    
                    //try to receive some data, this is a blocking call
                    if ((read_size = recv((temp_control_sock->sockfd), buf, BUFLEN, 0)) == -1){printf("i = %ld\n", i);perror("can't recv()");return (-1);}
                    if(read_size == 0)
                    {
                        puts("Client disconnected");
                        temp_control_sock->f_aktiv = 0;
                        temp_sock_fds->fd = -1;
                        close(temp_control_sock->sockfd);    //close socket
//                        exit(EXIT_SUCCESS);        //exit process
                    }
                    else
                    {
                        buf[read_size] = 0;
                        
                        //print details of the client/peer and the data received
                        printf("Received packet from %s:%d\n", inet_ntoa(temp_control_sock->sock.sin_addr), ntohs(temp_control_sock->sock.sin_port));
                        printf("Data: %s\n" , buf);
                        
                        revers_string(buf);
                        
                        //Send the message back to client
                        write(temp_control_sock->sockfd , buf , strlen(buf));
                    }
                }
            }
            
            
            count_client = 0;
            for(unsigned long i = 1; i < new_count_client; ++i)
            {
                temp_control_sock = (struct st_control_sock *)(ptr_control_sock + i*sizeof(struct st_control_sock));
                if(1 == (temp_control_sock->f_aktiv))
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
    close(server_control_sock->sockfd);    //close socket
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






