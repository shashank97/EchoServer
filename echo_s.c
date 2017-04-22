/* File Name: echo_s.c
 * Author: Fortythree Shioahongtu
 * Description: Create an echo server that takes port number as argument(s) 
 * Supports both UTD and TCP connections 
 * ----------------------    DISCLAIMER -------------------
 *  SOME OF THE FOLLOWING CODES ARE TAKEN FROM www.linuxhowtos.org/C_C++/socket.htm*/

#include "echo_s.h"

int main(int argc, char *argv[]){
	signal(SIGCHLD, SIG_IGN);
	int pid1, pid2;
	if (argc <2 || argc > 4){
		fprintf(stderr, "Please follow the format, \"echoServer <port1> [<port2> <port3>]\" with port 2 and 3 being optional\n");
		exit(0);
	}
	
	//crate child process for 2nd port
	if (argc >= 3){
		if ((pid1 = fork()) < 0){
			error("Error on creating child process 1");
		}
		if(pid1 == 0){
			connectTCP(atoi(argv[2]));
		}
	}//end of child process for 2nd port

	//create child process for 3rd port
	if (argc == 4){
		if ((pid2 = fork()) < 0){
			error("ERROR on creating child process 2");
		}
		if(pid2 == 0){
			connectTCP(atoi(argv[3]));
		}
	}//end of child process for 3rd port
	
	//if there is only 1 port specified, create 2 well-known ports
	if (argc == 2){
		
		if ((pid1 = fork()) < 0){
			error("ERROR on creating 1 port child process 1");
		}

		if (pid1 == 0){
			connectTCP(2343);
		}
	 	else if (pid1 != 0){		
			if ((pid2 = fork()) < 0){
				error("ERROR on creating 1 port child process 2");
			}
			if (pid2 == 0){
				connectTCP(2533);
			}
		}
	}//end of 1 port if statement
	
	connectTCP(atoi(argv[1]));
	exit(0);
}

void error(const char *msg){
	perror(msg);
	exit(1);
}

//send the message received to a log server
void toLog(const char *IP, const char *msg){	
	int sockfd;
	struct sockaddr_in serv_addr;

	int portno = 9999;
	struct hostent *server = gethostbyname("localhost");
	
	//if host not found
	if (server == NULL){
		error("ERROR no such host");
	}

	bzero ((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr_list[0],
	      (char *)&serv_addr.sin_addr.s_addr,
	      server->h_length);
	serv_addr.sin_port = htons(portno);

	const size_t BUFFER_SIZE = 512;
	//sets the memory to zero
	char *buffer = (char *)calloc(BUFFER_SIZE, sizeof(char));
	
	//Combine IP and msg into 1 and store it into buffer
	strcat (buffer, IP);
	strcat (buffer, "\t");
	strcat (buffer, msg);

	//comunicate using UDP
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0){
		error("ERROR opening socket");
	}

	socklen_t serverAddressLength = sizeof(serv_addr);
	int numBytes = (int)sendto(sockfd, buffer, strlen(buffer), 0,
				(struct sockaddr *)&serv_addr, serverAddressLength);
	if (numBytes < 0){
		error("ERROR sending to log server");
	}
	
	//free buffer to prevent memory leak
	free(buffer);
}//end of toLog

//make connection using UDP
void connectUDP(int portno){
	int sockfd, length, n, pid;
	socklen_t fromlen;
	struct sockaddr_in server;
	struct sockaddr_in from;
	char buf[1024];

	//create a datagram socket
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0){
		error("Opening socket");
	}
	length = sizeof(server);
	bzero(&server, length);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(portno);

	if(bind(sockfd, (struct sockaddr *)&server,length)<0){
		error("binding");
	}
	fromlen = sizeof(struct sockaddr_in);
	
	//infinite loop to recieve message and send messages
	while (true){
			bzero(buf, 1024);	
			n = recvfrom (sockfd, buf, 1024, 0, (struct sockaddr *)&from, &fromlen);
			if (n < 0){
				error("ERROR on recfrom");
			}
			write(1, "Received a datagram: ", 21);
			write(1, buf, n);
			n = sendto(sockfd, buf, 17, 0, (struct sockaddr *)&from, fromlen);
			toLog(inet_ntoa(from.sin_addr), buf);
			if (n < 0){
				error("ERROR on sendto");
			}
	}//end of while
}//end of function

//make connection using TCP
void connectTCP(int portno){
	
	//create a child process each time connectTCP is called so a UDP connection will be called as well
	int UDPpid;
	if((UDPpid = fork()) < 0){
		error("ERROR on making UDP child process");
	}
	if (UDPpid == 0){
	connectUDP(portno);
	}

	int sockfd, newsockfd, n;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, serv2_addr, serv3_addr, cli_addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){
		error("ERROR opening socket");
	}

		//set all values in a buffer to 0
		bzero((char *)&serv_addr, sizeof(serv_addr));

		//set the structure variables in serv_addr to the correct info
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;	
		serv_addr.sin_port = htons(portno);
		if (bind(sockfd, (struct sockaddr *) &serv_addr,
			sizeof(serv_addr)) < 0){
			error("ERROR on binding");
		}
	
	//allows the process to listen on the socket for connections
	listen(sockfd, 5);
	clilen = sizeof(cli_addr);
	int pid;

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
			printf("Received a TCP: %s", buffer);
			toLog(inet_ntoa(cli_addr.sin_addr), buffer);
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
}//end of connectTCP

