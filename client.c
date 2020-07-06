#include<stdio.h>      //printf
#include<string.h>     //strlen
#include<sys/socket.h> //socket
#include<arpa/inet.h>  //inet_addr
#include <pthread.h>
#include <stdio.h>
int sock,read_size,i=0,messageLength=2000;
char message[2000] , *server_reply;

void *thread_routine(int arg){
	int newSocket = arg;
	server_reply=malloc(messageLength);
	
    while((read_size = recv(newSocket , server_reply , messageLength , 0)) > 0 ) {
		if(strcmp(server_reply, message)!=0){
			printf("\n");
			puts(server_reply);
		}
		server_reply=malloc(messageLength);
    }
    
    pthread_exit(NULL);
}
int main(int argc , char *argv[]){
    
    struct sockaddr_in server;
    sock = socket(AF_INET , SOCK_STREAM , 0); 
	
    if (sock == -1) {
        printf("Can't create socket");
    }

    puts("Socket created");
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 8080 );
 
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0) {
        perror("connection error"); 
		return 1;
    }
	
	pthread_t thread_id; 
	void *thread_result;
   	pthread_create( &thread_id, NULL, thread_routine, sock );
    puts("connection established..\n");
	
    while(1) {
	printf("\n");
        printf("Message: ");  scanf("%s" , message);
        if( send(sock , message , messageLength , 0) < 0) {
            puts("Message failed!"); return 1;
        }
         
    }
    close(sock);
    return 0;
}
