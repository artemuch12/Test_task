/*Исходный файл содержащий реализацию функция отвечающих за обработку строк на
сервере.*/
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/functions_strings.h"


int string_counter_token(char *string)
{
  int counter_tokens;
  int counter;

  for(counter = 0, counter_tokens = 0; (string[counter] != '\n'); counter++)
  {
    if(string[counter] == ' ')
    {
      counter_tokens = counter_tokens + 1;
    }
  }
  return (counter_tokens + 1);
}


char **string_checking(char *string, int count_tokens)
{
  char **lexem = NULL;
  int counter;
  int inner_counter;
  int counter_tokens;

  lexem = (char **)malloc(counter_tokens*sizeof(char *));
  for(counter_tokens = 0; counter_tokens < count_tokens; counter_tokens++)
  {
    lexem[counter_tokens] = (char *)malloc(sizeof(char));
  }
  counter_tokens = 0;
  for(counter = 0, inner_counter = 0; (string[counter] != '\n'); counter++)
  {
    if(string[counter] != ' ')
    {
      lexem[counter_tokens] = (char *)realloc(lexem[counter_tokens],
        sizeof(char)*(inner_counter+1));
      lexem[counter_tokens][inner_counter] = string[counter];
      inner_counter = inner_counter + 1;
      lexem[counter_tokens][inner_counter] = '\0';
    }
    else
    {
      inner_counter = inner_counter + 1;
      lexem[counter_tokens][inner_counter] = '\n';
      inner_counter = 0;
      counter_tokens = counter_tokens + 1;
    }
  }
  inner_counter = inner_counter + 1;
  lexem[counter_tokens][inner_counter] = '\n';
  return lexem;
}
