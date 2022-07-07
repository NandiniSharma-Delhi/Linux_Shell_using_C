# Linux_Shell_using_C
implementation of linux shell in C language

Name: Nandini sharma
rollno: 200101122
language: c
os : linux
run the code as:
$gcc 200101122_assign2.c
$./a.out


limitations:
*for proper running all parts of commands should be space separated
example:
correct: history > newfile
	ls -l | more
incorrect: history> newfile
	ls -l|more

*4 environment variables can be set and echoed
HOME,SHELL,TERM,USER

*echo command can be run for environment variables and other texts but without " "
example: echo $HOME $SHELL

*history file deleted after exit
*built in functions supported:
cd
pwd
help
history
exit
echo
* pipe cannot execute builtin commands
* pipe cannot run like: cmd1 | cmd2 >output
*input output redirections for comands without pipe work well
* may not be executing some specific commands as part of pipe
