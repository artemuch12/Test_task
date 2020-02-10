/*Заголовочный файл содержащий прототипы всех вызываемых функций*/
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct message
{
  char string[255];
};

/*Прототипы функций обработки строк*/
int string_counter_token(char *);
char **string_checking(char *, int );
