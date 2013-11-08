/* Manpage PIPE(2) Example */
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define _POSIX_C_SOURCE 4

void exitError(const char *errstr) {
    perror(errstr);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int pipefd[2];
    pid_t cpid;
    char buf;
    FILE *read_stream;
    FILE *write_stream;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (pipe(pipefd) == -1)
        exitError("pipe");

    switch(cpid = fork()) {
    case -1:
        exitError("fork");
    case 0:
        /* We will read from the pipe */
        close(pipefd[1]);

        if((read_stream = fdopen(pipefd[0], "r")) != NULL)
            exitError("fdopen");

        while (fgets(&buf, 1, pipefd[0]) > 0)
            fputs(&buf, stdout);

        fputs("\n", stdout);
        close(pipefd[0]);
        _exit(EXIT_SUCCESS);

    default:
    /* Parent writes argv[1] to pipe */
        close(pipefd[0]);          /* Close unused read end */
        write(pipefd[1], argv[1], strlen(argv[1]));
        close(pipefd[1]);          /* Reader will see EOF */
        wait(NULL);                /* Wait for child */
        exit(EXIT_SUCCESS);
    }
}
