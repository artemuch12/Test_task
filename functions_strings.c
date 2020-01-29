/*Исходный файл содержащий реализацию функция отвечающих за обработку строк на
сервере.*/
#include <malloc.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
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

/*Функция сравнивает две строки, для определения их соответсвия между ними. На
первом этапе сравнивается длина строк. Если одна строк длиннее другой, то на
выход функции отправляется число: -1 если "левая" строка больше "правой", в об-
ратном случае отпраляется 1. В случае если обе строки равны по длине, то насту-
пает следующий этап: посимвольное сравнение. Если какой-то либо символ, оказы-
вается больше, чем в другой строке в аналогичной позиции, то функция возвращает
единицу, если меньше, то -1. Если после перебора всех символов не находится от-
личных сиволов, между двумя строками, то функция возвращает 0.*/
/*Входные данные: 2 строки, заканчивающиеся на символы'/0' и '/n'.*/
/*Выходные данные: число из ряда: -1, 0, 1*/
int string_compreson(char *string_1, char *string_2)
{
    int len_str_1, len_str_2;
    int out_data;
    int counter;
    int symbol_str_1, symbol_str_2;

    len_str_1 = strlen(string_1);
    len_str_2 = strlen(string_2);

    if (len_str_1 < len_str_2)
    {
        return -1;
    }
    if (len_str_1 > len_str_2)
    {
        return 1;
    }
    if (len_str_1 = len_str_2)
    {
        for(counter = 0; counter < len_str_1; counter++)
        {
            symbol_str_1 = (int )string_1[counter];
            symbol_str_2 = (int )string_2[counter];
            if (symbol_str_1 < symbol_str_2)
            {
                return -1;
            }
            if (symbol_str_1 > symbol_str_2)
            {
                return 1;
            }
        }
        return 0;
    }
}
