#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<errno.h>
#include<pwd.h>
#include<signal.h>
#include<sys/stat.h>
#include "built.h"
#include "pipeline.h"
char * temp;
char *builtin_funcs[] = {"cd","pwd","echo","quit","pinfo", "jobs", "kjob","killall", "fg"};

int (*builtinfunc[])(char **) = {&func_cd, &func_pwd, &func_echo,&func_exit,&func_pinfo, &func_listjobs, &func_kjob, &func_killall, &func_fg};

//Struct to store the background processes
typedef struct BgProcess
{
    pid_t pid;
    char* name;
    int status;
    int flag;
}bgprocess;

bgprocess *array;
int numjob=0;

//Initialise the background array
int init_array()
{
    array = malloc(sizeof(bgprocess)* 512);
}

//Function to execute the command 'jobs'
int jobs()
{
    int i=0;
    int p=0;
    for(i=0;i<numjob;i++)
    {
        if(array[i].flag==1)
        {
            p++;
            printf("[%d] %s [%d]\n",p,array[i].name, array[i].pid);
        }
    }
    if(p==0)
        printf("No background jobs running\n");
    return 1;
}

//Converting a string to an integer
int toNum(char a[]) 
{
    int c, sign, offset, n;
    n = 0;
    for (c =0; a[c] != '\0'; c++) {
        n=n*10 + (a[c]-'0');
    }
    return n;
}

//Function to execute the command 'kjob'
int kjob(char **argument)
{
    int num;
    int sig;
    num = toNum(argument[1]);
    sig = toNum(argument[2]);
    int i,j=0;
    if(num==0)
    {
        printf("No such job\n");
        return 1;
    }
    else
    {
        j=0;
        for(i=0;i<numjob;i++)
        {
            if(array[i].flag==1)
            {
                j++;
                if(j==numjob)
                {
                    if(sig == 9)
                        array[i].flag=0;
                    break;
                }
            }
        }
    }
    if(j==0 || j != num)
    {
        printf("No such job\n");
    }
    else
        kill(array[i].pid, sig);
    return 1;
}

//Function to execute the command 'killall'
int killall()
{
    int i;
    for(i=0;i<numjob;i++)
    {
        if(array[i].flag==1)
            kill(array[i].pid, 9);
    }
    printf("Kill all\n");
    return 1;
}

//Function to execute the function 'fg'
int fg(char **argument)
{
    int i,j;
    int num;
    num = toNum(argument[1]);
    j=0;
    pid_t pid;
    if(num ==0)
    {
        printf("No such job\n");
        return 1;
    }
    for(i=0;i<numjob;i++)
    {
        if(array[i].flag==1)
        {
            j++;
            if(j==num)
            {
                pid = array[i].pid;
                break;
            }
        }
    }
    if(j!=num || j==0)
    {
        printf("No such job\n");
        return 1;
    }
    int status;
    pid_t wpid;
    array[i].flag=0;
    printf("Waiting for process no. %d to finish\n",pid);
    do
    {
        wpid = waitpid(pid, &status, WUNTRACED);
    }while(!WIFEXITED(status) && !WIFSIGNALED(status));
    return 1;
}

