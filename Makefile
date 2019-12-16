all: server
server: server.o
	gcc server.o -pthread -o server
server.o: server.c
	gcc -g server.c -c
clean:
	rm *.o
	server
astyle:
	astyle --style=allman server.c
	rm *.orig
