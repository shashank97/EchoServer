/* File Name: echo_s.c
 * Author: Fortythree Shioahongtu
 * Description: Create an echo server that takes port number as argument(s) 
 * Supports both UTD and TCP connections */

#include "echo_s.h"

//return the type of error
void error(const char *msg){
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[]){
	signal(SIGCHLD, SIG_IGN);
	if (argc <2){
		fprintf(stderr, "Please follow the format, \"echoServer <port1> [<port2> <port3>]\" with port 2 and 3 being optional\n");
	}
	connectTCP(argc , argv);
	exit(0);
}


//make connection using TCP
void connectTCP(int argc, char *argv[]){
	int sockfd, newsockfd, n, pid1, pid2, pid3;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	
	//create the first child process for the first port
	if((pid1 = fork()) < 0 || (pid2 = fork()) < 0){
		error("ERROR creating child processes");
	}

	//only child processes is going to run
	if(pid1 == 0){// || pid2 == 0){
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0){
			error("ERROR opening socket");
		}
	
		//sets all values in a buffer to 0
		bzero((char *)&serv_addr, sizeof(serv_addr));
	
		//set the structure variables in serv_addr to the correct info
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		
		//assign the first port to the first process
		if(pid1 == 0){
			serv_addr.sin_port = htons(atoi(argv[1]));
			if (bind(sockfd, (struct sockaddr *) &serv_addr,
				sizeof(serv_addr)) < 0){
				error("ERROR on binding");
			}
		}

		if(pid2 == 0){
			//if no 2nd port is provided, assign the 2nd port to a well-known port
			if(argv[2] == NULL){
				serv_addr.sin_port = htons(atoi("4343"));
			}
			else{
				serv_addr.sin_port = htons(atoi(argv[2]));
			}
			if (bind(sockfd, (struct sockaddr *) &serv_addr,
				sizeof(serv_addr)) < 0){
				error("ERROR on binding");
				}

		}
		
		//allows the process to listen on the socket for connections
		listen(sockfd, 5);
		clilen = sizeof(cli_addr);
		int pid;	
		
		//inifite loop to keep the server up while creating new processes
		while(true){
			newsockfd = accept(sockfd, (struct sockaddr *) 
				&cli_addr, &clilen);
			if (newsockfd < 0){
				error("ERROR on accept");
			}
			if((pid = fork()) < 0){
				error("ERROR on fork");
			}
			
			//child process. Read and write message
			if (pid == 0){
				close(sockfd);
				bzero(buffer, 256);
				n = read(newsockfd, buffer, 255);
	
				if( n < 0){
					error("ERROR reading from socket");
				}
				printf("%s", buffer);
				n = write(newsockfd, buffer, 18);
				if (n < 0){
					error("ERROR writing to socket");
				}
				exit(0);
			}
			else{
				close(newsockfd);
			}

		} //end of while
		close(sockfd);
	} //end of child process
}//end of function
