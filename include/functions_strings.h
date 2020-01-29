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
void check_tokens(char **);
void string_tokens(char *, char **);
int string_compreson(char *, char *);
