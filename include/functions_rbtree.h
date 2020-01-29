/*Заголовочный файл содержащий прототипы всех вызываемых функций*/
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*Константы связанные с ошибками*/
#define MAX_CLIENT_QUEUE 2
#define ERR_SOCKET 1
#define ERR_MALLOC 2
#define ERR_REALLOC 4
#define ERR_CANCEL 8
#define ERR_JOIN 16
#define ERR_CLOSE 32
#define ERR_RECV 64
#define ERR_SEND 128

/*Цвета дерева*/
#define RED 0
#define BLACK 1

struct rbtree
{
  char *key;
  int data;
  int color;
  struct rbtree *parent;
  struct rbtree *left;
  struct rbtree *right;
};
static struct rbtree empty_node = {0, 0, BLACK, NULL, NULL, NULL};
static struct rbtree *null_node = &empty_node;

/*Прототипы функция для работы красно-черного дерева*/
struct rbtree *left_rotate(struct rbtree *, struct rbtree *);
struct rbtree *right_rotate(struct rbtree *, struct rbtree *);
struct rbtree *rbtree_adding(struct rbtree *, char *, int );
struct rbtree *rbtree_fix_add(struct rbtree *, struct rbtree *);
struct rbtree *rbtree_search(struct rbtree *, char *);
void rbtree_delete(struct rbtree *);
