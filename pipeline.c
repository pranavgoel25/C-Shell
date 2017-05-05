#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<errno.h>
#include<pwd.h>
#include<signal.h>
#include<sys/stat.h>

int killjob(char **argument)
{
    int num;
    int numjob=0;
    int sig;
    int array[10];
//    num = toNum(argument[1]);
//    sig = toNum(argument[2]);
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
            if(array[i]==1)
            {
                j++;
                if(j==numjob)
                {
                    if(sig == 9)
                        array[i]=0;
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
        kill(array[i], sig);
    return 1;
}
char input[64],output[64];
char *command[512];

//Checking for input redirection in the command
int redirect_in(char *command[],int in)
{
    int u;
    for(u=0;command[u]!='\0';u++)
    {
        if(strcmp(command[u],"<")==0)
        {        
            command[u]=NULL;
            strcpy(input,command[u+1]);
            in=1;           
        }
    }
    return in;
}

//Checking for output redirection in the command
int redirect_out(char *command[], int out)
{
    int v;
    for(v=0;command[v]!='\0';v++)
    {
        if(strcmp(command[v],">")==0)
        {      
            command[v]=NULL;
            strcpy(output,command[v+1]);
            out=1;
        }
    }
    return out;
}

//Method to execute piping
void piping(char** argument)
{
    int p1[2], p2[2];
    int number = 0;

    pid_t pid;
    int end=0;
    int i=0,j,k=0,l;
    l=0;
    //Counting number of commands seperated by pipi
    while (argument[l] != NULL)
    {
        if (strcmp(argument[l],"|") == 0)
            number++;
        l++;
    }
    number++;
    j=0;
    //Running each pipe seperated command one at a time
    while(argument[j]!=NULL)
    {
        k = 0;
        while (strcmp(argument[j],"|") != 0)
        {
            command[k] = argument[j];
            j++;	
            if (argument[j] == NULL)
            {
                k++;
                break;
            }
            k++;
        }
        command[k] = NULL;
        //If even numbered command
        if (i%2 != 0)
            pipe(p1); 
        //If odd numbered command
        else
            pipe(p2); 

        int in=0;
        int out=0;

        pid=fork();
        if(pid==0)
        {
            //First command
            if(i==0)
            {
                int u;
                in=0;
                //Checking for input redirection
                in = redirect_in(command,in);
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
                dup2(p2[1], 1);
            }
            //Last command
            else if(i==number-1)
            {
                out=0;
                //Checking for output redirection
                out = redirect_out(command, out);
                if(out)
                {
                    int fd1 ;
                    if ((fd1 = creat(output , 0755)) < 0) {
                        perror("Couldn't open the output file");
                        exit(0);
                    }           

                    dup2(fd1, 1);
                    close(fd1);
                }
                if(number%2!=0)
                    dup2(p1[0],0);
                else
                    dup2(p2[0],0);
            }
            else
            { 
                if (i%2!=0)
                {
                    dup2(p2[0],0); 
                    dup2(p1[1],1);
                }
                else
                { 
                    dup2(p1[0],0); 
                    dup2(p2[1],1);					
                } 
            }

            if (execvp(command[0],command) < 0)
            {
                printf("Command could not be executed\n");
            }		
        }
        else if(pid==-1)
        {
            printf("Child process could not be created\n");
            return;
        }
        //Parent process
        else
        {
            //First command
            if (i==0)
                close(p2[1]);
            //Last command
            else if (i==number-1)
            {
                if (number%2 != 0)
                    close(p1[0]);
                else
                    close(p2[0]);
            }
            else
            {
                if (i%2==1)
                {					
                    close(p2[0]);
                    close(p1[1]);
                }
                else
                {					
                    close(p1[0]);
                    close(p2[1]);
                }
            }
            //waiting for command to finish
            waitpid(pid,NULL,0);
        }
        j++;		
        i++;	
    }
}

