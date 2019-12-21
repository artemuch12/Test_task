/*Программа-сервер на TCP сокете. При подключении нового клиента открывает отде-
льный поток для обработки его запросов. В качестве запросов со стороны клиента
выступает строка, не превышающая 255 символов. При обработке запроса строка де-
лится на лексемы (или токены). Если лексемы удовлетворяют ряду критериев, то
клиент отсылается сообщение OK, иначе клиенту ничего не передается.
Для работы сервера не требуется входных параметров.*/
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


#define MAX_CLIENT_QUEUE 2
#define ERR_SOCKET 1
#define ERR_MALLOC 2
#define ERR_REALLOC 4
#define ERR_CANCEL 8
#define ERR_JOIN 16
#define ERR_CLOSE 32

struct message
{
    char string[255];
};
struct message mess_in;
struct message mess_out;
struct sockaddr_in server;

pthread_t *sock_recv = NULL;

int file_descript;
int flag = 0;
int *pth_file_descript = NULL;

void check_tokens(char **);
void string_tokens(char *, char **);
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
        puts("Realloc, sock_recv");
        exit(ERR_REALLOC);
      }
      number_pthread = realloc(number_pthread, (counter_pthread+1)*sizeof(int));
      if(number_pthread == NULL)
      {
        puts("Realloc, number_pthread");
        exit(ERR_REALLOC);
      }

      pth_file_descript = realloc(pth_file_descript, (counter_pthread+1)*sizeof(int));
      if(pth_file_descript == NULL)
      {
        puts("Realloc, pth_file_descript");
        exit(ERR_REALLOC);
      }
      number_pthread[counter_pthread] = counter_pthread;
    }
      for(i = 0; i < counter_pthread; i++)
      {
        err = pthread_cancel(sock_recv[i]);
        if(err != 0)
        {
          printf("Cancel %d\n", i);
          exit(ERR_CANCEL);
        }
        pthread_join(sock_recv[i], status);
        if(err != 0)
        {
          printf("Join %d\n", i);
          exit(ERR_JOIN);
        }
      }
    free(sock_recv);
    free(number_pthread);
    free(pth_file_descript);
    err = close(file_descript);
    if(err != 0)
    {
      printf("PTH_FL");
      exit(ERR_CLOSE);
    }
    return 0;
}
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
/*Обработчик сигнала SIGTERM. Если приходит данный сигнал, сервер инициализирует
свое отключение.*/
void handler_sigterm(int sig, siginfo_t *si, void *unused)
{
    printf("SIGTERM!");
    flag = 1;
}
/*Функция определяющая работу отдельного потока */
void *flow_clients(void *data_input)
{
  struct message mess_in;
  struct message mess_out;
  int *number_pthread;
  int i;
  int err;
  char *tokens[3] = {NULL, NULL, NULL};
  number_pthread = (int *)data_input;
  while(1)
  {
    err = recv(pth_file_descript[*number_pthread], &mess_in, sizeof(struct message), 0);
    if(err != -1)
    {
      printf("recv");
    }
    string_tokens(mess_in.string, tokens);
    if(tokens[0] != NULL)
    {
      check_tokens(tokens);
      if(tokens[0][0] != 1)
      {
        printf("Ok");
        strcpy(mess_out.string, "Ok");
        err = send(pth_file_descript[*number_pthread],  &mess_out, sizeof(struct message), 0);
        if(err != -1)
        {
          printf("recv");
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
