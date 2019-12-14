/*Программа-сервер на UDP сокете. Временно: принимает строку от одного клиента
разделяет её на составляющие. Если лексемы удовлетворяют ряду критериев, то
клиент отсылается сообщение OK, иначе клиенту ничего не передается.*/
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <sys/socket.h>
#include <sys/types.h>

struct message
{
    char string[255];
};
struct message mess_in;
struct message mess_out;

int file_descript;

void check_tokens(char **);
void string_tokens(char *, char **, int *);
void handler_sigterm(int, siginfo_t *, void *);

int main(int argc, char const *argv[])
{
    struct sigaction signal_sigtrem;
    struct sockaddr_in client;
    struct sockaddr_in server;
    int count_token;
    int i;
    char *tokens[3];
    socklen_t addr_in_size = sizeof(struct sockaddr_in);

    signal_sigtrem.sa_flags = 0;
    sigemptyset(&signal_sigtrem.sa_mask);
    signal_sigtrem.sa_sigaction = handler_sigterm;
    sigaction(SIGTERM, &signal_sigtrem, NULL);

    server.sin_family = AF_INET;
    server.sin_port = htons(8888);
    inet_aton("127.0.0.1", &server.sin_addr);
    file_descript = socket(AF_INET, SOCK_DGRAM, 0);
    bind(file_descript, (struct sockaddr *)&server, addr_in_size);

    strcpy(mess_out.string, "OK");
    while(1)
    {
        recvfrom(file_descript, &mess_in, sizeof(mess_in), 0, (struct sockaddr *) &client, &addr_in_size);
        string_tokens(mess_in.string, tokens, &count_token);
        check_tokens(tokens);
        if((tokens[0] != NULL) && (count_token > 2))
        {
            for(i = 0; i < count_token-1; i++)
            {
                printf("%s\n", tokens[i]);
            }
            sendto(file_descript, &mess_out, sizeof(mess_out), 0, (struct sockaddr *) &client, addr_in_size);
        }
    }
    return 0;
}
/*Функция разделяющая приходящую строку на лексемы (tokens). Если лексем больше
трех, то обработаны будут только первые 3. За основу используетя потокобезопас-
ная strtok_r. */
void string_tokens(char *string, char **tokens, int *count_token)
{
    int i;
    char *savestr;
    tokens[0] = strtok_r(string, ", ", &savestr);
    *count_token = 1;
    for(i = 1; (i < 4) && (tokens[i-1] != NULL); i++)
    {
        tokens[i] = strtok_r(NULL, ", ", &savestr);
        *count_token = *count_token + 1;
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
            if(tokens[2][lenght_token-1] == '\n')
            {
                lenght_token = lenght_token - 1;
            }
            for(i = 0; i < lenght_token; i++)
            {
                count_alfbet = 0;
                for(j = 0; j < 10; j++)
                {
                    if(alfbet[j] != tokens[2][i])
                    {
                        count_alfbet = count_alfbet + 1;
                    }
                }
                if(count_alfbet == 10)
                {
                    tokens[0] = NULL;
                }
            }
        }
        else
        {
            tokens[0] = NULL;
        }
    }
    else if(0 != strcmp(tokens[0], "GET"))
    {
        tokens[0] = NULL;
    }
}
/*Обработчик сигнала SIGTERM. Если приходит данный сигнал, сервер инициализирует
свое отключение.*/
void handler_sigterm(int sig, siginfo_t *si, void *unused)
{
    printf("SIGTERM!");

}
