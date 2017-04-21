/* File Name: echo_s.h
 * Author: Fortythree Shiaohongtu
 * Description: Header files for echo_s.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netdb.h>

void error(const char *msg);

void toLog(char *IP[], char *msg[]);

void connectUDP(int portno);

void connectTCP(int portno);
