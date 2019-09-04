#include <sys/wait.h>
#include<string.h>
#include<iostream>
#include<stdlib.h>
#include<unistd.h>
using namespace std;
void  parse(char *line, char **argv)
{
     while (*line != '\0') {       /* if not the end of line ....... */
          while (*line == ' ' || *line == '\t' || *line == '\n')
               *line++ = '\0';     /* replace white spaces with 0    */
          *argv++ = line;          /* save the argument position     */
          while (*line != '\0' && *line != ' ' &&
                 *line != '\t' && *line != '\n')
               line++;             /* skip the argument until ...    */
     }
     *argv = 0;                 /* mark the end of argument list  */
}

void cd(char *path)
{
    char cwd[256];
    if(chdir(path) != 0)
        perror("chdir() error()");
    else
    {
        if (getcwd(cwd, sizeof(cwd)) == NULL)
            perror("getcwd() error");
        else
            printf("current working directory is: %s\n", cwd);
    }
    exit(0);
}
char** findPipe(char **argv)
{
    char *pipeArgu[10];
    int i=0;
    while(**argv != 0)
    {
        if(**argv == '|')
        {
            *argv++;
             pipeArgu[i++] = *argv;
        }
    }
    return pipeArgu;
}

int checkOpRedirection(char *buff)
{
    int i=0;
    while(buff[i]!='\0')
    {
        if(buff[i] == '>' && buff[i+1] == '>')
        {
            return 2;
        }
        else if(buff[i] == '>' && buff[++i] != '>')
        {
            return 1;
        }
        i++;
    }
    return 0;
}

void changePrompt()
{
    //cout << "Hello";
    //char *arg[1];
    //arg[0] = 0;
    execl("/home/lenovo/Desktop/Temporary Codes/myrc","myrc",NULL);
    perror("execl script.sh");
}
void input(char *buf)
{
    fgets(buf, 1023, stdin);
    if (buf[strlen(buf) - 1] == '\n')
        buf[strlen(buf) - 1] = 0;
}
void execute(char **argv)
{
    pid_t pid;
    int status;
    if ((pid = fork()) < 0)
        perror("fork error");
    else if (pid == 0)
    {
        execvp(*argv,argv);
        printf("couldn’t execute: %s\n", argv[0]);
        exit(127);
    }
    if ((pid = waitpid(pid, &status, 0)) < 0)
    perror("waitpid error");
    printf("%% ");
}

void opRedirectionLogic(int check,char ** argv)
{
    char *param1[10],*param2[10];
    int i=0,j=0;
    /*for(;*argv != 0 ;*argv++)
        cout << *argv;*/


    string temp1 = ">",temp2 = ">>";
    while(!(strcmp(argv[i], temp1.c_str()) == 0 || strcmp(argv[i], temp2.c_str()) == 0))
    {
        param1[j++] = argv[i++];
    }
    param1[j] = 0;
    j = 0; i++;
    while(argv[i]!= 0)
    {
        param2[j++] = argv[i++];
    }
    param2[j] = 0;

    for(int i=0;param1[i]!=0;i++)
    {
        cout << param1[i];
    }
    cout <<endl;
    for(int i=0;param2[i] !=0;i++)
    {
        cout << param2[i];
    }
    if(check==1)
    {

    }
    else if(check==2)
    {

    }

}
int main(void)
{
    char buf[1024];
    char *argv[50];
    printf("%% ");
    while(1)
    {
        input(buf);
       // auto opRedirec = checkOpRedirection(buf);
        parse(buf,argv);

        //cout << opRedirec<<endl;
        //opRedirectionLogic(opRedirec,argv);
        execute(argv);
    }
    exit(0);
}
