
all: echoServer

echoServer: echo_s.o echo_s.h
	g++ -o echoServer echo_s.o echo_s.h
echo_s.o: echo_s.c
	g++ -c echo_s.c
clean: 
	rm -rf echo_s.o echoServer