//Function to execute commands other than the builtin commands
int execcmd(char **argument)
{
    int i=0;
    int flag=0;
    //Process to check presence of & in the command
    for(i=0;;i++)
    {
        if(argument[i]==NULL)
            break;
    }
    //If command has & in it
    if(argument[i-1]!=NULL && strcmp(argument[i-1],"&")==0)
    {
        argument[i-1]='\0';
        flag=1;
    }
    int j;
    int z=0;
    //Checking if command has pipe in it
    if(flag==0)
    {
        for(j=0;j<i;j++)
        {
            if(strcmp(argument[j],"|")==0)
            {
                z = 1;
                piping(argument);
                return 1;
            }
        }
    }
    //Executing command if it does not have pipe
    if(z==0)
    {
        pid_t pid;
        int status=0;
        char input[64],output[64];
        int in=0;
        int out=0;
        pid =fork();
        if(pid==0)
        {
            int in=0;
            int out=0;
            int i=0;
            //Checking for redirection and concatenation in the command
            for(i=0;argument[i]!='\0';i++)
            {
                if(strcmp(argument[i],"<")==0)
                {        
                    argument[i]=NULL;
                    strcpy(input,argument[i+1]);
                    in=2;           
                }        

                else if(strcmp(argument[i],">")==0)
                {      
                    printf("Out redirect\n");
                    argument[i]=NULL;
                    strcpy(output,argument[i+1]);
                    out=1;
                }
                else if(strcmp(argument[i], ">>")==0)
                {
                    argument[i]=NULL;
                    strcpy(output, argument[i+1]);
                    out=2;
                }
            }
            if(in)
            {   int fd0;
                if ((fd0 = open(input, O_RDONLY, 0)) < 0) 
                {
                    perror("Couldn't open input file");
                    exit(0);
                }  
                dup2(fd0,0);
                close(fd0);
            }
            if(out==2)
            {
                int fd2;
                if((fd2 = open(output, O_APPEND | O_CREAT| O_WRONLY , S_IRUSR | S_IWUSR | S_IXUSR,0))<0)
                {
                    perror("Couldn't open file");

                    exit(0);
                }
                dup2(fd2,1);
                close(fd2);
            }

            else if (out==1)
            {
                int fd1 ;
                if ((fd1 = creat(output , 0755)) < 0) {
                    perror("Couldn't open the output file");
                    exit(0);
                }           

                dup2(fd1, 1);
                close(fd1);
            }
            if(execvp(argument[0], argument) == -1)
            {
                fprintf(stderr,"Error in executing command\n");
            }
        }
        else if(pid < 0)
        {
            fprintf(stderr,"Error in forking\n");
        }
        else
        {

            if(flag==0)
            {
                //If there is no & in the command, wait for the command to finish
                waitpid(pid, &status, WUNTRACED);

            }
            else if(flag==1)
            {
                //If command has & in it, make a new entry in the background array
                array[numjob].name = malloc(sizeof(char) * 100);
                strcpy(array[numjob].name,argument[0]);
                array[numjob].pid = pid;
                array[numjob].status=status;
                array[numjob].flag=1;
                numjob++;
            }
        }
    }
    return 1;

}
int execute(char ** tokens)
{
    int i;
    
    //If no command
    if(tokens[0] == NULL)
    {
        fprintf(stderr,"No command found\n");
        return 1;
    }

    int num_builtin=9;
    for(i=0;i<num_builtin;i++)
    {
        if(strcmp(tokens[0], builtin_funcs[i])==0)
        {
            //If a builtin is a command
            return (*builtinfunc[i])(tokens);
        }
    }
    //Not builtin
    return execcmd(tokens);
}

#define delimiter " \t\n"
int tokenize(char *command)
{
    int flag;
    int size=512;
    char **tokens;
    char **token2;
    token2 = malloc(size * sizeof(char*));
    tokens = malloc(size * sizeof(char*));
    char *word;
    int index=0;
    int pos=0;
    char *word2;
    //Seperating the commands from the ; delimiter
    word2 = strtok(command, ";");
    while(word2!=NULL)
    {
        token2[index]=word2;
        index++;
        word2 = strtok(NULL, ";");
    }
    token2[index]=NULL;
    pos=index;
    index=0;
    int i;
    for(i=0;i<pos;i++)
    {
        //Tokenizing every seperated command one by one
        word = strtok(token2[i]," \t\n\"");

        while (word!=NULL)
        {
            tokens[index]=word;
            index++;
            word = strtok(NULL," \t\n\"");
        }
        tokens[index] = NULL;
        //Executing each command one by one
        flag=execute(tokens);
        index=0;

    }
    return flag;
}
