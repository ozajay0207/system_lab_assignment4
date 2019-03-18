#include <stdio.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <unistd.h> 
#include <string.h> 
#include <pthread.h> 
#include <time.h>

#define NO_OF_USERS 3
#define NO_OF_SERVICES 5
#define USER_DATA_SIZE 100
#define BUFFER_SIZE 10000
#define PORT 8080 

//TO CALCULATE TIME
int gettimeofday(struct timeval *,struct timezone *);

//STRUCTURE FOR USER 
struct credentials{
	char user_id[USER_DATA_SIZE];
	char password[USER_DATA_SIZE];
}users[NO_OF_USERS];

//INITIALIZE THE USER AND THEIR CREDENTIALS
void initialize_user_credentials(struct credentials users[]){
	strcpy(users[0].user_id,"jay");
	strcpy(users[0].password,"jay123");
	strcpy(users[1].user_id,"harsh");
	strcpy(users[1].password,"jay123");
	strcpy(users[2].user_id,"monika");
	strcpy(users[2].password,"jay123");
}

//INITIALIZE THE SERVICES TO OFFER THE CLIENTS
void initialize_services(char services[]){

	char service_list[NO_OF_SERVICES][200];
	strcpy(service_list[0],"1.Echo Request-Reply");
	strcpy(service_list[1],"2.Application Layer RTT Measurement");
	strcpy(service_list[2],"3.File Upload");
	strcpy(service_list[4],"999.Done");
	strcpy(service_list[3],"4.Add 2 numbers");

	//COPY THE SERVICES TO A COMMON MESSAGE STRING
	strcpy(services,service_list[0]);
	for(int i=1;i<NO_OF_SERVICES;i++){
		strcat(services,"\n");
		strcat(services,service_list[i]);
	}

}

//SERVICE 1 : ECHO REQUEST REPLY 
void echo_request_reply(int new_socket){

	int read_val;
	char data_buffer[BUFFER_SIZE] =""; 
	struct timeval start, stop;
	double secs = 0;
	
	//READ THE FIRST ECHO MESSAGE
	read_val = read( new_socket , data_buffer, BUFFER_SIZE); 

	//LOOP UNTIL CLIENT SENDS 'N' FOR ECHO MESSAGE
	while(1){

		//INIT THE TIMER	
		gettimeofday(&start, NULL);

		//SEND-RECEIVE MESSAGE (1 ROUND)
		printf("Sending : %s\n",data_buffer);
		send(new_socket , data_buffer , BUFFER_SIZE , 0 ); 
		strcpy(data_buffer,"");
		read_val = read( new_socket , data_buffer, BUFFER_SIZE); 
		printf("Received : %s\n",data_buffer);	

		//STOP THE TIMER
		gettimeofday(&stop, NULL);

		//CALCULATE THE TIME IN MILLISECONDS
		secs = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
		strcpy(data_buffer,"");
		sprintf(data_buffer,"%f",secs);
	
		//SEND THE TIME TO CLIENT FOR STORAGE
		send(new_socket,data_buffer,BUFFER_SIZE,0);
		
		//CHECK FOR NEXT MESSAGE
		read_val = read( new_socket , data_buffer, BUFFER_SIZE); 

		//BREAK IF CHOICE IS 'N'
		if(strcmp(data_buffer,"n")==0)
			break;	
	};
}

