#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define _POSIX_SOURCE

int main(int argc, char **argv)
{

	int result;
    char buf[20];
	
	int pipefds[2];
	if(pipe(pipefds) != 0){
		perror("Pipes are screwed up, call a plumber");
		exit(-1);
	}
	
	switch((result = fork())){

	case -1:
        perror("fork");
        exit(EXIT_FAILURE);
		break;
	case 0:
		//child case

		close(pipefds[0]);
		FILE *output = fdopen(pipefds[1], "w");
		fputs(buf, output);
	
		break;
	default:
		//parent case -- result holds pid of child

		close(pipefds[1]);
		FILE *input = fdopen(pipefds[0], "r");
		fgets(buf, 10, input);

		break;
	}
	wait
	return 0;
}
