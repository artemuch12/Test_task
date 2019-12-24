all: server
server: server.o libraris
	gcc server.o -pthread -o server -L. -lfunctions
server.o: server.c
	gcc -g server.c -c
libraris:
	gcc -g functions.c -c
	ar -rc libfunctions.a functions.o
clean:
	rm *.o
	server
astyle:
	astyle -K --style=allman server.c
	astyle -K --style=allman functions.c
	rm *.orig