//SERVICE 2 : APPLICATION LAYER RTT MEASUREMENT
void AL_RTT(int new_socket){

	int read_val;
	int iterations;
	int mssg_length=0;
	char data_buffer[BUFFER_SIZE] =""; 
	char mssg_buffer[BUFFER_SIZE]="";
	struct timeval start, stop;
	double secs = 0;
	double temp_secs = 0;

	//NO. OF BYTES OF MESSAGE TO BE SENT (RECEIVED)
	strcpy(data_buffer,"");
	read_val = read( new_socket , data_buffer, BUFFER_SIZE); 
	sscanf(data_buffer, "%d", &mssg_length); 	
	printf("No of Bytes Sent:%s\n",data_buffer);

	//NO. OF ITERATIONS TO PERFORM THE SENDING (RECEIVED)
	strcpy(data_buffer,"");
	read_val = read( new_socket , data_buffer, BUFFER_SIZE); 			
	sscanf(data_buffer, "%d", &iterations); 
	printf("No of Iterations:%d\n",iterations);

	//CREATE A DUMMY MESSAGE OF SIZE IN BYTES RECEIVED
	strcpy(mssg_buffer,"A");
	for(int i=1;i<mssg_length;i++)
		strcat(mssg_buffer,"A");
	strcat(mssg_buffer,"\n");
	
	//SEND THE MESSAGE FOR RECEIVED NO. OF ITERATIONS
	for(int i=0;i<iterations;i++){

		temp_secs=0;

		//START THE TIMER
		gettimeofday(&start, NULL);

		//SEND-RECEIVE THE MESSAGE (1 RTT)
		strcpy(data_buffer,mssg_buffer);	
		send( new_socket , data_buffer, BUFFER_SIZE,0); 
		strcpy(data_buffer,"");
		read_val = read(new_socket,data_buffer,BUFFER_SIZE);
		gettimeofday(&stop, NULL);

		//CALCULATE THE TIME IN  MILLISECONDS		
		temp_secs = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);

		//SUM THE TIME FOR EACH RTT 
		secs += temp_secs;
	}

	//SEND THE AVERAGE RTT OVER THE ITERATIONS
	strcpy(data_buffer,"");
	sprintf(data_buffer,"%f",secs/iterations);
	send(new_socket,data_buffer,BUFFER_SIZE,0);

}

//SERVICE 3 : TO UPLOAD A FILE AND GET INTO SERVER
void upload_file(int new_socket){

	int read_val;
	char data_buffer[BUFFER_SIZE] =""; 
	struct timeval start, stop;
	double secs = 0;
	char file_buff[BUFFER_SIZE]="";
	char *filename;
	char temp[500];
	FILE *fp;

	//READ THE FILENAME
	read_val = read( new_socket , temp, BUFFER_SIZE ); 			

	//START THE TIMER
	gettimeofday(&start, NULL);

	//EXTRACT FILENAME AND MAKE SERVER FILENAME ACCORDINGLY TO REPLICATE
	filename = strtok(temp,".");
	strcat(temp,"_server.txt");
	printf("Writing to File:%s\n",filename);

	//OPEN FILE FOR WRITING
	fp=fopen(filename,"w");	
	
	//LOOP UNTIL END OF FILE		
	do{				
		strcpy(data_buffer,"");
		read_val = read( new_socket , data_buffer, BUFFER_SIZE ); 
		if(strcmp(data_buffer,"-999")!=0)
			fprintf(fp,"%s",data_buffer);
	}while(strcmp(data_buffer,"-999")!=0);				
	fclose(fp);

	//STOP THE TIMER : RECEIVED THE ENTIRE FILE
	gettimeofday(&stop, NULL);

	//CALCULATE THE TIME IN MILLISECONDS
	secs = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
	
	//SEND THE UPLOADING TIME TO CLIENT
	strcpy(data_buffer,"");
	sprintf(data_buffer,"%f",secs);
	send(new_socket,data_buffer,BUFFER_SIZE,0);
}

void add_2_numbers(int new_socket){
	int read_val;
	char data_buffer[BUFFER_SIZE] =""; 
	char mssg_buffer[BUFFER_SIZE]="";
	struct timeval start, stop;
	double secs = 0;
	double temp_secs = 0;
	int num1=0,num2=0;
	int sum=0;

	strcpy(data_buffer,"1");
	send(new_socket,data_buffer,BUFFER_SIZE,0);
	strcpy(data_buffer,"Enter first number:");
	send(new_socket,data_buffer,BUFFER_SIZE,0);

	strcpy(data_buffer,"2");
	send(new_socket,data_buffer,BUFFER_SIZE,0);
	strcpy(data_buffer,"");
	read_val = read( new_socket , data_buffer, BUFFER_SIZE ); 
	printf("Input 1:%s\n",data_buffer);
	sscanf(data_buffer,"%d",&num1);

	strcpy(data_buffer,"1");
	send(new_socket,data_buffer,BUFFER_SIZE,0);
	strcpy(data_buffer,"Enter second number:");
	send(new_socket,data_buffer,BUFFER_SIZE,0);

	strcpy(data_buffer,"2");
	send(new_socket,data_buffer,BUFFER_SIZE,0);
	strcpy(data_buffer,"");
	read_val = read( new_socket , data_buffer, BUFFER_SIZE ); 
	printf("Input 2:%s\n",data_buffer);
	sscanf(data_buffer,"%d",&num2);

	sum=num1+num2;
	strcpy(data_buffer,"1");
	send(new_socket,data_buffer,BUFFER_SIZE,0);
	sprintf(data_buffer,"Sum:%d\n",sum);
	send(new_socket,data_buffer,BUFFER_SIZE,0);

	strcpy(data_buffer,"999");
	send(new_socket,data_buffer,BUFFER_SIZE,0);

}

