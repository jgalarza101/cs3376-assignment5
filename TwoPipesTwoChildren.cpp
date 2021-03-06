#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
	int status;

	char *cat_args[] = {"ls", "-ltr", NULL};
	char *grep_args[] = {"grep", "3376", NULL};
	char *wc_args[] = {"wc", "-l", NULL};
	
	// create two pipes to send the output of "ls" process to "grep" process and to "wc" process
	int pipe1[2];
	int pipe2[2];

	pipe(pipe1);
	pipe(pipe2);

	pid_t pid_1, pid_2;

	// first child process
	if( !(pid_1 = fork()) ) {
		close(pipe1[0]);
		dup2(pipe1[1], 1); /* redirect standard output to pipe_1 write end */
		close(pipe1[1]);
		execvp(*cat_args, cat_args);
		exit(0);
	}
	
	// second child process
	else if( !(pid_2 = fork()) ) {
		close(pipe1[1]);
		dup2(pipe1[0], 0); /* redirect standard input to pipe_1 read end */
		close(pipe1[0]);
		close(pipe2[0]);
		dup2(pipe2[1], 1); /* redirect standard output to pipe_2 write end */
		close(pipe2[1]);
		execvp(*grep_args, grep_args);
	}
	
	// parent process
	else {
		close(pipe1[1]);
		close(pipe1[0]);
		close(pipe2[1]);
		dup2(pipe2[0], 0); /* redirect standard input to pipe_2 read end */
		close(pipe2[0]);
		execvp(*wc_args, wc_args);
	}
	
	close(pipe2[1]);
	close(pipe2[0]);

	return(0);
}
