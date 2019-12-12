/*Программа-сервер на UDP сокете. Временно: принимает строку от одного клиента
разделяет её на составляющие, и отсылает ответ кодходящее кол-ство составляющих
или нет.*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
struct message mess;

char *string_tokens();

int main(int argc, char const *argv[])
{
  struct sockaddr_in client;
  struct sockaddr_in server;
  int file_descript;
  char *tokens[3];
  socklen_t addr_in_size = sizeof(struct sockaddr_in);


  server.sin_family = AF_INET;
  server.sin_port = htons(8888);
  inet_aton("127.0.0.1", &server.sin_addr);
  file_descript = socket(AF_INET, SOCK_DGRAM, 0);
  bind(file_descript, (struct sockaddr *)&server, addr_in_size);
  while(1)
  {
    recvfrom(file_descript, &mess, sizeof(mess), 0, (struct sockaddr *) &client, &addr_in_size);
    //string_tokens(mess.string);
    printf("%s\n", mess.string);
    strcpy(mess.string, "OK");
    sendto(file_descript, &mess, sizeof(mess), 0, (struct sockaddr *) &client, addr_in_size);
  }
  return 0;
}
/*Функция разделяющая приходящую строку на лексемы (tokens). Если лексем 2 или
3 лексемы, то функция вернет их в основную функцию, если в строке содержалась 1
или больше 3 лексем, то функция ничего не должна будет вернуть. За основу будет
использоваться потокобезопасная strtok_r. */
char *string_tokens(char *string, char **tokens)
{
  char *savestr;
  int i;
  tokens[0] = strtok_r(string, ",", &savestr);
  for(i = 1; )
  {
    tokens[i] = strtok_r(NULL, ",", &savestr);
    for()

  }
}
