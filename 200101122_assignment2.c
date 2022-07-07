#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
//history file
FILE* historyfile;
/*
  Function Declarations for builtin shell commands:
 */
int mysh_history(char **args);
int mysh_pwd(char **args);
int mysh_echo(char **args);
int mysh_cd(char **args);
int mysh_help(char **args);
int mysh_exit(char **args);
int mysh_envi(char **args);

/*
  List of builtin commands, followed by their corresponding functions.
 */
char *internals[] = {
  "cd",
  "help",
  "exit",
  "history",
  "pwd",
  "echo",
  "set",
};

int (*builtin_func[]) (char **) = {
  &mysh_cd,
  &mysh_help,
  &mysh_exit,
  &mysh_history,
  &mysh_pwd,
  &mysh_echo,
  &mysh_envi
};

int mysh_num_builtins() {
  return sizeof(internals) / sizeof(char *);
}

/*
  Builtin function implementations.
*/


int mysh_cd(char **args)
{
  if (args[1] == NULL) {
    char *p=getenv("USER");
    char g[100]="/home/";
    strcat(g,p);
    chdir(g);
  } else {
    if (chdir(args[1]) != 0) {
      perror("mysh");
    }
  }
  return 1;
}

//1 is to continue do while
//0 is to break out
int mysh_help(char **args)
{
  int i;
  printf("custom shell by nandini\n");
  printf("The following are built in:\n");

  for (i = 0; i < mysh_num_builtins(); i++) {
    printf("  %s\n", internals[i]);
  }
  printf("Use the man command for information on other commands.\n");
  return 1;
}

//arguments are extra
int mysh_exit(char **args)
{
    fclose(historyfile);
    remove("myshellhistory.txt");
  return 0;
}
//echo command
int mysh_echo(char ** args)
{
	int i = 1;
	while (1){
		if (args[i] == NULL){
			break;
		}
    else if(strcmp(args[i],"$HOME")==0)
    {
        printf("%s ",getenv("HOME"));
    }
    else if(strcmp(args[i],"$USER")==0)
    {
      printf("%s ",getenv("USER"));
    }
    else if(strcmp(args[i],"$SHELL")==0)
    {
      printf("%s ",getenv("SHELL"));
    }
    else if(strcmp(args[i],"$TERM")==0)
    {
      printf("%s ",getenv("TERM"));
    }
    else{
		printf("%s ", args[i]);}
		i++;
	}
    //updated it to include $variable printing
	printf("\n");
    return 1;
}

//to print history
int mysh_history(char **args)
{
    int num=1;
    int c;
    fseek(historyfile,0,SEEK_SET);

    c=fgetc(historyfile);
    while(c!=EOF)
    {
        printf("%c",c);

        c=fgetc(historyfile);
    }


    return 1;
}

int mysh_pwd(char **args)
{
    char pwd[1000];
    getcwd(pwd,1000);
    printf("%s\n",pwd);
    return 1;
}

int mysh_envi(char **args)
{
  //you can echo them using echo
  if(((strcmp(args[0],"set")==0)&&(strcmp(args[2],"=")==0)))
  {
      char *p=args[1];
      char *q=args[3];
      setenv(p,q,1);
  }
  return 1;
}
//to run external commands
int mysh_launch(char **args)
{
  pid_t pid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("mysh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("mysh");
  } else {

    wait(NULL);
  }

  return 1;
}





//to compare if redirection symbols is there
int compare(char **line,char* symb)
{
    int i=0;
    while(line[i]!=NULL)
    {
        if(strcmp(line[i],symb)==0)
        {
            return i;
        }
        i++;
    }
    return -1;
}


