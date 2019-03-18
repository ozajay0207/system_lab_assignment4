#include <sys/socket.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <time.h>

#define PORT 8080 
#define TIMING_DATA 100
#define TIME_STRING_SIZE 100
#define USER_DATA_SIZE 100
#define BUFFER_SIZE 10000

//TO TAKE INPUT & CHECK THE AUTHENTICATION OF CLIENT CREDENTIALS
int check_auth(int sock){

	int read_val;
	char user_id[USER_DATA_SIZE];
	char password[USER_DATA_SIZE];
	char data_buffer[BUFFER_SIZE] =""; 

	//TAKE USERID AND PASSWORD FROM USER
	printf("Enter User Id:");
	scanf("%s",user_id);
	printf("Enter Password:");
	scanf("%s",password);

	//SEND THE CREDENTIALS TO SERVER FOR CHECKING
	strcpy(data_buffer,user_id);
	send(sock,data_buffer,BUFFER_SIZE,0);
	strcpy(data_buffer,password);
	send(sock,data_buffer,BUFFER_SIZE,0);
	strcpy(data_buffer,"");

	//CHECK FOR AUTHENTICATION REPLY
	read_val = read( sock , data_buffer, BUFFER_SIZE); 
	if(strcmp(data_buffer,"not_auth")==0){
		printf("Authentication Failed\n");
		return 0;
	}

	printf("Client %s Authenticated\n",data_buffer);
	return 1;
}


int main(int argc, char const *argv[]) 
{ 
	int check=0;
	struct sockaddr_in address; 
	int sock = 0, read_val; 
	struct sockaddr_in serv_addr; 
	char choice[50];
	char mssg[500];
	char data_buffer[BUFFER_SIZE] ="";		
	char file_buff[BUFFER_SIZE]="";
	int no_of_inputs=0;
	int no_of_resends=0;
	int no_of_lines=0;
	int no_of_outputs=0;
	FILE *fp;

	//INTIALIZE THE CONNECTION AND SOCKETS
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 
	memset(&serv_addr, '0', sizeof(serv_addr)); 
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
	
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0){ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

	//CONNECT TO SERVER WHICH IS ACCPETING CONNECTIONS
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	}

	//AUTHENTICATE THE CLIENT AND CLOSE IF FAIL
	check = check_auth(sock);	
	if(check==0)
		return 0;
	 	
	//READ THE SERVICES AND DISPLAY IT TO USER
	strcpy(data_buffer,"");
	read_val = read( sock , data_buffer, BUFFER_SIZE); 
	printf("%s\n",data_buffer); 

	//GET INPUT
	scanf("%s",choice);
	send(sock , choice , BUFFER_SIZE , 0 ); 

	while(1){	
		
		START:
		printf("At Start \n");
		//NO. OF INPUTS
		strcpy(data_buffer,"");
		read_val = read( sock , data_buffer, BUFFER_SIZE); 
		sscanf(data_buffer, "%d",&no_of_inputs);			
		printf("No. of inputs:%d\n",no_of_inputs);

		for(int i=0;i<no_of_inputs;i++){	
			//PRINT MESSAGE
			strcpy(data_buffer,"");
			read_val = read( sock , data_buffer, BUFFER_SIZE); 
			printf("%s\n",data_buffer); 

			//GET INPUT
			scanf("%s",mssg);
			send(sock , mssg , BUFFER_SIZE , 0 ); 
		}

		//DECISION OF LOOP BREAK
		strcpy(data_buffer,"");
		read_val = read( sock , data_buffer, BUFFER_SIZE); 
		printf("Decision:%s\n",data_buffer);
		if(strcmp(data_buffer,"break_loop")==0)
			break;
		else if(strcmp(data_buffer,"continue looping")==0){
			goto START;
		}

		//FILE UPLOAD
		if(strcmp(choice,"3")==0){
			//OPEN THE FILE TO UPLOAD TO SERVER
			strcpy(data_buffer,"");
			read_val = read( sock , data_buffer, BUFFER_SIZE); 
			fp=fopen(data_buffer,"r");

			if(fp!=NULL){
				
				//COUNT LINES
				while (fgets(file_buff, sizeof(file_buff), fp) != NULL){
					no_of_lines += 1;
				}
				sprintf(mssg,"%d",no_of_lines);
				send(sock , mssg , BUFFER_SIZE , 0 ); 	
			}
			fclose(fp);
		}

		//NO. OF RESENDS
		strcpy(data_buffer,"");
		read_val = read( sock , data_buffer, BUFFER_SIZE); 
		printf("No of resends %s\n",data_buffer);			
		sscanf(data_buffer, "%d", &no_of_resends);


		for(int i=0;i<no_of_resends;i++){			
			//GET MESSAGE
			strcpy(data_buffer,"");
			read_val = read( sock , data_buffer, BUFFER_SIZE); 
			printf("Received from server:%s\n",data_buffer); 

			//RESEND
			strcpy(mssg,data_buffer);
			send(sock , mssg , BUFFER_SIZE , 0 ); 
			printf("Sending it back\n");
		}

	}

		//NO. OF OUTPUTS
		strcpy(data_buffer,"");
		read_val = read( sock , data_buffer, BUFFER_SIZE); 
		sscanf(data_buffer, "%d", &no_of_outputs);			

		for(int i=0;i<no_of_outputs;i++){	
			
			//GET OUTPUT MESSAGE
			strcpy(data_buffer,"");
			read_val = read( sock , data_buffer, BUFFER_SIZE); 
			printf("Received from server:%s\n",data_buffer); 
		}

		
	
	return 0; 
} 

