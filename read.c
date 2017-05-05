#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<errno.h>
#include<pwd.h>
#include<sys/stat.h>

void sighandle(int num);
char *read_command() 
{
    int size=1024;
    int out=0;

   // char array[1024];
    char *array=malloc(sizeof(char)*1024);
    signal(SIGINT, sighandle);
    fgets(array, 1024, stdin);
    int len;
    len=strlen(array);
    return array;
}

void sighandle(int num)
{
    signal(SIGINT, sighandle);
    printf("\n");
    fflush(stdout);
}

