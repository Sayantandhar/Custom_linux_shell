#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<stdbool.h>
#include<sys/wait.h>
#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

/* Splits the string by space and returns the array of tokens
*
*/
char **tokenize(char *line)
{
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0, tokenNo = 0;

  for(i =0; i < strlen(line); i++){

    char readChar = line[i];

    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
      token[tokenIndex] = '\0';
      if (tokenIndex != 0){
	tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
	strcpy(tokens[tokenNo++], token);
	tokenIndex = 0; 
      }
    } else {
      token[tokenIndex++] = readChar;
    }
  }
 
  free(token);
  tokens[tokenNo] = NULL ;
  return tokens;
}

void execArgs(char** parsed)
{
    pid_t pid = fork(); 
  
    if (pid == -1) {
        printf("\nFailed forking child..");
        return;
    } else if (pid == 0) {
        if (execvp(parsed[0], parsed) < 0) {
            printf("\nCould not execute command..");
        }
        exit(0);
    } else {
        waitpid(pid,NULL,0); 
        return;
    }
}

int back( char* lin){
	char* x = strtok(lin, "&");
	char** comm = tokenize(x);
	pid_t pid = fork(); 
  
    if (pid == -1) {
        printf("\nFailed forking child..");
        return 0;
    } else if (pid == 0) {
		printf("bg\n");
        if (execvp(comm[0], comm) < 0) {
            printf("\nCould not execute command..");
            return 0;
        }
        exit(0);
    }
    else {  
        return pid;
    }

}
void series(char* lin){
	char* x = strtok (lin, "&&");
	while(x!=NULL){
		char** comm = tokenize(x);
		execArgs(comm);
		x = strtok(NULL,"&&");
	}
	return;

}
void parallel(char* lin){
	char* x = strtok (lin, "&&&");
	int *y = (int*)malloc(MAX_TOKEN_SIZE * sizeof(int));
	int num =0;
	while(x!=NULL){
		char** comm = tokenize(x);
	pid_t pid = fork(); 
  
    if (pid == -1) {
        printf("\nFailed forking child..");
        return ;
    } else if (pid == 0) {
        if (execvp(comm[0], comm) < 0) {
            printf("\nCould not execute command..");
            return ;
        }
        exit(0);

	}
	else{
        	y[num++] = pid;
        }
	x = strtok(NULL,"&&&");
}
	for(int j = 0; j < num; j++){
		waitpid(y[j],NULL,0);
	}
	return ;
}

void killpro(int* y,int n){
	for(int i =0; i<n; i++){
		if(y[i]!=0){
			kill(y[i],SIGTERM);
		}
	}
}
int main(int argc, char* argv[]) {
	char  line[MAX_INPUT_SIZE];            
	char  **tokens;              
	int i,p;
	int *y = (int*)malloc(MAX_TOKEN_SIZE * sizeof(int));
	int n =0;
	while(1) {			
		/* BEGIN: TAKING INPUT */
		bzero(line, sizeof(line));
		printf("$ ");
		scanf("%[^\n]", line);
		getchar();
		bool spe = true;

		/* END: TAKING INPUT */

		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);

		
       //do whatever you want with the commands, here we just print them
		for (i=0;tokens[i]!=NULL;i++){
			if(strcmp(tokens[i],"&")==0){

				y[n++]=back(line);
				spe = false;
				break;
			}
			if(strcmp(tokens[i],"&&")==0){
				series(line);
				spe = false;
				break;
			}

			 if (strcmp(tokens[i],"&&&")==0){
				parallel(line);
				spe = false;
				break;
			}
			
		}
		if(strcmp(tokens[0],"exit")==0){
			spe=false;
			killpro(y,n);
			break;
		}
       		if(spe == true){
			execArgs(tokens);
		}

		// Freeing the allocated memory	
		for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);
		for(int j=0;j<n;j++){
			if(y[j]!=0){
			p = waitpid(y[j],NULL,WNOHANG);
			if(p!=0){
				printf("Shell: Background process finished\n");
				y[j]= 0;  
			}
		}
		}

	}
	return 0;

}
