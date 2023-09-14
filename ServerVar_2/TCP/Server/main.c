
#include "main.h"

#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>




char name_thread[MAX_LEN_NAME] = "Wait_process";
void *threads_end_process(void *param); /* stream function */

void revers_string(char *str);

char f_start;
int socket_desc , new_client_sock , c , read_size;
struct st_control_process control_process[POOL_PROCESS];

int main(int argc , char *argv[])
{
	struct sockaddr_in sock_serv, sock_client;
	char buf[BUFLEN];
	char flag_seach;
	unsigned int count_process;
	
	for(unsigned int i = 0; i < POOL_PROCESS; ++i)
	{
	  if (pipe(&control_process[i].fd1[0]) == -1) {fprintf(stderr, "Pipe Failed");return 1;}
	  if (pipe(&control_process[i].fd2[0]) == -1) {fprintf(stderr, "Pipe Failed");return 1;}
	  
	  fcntl( control_process[i].fd2[0], F_SETFL, fcntl(control_process[i].fd2[0], F_GETFL) | O_NONBLOCK);
//	  fcntl( control_process[i].fd2[1], F_SETFL, fcntl(control_process[i].fd2[1], F_GETFL) | O_NONBLOCK);
	  
	  control_process[i].f_activ = 0;
	  control_process[i].port = PORT + i + 1;
	}
	
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
	for(unsigned int i = 0; i < POOL_PROCESS; ++i)
	{
	    pid_t t;
	    control_process[i].num_process = i;
            t=fork();
	    
            if(t==-1)
            {
               printf("Error can't creat process\n");
               return (-1);
            }
	    
	    if(0 != t)
	    {	// begin parents process
	    	close(control_process[i].fd1[0]); // Close reading end of first pipe
	    	close(control_process[i].fd2[1]); // Close writing end of second pipe
	    	sleep (2);
	    }	// end parents process
	    else
	    {	// begin child process
	            close(control_process[i].fd1[1]); // Close writing end of first pipe
	            close(control_process[i].fd2[0]); // Close reading end of second pipe
	    	    control_process[i].f_activ = 0;
	    	    printf("Process %d actio\n", i);
	    	    
		    //create a TCP socket
		    if ((socket_desc = socket(AF_INET, SOCK_STREAM, 0)) == -1){perror("can't open socket");return (-1);}
		    puts("Socket created");
	            /////////////////////////////////////////////////////
	            // zero out the structure
	            memset((char *) &sock_serv, 0, sizeof(sock_serv));
	            // Prepare the sockaddr_in structure
	            sock_serv.sin_family = AF_INET;
	            sock_serv.sin_addr.s_addr = htonl(INADDR_ANY);
	            sock_serv.sin_port = htons(control_process[i].port);
	            /////////////////////////////////////////////////////
	            printf("Parent Received packet from %s:%d\n", inet_ntoa(sock_serv.sin_addr), ntohs(sock_serv.sin_port));
	            //Bind
	            if( bind(socket_desc,(struct sockaddr *)&sock_serv , sizeof(sock_serv)) < 0){perror("bind failed. Error");return (-1);}//print the error message
//	            puts("bind done");
	
	            //Listen for connect with client
	            listen(socket_desc , 3);
	
	            //Accept and incoming connection
	            c = sizeof(struct sockaddr_in);
//////////////////////////////////////////////////////////////////////////////////////////////////
  		    control_process[i].f_activ = 0;
//////////////////////////////////////////////////////////////////////////////////////////////////
		    while(1)
		    {
		        if(0 != control_process[i].f_activ)
		        {
			  //try to receive some data, this is a blocking call
			  if ((read_size = recv(new_client_sock, buf, BUFLEN, 0)) == -1){printf("i = %d\n", i);perror("can't recv()");return (-1);}
			  if(read_size == 0)
			  {
			      puts("Client disconnected");
			      control_process[i].f_activ = 0;
			      close(new_client_sock);    //close socket
//			      exit(EXIT_SUCCESS);        //exit process
                              write(control_process[i].fd2[1], &control_process[i].f_activ, (1));		// Write input string and close writing end of first pipe.
			  }
			  buf[read_size] = 0;
			  
			  //print details of the client/peer and the data received
			  printf("Received packet from %s:%d\n", inet_ntoa(sock_client.sin_addr), ntohs(sock_client.sin_port));
			  printf("Data: %s\n" , buf);
			  
			  revers_string(buf);
			  
			  //Send the message back to client
			  write(new_client_sock , buf , strlen(buf));
			}
			else
			{
			    read(control_process[i].fd1[0], &control_process[i].f_activ, 1);	// Read string from child, print it and close reading end.
			    printf("control_process.f_activ = %d\n", control_process[i].f_activ);
			    if(0 != control_process[i].f_activ)
			    {
			    	if((-1) == control_process[i].f_activ){printf("Close process %d\n", i);exit(EXIT_SUCCESS);}	//exit process
			        //accept connection from an incoming client
			        new_client_sock = accept(socket_desc, (struct sockaddr *)&sock_client, (socklen_t*)&c);
			        if (new_client_sock < 0){perror("accept failed");continue;}  // continied accept
			        puts("Connection accepted");
			        printf("num proces = %d\n", i);
//	  		        control_process[i].f_activ = 1;
	  		    }
			}
		    }
	    }	// end child process
	}
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

	f_start = 1;
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
	pthread_t tid_end_process;	 /* thread ID */
	pthread_attr_t attr_end_process; /* thread attributes */
	pthread_attr_init(&attr_end_process);							/* get default attribute values */
	if(0 != pthread_create(&tid_end_process, &attr_end_process, threads_end_process, /*&socket_desc*/name_thread))	/* create a new thread */
 	{perror ("PTHREAD_CREATE");return (-1);}
 //////////////////////////////////////////////////////////////////////////////////////////////
 //////////////////////////////////////////////////////////////////////////////////////////////
 	


	//create a TCP socket
	if ((socket_desc = socket(AF_INET, SOCK_STREAM, 0)) == -1){perror("can't open socket");return (-1);}
	puts("Socket created");
	/////////////////////////////////////////////////////
	// zero out the structure
	memset((char *) &sock_serv, 0, sizeof(sock_serv));
	// Prepare the sockaddr_in structure
	sock_serv.sin_family = AF_INET;
	sock_serv.sin_addr.s_addr = htonl(INADDR_ANY);
//	server.sin_addr.s_addr = INADDR_ANY;
	sock_serv.sin_port = htons(PORT);
	/////////////////////////////////////////////////////

	//Bind
	if( bind(socket_desc,(struct sockaddr *)&sock_serv , sizeof(sock_serv)) < 0){perror("bind failed. Error");return (-1);}//print the error message
	puts("bind done");
	
	//Listen for connect with client
	listen(socket_desc , 3);
	
	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);
	
	
	//Receive a message from client
	while(1)
	{
//////////////////////////////////////////////////////////////////////////////////////////////////
		//accept connection from an incoming client
		new_client_sock = accept(socket_desc, (struct sockaddr *)&sock_client, (socklen_t*)&c);
		if (new_client_sock < 0){perror("accept failed");continue;}  // continied accept
		puts("Connection accepted");
//////////////////////////////////////////////////////////////////////////////////////////////////
		
		if(0xFF == f_start){sleep (2);return 0;}
		
		/////////////////////////////////////////////////////////////
		/*
		for(unsigned int i = 0; i < POOL_PROCESS; ++i)
		{
		    if(0 != control_process[i].f_activ)
		    {
			    char temp;
			    ssize_t temp_byte;
			    temp_byte = read(control_process[i].fd2[0], &temp, (1));		// Write input string and close writing end of first pipe.
//			    printf("temp_byte = %ld\n", temp_byte);
			    if(0 < temp_byte)
			    {control_process[i].f_activ = temp;}
		    }
		}
		*/
		/////////////////////////////////////////////////////////////
		
		flag_seach = 0;
		for(unsigned int i = 0; i < POOL_PROCESS; ++i)
		{
		    if(0 == control_process[i].f_activ)
		    {
		    	flag_seach = 1;
		    	control_process[i].f_activ = 1;
                	write(control_process[i].fd1[1], &control_process[i].f_activ, (1));		// Write input string and close writing end of first pipe.
                	
		    	
		    	buf[0] = (*((char*)&(control_process[i].port) + 0));
			buf[1] = (*((char*)&(control_process[i].port) + 1));
		        buf[2] = 0;
		        //Send the message back to client
		        write(new_client_sock , buf , strlen(buf));
		        close(new_client_sock);			// close asept parents process
		        break;
		    }
		}
		if(0 == flag_seach)
		{
		   buf[0] = 0;
		   buf[1] = 0;
//		   buf[2] = 0;
		   //Send the message back to client
		   write(new_client_sock , buf , 2);
		   close(new_client_sock);			// close asept parents process
		}
		else{f_start = 0;}
		///////////////////////////////////////////////////////////////////////
		
		
		count_process = 0;
		for(unsigned int i = 0; i < POOL_PROCESS; ++i)
		{
		    if(0 != control_process[i].f_activ){++count_process;}
		}
		printf("Count use process = %d\n", count_process);
		
		
//////////////////////////////////////////////////////////////////////////////////////////////////
	}
	
	
	
	
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

