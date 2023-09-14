
#include "main.h"

#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>





#define  HAVE_MSGHDR_MSG_CONTROL

ssize_t read_fd(int fd, void *ptr, size_t nbytes, int *recvfd);
ssize_t write_fd(int fd, void *ptr, size_t nbytes, int sendfd);
void err_quit(char *s);


void prepai_deskriptor(struct nlmsghdr* nlhdr);





char name_thread[MAX_LEN_NAME] = "Wait_process";
void *threads_end_process(void *param); /* stream function */

void revers_string(char *str);

char f_start;
int socket_desc , new_client_sock , c , read_size;
struct st_control_process control_process[POOL_PROCESS];

struct sockaddr_in sock_serv, sock_client;

int main(int argc , char *argv[])
{
	
	char buf[BUFLEN];
	char flag_seach;
	unsigned int count_process;
	
	for(unsigned int i = 0; i < POOL_PROCESS; ++i)
	{
	  if (pipe(&control_process[i].fd1[0]) == -1) {fprintf(stderr, "Pipe Failed");return 1;}
	  if (pipe(&control_process[i].fd2[0]) == -1) {fprintf(stderr, "Pipe Failed");return 1;}
	  
	  if (socketpair(AF_LOCAL, SOCK_STREAM, 0, &control_process[i].sock_fd1[0])== -1) {fprintf(stderr, "sock unix Pipe Failed");return 1;}
//	  if (socketpair(AF_LOCAL, SOCK_STREAM, 0, &control_process[i].sock_fd2[0])== -1) {fprintf(stderr, "sock unix Pipe Failed");return 1;}
	  
	  fcntl( control_process[i].fd2[0], F_SETFL, fcntl(control_process[i].fd2[0], F_GETFL) | O_NONBLOCK);
//	  fcntl( control_process[i].fd2[1], F_SETFL, fcntl(control_process[i].fd2[1], F_GETFL) | O_NONBLOCK);
	  
	  control_process[i].f_activ = 0;
	  control_process[i].port = PORT + i + 1;
	}
	
	
	
	
	
	
	
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
	
	c = sizeof(struct sockaddr_in);
	
	
	
	
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
	    	
	    	close(control_process[i].sock_fd1[0]); // Close deskript chanel
	    	sleep (2);
	    }	// end parents process
	    else
	    {	// begin child process
	            close(control_process[i].fd1[1]); // Close writing end of first pipe
	            close(control_process[i].fd2[0]); // Close reading end of second pipe
	            
	            close(control_process[i].sock_fd1[1]); // Close deskript chanel
	    	    control_process[i].f_activ = 0;
	    	    printf("Process %d actio\n", i);
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
			    
			    
			    unsigned int j;
			    char temp_string[MAX_LEN_STRING];
			    read(control_process[i].fd1[0], temp_string, (sizeof(struct sockaddr_in)) + 2);	// Read string from child, print it and close reading end.
//			    read(control_process[i].fd1[0], &control_process[i].f_activ, 1);	// Read string from child, print it and close reading end.

			    
			    for(j = 0; j < (sizeof(struct sockaddr_in)); ++j)
			    {
			    	(*((char*)&(sock_client) + j)) = temp_string[j];
			    }
                            control_process[i].f_activ = temp_string[j];

			    printf("control_process.f_activ = %d\n", control_process[i].f_activ);
			    if(0 != control_process[i].f_activ)
			    {
			    	if((-1) == control_process[i].f_activ){printf("Close process %d\n", i);exit(EXIT_SUCCESS);}	//exit process
			    	/*
			        //accept connection from an incoming client
			        new_client_sock = accept(socket_desc, (struct sockaddr *)&sock_client, (socklen_t*)&c);
			        if (new_client_sock < 0){perror("accept failed");continue;}  // continied accept
			        puts("Connection accepted");
			        */
			        struct nlmsghdr zer_nlhdr;
			        prepai_deskriptor(&zer_nlhdr);
			        read_fd(control_process[i].sock_fd1[0], (void *)(&zer_nlhdr), zer_nlhdr.nlmsg_len, &new_client_sock);
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
 	


	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	
	
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
			unsigned int j;
			char temp_string[MAX_LEN_STRING];
			for(j = 0; j < (sizeof(struct sockaddr_in)); ++j)
			{
			    temp_string[j] = (*((char*)&(sock_client) + j));
			}
			/////////////////////////////////////////////////////
		    	flag_seach = 1;
		    	control_process[i].f_activ = 1;
		    	temp_string[j] = control_process[i].f_activ;
		    	temp_string[j + 1] = 0;
		    	/////////////////////////////////////////////////////
		    	
//                	write(control_process[i].fd1[1], &control_process[i].f_activ, (1));		// Write input string and close writing end of first pipe.
                	write(control_process[i].fd1[1], temp_string, (j + 2));		// Write input string and close writing end of first pipe.
                	
                	
			struct nlmsghdr zer_nlhdr;
			prepai_deskriptor(&zer_nlhdr);
			write_fd(control_process[i].sock_fd1[1], (void *)(&zer_nlhdr), zer_nlhdr.nlmsg_len, new_client_sock);
		        close(new_client_sock);			// close asept parents process
		        break;
		    }
		}
		
		/**/
		if(0 == flag_seach)
		{
		   //Send the message back to client
//		   write(new_client_sock , buf , 2);
                   shutdown(new_client_sock, SHUT_WR);
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
	        printf("End process\n");
		for(unsigned int i = 0; i < POOL_PROCESS; ++i)
		{
//		    char temp = (-1);
		    unsigned int j;
		    char temp_string[MAX_LEN_STRING];
		    for(j = 0; j < (sizeof(struct sockaddr_in)); ++j)
		    {
		        temp_string[j] = (*((char*)&(sock_client) + j));
		    }
		    /////////////////////////////////////////////////////
		    control_process[i].f_activ = (-1);
		    temp_string[j] = control_process[i].f_activ;
		    temp_string[j + 1] = 0;
		    /////////////////////////////////////////////////////
		    write(control_process[i].fd1[1], temp_string, (j + 2));		// Write input string and close writing end of first pipe.
//                    write(control_process[i].fd1[1], &temp, (1));		// Write input string and close writing end of first pipe.
		    wait(NULL);
		}
		printf("End Thread\n");
		f_start = 0xFF;
	    	close(socket_desc);			// close asept parents process
	        exit(EXIT_SUCCESS);        //exit process
	    }
        }
    }
}




