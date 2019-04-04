

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#define clear() printf("\033[H\033[J")
int pipe_count=0, fd;
static char* args[512];
char input_buffer[1024];
char *cmd_exec[100];
int flag, len;
char cwd[1024];
int background=0;
int flag_pipe=1;
pid_t pid;
int no_of_lines;
int dash_flag;
int pid, status;
char history_data[1000][1000];
char current_directory[1000];
char his_var[2000];

void echo_call(char *);
void clear_variables();
void dash_execute();
void change_directory();
void parent_directory();
static int split(char *cmd_exec, int, int, int);
void with_pipe_execute();
static int command(int, int, int, char *cmd_exec);
void prompt();
void sigintHandler(int sig_num);

void sigintHandler(int sig_num)
{
    signal(SIGINT, sigintHandler);
    fflush(stdout);
}
void init_shell()
{
    clear();
    printf("\n\n\n\n******************"
        "************************");
    printf("\n\n\n\t****KATTAPPA SHELL(KASH)****");

    printf("\n\n\n\n*******************"
        "***********************");
    char* username = getenv("USER");
    printf("\n\n\nUSER : @%s", username);
    printf("\n");
    sleep(3);
    clear();
}
void openHelp()
{
    puts("\n***WELCOME TO KASH SHELL HELP***"
        "\nWE CAN DO...:"
        "\n\n>cd"
        "\n>ls"
        "\n>exit"
        "\n tellafortune"
        "\n>all other general commands available in UNIX shell"
        "\n>pipe handling(multiple pipes)"
        "\n>kash_story"
        );
    return;
}
void tellafortune()
{
        system("fortune |cowsay");
        return;
}
void kash_story()
{
        system("kash_story");
        return;
}
void clear_variables()
{
  fd =0;
  flag=0;
  len=0;
  no_of_lines=0;
  pipe_count=0;
  flag_pipe=0;
  input_buffer[0]='\0';
  cwd[0] = '\0';
  pid=0;
  dash_flag=0;
}

void echo_call(char *echo_val)
{
        printf("%s",echo_val);

}

void change_directory()
{
char *h="/home";
if(args[1]==NULL)
        chdir(h);
else if ((strcmp(args[1], "~")==0) || (strcmp(args[1], "~/")==0))
        chdir(h);
else if(chdir(args[1])<0)
    printf("bash: cd: %s: No such file or directory\n", args[1]);

}


void parent_directory()
{
if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
         printf("%s\n", cwd );
        }
else
       perror("getcwd() error");


}

static int split(char *cmd_exec, int input, int first, int last)
{
    char *new_cmd_exec1;
    new_cmd_exec1=strdup(cmd_exec);
        {
        int m=1;
        args[0]=strtok(cmd_exec," ");
        while((args[m]=strtok(NULL," "))!=NULL)
              m++;
        args[m]=NULL;
        if (args[0] != NULL)
            {

            if (strcmp(args[0], "exit") == 0)
                    exit(0);
            if(strcmp("cd",args[0])==0)
                    {
                    change_directory();
                    return 1;
                    }
            else if(strcmp("pwd",args[0])==0)
                    {
                    parent_directory();
                    return 1;
                    }
            }
        }
    return command(input, first, last, new_cmd_exec1);
}

void dash_execute()
{
  char dash_val[1000];
  char *tokenise_dash[100], *num_ch[10];
  int i, n=1, num, index=0;
  i=1;
 if(input_buffer[i]=='-')
    {
        n=1;
        num_ch[0]=strtok(input_buffer,"-");
        while ((num_ch[n]=strtok(NULL,"-"))!=NULL)
              n++;
        num_ch[n]=NULL;
        num = atoi(num_ch[1]);

        index = no_of_lines-num;
        strcpy(dash_val, history_data[index]);

    }
}
int history()
{
        FILE *f1;
        char c;
        f1=fopen("history.txt","r");
        if(f1==NULL)
        {
                printf("file open error");}
        else
                c=fgetc(f1);
                while(c!=EOF)
                {
                        printf("%c",c);
                        c=fgetc(f1);
                }
        fclose(f1);
        return 0;
}