//CONTROLLER FUNCTION TO HANDLE EACH CLIENT (1 PER THREAD)
void *handle_client(int *new_socket1){

	char user_id[100];
	char password[100];
	int authenticated=-1;
	int new_socket;
	int read_val;	
	char buffer_choice[BUFFER_SIZE]=""; 
	char data_buffer[BUFFER_SIZE] =""; 
	struct timeval start, stop;
	double secs = 0;
	double temp_secs = 0;
	char services[BUFFER_SIZE];

	//INITIALIZE THE SERVICES
	initialize_services(services);
	new_socket=*new_socket1;		

	//RECEIVE THE CREDENTIALS (USER_ID AND PASSWORD)
	strcpy(data_buffer,"");
	read_val = read(new_socket,data_buffer,BUFFER_SIZE);
	strcpy(user_id,data_buffer);
	strcpy(data_buffer,"");
	read_val = read(new_socket,data_buffer,BUFFER_SIZE);
	strcpy(password,data_buffer);

	//CHECK FOR THE AUTHENTICATIONS
	for(int i=0;i<3;i++){
		if(strcmp(users[i].user_id,user_id)==0){
			if(strcmp(users[i].password,password)==0){
				authenticated=i;
			}
		}
	}	
	
	//RETURN IF AUTHENTICATION FAILS - ELSE SEND CLIENT CONFIRMATION
	if(authenticated==-1){
		send(new_socket,"not_auth",BUFFER_SIZE,0);
		return;
	}else
		send(new_socket,data_buffer,BUFFER_SIZE,0);
	printf("Client %d Authenticated\n",authenticated);
	strcpy(data_buffer,"");
	sprintf(data_buffer,"%d",authenticated);
	send(new_socket , services , BUFFER_SIZE , 0 ); 
 	

	while(1){
		strcpy(buffer_choice,"");
		strcpy(data_buffer,"");
		read_val = read( new_socket , buffer_choice, BUFFER_SIZE);
		printf("Service Selected by client %d is : -------- %s\n",authenticated,buffer_choice); 
		if(strcmp(buffer_choice,"1")==0){
			echo_request_reply(new_socket);
		}		
		else if(strcmp(buffer_choice,"2")==0){
			AL_RTT(new_socket);
		}	
		else if(strcmp(buffer_choice,"3")==0){
			upload_file(new_socket);
		}
		else if(strcmp(buffer_choice,"4")==0){
			add_2_numbers(new_socket);
		}
		else if(strcmp(buffer_choice,"999")==0){
			break;
		}

	}

}


int main(int argc, char const *argv[]) 
{

	//INITIALIZE THE CONNECTION AND SOCKET ALONG WITH EXCEPTION HANDLING
	int server_fd, new_socket, read_val; 
	struct sockaddr_in address; 
	int opt = 1; 
	pthread_t tid; 
	int addrlen = sizeof(address); 	
	
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){ 
		perror("Socket Failed."); 
		exit(EXIT_FAILURE); 
	} 
	
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt))){ 
		perror("Set Socktet Option"); 
		exit(EXIT_FAILURE); 
	} 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 
	
	//BIND THE ADDRESS AND PORT
	if (bind(server_fd, (struct sockaddr *)&address,sizeof(address))<0){ 
		perror("Binding Failed"); 
		exit(EXIT_FAILURE); 
	} 
	if (listen(server_fd,NO_OF_USERS) < 0){ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 

	//INIT USER CREDENTIALS
	initialize_user_credentials(users);

	//LISTEN TO USER IF ANY
	while(1){

		//CHECK FOR CONNECTION ACCEPT BY ANY CLIENT
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen))<0)
		{ 
			perror("Accept Received"); 
			exit(EXIT_FAILURE); 
		} 

		//SERVE THE CLIENT IN A SEPERATE THREAD
		pthread_create(&tid, NULL, handle_client,(void *) &new_socket); 

	}
	return 0; 
} 

