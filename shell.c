#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<errno.h>
#include<pwd.h>
#include<sys/stat.h>
#include "read.h"
#include "execute.h"
#include "built.h"
#include "pipeline.h"
char main_dir[1000];

int func_cd(char **argument)
{
    //If command is cd
    if(argument[1]==NULL)
        fprintf(stderr,"Expected argument to \"cd\" not found\n");
    else if(strcmp(argument[1],"~")==0)
    {
        int flag;
        flag = chdir(main_dir);
        if(flag)
            fprintf(stderr,"Could not change directory\n");
    }
    else
    {
        int flag;
        flag = chdir(argument[1]);
        if(flag!=0)
        {
            fprintf(stderr,"Could not change directory\n");
        }
    }
    return 1;

}
int subsequence(char str1[],char str2[], int m, int n)
{
    //Checking if current directory is a child of the executing directory
    if(m==0)
    {
        return 1;
    }
    if(n==0)
    {
        return 0;
    }

    if(str1[m-1] == str2[n-1])
        return subsequence(str1, str2, m-1, n-1);
    else
        return subsequence(str1, str2, m, n-1);
}

int main(int argc, char **argv)
{
    //storing the 'executing directory' in main_dir
    getcwd(main_dir, sizeof(main_dir));
    init_array();
    int flag;
    do
    {
        //Getting username
        struct passwd *p;
        p = getpwuid(getuid());
        //Getting hostname
        char hostname[1024];
        gethostname(hostname,1023);
        //Getting current working directory
        char cwd[1024];
        getcwd(cwd,sizeof(cwd));
        

        if(strcmp(cwd,main_dir)==0)
        {
            //If current working directory is same as executing directory
            printf("<%s@%s:~>",p->pw_name,hostname);
        }
        else if(subsequence(main_dir,cwd,strlen(main_dir),strlen(cwd)))
        {
            //If current working directory is child directory of executing directory
            printf("<%s@%s:~",p->pw_name,hostname);
            int i;
            int m,n;
            m=strlen(main_dir);
            n=strlen(cwd);
            for(i=m;i<n;i++)
            {
                printf("%c",cwd[i]);
            }
            printf(">");
        }
        else
        {
            //If out of executing directory
            printf("<%s@%s:%s>",p->pw_name,hostname,cwd);
        }

        //Function to get command from user
        int out=0;
        char *command;
        command=read_command();
        /*if(command==NULL)
          {
          out=1;
          flag=0;
          }*/
        //printf("%c\n",command[0]);
        if(command[0] - 'x' == 0)
        {
            printf("\n");
            out=1;
        }
        if(out==0)
        {
            //Executing command given by user
            flag = tokenize(command);
            free(command);
        }
        else
            flag=0;

    }while(flag);
    
    return 0;
}