static int command(int input, int first, int last, char *cmd_exec)
{
  int mypipefd[2], ret, input_fd, output_fd;
  ret = pipe(mypipefd);
  if(ret == -1)
      {
        perror("pipe");
        return 1;
      }
  pid = fork();
  if (pid == 0)
  {
    if (first==1 && last==0 && input==0)
    {
      dup2( mypipefd[1], 1 );
    }
    else if (first==0 && last==0 && input!=0)
    {
      dup2(input, 0);
      dup2(mypipefd[1], 1);
    }
    else
    {
      dup2(input, 0);
    }
    if(strcmp(args[0],"history")==0)
        {
                history();
        }
    if(strcmp(args[0], "echo") == 0)
        {
                echo_call(cmd_exec);
        }

    else if(strcmp(input_buffer, "tellafortune") == 0)
            {
              tellafortune();
            }
    else if(strcmp(input_buffer, "kash_story") == 0)
            {
              kash_story();
            }
    else if(strcmp(input_buffer, "help") == 0)
            {
              openHelp();
            }

    else if(execvp(args[0], args)<0) printf("%s:command not found\n", args[0]);
              exit(0);
  }
  //  if(!background)
//      {
//              wait(NULL);
//      }
   else
   {
      waitpid(pid, 0, 0);
   }
   if (last == 1)
     close(mypipefd[0]);
   if (input != 0)
     close(input);
   close(mypipefd[1]);
   return mypipefd[0];

}


void with_pipe_execute()
{

int i, n=1, input, first;

input=0;
first= 1;

cmd_exec[0]=strtok(input_buffer,"|");

while ((cmd_exec[n]=strtok(NULL,"|"))!=NULL)
      n++;
cmd_exec[n]=NULL;
pipe_count=n-1;
for(i=0; i<n-1; i++)
    {
      input = split(cmd_exec[i], input, first, 0);
      first=0;
/*      if(strcmp(cmd_exec[i-1],"&")==0)
        {
                cmd_exec[i-1]=NULL;
                background=1;
        }*/
    }
input=split(cmd_exec[i], input, first, 1);
input=0;
return;

}

void prompt()
{
  char shell[1000];
   if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
          strcpy(shell, "Kattappa_shell:");
          printf("\n");
          strcat(shell, cwd);
          strcat(shell, "\nadichuthooku>>>");
          printf("%s", shell);
        }
   else
       perror("getcwd() error");

}


int main()
{
    int status,i;
    char hi[1000];
    char ch[2]={"\n"};
    init_shell();
    getcwd(current_directory, sizeof(current_directory));
    signal(SIGINT, sigintHandler);
    while (1)
    {
      clear_variables();
      prompt();
      fgets(input_buffer, 1024, stdin);

        strcpy(hi,input_buffer);
        //i++;
        FILE *fp;
        fp=fopen("history.txt","a");
        if(fp==NULL)
                printf("cannot open file");
        else
        {
                if(strlen(hi)>0)
                {
                        fputs(hi,fp);
                        fputs("\n",fp);
                }
                fclose(fp);
        }

      if(strcmp(input_buffer, ch)==0)
            {
              continue;
            }
      len = strlen(input_buffer);
      input_buffer[len-1]='\0';
      strcpy(his_var, input_buffer);
      if(strcmp(input_buffer, "exit") == 0)
            {
              flag = 1;
              break;
            }
        if(input_buffer[0]=='-')
              {
                dash_flag=1;
                dash_execute();
              }
        with_pipe_execute();
        waitpid(pid,&status,0);
     }
        if(flag==1)
        {
         printf("\nBye...cya soon\n");
        exit(0);
        return 0;
        }
return 0;
}
