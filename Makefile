all: server
server: server.o libraris
	gcc server.o -pthread -o server -L. -lfunctions_strings -lfunctions_rbtree
server.o: server.c
	gcc -g server.c -c
libraris:
	gcc -g functions_strings.c -c
	ar -rc libfunctions_strings.a functions_strings.o
	gcc -g functions_rbtree.c -c
	ar -rc libfunctions_rbtree.a functions_rbtree.o
clean_obj:
	rm *.o
	rm *.a
clean_elf:
	rm server
astyle:
	astyle -K --style=allman server.c
	astyle -K --style=allman functions_strings.c
	astyle -K --style=allman functions_rbtree.c
	rm *.orig
