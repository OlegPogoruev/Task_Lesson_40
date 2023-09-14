
#include "main.h"

void get_input_string(char *str_in);

int main(int argc , char *argv[])
{
	int sock_client;
	struct sockaddr_in server;
	char buf_send[BUFLEN] , buf_receiv[BUFLEN];
	int recv_len;				// len received mesage


	//Create socket
    if ( (sock_client=socket(AF_INET, SOCK_STREAM, 0)) == -1){perror("can't socket");return -1;}
	puts("Socket created");
	
	/////////////////////////////////////////////////////
	// zero out the structure
	memset((char *) &server, 0, sizeof(server));
//	slen = sizeof(server);				// len structure socket
	// init structure
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	/////////////////////////////////////////////////////
	if (inet_aton(SERVER , &server.sin_addr) == 0){fprintf(stderr, "inet_aton() failed\n");close(sock_client);return -1;}
//    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    /////////////////////////////////////////////////////

	//Connect to remote server
	if (connect(sock_client , (struct sockaddr *)&server , sizeof(server)) < 0){perror("connect failed. Error");close(sock_client);return -1;}
	puts("Connected\n");
	
	//keep communicating with server
	while(1)
	{
		printf("Enter message : ");
//		scanf("%s" , message);
		get_input_string(buf_send);

		//Send some data
		if( send(sock_client , buf_send , strlen(buf_send) , 0) < 0){perror("Send failed");close(sock_client);return -1;}
		
		//Receive a reply from the server
		if( (recv_len = recv(sock_client , buf_receiv , BUFLEN , 0)) < 0){perror("recv failed");break;}
		buf_receiv[recv_len] = 0;
		puts("Server reply :");
		puts(buf_receiv);
        if (strcmp (buf_receiv, "tixe")==0){break;}
	}
	
	close(sock_client);
	return 0;
}






void get_input_string(char *str_in)
{
    int ch = 0;
    int temp_count = 0;
    while ((ch = getchar()) != '\n')
    {
        (*(str_in + temp_count)) = ch;
        ++temp_count;
        if(temp_count > (BUFLEN - 2 ))
        {
//          (*(str_in + temp_count)) = '\n';
//          ++temp_count;
          break;
        }
    }
//    (*(str_in + temp_count)) = '\n';
    (*(str_in + temp_count)) = 0;    
}










