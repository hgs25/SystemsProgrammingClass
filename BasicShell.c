/*
* Tokenizes an inputted string. delimiters are space, tab, and '|'.
* @author Juan Valenzuela
* @version 2.5.17
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

char input[150]; //char array for input string
char * tokens[50], * commands[50], *files[50]; //array of pointers to tokens, commands, and files
int tokenNum=0, cmdnum=0; //number of tokens

void printDir()
{
  char cwd[1024];
  if(getcwd(cwd, sizeof(cwd)) != NULL)
    printf("Current working dir: %s\n", cwd);
  else
    perror("Could not get current directory");
}

int main()
{
  int i, success, cstatus;
  memset(input, 0, sizeof(input)); //ensures that array begins empty
  memset(tokens, 0, sizeof(tokens));
  memset(commands, 0, sizeof(commands));
  printf("please input commands\n");
  scanf("%[^\n]", input);
  if(input[0] != '\0') //skip if theres no input
  {
    //save pointers returned by strtok() to tokens array
    tokens[0]=strtok(input, " |\t\v");
    if(tokens[0] != NULL) //check if no non-delimiter character was inputted
      tokenNum++;
    while(tokens[tokenNum-1] != NULL) //continues if previous token isn't empty
    {
      tokens[tokenNum]=strtok(NULL, " |\t\v");
      if(tokens[tokenNum] != NULL)
        tokenNum++; //add to token count if the current token contains a string
      else
        break; //leave the while loop if the current token is empty
    }
  }

  //copy commands/files in tokens[] into commands[] and files[]
  int j=0, k=0;
  for(i=0; i<tokenNum; i++)
  {
    if(strcmp(tokens[i], ">") == 0 || strcmp(tokens[i], "<") == 0)
    {
      files[k] = tokens[i+1];
      k++;
      i++;
    }
    else if(strcmp(tokens[i], "cd") == 0 || strcmp(tokens[i], "pwd") == 0)
    {
      commands[j] = tokens[i];
      cmdnum++;
      files[k] = tokens[i+1];
      k++;
      i++;
    }
    else
    {
      commands[j] = tokens[i];
      cmdnum++;
      j++;
    }
  }

  //print list of tokens and commands
  for(i=0; i<tokenNum; i++)
  {
    printf("\n%d: %s", i+1, tokens[i]); //prints each token string
  }
  printf("\nthere are %d tokens.\n", tokenNum); //prints the number of tokens
  for(i=0; i<cmdnum; i++)
  {
    printf("\n%d: %s", i+1, commands[i]); //prints each token string
  }
  printf("\nthere are %d commands.\n", cmdnum); //prints the number of tokens

  //check for possible redirection
  fpos_t redirect_out_pos;
  int redirect_out_fd, redirect_in_fd;

  if(tokens[1] != NULL)
  {
    if(strcmp(tokens[1], ">") == 0)
    {
      fflush(stdout);
      fgetpos(stdout, &(redirect_out_pos));
      redirect_out_fd = dup(fileno(stdout));
      freopen(files[0], "w", stdout);
    }
    else if(strcmp(tokens[1], "<") == 0)
    {
      if(access(files[0], R_OK) >= 0)
      {
        redirect_in_fd = dup(fileno(stdin));
        freopen(files[0], "r", stdin);
      }
      else
      {
        perror("input redirection ");
        return 1;
      }
    }
  }

  //check if built in
  if(strcmp(commands[0], "cd") == 0) //change directory if the 1st token is cd
  {
    char * newDirect; //new directory we will be changing to
    if(files[0] != NULL)
    {
      newDirect = tokens[1]; //change directory to 2nd argument if there is one
    }
    else
    {
      newDirect = "/home"; //default change to home directory
    }

    success = chdir(newDirect);
    if(success == 0)
      printf("changed directory\n");
    else
      perror("Unable to change directory ");
    printDir(); //print the current working directory
  }
  else if(strcmp(commands[0], "pwd") == 0) //print working directory if the 1st token is pwd
  {
    printDir();
  }
  else if(strcmp(commands[0], "exit") == 0) //exit if the 1st token is exit
  {
    printf("I quit\n");
    exit(0);
  }
  else
  {
    pid_t pid = fork(), c;

    if(pid == 0) //child process
    {
      printf("Child PID = %ld\n", (long) getpid());
      success = execvp(commands[0], commands);
      //error check
      if(success == -1)
      {
        perror("Child: Child could not execute command. ");
        _exit(1);
      }
    }

    else if(pid > 0) //parent process
    {
      c = wait(&cstatus); //wait for child to end
      printf("Parent: Child %ld has exited with status = %d\n", (long) c, cstatus);
    }

    else
    {
      fprintf(stderr, "Fork Failed\n");
      exit(1);
    }
  }
  //redirection cleanup
  if(tokens[1] != NULL)
  {
    if(strcmp(tokens[1], ">") == 0)
    {
      fflush(stdout);
      dup2(redirect_out_fd, fileno(stdout));
      close(redirect_out_fd);
      clearerr(stdout);
      fsetpos(stdout, &(redirect_out_pos));
    }
    else if(strcmp(tokens[1], "<") == 0)
    {
      dup2(redirect_in_fd, fileno(stdin));
      close(redirect_in_fd);
    }
  }

  return 0;
}
