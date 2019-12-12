all: server
server: server.o
	gcc server.o -o server
server.o: server.c
	gcc -g server.c -c
clean:
	rm *.o
	server
astyle:
	astyle --style=allman server.c
