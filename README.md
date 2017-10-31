# C-Shell for OS
Implememted a shell with builtin features like pwd,cd,echo. Moreover, you can execute external commands like ls,ssh,ps,exit,etc.
It gives the process info about a process when its pid is given as an input to the pinfo command.
Background processes are also supported in this shell.
Multiple commands seperated by the token ';' are also supported.

The system calls fork and exec were used to implement this program.

Files:
shell.c: the main file where all the functions are being imported and used in order for the shell to function in the correct manner.
built.c: This file contains the functions of all the builtin commands.
built.h: The header file for all the builtin commmands.
read.c: This file contains the function which reads the command from the user as an input and passes it on for further processing.
read.h: This is the header file for the read command.
execute.c: This file contains all the functions to execute the command given in by the user. It first tokenizes the command and then parses the command to execute it.
execute.h: This is the header file for the execute.c file and contains all the function declerations for the execution of the command given in by the user.
pipeline.c: This file contains all the functions which will execute commands seperated by pipes. Redirection in pipes is also supported.
pipeline.h: This is the header file for pipeline.c
Makefile: The file which will compile the program and throw out a file which when executed will make the shell operational.



STEPS TO COMPILE AND RUN THE CODE:
Run the following commands-
1. make
2. ./main