/* Control passes streaming function */
void *threads_end_process(void *param) 
{
    unsigned int count_flag = 0;
    printf("Thread begin\n");
    while(1)
    {
        if(0 == f_start)
        {
	    count_flag = 0;
	    for(unsigned int i = 0; i < POOL_PROCESS; ++i)
	    {
	    
		    if(0 != control_process[i].f_activ)
		    {
			    char temp;
			    ssize_t temp_byte;
			    temp_byte = read(control_process[i].fd2[0], &temp, (1));		// Write input string and close writing end of first pipe.
//			    printf("temp_byte = %ld\n", temp_byte);
			    if(0 < temp_byte)
			    {control_process[i].f_activ = temp;}
		    }
	    
	        if(0 == control_process[i].f_activ){++count_flag;}
	    }
	    
	    if(POOL_PROCESS == count_flag)
	    {
	        printf("End Thread\n");
		for(unsigned int i = 0; i < POOL_PROCESS; ++i)
		{
		    char temp = (-1);
                    write(control_process[i].fd1[1], &temp, (1));		// Write input string and close writing end of first pipe.
		    wait(NULL);
		}
		f_start = 0xFF;
	    	close(socket_desc);			// close asept parents process
	        exit(EXIT_SUCCESS);        //exit process
	    }
        }
    }
}



