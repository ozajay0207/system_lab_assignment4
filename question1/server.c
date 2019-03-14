// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <pthread.h> 
#include <time.h>

#define NO_OF_SERVICES 1
#define PORT 8080 

void init_services(char services[]){

	//services[1] = "2.Service";
	//services[2] = "3.Service";
}

void handle_client(int new_socket){
	int valread;
	int iterations;
	char buffer_choice[10000]=""; 
	char buffer[10000] =""; 
	char file_buff[10000]="";
	char *filename;
	char temp[500];
	clock_t t;
	float time;
	char services[10000]="1.Echo Request-Reply\n2.Application Layer RTT Measurement\n3.File Upload\n4.Exit";
	FILE *fp;

	//init_services(services);
	//printf("Length:%ld\n",strlen(services));	
	send(new_socket , services , 10000 , 0 ); 
	//printf("%s\n",services);
 	
	while(1){
		strcpy(buffer_choice,"");
		strcpy(buffer,"");
		valread = read( new_socket , buffer_choice, 10000);
//		printf("no of bytes read by read is %d\n", valread); 
		printf("Choice: -------- %s\n",buffer_choice); 
		if(strcmp(buffer_choice,"1")==0){
			valread = read( new_socket , buffer, 10000); 
			printf("Received String:%s\n",buffer);
			printf("Sending it back to client.\n");
			send(new_socket , buffer , 10000 , 0 ); 
		}		
		else if(strcmp(buffer_choice,"2")==0){
			strcpy(buffer,"");
			valread = read( new_socket , buffer, 10000); 
			printf("No of Bytes Sent:%s\n",buffer);
			strcpy(buffer,"");
			valread = read( new_socket , buffer, 10000); 			
			sscanf(buffer, "%d", &iterations); 
			printf("No of Iterations:%d\n",iterations);
			for(int i=0;i<iterations;i++){
				strcpy(buffer,"");	
				valread = read( new_socket , buffer, 10000); 
				printf("%s",buffer);
			}
		}	
		else if(strcmp(buffer_choice,"3")==0){
			valread = read( new_socket , temp, 10000 ); 			
			t=clock();
			filename = strtok(temp,".");
			strcat(temp,"_server.txt");
			printf("Writing to File:%s\n",filename);
			fp=fopen(filename,"w");
			
			do{				
				strcpy(buffer,"");
				valread = read( new_socket , buffer, 10000 ); 
				printf(":::::::::::%s\n",buffer);
				if(strcmp(buffer,"-999")!=0)
					fprintf(fp,"%s",buffer);
			}while(strcmp(buffer,"-999")!=0);				
			fclose(fp);
			t=clock()-t;
			time=((float)t)/CLOCKS_PER_SEC;
			strcpy(buffer,"");
			sprintf(buffer,"%f",time);
			send(new_socket,buffer,10000,0);
		}
		else if(strcmp(buffer_choice,"4")==0){
			break;
		}

	}

}

int main(int argc, char const *argv[]) 
{ 
	int server_fd, new_socket, valread; 
	struct sockaddr_in address; 
	int opt = 1; 
	pthread_t tid; 
	int addrlen = sizeof(address); 
	
	// Creating socket file descriptor 
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{ 
			perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	// Forcefully attaching socket to the port 8080 
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
												&opt, sizeof(opt))) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 
	
	// Forcefully attaching socket to the port 8080 
	if (bind(server_fd, (struct sockaddr *)&address, 
								sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	if (listen(server_fd, 3) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 

	while(1){
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
						(socklen_t*)&addrlen))<0) 
		{ 
			perror("accept"); 
			exit(EXIT_FAILURE); 
		} 
		pthread_create(&tid, NULL, handle_client, new_socket); 

	}
	return 0; 
} 

