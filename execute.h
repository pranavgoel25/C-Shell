#ifndef __FUN__
#define __FUN
#include<stdio.h>

void init_array();
int execcmd(char **argument);
int execute(char ** tokens);
int tokenize(char *command);
#endif