//to execute commands which are not piped
int mysh_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }
  //copying stds to some integers so that later we can come back from files
    int std_in,std_out,std_err;
    std_in = dup(0);
    std_out = dup(1);
    std_err = dup(2);

    if(compare(args,"<")>=0)
    {
        int pos=compare(args,"<");
        int inp=open(args[pos+1],O_RDONLY);
        if(inp<0)
        {
            perror("mysh");
            return -1;
        }
        if(dup2(inp,0)<0)
        {
            perror("mysh");
			return -1;
        }
        close(inp);
        args[pos]=NULL;
        args[pos+1]=NULL;
    }
    if(compare(args,">")>=0)
    {
        int pos=compare(args,">");
        int outp=open(args[pos+1],O_WRONLY | O_TRUNC | O_CREAT,0755);
        if(outp<0)
        {
            perror("mysh");
            return -1;
        }
        if(dup2(outp,1)<0)
        {
            perror("mysh");
			return -1;
        }
        close(outp);
        args[pos]=NULL;
        args[pos+1]=NULL;
    }
    if(compare(args,">>")>=0)
    {
        int pos=compare(args,">>");
        int outt=open(args[pos+1],O_WRONLY | O_APPEND | O_CREAT,0755);
        if(outt<0)
        {
            perror("mysh");
            return -1;
        }
        if(dup2(outt,1)<0)
        {
            perror("mysh");
			return -1;
        }
        close(outt);
        args[pos]=NULL;
        args[pos+1]=NULL;
    }
    if(compare(args,"2>")>=0)
    {
        int pos=compare(args,"2>");
        int oute=open(args[pos+1],O_WRONLY | O_CREAT,0755);
        if(oute<0)
        {
            perror("mysh");
            return -1;
        }
        if(dup2(oute,2)<0)
        {
            perror("mysh");
			return -1;
        }
        close(oute);
        args[pos]=NULL;
        args[pos+1]=NULL;
    }
    if(compare(args,"2>>")>=0)
    {
        int pos=compare(args,"2>>");
        int oute=open(args[pos+1],O_WRONLY | O_CREAT |O_APPEND,0755);
        if(oute<0)
        {
            perror("mysh");
            return -1;
        }
        if(dup2(oute,2)<0)
        {
            perror("mysh");
			return -1;
        }
        close(oute);
        args[pos]=NULL;
        args[pos+1]=NULL;
    }

  for (i = 0; i < mysh_num_builtins(); i++) {
    if (strcmp(args[0], internals[i]) == 0) {
    int pec= (*builtin_func[i])(args);
    //coming back to std I/O
    dup2(std_in, 0);
	dup2(std_out, 1);
	dup2(std_err, 2);
    return pec;
    }
  }

  int cap= mysh_launch(args);
  dup2(std_in, 0);
		dup2(std_out, 1);
		dup2(std_err, 2);
        return cap;
}
//for executing pipes
int executepipe(char** parsed, char** parsedpipe)
{
    // 0 is read end, 1 is write end
    //printf("pipie\n");
    int pipefd[2];
    pid_t p1, p2;

    if (pipe(pipefd) < 0) {
        printf("Pipe could not be initialized\n");
        return -1;
    }
    p1=fork();
    if (p1 < 0) {
        printf("Could not fork\n");
        return -1;
    }

    if (p1 == 0) {
        // Child 1 executing..
        // It only needs to write at the write end
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        if (execvp(parsed[0],parsed) < 0) {
            printf("Could not execute command 1\n");
            exit(0);
        }
    } else {
        // Parent executing
        p2 = fork();

        if (p2 < 0) {
            printf("Could not fork\n");
            return -1;
        }

        // Child 2 executing.
        // It only needs to read at the read end
        if (p2 == 0) {
          //printf("inside\n");
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
            if (execvp(parsedpipe[0],parsedpipe) < 0) {
                printf("Could not execute command 2\n");
                exit(0);
            }
        } else {
            // parent executing, waiting for two children
            wait(NULL);
            wait(NULL);
        }
    }
}


//to read command line
#define LINESIZE 1000
char *mysh_read_line(void)
{

  int bufsize = LINESIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  while (1) {
    // Read a character
    c = getchar();

    if (c == EOF) {
      exit(EXIT_SUCCESS);
    } else if (c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      bufsize += LINESIZE;
      buffer = realloc(buffer, bufsize);
    }
  }

}

//to separate 2 piped commands
int piped(char **line,char ***lin1,char ***lin2,int num)
{

    *lin1=(char **)malloc(30*sizeof(char *));
    *lin2=(char **)malloc(30*sizeof(char *));
    int i=0;
    while(i<num)
    {

        if(strcmp(line[i],"|")==0)
        {
            break;
        }
        i++;
    }
    //printf("bojo ");
    //printf("%d",i);
    if(i==num)
    {
      //printf("bojo2");
        int j=0;
        while(j<num)
        {
            (*lin1)[j]=line[j];
            j++;
        }
        (*lin1)[j]=NULL;
        (*lin2)[0]=NULL;
    }
    else
    {
        int j=0;
        while((strcmp(line[j],"|")!=0)&& j<num)
        {
        (*lin1)[j]=line[j];
        j++;
        }
        (*lin1)[j]=NULL;
        j++;
        int k=0;
        while(line[j]!=NULL)
        {
            (*lin2)[k]=line[j];
            j++;
            k++;
        }
        (*lin2)[k]=NULL;
        //printf("%d",k);


    }


    return 1;

}




#define NUMOFTOKENS 64
#define mysh_TOK_DELIM " \t\r\n\a"
//to split into tokens
char **splitline(char *line,int *p)
{
  int bufsize = NUMOFTOKENS, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  token = strtok(line, mysh_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += NUMOFTOKENS;
      tokens = realloc(tokens, bufsize * sizeof(char*));
    }

    token = strtok(NULL, mysh_TOK_DELIM);
  }
  tokens[position] = NULL;
  *p=position;
  return tokens;
}
void clearsc(){  //checked...
    #if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
        system("clear");
    #endif

    #if defined(_WIN32) || defined(_WIN64)
        system("cls");
    #endif
}
void start_shell() //checked..
{
    clearsc();
    printf("\n******************\t\t******************\t\t******************");
    printf("\n\n********************myshell by nandini sharma****************************");
    printf("\n******************\t\t******************\t\t******************");
    char* user = getenv("USER");
    printf("\n\nuser: @%s\n", user);
    sleep(1);
    clearsc();

}
//main
int main(int argc, char **argv)
{
    start_shell();
  historyfile=fopen("myshellhistory.txt","w+");
  char *line;
  char **args;
  char **par1;
  char **par2;
  int status;
  int num=1;
  int new;

  do {
    printf("> ");
    line = mysh_read_line();
    //printf("%s\n",line);
    fprintf(historyfile,"%d ",num);
    num++;
    fprintf(historyfile,"%s\n",line);
    //printf("hogya");
    args = splitline(line,&new);
    //printf("%d",new);
    new=piped(args,&par1,&par2,new);
    //printf("hi\n");

    if(par2[0]==NULL)
    {//printf("normal\n");
        status = mysh_execute(args);
        free(line);
    free(args);}
    else
    {//printf("pagal\n");
    //par1=splitline(line);
    //par2=splitline(line);
    status=executepipe(par1,par2);
    free(par1);
    free(par2);
    }
  } while (status);

  return EXIT_SUCCESS;
}
