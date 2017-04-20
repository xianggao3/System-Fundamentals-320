#define _GNU_SOURCE
#include "sfish.h"
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

#define RESET       0
#define BRIGHT      1


#define BLACK       0
#define RED         1
#define GREEN       2
#define YELLOW      3
#define BLUE        4
#define MAGENTA     5
#define CYAN        6
#define WHITE       7

int usercl=7;
int userbold=0;
int machinecl=7;
int machinebold=0;

int main(int argc, char** argv, char* envp[]) {
    //DO NOT MODIFY THIS. If you do you will get a ZERO.
    rl_catch_signals = 0;
    //This is disable readline's default signal handlers, since you are going
    //to install your own.
    int user =0;
    int machine = 0;
    char *cmd;
    int exitCOde=0;
    cmd="sfish> ";
    while((cmd = readline(cmd))!= NULL) {
        if (strcmp(cmd,"quit")==0)
            break;
        char *builtin; 
        if((builtin=strtok(cmd, " "))!=NULL){
        
            if (strcmp(builtin,"exit")==0){
                exit(0);
            }
            else if (strcmp(builtin,"help")==0){
                char help[]="";
                printf("help menu%s\n",help);
            }
            else if (strcmp(builtin,"cd")==0){
                if(builtin != NULL){
                    builtin = strtok (NULL, " ");
                }
                //
                if (builtin==NULL){
                    chdir(getenv("HOME"));
                }else if (strcmp(builtin,".")==0){
                    chdir(".");
                }else if (strcmp(builtin,"..")==0){
                    chdir("..");
                }else if (strcmp(builtin,"-")==0){
                    chdir(getcwd(builtin,sizeof(builtin)));
                }else{
                    char currdir[50]; 
                    getcwd(currdir,sizeof(currdir));
                    char slash[]="/";
                    char* result =malloc(512);

                    strcpy(result, currdir);
                    strcat(result, slash);
                    strcat(result, builtin);
                    if(chdir(result)!=0){
                        char error[]="Not an existing directory";
                        printf("%s\n", error);
                    }
                    free(result);
                }
                char currdir[1024];
                getcwd(currdir,sizeof(currdir));
               // printf("%s\n",currdir);
            }
            else if(strcmp(builtin,"pwd")==0){
                pid_t pid1 =fork();
                if(pid1==0){
                    char currdir[1024];
                    getcwd(currdir,sizeof(currdir));
                    printf("%s\n",currdir);
                }else{
                    int returnStatus;
                    waitpid(pid1,&returnStatus,0);
                }
            }
            else if(strcmp(builtin,"prt")==0){
                pid_t pid1 =fork();
                if(pid1==0){
                    //char *prtret= getenv("ERRORLEVEL");                    
                    printf("%d\n",exitCOde);
                }else{
                    int returnStatus;
                    waitpid(pid1,&returnStatus,0);
                }
            }
            else if (strcmp(builtin,"chpmt")==0){
                if((builtin=strtok(NULL," "))!=NULL){
                    if(strcmp(builtin, "user")==0){
                        if((builtin=strtok(NULL," "))!=NULL){
                            if (strcmp(builtin,"0")==0){
                                user =0;
                                //printf("user:%d,machine: %d\n",user,machine);
                            }   
                            else if (strcmp(builtin,"1")==0){
                                user = 1;
                                //printf("user:%d,machine: %d\n",user,machine);
                            }
                            else{
                            } 
                        }
                    }else if (strcmp(builtin,"machine")==0){
                        if ((builtin=strtok(NULL," "))!=NULL){
                            if (strcmp(builtin,"0")==0){
                                machine = 0;
                                //printf("user:%d,machine: %d\n",user,machine);
                            }
                            else if (strcmp(builtin,"1")==0){
                                machine = 1;
                                //printf("user:%d,machine: %d\n",user,machine);
                            }
                            else {
                            }
                        }
                    }

                }
            }else if (strcmp(builtin,"chclr")==0){
                if((builtin=strtok(NULL," "))!=NULL){
                    if(strcmp(builtin, "user")==0){
                        if((builtin=strtok(NULL," "))!=NULL){
                            if (strcmp(builtin,"black")==0){
                                usercl =0;
                            }   
                            else if (strcmp(builtin,"red")==0){
                                usercl = 1;
                            }  
                            else if (strcmp(builtin,"green")==0){
                                usercl = 2;
                            }  
                            else if (strcmp(builtin,"yellow")==0){
                                usercl = 3;
                            }  
                            else if (strcmp(builtin,"blue")==0){
                                usercl = 4;
                            }  
                            else if (strcmp(builtin,"magenta")==0){
                                usercl = 5;
                            }  
                            else if (strcmp(builtin,"cyan")==0){
                                usercl = 6;
                            }  
                            else if (strcmp(builtin,"white")==0){
                                usercl = 7;
                            }
                            else{
                            }
                            if((builtin=strtok(NULL," "))!=NULL){
                                if (strcmp(builtin,"0")==0){
                                    userbold=0;
                                }
                                else if (strcmp(builtin,"1")==0){
                                    userbold = 1;
                                }else {
                                }
                            }
                        }
                    }else if (strcmp(builtin,"machine")==0){
                        if ((builtin=strtok(NULL," "))!=NULL){
                            if (strcmp(builtin,"black")==0){
                                machinecl =0;
                                //printf("user:%d,machine: %d\n",user,machine);
                            }   
                            else if (strcmp(builtin,"red")==0){
                                machinecl= 1;
                                //printf("user:%d,machine: %d\n",user,machine);
                            }  
                            else if (strcmp(builtin,"green")==0){
                                machinecl = 2;
                                //printf("user:%d,machine: %d\n",user,machine);
                            }  
                            else if (strcmp(builtin,"yellow")==0){
                                machinecl = 3;
                                //printf("user:%d,machine: %d\n",user,machine);
                            }  
                            else if (strcmp(builtin,"blue")==0){
                                machinecl = 4;
                                //printf("user:%d,machine: %d\n",user,machine);
                            }  
                            else if (strcmp(builtin,"magenta")==0){
                                machinecl = 5;
                                //printf("user:%d,machine: %d\n",user,machine);
                            }  
                            else if (strcmp(builtin,"cyan")==0){
                                machinecl = 6;
                                //printf("user:%d,machine: %d\n",user,machine);
                            }  
                            else if (strcmp(builtin,"white")==0){
                                machinecl = 7;
                                //printf("user:%d,machine: %d\n",user,machine);
                            }
                            else{
                            }
                            if((builtin=strtok(NULL," "))!=NULL){
                                if (strcmp(builtin,"0")==0){
                                    machinebold=0;
                                }
                                else if (strcmp(builtin,"1")==0){
                                    machinebold = 1;
                                }else {
                                }
                            }
                        }
                    }

                }
            }else{//part 2 start
                char *token;
                int argindex=0;
                int maxindices=10;
                char** arguments=malloc(100);
                
                do{
                    token = strsep(&cmd," ");
                    arguments[argindex]=token;
                    argindex++;
                 }while(token!=NULL);

                 pid_t pid=fork();

                 if (pid == 0){
                    exitCOde= execvpe(arguments[0], arguments, envp);
                    if(exitCOde==0){
                        fprintf(stderr,"%s: invalid command\n", builtin);
                    }
                 } else {
                    int returnStatus;
                    waitpid(pid, &returnStatus, 0);
                 }        
                 free(arguments);
            
            }
        
        }




        //All your debug print statments should be surrounded by this #ifdef
        //block. Use the debug target in the makefile to run with these enabled.
        #ifdef DEBUG
        fprintf(stderr, "Length of command entered: %ld\n", strlen(cmd));
        #endif
        //You WILL lose points if your shell prints out garbage values.
        
        char *username = getenv("USER");
        char dash[] ="-";
        char at[] = "@";
        char usersettings[13];
        char machinesettings[13];
        char *host=malloc(100);
        gethostname(host, sizeof(host));
        char regsettings[13];
        //cmd = (char*)realloc(cmd,strlen(user)+strlen(host)+10);
        sprintf(usersettings, "%c[%d;%d;%dm", 0x1B, userbold, usercl + 30, 40);
        sprintf(machinesettings, "%c[%d;%d;%dm", 0x1B, machinebold, machinecl + 30, 40);
        sprintf(regsettings, "%c[%d;%d;%dm", 0x1B, 0 ,37, 40);
        if((user ==0)&&(machine ==0)){
            strcpy(cmd,"sfish");
        }else if ((user==0)&&(machine==1)){
            strcpy(cmd,"sfish");
            strcat(cmd,dash);
            strcat(cmd,machinesettings);
            strcat(cmd,host);
        }else if((user==1)&&(machine==0)){
            strcpy(cmd,"sfish");
            strcat(cmd,dash);
            strcat(cmd,usersettings);
            strcat(cmd,username);
        }else{
            strcpy(cmd,"sfish");
            strcat(cmd,dash);
            strcat(cmd,usersettings);
            strcat(cmd,username);
            strcat(cmd,at);
            strcat(cmd,machinesettings);
            strcat(cmd,host);
        } 
        strcat(cmd,regsettings);
        ///  crashing here?
        char currd[100];
        getcwd(currd,sizeof(currd));
        strcat(cmd,":[");
        if(strcmp(currd,getenv("HOME"))==0){
            strcat(cmd,"~");
        }else{
            //strsep(currd,getenv("HOME"));
            strcat(cmd,currd);
        }
         
        strcat(cmd,"]> ");   
        
        //free(host);
        
    }

    //Don't forget to free allocated memory, and close file descriptors.
    free(cmd);
    //WE WILL CHECK VALGRIND!

    return EXIT_SUCCESS;
}
