/*Программа-сервер на TCP сокете. При подключении нового клиента открывает отде-
льный поток для обработки его запросов. В качестве запросов со стороны клиента
выступает строка, не превышающая 255 символов. При обработке запроса строка де-
лится на лексемы (или токены). Если лексемы удовлетворяют ряду критериев, то
клиент отсылается сообщение OK, иначе клиенту ничего не передается.
Для работы сервера не требуется входных параметров.*/
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

struct message
{
    char string[255];
};
struct message mess_in;
struct message mess_out;
struct sockaddr_in server;

pthread_t *sock_recv;

int file_descript;
int flag = 0;
int *pth_file_descript;

void check_tokens(char **);
void string_tokens(char *, char **, int *);
void handler_sigterm(int, siginfo_t *, void *);
void *flow_clients(void *);


int main(int argc, char const *argv[])
{
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
      exit(-1);
    }
    bind(file_descript, (struct sockaddr *)&server, addr_in_size);
    listen(file_descript, MAX_CLIENT_QUEUE);
    sock_recv = (pthread_t *)malloc(1*sizeof(pthread_t));
    pth_file_descript = (int *)malloc(1*sizeof(int));
    number_pthread =  (int *)malloc(1*sizeof(int));
    number_pthread[counter_pthread] = counter_pthread;
    while(flag != 1)
    {

      pth_file_descript[counter_pthread] = accept(file_descript, NULL, NULL);

      pthread_create(&sock_recv[counter_pthread], NULL, flow_clients, &number_pthread[counter_pthread]);
      puts("Error.2");
      counter_pthread++;
      sock_recv = realloc(sock_recv, 1*sizeof(pthread_t));
      number_pthread = realloc(number_pthread, 1*sizeof(number_pthread));
      number_pthread[counter_pthread] = counter_pthread;
      pth_file_descript = realloc(pth_file_descript, 1*sizeof(int));
    }
    if(counter_pthread > 0)
    {
      for(i = 0; i < counter_pthread; i++)
      {
        pthread_cancel(pth_file_descript[i]);
        pthread_join(pth_file_descript[i], NULL);
      }
      for(i = 0; i < counter_pthread; i++)
      {
        close(pth_file_descript[i]);
      }
    }

    free(sock_recv);
    free(number_pthread);
    free(pth_file_descript);
    close(file_descript);
    return 0;
}
/*Функция разделяющая приходящую строку на лексемы (tokens). Если лексем больше
трех, то обработаны будут только первые 3. За основу используетя потокобезопас-
ная strtok_r.
Для работы функции необходимо 3 входных параметра: строка полученная от клиента,
массив в который будут передоваться разделенные лексемы, и указатель на перемен-
ную в которой будет хранится количество лексем. Выходных параметров нет.*/
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
    flag = 1;
}
/*Функция определяющая работу отдельного потока */
void *flow_clients(void *data_input)
{
  struct message mess_in;
  struct message mess_out;
  int *number_pthread;
  int count_token;
  int i;
  char *tokens[3];
  number_pthread = (int *)data_input;
  recv(pth_file_descript[*number_pthread], &mess_in, sizeof(mess_in), 0);
  string_tokens(mess_in.string, tokens, &count_token);
  check_tokens(tokens);
  if((tokens[0] != NULL) && (count_token > 2))
  {
    for(i = 0; i < count_token-1; i++)
    {
        printf("%s\n", tokens[i]);
    }
    strcpy(mess_out.string, "Ok");
    send(pth_file_descript[*number_pthread], &mess_out, sizeof(mess_out), 0);
    count_token = 0;
  }
  pthread_exit(0);
}
