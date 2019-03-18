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

//SERVICE 1 : ECHO REQUEST-REPLY
void echo_request_reply(int sock){

	int read_val;
	int counter=0;
	char mssg_buffer[BUFFER_SIZE]="";
	char data_buffer[BUFFER_SIZE] =""; 
	char timings[TIMING_DATA][TIME_STRING_SIZE];
	char choice;
	
	//REPEAT UNTIL 'N' FROM USER
	do{
		
		//TAKE MESSAGE STRING INPUT FROM USER
		strcpy(mssg_buffer,"");
		printf("Enter a Message String:");
		scanf("%s",mssg_buffer);
		send(sock , mssg_buffer , BUFFER_SIZE , 0 ); 

		//RECEIVE-SEND THE STRING TO SERVER
		read_val = read( sock , data_buffer, BUFFER_SIZE); 
		printf("String from Server:%s\n",data_buffer);
		strcpy(mssg_buffer,data_buffer);
		printf("Sending string back to server...\n");
		send(sock , mssg_buffer , BUFFER_SIZE , 0 ); 
		
		//READ THE TIME FOR CURRENT ROUND AND SAVE IN ARRAY
		strcpy(mssg_buffer,"");
		read_val=read(sock,mssg_buffer,BUFFER_SIZE);
		strcpy(timings[counter++],mssg_buffer);

		printf("Do you want to send another echo message (y/n) ?\n");
		getchar();
		scanf("%c",&choice);

	}while(choice == 'y' || choice == 'Y');

	//SEND STOP TO SERVER FOR CURRENT SERVICE 
	strcpy(mssg_buffer,"n");
	send(sock , mssg_buffer , BUFFER_SIZE , 0 ); 

	//LIST THE TIMINGS ROUND WISE CATEGORICALLY
	printf("Timings::\n");
	for(int i=0;i<counter;i++){
		printf("%d : %s\n",(i+1),timings[i]);
	}	
	
}

//SERVICE 2 : APPLICATION LAYER RTT MEASUREMENT
void AL_RTT(int sock){

	int read_val;
	int iterations;
	int mssg_length=0;
	char mssg_buffer[BUFFER_SIZE]="";
	char data_buffer[BUFFER_SIZE] =""; 

	//TAKE LENGTH OF MESSAGE AND NO. OF ITERATIONS AS INPUT FROM USER : SEND IT TO SERVER
	printf("Enter the length of message to send:");
	scanf("%d",&mssg_length);
	strcpy(mssg_buffer,"");
	sprintf(mssg_buffer,"%d",mssg_length);
	send(sock , mssg_buffer , BUFFER_SIZE , 0 ); 
	printf("Enter no of itertions to send message for :");
	scanf("%d",&iterations);
	strcpy(mssg_buffer,"");
	sprintf(mssg_buffer,"%d",iterations);
	send(sock , mssg_buffer , BUFFER_SIZE , 0 ); 


	//RECEIVE-SEND THE MESSAGE FOR SPECIFIED ITERATIONS	
	for(int i=0;i<iterations;i++){
		read_val = read(sock,data_buffer,BUFFER_SIZE);
		send(sock , data_buffer , BUFFER_SIZE , 0 ); 
	}

	//GET THE AVERAGE ROUND TRIP TIME FROM SERVER
	read_val=read(sock,mssg_buffer,BUFFER_SIZE);
	printf("Average RTT is :%s\n",mssg_buffer);
}

//SERVICE 3 : TO UPLOAD THE FILE TO SERVER
void upload_file(int sock){

	int read_val;
	int iterations;
	int mssg_length=0;
	char mssg_buffer[BUFFER_SIZE]="";
	char data_buffer[BUFFER_SIZE] ="";
	char filename[500]="";
	char file_buff[1024]="";
	FILE *fp;
 
	//TAKE THE FILENAME AND SEND IT TO SERVER
	strcpy(filename,"");
	strcpy(mssg_buffer,"");
	printf("Enter the name of the file to upload:");
	scanf("%s",filename);
	printf("Filename:%s\n",filename);
	send(sock, filename , BUFFER_SIZE,0);

	//OPEN THE FILE TO UPLOAD TO SERVER
	fp=fopen(filename,"r");
	if(fp!=NULL){
		
		//SEND LINE BY LINE FILE DATA
		while (fgets(file_buff, sizeof(file_buff), fp) != NULL){
			send(sock , file_buff , BUFFER_SIZE , 0 ); 
		}	
		//SEND END OF FILE 
		strcpy(file_buff,"-999");
		send(sock , file_buff , BUFFER_SIZE , 0 ); 

		//READ THE TIME TAKEN TO UPLOAD
		read_val=read(sock,mssg_buffer,BUFFER_SIZE);
		printf("Time taken to upload file is :%s\n",mssg_buffer);
		}else{
		printf("Issue opening the file\n");
	}
	fclose(fp);
}

void other_services(int sock){

	int read_val;
	char mssg_buffer[BUFFER_SIZE]="";
	char data_buffer[BUFFER_SIZE] =""; 
	char service_type[BUFFER_SIZE];

	printf("Other Services\n");
	while(1){
		
		strcpy(data_buffer,"");
		read_val=read(sock,data_buffer,BUFFER_SIZE);	
		strcpy(service_type,data_buffer);
		if(strcmp(service_type,"1")==0){
			strcpy(data_buffer,"");
			read_val=read(sock,data_buffer,BUFFER_SIZE);
			printf("%s",data_buffer);			
		}else if(strcmp(service_type,"2")==0){
			scanf("%s",mssg_buffer);
			send(sock,mssg_buffer,BUFFER_SIZE,0);
		}else{
			printf("Exiting\n");
			break;
		}
	}
}

int main(int argc, char const *argv[]) 
{ 
	int check=0;
	struct sockaddr_in address; 
	int sock = 0, read_val; 
	struct sockaddr_in serv_addr; 
	int choice;
	char choice_str[2];
	char data_buffer[BUFFER_SIZE] ="";		

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

	//LOOP UNTIL CLIENT QUITS
	while(1){	

		//ASK FOR THE SERVICE TYPE
		printf("Enter a Choice:\n");
		scanf("%d",&choice);
		sprintf(choice_str, "%d", choice);
		send(sock , choice_str , BUFFER_SIZE , 0 ); 	
		
		if(choice==999)			
			break;
		
		switch(choice){
			case 1:
				echo_request_reply(sock);
			break;
			case 2:
				AL_RTT(sock);
			break;
			case 3:
				upload_file(sock);
			break;
			default:
				other_services(sock);
			break;
			
		}
	}	
	return 0; 
} 

