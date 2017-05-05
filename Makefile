main: shell.c built.c read.c execute.c pipeline.c
	gcc shell.c built.c read.c execute.c pipeline.c -o main -g
