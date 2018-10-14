#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <sys/wait.h>
#include <sys/stat.h> 
#include <fcntl.h>

#define word_len 1000
#define sent_len 100
#define total_len 100000

typedef int bool;
#define true 1
#define false 0

void parse_string(char *input, char **input_token){

	int i, j;
	char c, input2[total_len];
	
	for(i=0,j=0; i<strlen(input); ++i, ++j){
		
		c = *(input+i);
		if( c=='>' || c=='&' || c=='<' || c=='|'){
			input2[j]=' ';
			input2[j+1]=input[i];
			input2[j+2]=' ';
			j+=2;
		}
		else input2[j]=input[i];
	}

	strcpy(input, input2);

	//Read until newline and change i-th input to '\0'.
	for(i=0; i<strlen(input); ++i ){
		if(input[i] == '\n'){	
			input[i]='\0';
			break;
		}
	}

	fflush(stdout);
	// Parse input to tokens.
	i=0;	
	char delim[10]="\t \r \n";
	input_token[0]=strtok(input, delim);

	//printf("input_token%d : %s\n",i, input_token[i]);
	
	while(input_token[i] != NULL){
		++i;
		input_token[i] = strtok(NULL, delim);
		//printf("input_token%d : %s\n",i,input_token[i]);
	}

	return ;
}

void childSignalHandler(int signo){

	int status;
	while(waitpid(-1, &status, WNOHANG)>0);
}

bool redirect_check(char **instruction, char *file){

	int i=0;
	bool check_redir=false;
	while(instruction[i] != NULL){

		//printf("afasd:%s\n",instruction[i]);
		if(strcmp(instruction[i],">") == 0){

			check_redir=true;
			break;
		}
		++i;
	}
	if(check_redir == true){

		strcpy(file, instruction[i+1]);
		instruction[i]=NULL;
	}	
	
	return check_redir;
}

bool pipe_check(char **instruction, char ***instruction2){
	
	int i=0;
	bool check_pipe=false;
	while(instruction[i] != NULL){

		if(strcmp(instruction[i], "|") == 0){
			check_pipe=true;
			break;
		}
		++i;
	}

	if(check_pipe == true){
		
		*instruction2 = &instruction[i+1];
		instruction[i] = NULL;
	}

	return check_pipe;
}

void shell(char **instruction, int wait) {

	int status,fd[2];
	char file[100], **instruction2;
	bool check_pipe=false, check_redir=false;

	if(strcmp(instruction[0], "sleep") == 0 && wait == 0) 
		signal(SIGCHLD, childSignalHandler);
	
	check_redir = redirect_check(instruction, file);

	if((check_pipe=pipe_check(instruction,&instruction2)) == true){
		pipe(fd);
	}

	pid_t pid1;
	pid1 = fork();
	if(pid1 < 0){
		// error
		printf("{error while making child}");
		exit(-1);
	}
	else if(pid1 == 0){
		// child
		if(check_redir == true){
			int fd=open(file,\
				O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
			dup2(fd, 1);
			close(fd);
			execvp(instruction[0], instruction);
			exit(0);
		}
		else if(check_pipe == true){

			close(fd[0]);
			dup2(fd[1], STDOUT_FILENO); //stdout to pipe(Write)
			close(fd[1]);
			execvp(instruction[0], instruction);
			exit(0);
		}
		else{
			//printf("%s\n", instruction[0]);
			execvp(instruction[0], instruction);
			printf("{ERROR Not exist:%s}\n", instruction[0]);
			fflush(stdout);
			exit(0);
		}
	}
	else{
		// parent
		if(check_pipe == true){
			
			close(fd[1]);
			waitpid(pid1, &status, 0);
			pid_t pid2;
			pid2 = fork();

			if(pid2<0){
				printf("{error}\n");
				exit(-1);
			}

			else if(pid2 == 0){
				//child
				//close(fd[1]);
				dup2(fd[0], STDIN_FILENO); //stdin導向pipe(Read)
				close(fd[0]);
				execvp(instruction2[0], instruction2);
				exit(0);
			}

			else{
				int status2;
				close(fd[0]);
				waitpid(pid2, &status2, 0);
			}

		}
		
		if(strcmp(instruction[0], "sleep") != 0 && check_pipe != true)
			waitpid(pid1, &status, 0);
		
		if(wait == 1) waitpid(pid1, &status, 0);
		
		return ;
	}
}

void run(char **input_token){
	
	int i=0, j=0, tmp=0;
	char *instruction[sent_len];

	while(input_token[i] != NULL){

		instruction[tmp]=input_token[i];
		if(strcmp(input_token[i], "&") == 0){
			
			printf("%s\n", instruction[tmp]);
			instruction[tmp]=NULL;			
	
			shell(instruction, 0);

			// clear the instruction
			for(j=0; j<tmp; ++j) instruction[j] = NULL;
			tmp=0;
			++i;
		}

		if((instruction[tmp] = input_token[i]) == NULL) break;
		++tmp;
		++i;
	}
	instruction[tmp]=NULL;

	if(instruction[0] != NULL) shell(instruction, 1);
}

void free_buffer(char *input, char **input_delim){
	int i;
	for(i=0; i<strlen(input); i++) *(input+i)='\0';

	i=0;
	while(input_delim[i] != NULL){
		input_delim[i]=NULL;
		++i;
	}
}

int main(){

	char *input = (char *)malloc(total_len * sizeof(char));
	char *input_token[sent_len];
	
	printf(">");
	while(fgets(input,total_len,stdin)){


		if(strcmp(input,"exit\n") == 0) exit(0);

		parse_string(input, input_token);
		
		run(input_token);
		free_buffer(input, input_token);
		
		fflush(stdout);
		printf(">");
	}
	return 0;
}
