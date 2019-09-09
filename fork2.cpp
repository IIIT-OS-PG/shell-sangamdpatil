#include <sys/wait.h>
#include<string.h>
#include<iostream>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include <fstream>
#include <sys/stat.h>
#include<unordered_map>
#include<pwd.h>
#include<vector>
#include<sstream>
using namespace std;
int EXIT_STATUS=0;
void opRedirectionLogic(int check,char ** argv);
void execute(char **argv);
void  parse(char *buf, char **argv,char delimiter)
{
     while (*buf != '\0')
     {
          while (*buf == delimiter)
          {
              *buf++ = '\0';
          }
          *argv++ = buf;
          while (*buf != '\0' && *buf != delimiter)
          {
             buf++;
          }
     }
     *argv = 0;
}

void fileOpen( char *buf)
{
    int i=strlen(buf)-1;
    while( i>0 && buf[i]!='.')
    {
        i--;
    }

    char *ptr= &buf[i+1];
    string ext(ptr);//cout << ext << endl;
    ifstream myfile("/home/lenovo/Desktop/Temporary Codes/extMapping");
    string str,path;
    char *argv[10],*temp[10];char charStr[100];
    while(getline(myfile,str))
    {
        //char charStr[str.size()+1];
        strcpy(charStr,str.c_str());
        for(i=0;charStr[i]!='=';i++)
        {

        }
        charStr[i] = '\0';
        i++;
        argv[0] = charStr;
        argv[1] = &charStr[i];//cout << argv[0]<<"="<<argv[1]<<endl;
        if(strcmp(argv[0],ext.c_str())==0)
        {
            temp[0] = argv[1];//cout << temp[0]<<endl;
            break;
        }
    }

    for(i=0;buf[i]!=' ';i++)
    {

    }
    i++;
    temp[1] = &buf[i];
    temp[2] = 0;
    execute(temp);


}

void cd(char *path)
{
    char cwd[256];
    if(strcmp(path,"~")==0)
        path = "/home";
    //cout<<path<<endl;~

    if(chdir(path) != 0)
        perror("chdir() error()");
    else
    {
        if (getcwd(cwd, sizeof(cwd)) == NULL)
            perror("getcwd() error");
        //else
          //  printf("current working directory is: %s\n", cwd);
    }
    //exit(0);
}
/*char** findPipe(char **argv)
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
}*/

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

int checkIpRedirection(char *buff)
{
    int i=0;
    while(buff[i]!='\0')
    {
        if(buff[i] == '<' )
            return 1;
        i++;
    }
    return 0;
}

void showPrompt()
{

    if(getuid()==0)
        cout << "#> ";
    else
        cout <<"$> ";
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
    //char binPath[50] = "/bin/";
    //strcat(binPath,argv[0]);
    if ((pid = fork()) < 0)
        perror("fork error");
    else if (pid == 0)
    {
        execvp(*argv,argv);
        printf("couldn’t execute: %s\n", argv[0]);
        exit(127);
    }
    //if ((pid = waitpid(pid, &status, 0)) < 0)
      //  perror("waitpid error");
    waitpid(pid,&status,0);
    //cout << status;
    if (WIFEXITED(status))
        EXIT_STATUS = WEXITSTATUS(status);
}

void ipRedirectionLogic(int check,char **argv)
{
    char *param1[10],*param2[10];
    int i=0,j=0;
    string temp = "<";
    while(!(strcmp(argv[i], temp.c_str()) == 0))
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
    //------------------------------
    int newfd;
    if ((newfd = open(param2[0], O_RDONLY)) < 0)
    {
        perror(param2[0]);	/* open failed */
        exit(1);
    }
    auto defaultStdIn = dup(0);
    dup2(newfd,0);
    execute(param1);
    dup2(defaultStdIn,0);
}

void opRedirectionLogic(int check,char ** argv)
{
    char *param1[10],*param2[10];
    int i=0,j=0;
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
    //------------------------------
    int newfd;
    if(check==1)
    {
        if ((newfd = open(param2[0], O_CREAT|O_TRUNC|O_WRONLY)) < 0)
        {
            perror(param2[0]);	/* open failed */
            exit(1);
        }
        if(fchmod(newfd,S_IRWXU | S_IRWXG | S_IRWXO)!=0)
		{
			perror("\nfchmod");
			return;
		}
    }
    else if(check==2)
    {
        if((newfd = open(param2[0],O_WRONLY | O_APPEND)) < 0)
        {
            perror(param2[0]);
            exit(1);
        }
    }
    auto defaultStdOut = dup(1);
    dup2(newfd,1);
    execute(param1);
    dup2(defaultStdOut,1);
    //perror(param1[0]);
    //exit(0);
}

int checkPipes(char *buf,char **cmds)
{
    int count=0,i=0;//cout <<"in pipescheck"<<endl;
    while(buf[i]!='\0')
    {
        if(buf[i] == '|')
            count++;
        i++;
    }
    parse(buf,cmds,'|');

    return count;

}
void trimLeft(char **str)
{
    int i=0,j=0,k=0;
    while(str[k]!=0)
    {
        while(str[k][i]==' ')
        {
            i++;
        }
        if(i==0)
            return;
        while(str[k][i]!='\0')
        {
            str[k][j] = str[k][i];
        }
        k++;
    }
}

