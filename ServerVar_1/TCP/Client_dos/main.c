
#include "main.h"

void get_input_string(char *str_in);

int main(int argc , char *argv[])
{
	int sock_client;
	struct sockaddr_in server;
	char buf_send[BUFLEN] , buf_receiv[BUFLEN];
	int recv_len;				// len received mesage
	unsigned int i;

          //Create socket
          if ( (sock_client=socket(AF_INET, SOCK_STREAM, 0)) == -1){perror("can't socket");return -1;}
	  puts("Socket created");
	  
	  /////////////////////////////////////////////////////
	  // zero out the structure
	  memset((char *) &server, 0, sizeof(server));
//	  slen = sizeof(server[i]);				// len structure socket
	  // init structure
	  server.sin_family = AF_INET;
	  server.sin_port = htons(PORT);
	  /////////////////////////////////////////////////////
	  if (inet_aton(SERVER , &server.sin_addr) == 0){fprintf(stderr, "inet_aton() failed\n");close(sock_client);return -1;}
//        server.sin_addr.s_addr = inet_addr("127.0.0.1");
    /////////////////////////////////////////////////////



        for(i = 0; i <= MAX_NUM_SOCKET; ++i)
        {

          //Create socket
          if ( (sock_client=socket(AF_INET, SOCK_STREAM, 0)) == -1){perror("can't socket");return -1;}
	  puts("Socket created");
	  
	  /////////////////////////////////////////////////////
	  // zero out the structure
	  memset((char *) &server, 0, sizeof(server));
//	  slen = sizeof(server[i]);				// len structure socket
	  // init structure
	  server.sin_family = AF_INET;
	  server.sin_port = htons(PORT);
	  /////////////////////////////////////////////////////
	  if (inet_aton(SERVER , &server.sin_addr) == 0){fprintf(stderr, "inet_aton() failed\n");close(sock_client);return -1;}
//        server.sin_addr.s_addr = inet_addr("127.0.0.1");
    /////////////////////////////////////////////////////
	  //Connect to remote server
	  if (connect(sock_client , (struct sockaddr *)&server , sizeof(server)) < 0){perror("connect failed. Error");close(sock_client);return -1;}
	  puts("Connected\n");
	}
	
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










