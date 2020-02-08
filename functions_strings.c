/*Исходный файл содержащий реализацию функция отвечающих за обработку строк на
сервере.*/
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/functions_strings.h"


/*Функция разделяющая приходящую строку на лексемы (tokens). Если лексем больше
трех, то обработаны будут только первые 3. За основу используетя потокобезопас-
ная strtok_r.
Для работы функции необходимо 3 входных параметра: строка полученная от клиента,
массив в который будут передоваться разделенные лексемы, и указатель на перемен-
ную в которой будет хранится количество лексем. Выходных параметров нет.*/
void string_tokens(char *string, char **tokens)
{
    int i;
    char *savestr;
    tokens[0] = strtok_r(string, ", ", &savestr);
    for(i = 1; (i < 3) && (tokens[i-1] != NULL); i++)
    {
        tokens[i] = strtok_r(NULL, ", ", &savestr);
    }
}

/*Функция обрабатывающая лексемы.
1. Функция рассматривает 1-ую лексему на то, что она является SET либо GET. Если
лексема не соответсвует данному критерию, то она указатель на неё заNULLяется.
2. Функция рассматривает 3-ию лексему (в случае SET) если она содержит не цифро-
вые символы заNULLяется указатель на первую лексему.*/
void check_tokens(char **tokens)
{
    int i;
    int j;
    int count_alfbet;
    int lenght_token;
    char alfbet[10] = {"0123456789"};
    if(0 == strcmp(tokens[0], "SET"))
    {
        if((NULL != tokens[2]) && (NULL != tokens[1]))
        {
            lenght_token = strlen(tokens[2]);
            for(i = 0; i < lenght_token; i++)
            {
                count_alfbet = 0;
                for(j = 0; j < 10; j++)
                {
                    if(alfbet[j] != tokens[2][i])
                    {

                        count_alfbet = count_alfbet + 1;
                    }
                    if('\n' == tokens[2][i])
                    {
                        count_alfbet = count_alfbet - 1;
                    }
                }
                if(count_alfbet == 10)
                {
                    tokens[0][0] = 1;
                }
            }
        }
        else
        {
            tokens[0][0] = 1;
        }
    }
    else if(0 != strcmp(tokens[0], "GET"))
    {
        tokens[0][0] = 1;
    }
}
