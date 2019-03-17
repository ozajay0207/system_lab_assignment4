// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <time.h>
#define PORT 8080 
#define READ_BUFF_SIZE 10000
int main(int argc, char const *argv[]) 
{ 
	struct sockaddr_in address; 
	int sock = 0, valread; 
	struct sockaddr_in serv_addr; 
	char *hello = "Hello from client"; 
	char buffer[10000] = {0}; 
	int choice,mssg_length,iterations;
	char choice_str[2];
	char mssg[10000]="";
	char filename[500]="";
	char file_buff[1024]="";
	FILE *fp;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 

	memset(&serv_addr, '0', sizeof(serv_addr)); 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
	
	// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	} 	
	valread = read( sock , buffer, READ_BUFF_SIZE); 
	while(1){
		
		printf("%s\n",buffer); 
		printf("Enter a Choice:\n");
		scanf("%d",&choice);
		sprintf(choice_str, "%d", choice);
		send(sock , choice_str , 10000 , 0 ); 	
		
		if(choice==4)			
			break;
		

		switch(choice){
			case 1:
				strcpy(mssg,"");
				printf("Enter a Message String:");
				scanf("%s",mssg);
				send(sock , mssg , 10000 , 0 ); 
				valread = read( sock , buffer, 10000); 
				printf("String from Server:%s\n",buffer);
				valread=read(sock,mssg,10000);
				printf("Time taken to upload file is :%s\n",mssg);
			break;
			case 2:
				printf("Enter the length of message to send:");
				scanf("%d",&mssg_length);
				strcpy(mssg,"");
				sprintf(mssg,"%d",mssg_length);
				send(sock , mssg , 10000 , 0 ); 
				printf("Enter no of itertions to send message for :");
				scanf("%d",&iterations);
				strcpy(mssg,"");
				sprintf(mssg,"%d",iterations);
				send(sock , mssg , 10000 , 0 ); 
				strcpy(mssg,"A");
				for(int i=1;i<mssg_length;i++)
					strcat(mssg,"A");
				strcat(mssg,"\n");
				for(int i=0;i<iterations;i++){
					//printf("Sending:%s\n",mssg);
					send(sock , mssg , 10000 , 0 ); 
				}
				valread=read(sock,mssg,10000);
				printf("Time taken to upload file is :%s\n",mssg);
			break;
			case 3:
				strcpy(filename,"");
				strcpy(mssg,"");
				printf("Enter the name of the file to upload:");
				scanf("%s",filename);
				printf("Filename:%s\n",filename);
				send(sock, filename , 10000,0);
				fp=fopen(filename,"r");
				if(fp!=NULL){
					while (fgets(file_buff, sizeof(file_buff), fp) != NULL){
						//printf("Uploading:%s\n",file_buff);
						send(sock , file_buff , 10000 , 0 ); 
					}	
					strcpy(file_buff,"-999");
					send(sock , file_buff , 10000 , 0 ); 
					valread=read(sock,mssg,10000);
					printf("Time taken to upload file is :%s\n",mssg);
				}else{
					printf("Issue opening the file\n");
				}
				fclose(fp);
			break;
			
		}
	}	
	return 0; 
} 

