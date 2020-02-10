/*Файл содержащий реализацию всех функций используемых в сервере*/
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/functions_rbtree.h"




/*Функция поворота налево*/
struct rbtree *left_rotate(struct rbtree *root, struct rbtree *x)
{
  if((x->right != null_node) && (x->right != NULL))
  {
    struct rbtree *y = NULL;
    y = x->right;
    x->right = y->left;
    if(y->left != null_node)
    {
      y->left->parent = x;
    }
    y->parent = x->parent;
    if(x->parent == null_node)
    {
      root = y;
    }
    else if(x == x->parent->left)
    {
      x->parent->left = y;
    }
    else
    {
      x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
    return root;
  }
}
/*Функция поворота направо*/
struct rbtree *right_rotate(struct rbtree *root, struct rbtree *x)
{
  if((x->left != null_node) && (x->left != NULL))
  {
    struct rbtree *y = NULL;
    y = x->left;
    x->left = y->right;
    if(y->right != null_node)
    {
      y->right->parent = x;
    }
    y->parent = x->parent;
    if(x->parent == null_node)
    {
      root = y;
    }
    else if(x == x->parent->right)
    {
      x->parent->right = y;
    }
    else
    {
      x->parent->left = y;
    }
    y->right = x;
    x->parent = y;
    return root;
  }
}
/*Функция вствавки в красно-черное дерево нового узла*/
struct rbtree *rbtree_adding(struct rbtree *root, char *keys, int data)
{
  struct rbtree *z = NULL;
  struct rbtree *y = NULL;
  struct rbtree *x = NULL;
  y = null_node;
  x = root;
  while ((x != null_node) && (x != NULL))
  {
    y = x;
    if(0 < string_compreson(keys, x->key))
    {
      x = x->left;
    }
    else if(0 > string_compreson(keys, x->key))
    {
      x = x->right;
    }
    else
    {
      x->data = data;
      return root;
    }
  }
  z = malloc(sizeof(*z));
  z->parent = y;
  strcpy(z->key, keys);
  z->data = data;
  z->left = null_node;
  z->right = null_node;
  z->color = RED;
  if((y == null_node) || (y == NULL))
  {
    root = z;
  }
  else if(0 < string_compreson(z->key, y->key))
  {
    y->left = z;
  }
  else
  {
    y->right = z;
  }

  //return root;
  return rbtree_fix_add(root, z);
}
/*Функция востановления свойств красно-черного дерева после установки нового
узла*/
struct rbtree *rbtree_fix_add(struct rbtree *root, struct rbtree *z)
{
  struct rbtree *y;
  while ((z->parent->color == RED) && (root != z))
  {
    if(z->parent == z->parent->parent->left)
    {
      y = z->parent->parent->right;
      if(y->color == RED)
      {
        z->parent->color = BLACK;
        y->color = BLACK;
        z->parent->parent->color = RED;
        z = z->parent->parent;
      }
      else
      {
        if(z == z->parent->right)
        {
          z = z->parent;
          root = left_rotate(root, z);
        }
        z->parent->color = BLACK;
        z->parent->parent->color = RED;
        root = right_rotate(root, z->parent->parent);
      }
    }
    else
    {
      y = z->parent->parent->left;
      if(y->color == RED)     /**/
      {
        z->parent->color = BLACK;
        y->color = BLACK;
        z->parent->parent->color = RED;
        z = z->parent->parent;
      }
      else
      {
        if(z == z->parent->left)
        {
          z = z->parent;
          root = right_rotate(root, z);
        }
        z->parent->color = BLACK;
        z->parent->parent->color = RED;
        root = left_rotate(root, z->parent->parent);
      }
    }

  }
  root->color = BLACK;
  return root;
}

/*Функция поиска узла по ключу*/
/*Если ключ совпадает с ключом узла, то */
struct rbtree *rbtree_search(struct rbtree *root, char *keys)
{
    struct rbtree *node = NULL;
    for(node = root; (node != null_node) && (node != NULL); )
    {
        if(0 < string_compreson(keys, node->key))
        {
            node = node->left;
        }
        else if(0 > string_compreson(keys, node->key))
        {
            node = node->right;
        }
        else if(0 == string_compreson(keys, node->key))
        {
            return node;
        }
    }
    return NULL;
}

/*Функция удаления дерева*/
/*Если указатель (получаемый на входе функции) не пуст, то проверяется на не
пустоту левый и правый потомки узла. Если один из них не пуст, то его указатель
передается рекурсивно-вызываемую функцию. После того кого, как удаляются листья,
то высвобождается память из раннее вызванного узла.*/
void rbtree_delete(struct rbtree *root)
{
    if((root != NULL) && (root != null_node))
    {
        rbtree_delete(root->left);
        rbtree_delete(root->right);
        free(root);
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
      for(counter = 0; counter < len_str_1; counter++)
      {
          symbol_str_1 = (int )string_1[counter];
          symbol_str_2 = (int )string_2[counter];
          if (symbol_str_1 < symbol_str_2)
          {
              return 1;
          }
          if (symbol_str_1 > symbol_str_2)
          {
              return -1;
          }
      }
      return 1;
    }
    if (len_str_1 > len_str_2)
    {
      for(counter = 0; counter < len_str_2; counter++)
      {
          symbol_str_1 = (int )string_1[counter];
          symbol_str_2 = (int )string_2[counter];
          if (symbol_str_1 < symbol_str_2)
          {
              return 1;
          }
          if (symbol_str_1 > symbol_str_2)
          {
              return -1;
          }
      }
      return -1;
    }
    if (len_str_1 == len_str_2)
    {
      for(counter = 0; counter < len_str_1; counter++)
      {
          symbol_str_1 = (int )string_1[counter];
          symbol_str_2 = (int )string_2[counter];
          if (symbol_str_1 < symbol_str_2)
          {
              return 1;
          }
          if (symbol_str_1 > symbol_str_2)
          {
              return -1;
          }
      }
      return 0;
    }
}
