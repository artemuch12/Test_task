/*Программа-сервер на TCP сокете. При подключении нового клиента открывает отде-
льный поток для обработки его запросов. В качестве запросов со стороны клиента
выступает строка, не превышающая 255 символов. При обработке запроса строка де-
лится на лексемы (или токены). Если лексемы удовлетворяют ряду критериев, то
клиент отсылается сообщение OK, иначе клиенту ничего не передается.
Для работы сервера не требуется входных параметров.*/
#include <errno.h>
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
#include "include/functions.h"

struct message mess_in;
struct message mess_out;
struct sockaddr_in server;

struct rbtree *tree = NULL;

pthread_t *sock_recv = NULL;
pthread_mutex_t border = PTHREAD_MUTEX_INITIALIZER;
int flag = 0;
int file_descript;
int *pth_file_descript = NULL;

void handler_sigterm(int, siginfo_t *, void *);
void *flow_clients(void *);

int main(int argc, char const *argv[])
{
    void **status;
    int err;
    struct sigaction signal_sigtrem;
    int i;
    int counter_pthread = 0;
    int *number_pthread = NULL;

    socklen_t addr_in_size = sizeof(struct sockaddr_in);

    signal_sigtrem.sa_flags = 0;
    sigemptyset(&signal_sigtrem.sa_mask);
    signal_sigtrem.sa_sigaction = handler_sigterm;
    sigaction(SIGTERM, &signal_sigtrem, NULL);

    server.sin_family = AF_INET;
    server.sin_port = htons(8888);
    inet_aton("127.0.0.1", &server.sin_addr);
    file_descript = socket(AF_INET, SOCK_STREAM, 0);
    if(file_descript == 0)
    {
        puts("Error: socket");
        exit(ERR_SOCKET);
    }
    bind(file_descript, (struct sockaddr *)&server, addr_in_size);
    listen(file_descript, MAX_CLIENT_QUEUE);
    sock_recv = (pthread_t *)calloc(1, sizeof(pthread_t));
    if(sock_recv == NULL)
    {
        puts("Malloc, sock_recv");
        exit(ERR_MALLOC);
    }
    pth_file_descript = (int *)calloc(1, sizeof(int));
    if(pth_file_descript == NULL)
    {
        puts("Malloc, number_pthread");
        exit(ERR_MALLOC);
    }
    number_pthread =  (int *)calloc(1, sizeof(int));
    if(number_pthread == NULL)
    {
        puts("Malloc, pth_file_descript");
        exit(ERR_MALLOC);
    }
    number_pthread[counter_pthread] = counter_pthread;

    pthread_mutex_init(&border, NULL);
    while(flag != 1)
    {

        pth_file_descript[counter_pthread] = accept(file_descript, NULL, NULL);
        if(pth_file_descript[counter_pthread] == 0)
        {
            puts("Error: socket");
            exit(ERR_SOCKET);
        }
        pthread_create(&sock_recv[counter_pthread], NULL, flow_clients, &number_pthread[counter_pthread]);
        counter_pthread++;
        sock_recv = realloc(sock_recv, (counter_pthread+1)*sizeof(pthread_t));
        if(sock_recv == NULL)
        {
            puts("Error: realloc");
            exit(ERR_REALLOC);
        }
        number_pthread = realloc(number_pthread, (counter_pthread+1)*sizeof(int));
        if(number_pthread == NULL)
        {
            puts("Error: realloc");
            exit(ERR_REALLOC);
        }

        pth_file_descript = realloc(pth_file_descript, (counter_pthread+1)*sizeof(int));
        if(pth_file_descript == NULL)
        {
            puts("Error: realloc");
            exit(ERR_REALLOC);
        }
        number_pthread[counter_pthread] = counter_pthread;
    }
    for(i = 0; i < counter_pthread; i++)
    {
        err = pthread_cancel(sock_recv[i]);
        if(err != 0)
        {
            puts("Error: cancel");
            exit(ERR_CANCEL);
        }
        pthread_join(sock_recv[i], status);
        if(err != 0)
        {
            puts("Error: join");
            exit(ERR_JOIN);
        }
    }
    free(sock_recv);
    free(number_pthread);
    free(pth_file_descript);
    pthread_mutex_destroy(&border);
    err = close(file_descript);
    if(err != 0)
    {
        puts("Error: pthread file descript close");
        exit(ERR_CLOSE);
    }
    return 0;
}

/*Функция определяющая работу отдельного потока */
void *flow_clients(void *data_input)
{
    struct message mess_in;
    struct message mess_out;
    struct rbtree *node;
    int *number_pthread;
    int time_variable;
    int i;
    int flag_search = 0;
    int err;
    char *tokens[3] = {NULL, NULL, NULL};
    number_pthread = (int *)data_input;
    while(1)
    {
        err = recv(pth_file_descript[*number_pthread], &mess_in, sizeof(struct message), 0);
        if(err != -1)
        {
            perror("Error:");
            printf("Error: recv\n");
            exit(ERR_RECV);
        }
        string_tokens(mess_in.string, tokens);
        if(tokens[0] != NULL)
        {
            check_tokens(tokens);
            if(tokens[0][0] != 1)
            {
                if(strcmp(tokens[0], "SET") == 0)
                {
                    pthread_mutex_lock(&border);
                    node = rbtree_adding(tree, tokens[1], atoi(tokens[2]));
                    pthread_mutex_unlock(&border);
                    if(node != NULL)
                    {
                      strcpy(mess_out.string, "Ok");
                      err = send(pth_file_descript[*number_pthread],  &mess_out, sizeof(struct message), 0);
                      if(err != -1)
                      {
                          printf("Error: send\n");
                          exit(ERR_SEND);
                      }
                    }
                }
                else if(strcmp(tokens[0], "GET") == 0)
                {
                    pthread_mutex_lock(&border);
                    node = rbtree_search(tree, tokens[1]);
                    if(node != NULL)
                    {
                      sprintf(mess_out.string, "%d", node->data);
                    }
                    pthread_mutex_unlock(&border);
                }
                if(node != NULL)
                {
                  err = send(pth_file_descript[*number_pthread],  &mess_out, sizeof(struct message), 0);
                  if(err != -1)
                  {
                      printf("Error: send\n");
                  }
                }
            }
        }
    }
    err = close(pth_file_descript[*number_pthread]);
    if(err != 0)
    {
        printf("PTH_FL %d\n", *number_pthread);
        exit(-1);
    }
    for(i = 0; i < 3; i++)
    {
        free(tokens[i]);
    }
    free(tokens);

    pthread_exit(0);
}

/*Обработчик сигнала SIGTERM. Если приходит данный сигнал, сервер инициализирует
свое отключение.*/
void handler_sigterm(int sig, siginfo_t *si, void *unused)
{
    printf("SIGTERM!");
    flag = 1;
}
