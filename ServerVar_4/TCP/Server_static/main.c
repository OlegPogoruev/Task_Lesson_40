
#include "main.h"





//struct st_control_threads control_threads;

void revers_string(char *str);


//#define MAX_SOCKET_CLIENT 4

int main(int argc , char *argv[])
{
/*
    int socket_desc, c, read_size;
    struct sockaddr_in sock_serv, sock_client;
    
    struct st_control_clients control_clients[MAX_SOCKET_CLIENT];
    */
    
    int c, read_size;
    struct st_control_sock control_sock[MAX_SOCKET_CLIENT];
//    struct st_array_pollfd array_pollfd[MAX_SOCKET_CLIENT];
    struct pollfd sock_fds[MAX_SOCKET_CLIENT];    // for events client
    
    unsigned long count_client;
    char f_start = 0;
    
    int maxfdp;
    
//    // two events
//    struct pollfd fds[2];
    
    for(unsigned long i = 0; i < MAX_SOCKET_CLIENT; ++i)
    {
        control_sock[i].f_aktiv = 0;
        
//        array_pollfd[i].sock_fds[0] = -1;
//        array_pollfd[i].sock_fds[1] = -1;
//        array_pollfd[i].sock_fds[2] = -1;
        sock_fds[i].fd = -1;
    }
    
    
    
    //create a TCP socket
    if ((control_sock[0].sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){perror("can't open socket");return (-1);}
    puts("Socket created");
    /////////////////////////////////////////////////////
    // zero out the structure
    memset((char *) &control_sock[0].sock, 0, sizeof(control_sock[0].sock));
    // Prepare the sockaddr_in structure
    control_sock[0].sock.sin_family = AF_INET;
    control_sock[0].sock.sin_addr.s_addr = htonl(INADDR_ANY);
//    control_sock[0].sock.sin_addr.s_addr = INADDR_ANY;
    control_sock[0].sock.sin_port = htons(PORT);
    /////////////////////////////////////////////////////

    //Bind
    if( bind(control_sock[0].sockfd,(struct sockaddr *)&control_sock[0].sock , sizeof(control_sock[0].sock)) < 0){perror("bind failed. Error");return (-1);}//print the error message
    puts("bind done");
    
    //Listen for connect with client
    listen(control_sock[0].sockfd , 3);
    
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    
    control_sock[0].f_aktiv = 1;
    
    
//    //accept connection from an incoming client
//    client_sockfd[0] = accept(socket_desc, (struct sockaddr *)&client_sockfd[0], (socklen_t*)&c);
//    if (client_sockfd[0] < 0){perror("accept failed");/*continue;*/return 1;}  // continied accept
//    puts("Connection accepted");
//    fcntl(socket_desc, F_SETFL, fcntl(socket_desc, F_GETFL) | O_NONBLOCK);    // init non block socket
    
 
    // of sock server we wait input/output data
    sock_fds[0].fd = control_sock[0].sockfd;
    sock_fds[0].events = POLLIN | POLLOUT;
    
    maxfdp = control_sock[0].sockfd + 1;
    puts("Begin cicle\n");
    
    while(1)
    {
        
        // wait of 10 sec
        int ret = poll( sock_fds, maxfdp, 10000 );
        
        // check vizov
        if ( ret == -1 ){printf("eror poll\n");}// error
        else if ( ret == 0 ){printf("taim-out, event not make poll. Count client: %ld\n", count_client);}// taim-out, event not make
        else
        {
            
            // view, event revents reinit structur
            if ( sock_fds[0].revents & POLLIN )
            {
                sock_fds[0].revents = 0;         // reset event
//                printf("reseiv data\n");    // make data of sock_serv
                //////////////////////////////////////////////////////////////////////////////////
		//accept connection from an incoming client
		for(unsigned long i = 1; i < MAX_SOCKET_CLIENT; ++i)
		{
		    if(0 == control_sock[i].f_aktiv)
		    {
		        control_sock[i].f_aktiv = 1;
		        control_sock[i].sockfd = accept(control_sock[0].sockfd, (struct sockaddr *)&control_sock[i].sock, (socklen_t*)&c);
			if (control_sock[i].sockfd < 0){perror("accept failed");continue;/*return (-1);*/}  // continied accept
			puts("Connection accepted");
			
			sock_fds[i].fd = control_sock[i].sockfd;
			sock_fds[i].events = POLLIN /*| POLLOUT | POLLRDHUP*/;
			
			if(maxfdp < (control_sock[i].sockfd + 1))
			{maxfdp = control_sock[i].sockfd + 1;}
			
			f_start = 1;
			break;
		    }
		}
                //////////////////////////////////////////////////////////////////////////////////
                
            }
            
            if ( sock_fds[0].revents & POLLOUT )
            {
                sock_fds[0].revents = 0;
                // make transeiv data of sock2
                printf("transmit data\n");
            }
            
            
            for(unsigned long i = 1; i < MAX_SOCKET_CLIENT; ++i)
            {
                
                if (sock_fds[i].revents & POLLIN)
                {
                    sock_fds[i].revents = 0;         // reset event
                    
                    //try to receive some data, this is a blocking call
                    if ((read_size = recv(control_sock[i].sockfd, control_sock[i].receiv_buf, BUFLEN, 0)) == -1){printf("i = %ld\n", i);perror("can't recv()");return (-1);}
                    if(read_size == 0)
                    {
                        puts("Client disconnected");
                        control_sock[i].f_aktiv = 0;
                        sock_fds[i].fd = -1;
                        close(control_sock[i].sockfd);    //close socket
//                        exit(EXIT_SUCCESS);        //exit process
                    }
                    else
                    {
                        control_sock[i].receiv_buf[read_size] = 0;
                        
                        //print details of the client/peer and the data received
                        printf("Received packet from %s:%d\n", inet_ntoa(control_sock[i].sock.sin_addr), ntohs(control_sock[i].sock.sin_port));
                        printf("Data: %s\n" , control_sock[i].receiv_buf);
                        
                        revers_string(control_sock[i].receiv_buf);
                        
                        //Send the message back to client
                        write(control_sock[i].sockfd , control_sock[i].receiv_buf , strlen(control_sock[i].receiv_buf));
                    }
                }
            }
            
            
            count_client = 0;
            for(unsigned long i = 1; i < MAX_SOCKET_CLIENT; ++i)
            {
                if(1 == control_sock[i].f_aktiv)
                {
                    ++count_client;
                }
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
    close(control_sock[0].sockfd);    //close socket
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






