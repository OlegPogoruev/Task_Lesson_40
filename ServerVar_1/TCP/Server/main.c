
#include "main.h"

#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>

char name_thread[MAX_LEN_NAME] = "Wait_process";
void *threads_end_process(void *param); /* stream function */

struct st_control_threads control_threads;

void revers_string(char *str);
void thread_and_program_end(void);

int socket_desc , new_client_sock , c , read_size;

int main(int argc , char *argv[])
{


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
	pthread_t tid_end_process;	 /* thread ID */
	pthread_attr_t attr_end_process; /* thread attributes */
	
	
	control_threads.f_start = 1;
	control_threads.error = 0;
 	control_threads.status = 0;
 	control_threads.count_process = 0;
	pthread_attr_init(&attr_end_process);							/* get default attribute values */
	if(0 != pthread_create(&tid_end_process, &attr_end_process, threads_end_process, /*&socket_desc*/name_thread))	/* create a new thread */
 	{perror ("PTHREAD_CREATE");return (-1);}
 //////////////////////////////////////////////////////////////////////////////////////////////
 //////////////////////////////////////////////////////////////////////////////////////////////
	
	
	while(0 == control_threads.status)
	{
	    if(1 == control_threads.f_start)
	    {
		    if(0 != control_threads.count_process)
		    {
			  wait(NULL);
			  --control_threads.count_process;
		    }
	    }
	    else
	    {
		    if(0 != control_threads.count_process)
		    {
			wait(NULL);
			--control_threads.count_process;
		    }
		    else
		    {
		    	control_threads.error = 0;
			control_threads.status = 1;
			close(socket_desc);
		    }
	    }
	  }

	
	return control_threads.error;
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

	struct sockaddr_in sock_serv, sock_client;
	char buf[BUFLEN];
	
	//create a TCP socket
	if ((socket_desc = socket(AF_INET, SOCK_STREAM, 0)) == -1){perror("can't open socket");control_threads.error = (-1);thread_and_program_end();pthread_exit(0);}
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
	if( bind(socket_desc,(struct sockaddr *)&sock_serv , sizeof(sock_serv)) < 0){perror("bind failed. Error");control_threads.error = (-1);thread_and_program_end();pthread_exit(0);}//print the error message
	puts("bind done");
	
	//Listen for connect with client
	listen(socket_desc , 3);
	
	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);
	
	
//	printf("main socket desk %d\n" , socket_desc);
	
	//Receive a message from client
	while(1)
	{


//////////////////////////////////////////////////////////////////////////////////////////////////
		//accept connection from an incoming client
		new_client_sock = accept(socket_desc, (struct sockaddr *)&sock_client, (socklen_t*)&c);
		if (new_client_sock < 0){perror("accept failed");continue;}  // continied accept
		puts("Connection accepted");
//////////////////////////////////////////////////////////////////////////////////////////////////
		pid_t t;
                t=fork();
		
                if(t==-1)
                {
                   printf("Error can't creat process\n");
                   control_threads.error = (-1);thread_and_program_end();pthread_exit(0);
                }
		
		if(!t)
		{	// begin child process
		    while(1)
		    {
			//try to receive some data, this is a blocking call
			if ((read_size = recv(new_client_sock, buf, BUFLEN, 0)) == -1){perror("can't recv()");control_threads.error = (-1);thread_and_program_end();pthread_exit(0);}
			if(read_size == 0)
			{
			    puts("Client disconnected");
//			    break;
			    close(new_client_sock);    //close socket
			    exit(EXIT_SUCCESS);        //exit process
			}
			buf[read_size] = 0;
			
			//print details of the client/peer and the data received
			printf("Received packet from %s:%d\n", inet_ntoa(sock_client.sin_addr), ntohs(sock_client.sin_port));
			printf("Data: %s\n" , buf);
			
			revers_string(buf);
			
			//Send the message back to client
			write(new_client_sock , buf , strlen(buf));
			if (strcmp (buf, "tixe")==0)
			{
				close(new_client_sock);    //close socket
				exit(EXIT_SUCCESS);        //exit process
			}
		    }
		}	// end child process
		
		
		
		++control_threads.count_process;
		printf("Count process = %d\n", control_threads.count_process);
		
		control_threads.f_start = 0;	//server have client
		//begin close asept parents process
		///////////////////////////////////////////////////////////////////////////
		close(new_client_sock);			// close asept parents process
		// end close asept parents process
		///////////////////////////////////////////////////////////////////////////


	}
	control_threads.error = 0;thread_and_program_end();pthread_exit(0);
}

void thread_and_program_end(void)
{
	control_threads.status = 1;
	close(socket_desc);
}






