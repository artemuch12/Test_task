/*Файл содержащий реализацию всех функций используемых в сервере*/
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
/*Функция поворота налево*/
struct rbtree *left_rotate(struct rbtree *root, struct rbtree *node)
{
  struct rbtree *right = node->right;
  node->right = right->left;
  if (right->left != null_node)
  {
    right->left->parent = node;
  }
  if (right != null_node)
  {
    right->parent = node->parent;
  }
  if (node->parent != null_node)
  {
    if (node == node->parent->left)
    {
      node->parent->left = right;
    }
    else
    {
      node->parent->right = right;
    }
  }
  else
  {
    root = right;
  }
  right->left = node;
  if(node != null_node)
  {
    node->parent = right;
  }
  return root;
}
/*Функция поворота направо*/
struct rbtree *right_rotate(struct rbtree *root, struct rbtree *node)
{
  struct rbtree *left = node->left;
  node->left = left->right;
  if (left->right != null_node)
  {
    left->right->parent = node;
  }
  if (left != null_node)
  {
    left->parent = node->parent;
  }
  if (node->parent != null_node)
  {
    if (node == node->parent->right)
    {
      node->parent->right = left;
    }
    else
    {
      node->parent->left = left;
    }
  }
  else
  {
    root = left;
  }
  left->right = node;
  if(node != null_node)
  {
    node->parent = left;
  }
  return root;
}
/*Функция вствавки в красно-черное дерево нового узла*/
struct rbtree *rbtree_adding(struct rbtree *root, char *keys, int data)
{
  struct rbtree *node = null_node;
  struct rbtree *parent = null_node;

  for(node = root; (node != null_node) && (node != NULL); )
  {
    parent = node;
    if(0 < strcmp(keys, node->key))
    {
      node = node->left;
    }
    else if(0 > strcmp(keys, node->key))
    {
      node = node->right;
    }
    else if(0 == strcmp(keys, node->key)) //Если ключ узла совпадает с искомым
    {                                    //ключом, то его поле дата изменяется,
      node->data = data;                 //на то значение которое передовалось в
      root = node;                       //функцию.
      return root;
    }
    else
    {
      return root;
    }
  }
  node = malloc(sizeof(*node));
  if(node == NULL)
  {
    return NULL;
  }
  node->key = keys;
  node->data = data;
  node->color = RED;
  node->parent = parent;
  node->left = null_node;
  node->right = null_node;
  if(parent != null_node)
  {
    if(parent != null_node)
    {
      parent->left = node;
    }
    else
    {
      parent->right = node;
    }
  }
  else
  {
    root = node;
  }
  return rbtree_fix_add(root, node);
}
/*Функция востановления свойств красно-черного дерева после установки нового
узла*/
struct rbtree *rbtree_fix_add(struct rbtree *root, struct rbtree *node)
{
  struct rbtree *uncle;
  while ((node != root) && (node->parent->color == RED))
  {
    if(node->parent == node->parent->parent->left)
    {
      uncle = node->parent->parent->right;
      if(uncle->color == RED)
      {
        node->parent->color = BLACK;
        uncle->color = BLACK;
        node->parent->parent->color = RED;
        node = node->parent->parent;
      }
      else
      {
        if(node == node->parent->right)
        {
          node = node->parent;
          root = left_rotate(root, node);
        }
        node->parent->color = BLACK;
        node->parent->parent->color = RED;
        root = right_rotate(root, node->parent->parent);
      }
    }
    else
    {
      uncle = node->parent->parent->left;
      if(uncle->color == RED)
      {
        node->parent->color = BLACK;
        uncle->color = BLACK;
        node->parent->parent->color = RED;
        node = node->parent->parent;
      }
      else
      {
        if(node == node->parent->left)
        {
          node = node->parent;
          root = right_rotate(root, node);
        }
        node->parent->color = BLACK;
        node->parent->parent->color = RED;
        root = left_rotate(root, node->parent->parent);
      }
    }
  }
  root->color = BLACK;
  return root;
}
struct rbtree *rbtree_search(struct rbtree *root, char *keys)
{
  struct rbtree *node = null_node;
  struct rbtree *parent = null_node;

  for(node = root; (node != null_node) && (node != NULL); )
  {
    parent = node;
    if(0 < strcmp(keys, node->key))
    {
      node = node->left;
    }
    else if(0 > strcmp(keys, node->key))
    {
      node = node->right;
    }
    else if(0 == strcmp(keys, node->key))
    {
      return node;
    }
    else
    {
      return root;
    }
  }
}
