#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<errno.h>
#include<pwd.h>
#include<sys/stat.h>

int func_pwd(char **argument)
{
    //If command is pwd
    char cwd[1024];
    getcwd(cwd,sizeof(cwd));
    printf("%s\n",cwd);
    return 1;
}


int func_echo(char **argument)
{
    //If command is echo
    if(argument[1]==NULL)
        printf("Expected argument to \"echo\" not found\n");
    else
    {
        int i=1;
        while(argument[i]!=NULL)
        {
            printf("%s ",argument[i]);
            i++;
        }
        printf("\n");
    }
    return 1;
}

int func_exit(char **argument)
{
    //If command is exit
    return 0;
}

int func_pinfo(char **argument)
{
    char str[1024];
    char proc[1024]="/proc/";
    char status[1024]="/status";
    strcat(proc,argument[1]);
    strcat(proc,status);
    FILE *fd;
    if((fd=fopen(proc,"r"))==NULL)
    {
        fprintf(stderr, "Unable to open file\n");
        return 1;
    }
    int count=1;
    while(fgets(str,60,fd)!=NULL)
    {
        if(count==1 || count==2 || count == 5 || count == 17)
            printf("%s",str);
        count++;
    }
    fclose(fd);
    return 1;

}
int jobs();
int func_listjobs()
{
    jobs();
    return 1;
}
int kjob(char **argument);
int func_kjob(char **argument)
{
    kjob(argument);
    return 1;
}
int killall();
int func_killall()
{
    killall();
    return 1;
}
int fg(char **argument);
int func_fg(char **argument)
{
    fg(argument);
    return 1;
}