ssize_t write_fd(int fd, void *ptr, size_t nbytes, int sendfd)
{
    struct msghdr   msg;
    struct iovec    iov[1];

#ifdef  HAVE_MSGHDR_MSG_CONTROL
    union {
      struct cmsghdr    cm;
      char              control[CMSG_SPACE(sizeof(int))];
    } control_un;
    struct cmsghdr  *cmptr;

    msg.msg_control = control_un.control;
    msg.msg_controllen = sizeof(control_un.control);

    cmptr = CMSG_FIRSTHDR(&msg);
    cmptr->cmsg_len = CMSG_LEN(sizeof(int));
    cmptr->cmsg_level = SOL_SOCKET;
    cmptr->cmsg_type = SCM_RIGHTS;
    *((int *) CMSG_DATA(cmptr)) = sendfd;
#else
    msg.msg_accrights = (caddr_t) &sendfd;
    msg.msg_accrightslen = sizeof(int);
#endif

    msg.msg_name = NULL;
    msg.msg_namelen = 0;

    iov[0].iov_base = ptr;
    iov[0].iov_len = nbytes;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
//ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);
    return(sendmsg(fd, &msg, 0));
}
/* end write_fd */


ssize_t read_fd(int fd, void *ptr, size_t nbytes, int *recvfd)
{
    struct msghdr   msg;
    struct iovec    iov[1];
    ssize_t         n;
    int             newfd;

#ifdef  HAVE_MSGHDR_MSG_CONTROL
    union {
      struct cmsghdr    cm;
      char              control[CMSG_SPACE(sizeof(int))];
    } control_un;
    struct cmsghdr  *cmptr;

    msg.msg_control = control_un.control;
    msg.msg_controllen = sizeof(control_un.control);
#else
    msg.msg_accrights = (caddr_t) &newfd;
    msg.msg_accrightslen = sizeof(int);
#endif

    msg.msg_name = NULL;
    msg.msg_namelen = 0;

    iov[0].iov_base = ptr;
    iov[0].iov_len = nbytes;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    if ( (n = recvmsg(fd, &msg, 0)) <= 0)
        return(n);

#ifdef  HAVE_MSGHDR_MSG_CONTROL
    if ( (cmptr = CMSG_FIRSTHDR(&msg)) != NULL &&
        cmptr->cmsg_len == CMSG_LEN(sizeof(int))) {
        if (cmptr->cmsg_level != SOL_SOCKET)
            err_quit("control level != SOL_SOCKET");
        if (cmptr->cmsg_type != SCM_RIGHTS)
            err_quit("control type != SCM_RIGHTS");
        *recvfd = *((int *) CMSG_DATA(cmptr));
    } else
        *recvfd = -1;       /* descriptor was not passed */
#else
/* *INDENT-OFF* */
    if (msg.msg_accrightslen == sizeof(int))
        *recvfd = newfd;
    else
        *recvfd = -1;       /* descriptor was not passed */
/* *INDENT-ON* */
#endif

    return(n);
}
/* end read_fd */


void err_quit(char *s)
{
    printf("Error: %s\n", s);
}






void prepai_deskriptor(struct nlmsghdr* nlhdr)
{
    char temp = 0x33;
//    struct nlmsghdr zer_nlhdr;
//    struct nlmsghdr* nlhdr;
//    nlhdr = &zer_nlhdr;
    memcpy (NLMSG_DATA (nlhdr), &temp, 1);
    nlhdr->nlmsg_len = NLMSG_LENGTH(1);
    nlhdr->nlmsg_pid = getpid(); /* self pid */
}






