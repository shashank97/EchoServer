//this will be a log server. It will simply maintain a simple log of the date, time, and ip address of each connection/action with the server

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

void log(int socketFD, struct sockaddr *address)
{
	if(socketFD < 0)
	{
		return;
	}

	const size_t BUFFER_SIZE = 256;

	socklen_t addressLength = sizeof(*address);

	int numBytes = (int)recvfrom(socketFD, buffer, BUFFER_SIZE -1, 0, address, &addressLength);

	if(numBytes<0)
	{
		error("recvfrom error in log");
	}
	else if(numBytes == 0)
	{
		printf("No message was sent to log");
	}
	else
	{
		setenv("message", buffer, 1);
		system("date '+%Y-&m-%d %R:%s' | tr -d '\n' >> echo.log; echo ' ' | tr -d '\n' >> echo.log; echo $message |tr -d '\n' >> echo.log; echo ' ' | tr -d '\n' >> hostname -I >> echo.log");
	}

	free(buffer);	
}

int main(int argc, char *argv[])
{
	int logSockFD, portno;
	socklen_t clilen;
	struct sockaddr_in log_addr, cli_addr;
	fd_set fdSet;

	logSockFD = socket(AF_INET, SOCK_DGRAM, 0);

	if(logSockD < 0)
	{
		error("Error opening log socket");
	}

	printf("Log server is running...\n");

	const int enable = 1;

	setsockopt(logSockFD, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

	//set server address
	bzero((char *)&log_addr, sizeof(log_addr));

	portno = 9956 // should be 9999 later on, but i left it like this so as to ensure it was open

	log_addr.sin_family = AF_INET;
	log_addr.sin_addr.s_addr = INADDR_ANY;
	log_addr.sin_port = htons(portno);

	if(bind(logSockFD, (struct sockaddr *) &log_addr, sizeof(log_addr)) < 0)
	{
		printf("Error on bind log socket");
	}

	bool whileTrue = true;

	while(whileTrue)
	{
		FD_ZERO(&fdSet);

		FD_SET(logSockFD, &fdSet);

		if(FD_ISSET(logSockFD, &fdSet))
		{
			pid_t pid = fork();

			//now we see if we are the child
			if(pid==0)//then we are the child
			{
				//have the child perform the log command
				log(logSockFD, (struct sockaddr *)&cli_addr);
				return 0;
			}
			else if(pid<0)
			{
				error("Log could not fork a child process");
			}	
			else
			{
				//this is the parent process
			}
		}
	}

	close(logSockFD);

	return 0;
}