void trimRight(char **str)
{
    int i=0,ptrLen;
    while(str[i]!= 0)
    {
        i++;
    }
    ptrLen = i;
    int k=0;i=0;
    while(k < ptrLen)
    {
        i=0;
        while(str[k][i]!='\0')
        {
            i++;
        }
        i--;
        while(str[k][i]==' ')
        {
            i--;
        }
        str[k][i+1] = '\0'; //cout << str[k].size();
        //cout << str[k];
        k++;
    }

}
void pipeExecute(char **cmds,int numPipes,char **argv)
{
    int RD=0,pid,status;
    char *temp[10];
    //cout <<numPipes;
    int fd[2];
    for(int i=0;i <= numPipes;i++)
    {
        pipe(fd);int j=0;

        trimLeft(temp);
        trimRight(temp);
        parse(cmds[i],temp,' ');
        //cout <<argv[i]<<endl;
       // while(temp[j]!=0){cout << temp[j]<<endl;j++;}
        //parse(argv[i],tempArgv,' ');while(tempArgv[j]!=0){cout << tempArgv[j];j++;}
        if((pid = fork())< 0)
              perror("fork error");
        if(pid==0)
        {//ls: cannot access '': No such file or directory

            if(i==0)
            {
                dup2(fd[1],1);
                close(fd[0]);
                close(fd[1]);
                execvp(temp[0],temp);

            }
            else if(i == numPipes)
            {
                dup2(RD,0);
                close(fd[0]);
                close(fd[1]);
                execvp(temp[0],temp);
            }
            else
            {
                dup2(RD,0);
                dup2(fd[1],1);
                close(fd[0]);
                close(fd[1]);
                execvp(temp[0],temp);
            }
        }
        else if(pid > 0)
        {   //cout <<"parent Before:"<<endl;
            wait(NULL);
            //cout <<"parent After:"<<endl;
            close(fd[1]);
            RD = fd[0];
        }
    }

}

void createAlias(char *buf,unordered_map<string,string> &aliasMap)
{
     //alias abs=ls -al ; abs

     char *argv[10],*cmd[10];
     parse(buf,argv,'=');
     parse(argv[0],cmd,' ');

     string key(cmd[1]);
     string val(argv[1]);//cout <<key<<val;
     aliasMap[key] = val;
     //aliasMap[cmd[1]] = argv[1];
     /*for(auto x:aliasMap)
        cout << x.first<<" "<<x.second<<endl;*/
}


bool isPresentInAlias(char buf[],unordered_map<string,string> &aliasMap)
{

    string key(buf);
    //stringal(argv[1]);
    char *temp[10];
    string val;
    if(aliasMap.find(key)!=aliasMap.end())
        val = aliasMap[key];
    else
        return false;

    char argv[val.size()+1];
    strcpy(argv,val.c_str());

    parse(argv,temp,' ');
    //temp[0] = argv[];cout<<"argv: " <<argv;
    //temp[1]=0;
    execute(temp);
    return true;
}

void loadEnv()
{
    execl("/home/lenovo/Desktop/Temporary Codes/myrc","myrc",NULL);
    perror("execl script.sh");
    //echo "hello"
    //echo "hello $PATH"
    //echo ""
}

void init()
{
    ifstream myfile("/home/lenovo/Desktop/Temporary Codes/mybashrc");
    string str;
    char *argv[10];
    while(getline(myfile,str))
    {
        char charStr[str.size()+1];
        strcpy(charStr,str.c_str());
        parse(charStr,argv,'=');
        setenv(argv[0],argv[1],0);//cout <<argv[0]<<"="<<argv[1]<<endl;
    }
    struct passwd *p;
    p = getpwuid(getuid());
    setenv("USER",p->pw_name,0);
    setenv("HOME",p->pw_dir,0);
    //cout <<getenv("$");
}
int main(void)
{
    char buf[1024],prompt[10];
    char *argv[50],*cmds[10];
    unordered_map<string,string> aliasMap;

    init();
    showPrompt();
    //cout << prompt<<endl;
    while(1)
    {
        input(buf);

        if(strstr(buf,"open "))
        {
            fileOpen(buf);
            showPrompt();
            continue;
        }

        if(strcmp(buf,"$?")==0)
        {
            if(EXIT_STATUS==0)
                cout << EXIT_STATUS<<endl;
            else
            {
                cout << EXIT_STATUS<<endl;
                EXIT_STATUS = 0;
            }
            showPrompt();
            continue;
        }

        if(strcmp(buf,"$$")==0)
        {
            cout << getpid()<<endl;
            EXIT_STATUS = 0;
            showPrompt();
            continue;
        }
        if(strlen(buf)==0)
         {
             showPrompt();
             EXIT_STATUS=148;
             continue;
         }
        if(strstr(buf,"alias"))
        {
            createAlias(buf,aliasMap);
            showPrompt();
            continue;
        }
        if(isPresentInAlias(buf,aliasMap))
        {
            showPrompt();
            continue;
        }
        auto numPipes = checkPipes(buf,cmds); //cout <<numPipes;
        if(numPipes > 0)
        {
            pipeExecute(cmds,numPipes,argv);
            showPrompt();
            continue;
        }

        auto checkout = checkOpRedirection(buf);
        auto checkin = checkIpRedirection(buf);

        parse(buf,argv,' ');
        if(strstr(argv[0],"cd"))
        {
            cd(argv[1]);
            showPrompt();
            continue;
        }
        if(checkout)
            opRedirectionLogic(checkout,argv);
        else if(checkin)
            ipRedirectionLogic(checkin,argv);
        else
            execute(argv);
        showPrompt();

    }
    exit(0);
}

