#include <stdio.h>
#include <string.h>
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h>
#include<stdbool.h>
int socket_desc,
    *new_sock,
    client_sock,
    c,
    read_size,id=-1,
    logoutId=-1,
    idTemp=0,
    clientTotalNumber=480,
    messageLength=2000;

int clientArray[480];
int logoutArray[480];

void tempWrite(char temp[],char socketid[]){
    strcat(temp,"client");
    strcat(temp,socketid);
}

void tempWrite2(char temp[],int socketid){
    char sayi[5];
    sprintf(sayi, "%d", socketid);
    strcat(temp,"client");
    strcat(temp,sayi);
}


pthread_mutex_t lock;


void *serverThread(void * arg){
    char *client_message,*temp;
    char socketid[5];
    char clientchr[]="client";
    char numberArray[5];
    //-----------------------------
    int newSocket;
    int counter=0;
    int threadinId;
    //-----------------------------
    newSocket = *(int*)arg;
    
    if(logoutId<0){
	clientArray[id]=newSocket;
	sprintf(socketid, "%d", id);
        threadinId = id;
    }
    else{
	clientArray[idTemp]=newSocket;
	sprintf(socketid, "%d", idTemp);
        threadinId = idTemp;
	logoutId--;
    }

    client_message = malloc(messageLength);
    temp=malloc(messageLength);
    tempWrite(temp,socketid);

    //client login message-------------
    strcat(temp,"(logged in)" );
    puts(temp);
    //client login message-------------
    if(id>0){
	//other client input message------------------
	while(counter<=id)
	{
	    if(threadinId!=counter&&clientArray[counter]!=0){
	    	send(clientArray[counter] , temp , messageLength , 0);
	    }
	    counter++;
	}
	//other client input message------------------
	
    //message of pre-entered clients------------------
	counter=0;
	while(counter<=id){
		if(threadinId!=counter&&clientArray[counter]!=0){
		    temp=malloc(messageLength);
		    tempWrite2(temp,counter);
		    strcat(temp,"(entered)\n");
		    send(clientArray[threadinId] , temp , messageLength , 0);
		}
		counter++;
	}
	//message of pre-entered clients------------------
    }
    temp=malloc(2000);
    tempWrite(temp,socketid);
    strcat(temp,":");
    while((read_size = recv(newSocket , client_message , messageLength , 0)) > 0 ) {
	int indis=6;
	bool state=false;
	write(newSocket , client_message , strlen(client_message));
	
	
	//understanding if there is a private message-------------------------
	//first word check-------------------
	    if(strncmp(clientchr,client_message,6)!=0){
	        state=true;
		goto privateMessage;
	    }
	//first word check-------------------
	
	//get the client number-----------------------------
	counter=indis;
	int basamakCount=0;
	while(basamakCount<5){
		if(isalnum(client_message[counter])
				!=0&&isalpha(client_message[counter])==0){
			numberArray[basamakCount]=client_message[counter];
			
		}
		else{
			if(counter==indis){
				state=true;}
			
			goto privateMessage;
		}
		basamakCount++;
		counter++;
		
	}
	//get the client number-----------------------------
	//understanding if there is a private message-------------------------
	privateMessage:
	counter=0;
	indis+=basamakCount;
	//sending messages to non-cliente----------------------
	if(id<atoi(numberArray)&&!state){
	    send(clientArray[threadinId],"client not found!!!" , 30 , 0);
	    strcat(temp,"(tried to send non-cliente):");
	    strcat(temp,client_message);
	}
	//sending messages to non-cliente----------------------
	//message to yourself----------------------
	else if(strcmp(numberArray,socketid)==0){
		send(clientArray[atoi(numberArray)],"you can't send it to yourself!!!" , 30 , 0);
		strcat(temp,"(tried to send it to yourself):");
		strcat(temp,client_message);
	}
	//message to yourself----------------------
	//private message-----------------------------------------------------
	else if(!state&&client_message[indis]==':'){
		strcat(temp,"-->");
		int s_id=atoi(numberArray);
		strcat(temp,client_message);
		send(clientArray[s_id] , temp , messageLength , 0);
	}
	//private message-----------------------------------------------------
	//other client message-----------------------
	else{
		counter=0;
		strcat(temp,client_message );
		while(counter<=id){
	    	    if(counter!=threadinId&&clientArray[counter]!=-1 ){
	        	send(clientArray[counter] , temp , messageLength , 0);
	    	    }
	    	    counter++;
		}//while
    	    }//else
	//other client message-----------------------

	//every client message-----------
	puts(temp);
	//every client message-----------
	
	
        

	//client reset--temp reset--default-----------------
	memset(numberArray, 0, 5);
	client_message = malloc(messageLength);
	temp=malloc(messageLength);
    tempWrite(temp,socketid);
	strcat(temp,":");
	//client reset--temp reset--default-----------------
    }

    
    //client stack------------------
    clientArray[threadinId]=0;
    logoutId++;
    logoutArray[logoutId]=threadinId;
    //client stack------------------
	//client logout--------------------------
    temp=malloc(messageLength);
    tempWrite(temp,socketid);
    strcat(temp,"(exited)");
    puts(temp);
    counter=0;
    clientArray[threadinId]=0;
    while(counter<=id){
		if(counter!=threadinId&&clientArray[counter]!=-1){
			send(clientArray[counter] , temp , messageLength , 0);
		}
		counter++;
    }
    //client logout--------------------------
    
    
    close(newSocket);
    pthread_exit(NULL);
}//thread


int main(int argc , char *argv[]){
    
    struct sockaddr_in server , client;
    socket_desc = socket(AF_INET , SOCK_STREAM , 0); //Create socket
    if (socket_desc == -1) printf("Could not create socket");
    puts("Socket created");
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8080 );

    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0){
        perror("bind hatasi"); return 1;}
    listen(socket_desc , 60);
    puts("Waiting for connection...");
    c = sizeof(struct sockaddr_in);
    
    pthread_t tid[60];
    new_sock = malloc(480*sizeof(int));
    while(1){

        client_sock = accept(socket_desc, (struct sockaddr *)&server, (socklen_t*)&c);
		if(logoutId<0){
			id++;
			idTemp=id;
		}
		else{
			idTemp=logoutArray[logoutId];
		}
	
		new_sock[idTemp] = client_sock;
			if(pthread_create(&tid[idTemp], NULL, serverThread, &new_sock[idTemp]) != 0 )
				printf("Failed to create thread\n");
	}
	
	
    if(read_size == 0) {
        puts("Client ayrildi");
        fflush(stdout);
    }
    else if(read_size == -1) {
        perror("okuma hatasi");
    }
	
    return 0;
}
