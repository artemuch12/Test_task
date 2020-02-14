#Компилятор
CC := gcc
#Флаги для компиляции
FLAGS := -c
FLAGS_FIN := -pthread -o
LIBRARIS := -L. -lfunctions_strings -lfunctions_rbtree
#Исходные файлы
SRC := server.c
LIBRARIS_RBTREE := functions_rbtree.c
LIBRARIS_STRING := functions_strings.c

all: server
server: server.o libraris
	$(CC) server.o $(FLAGS_FIN) $@ $(LIBRARIS)
server.o: $(SRC)
	$(CC) $(SRC) $(FLAGS)
libraris: $(LIBRARIS_STRING) $(LIBRARIS_RBTREE)
	$(CC) $(LIBRARIS_STRING) $(FLAGS)
	ar -rc libfunctions_strings.a functions_strings.o
	$(CC) $(LIBRARIS_RBTREE) $(FLAGS)
	ar -rc libfunctions_rbtree.a functions_rbtree.o

#Удаление объектных файлов и библиотек
clean_obj:
	rm *.o
	rm *.a
#Удаление исполняемого файла
clean_elf:
	rm server
#Корректировка стиля исходных файлов
astyle:
	astyle -K --style=allman server.c
	astyle -K --style=allman functions_strings.c
	astyle -K --style=allman functions_rbtree.c
	rm *.orig
