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
#include "include/functions_rbtree.h"
#include "include/functions_strings.h"



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
    int lenght;
    struct rbtree *node = null_node;
    struct rbtree *parent = null_node;

    for(node = root; (node != null_node) && (node != NULL); )
    {
        parent = node;
        if(0 < string_compreson(keys, node->key))
        {
            node = node->left;
        }
        else if(0 > string_compreson(keys, node->key))
        {
            node = node->right;
        }
        else if(0 == string_compreson(keys, node->key)) //Если ключ узла совпадает с искомым
        {
            //ключом, то его поле дата изменяется,
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
    lenght = strlen(keys);
    node->key = malloc(sizeof(char)*lenght);
    strcpy(node->key, keys);
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

/*Функция поиска узла по ключу*/
/*Если ключ совпадает с ключом узла, то */
struct rbtree *rbtree_search(struct rbtree *root, char *keys)
{
    struct rbtree *node = null_node;
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
        if((root->left != NULL) && (root->left != null_node))
        {
            rbtree_delete(root->left);
        }
        if((root->right != NULL) && (root->right != null_node))
        {
            rbtree_delete(root->right);
        }
        free(root);
    }
}
