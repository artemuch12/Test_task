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
#include "include/functions_strings.h"
#include "include/functions_rbtree.h"


struct rbtree *tree = NULL;
struct rbtree *node = NULL;

int flag = 0;

void handler_sigterm(int, siginfo_t *, void *);
void rbtree_print(struct rbtree *, int );

int main(int argc, char const *argv[])
{
    int err;
    struct sigaction signal_sigtrem;
    int i;
    char keys[255];
    int value;
    char string[20];
    socklen_t addr_in_size = sizeof(struct sockaddr_in);

    signal_sigtrem.sa_flags = 0;
    sigemptyset(&signal_sigtrem.sa_mask);
    signal_sigtrem.sa_sigaction = handler_sigterm;
    sigaction(SIGTERM, &signal_sigtrem, NULL);

    while(flag != 1)
    {
      puts("Введите команду:");
      fgets(string, 4, stdin);
      if(0 == strncmp(string, "set\0", 4))
      {
        puts("Введите ключ:");
        scanf("%s", keys);
        getchar();
        puts("Введите данные:");
        scanf("%d", &value);
        tree = rbtree_adding(tree, keys, value);
        if(tree != NULL)
        {
          puts("OK");
          getchar();
        }
      }
      else if(0 == strncmp(string, "get\0", 4))
      {
        puts("Введите ключ:");
        fgets(keys, 255, stdin);
        node = rbtree_search(tree, keys);
        if(node != NULL)
        {
          printf("Данные по ключу %s: %d\n", node->key, node->data);
          getchar();
        }
        else
        {
            puts("Problem!");
            getchar();
        }
      }
      else if(0 == strncmp(string, "prn\0", 4))
      {
        rbtree_print(tree, 1);
        getchar();
      }
      else if(0 == strncmp(string, "qui\0", 4))
      {
        rbtree_delete(tree);
        flag = 1;
      }
      else
      {
        puts("Неверная команда!");
      }
    }



    return 0;
}
/*Обработчик сигнала SIGTERM. Если приходит данный сигнал, сервер инициализирует
свое отключение.*/
void handler_sigterm(int sig, siginfo_t *si, void *unused)
{
    printf("SIGTERM!");
    flag = 1;
}
void rbtree_print(struct rbtree *root, int counter)
{
  struct rbtree *node = root;
  if ((node != null_node) && (node != NULL))
  {
    printf("Уровень: %d; Узел %s; Значение %d; ", counter, node->key,
        node->data);
    if(node->color == 1)
    {
      printf("цвет узла: черный\n");
    }
    else
    {
      printf("цвет узла: красный\n");
    }
    counter = counter + 1;
    rbtree_print(node->left, counter);
    rbtree_print(node->right, counter);
  }
}
