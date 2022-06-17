#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <error.h>
#include "sys/wait.h"

#include "exec.h"
#include "lexer.h"

const char *operands[] = {
    "|",
    "||",
    ">",
    ">>",
    "&&"
};

int valery_exec2(struct tokens_t *tokens)
{
    // while(tokens->pipes-- > 0) 
    // 1. fork().
    // 2. named pipe()?.
    // 3. execute current token with potential args (next token)
    // 4. write/redirect result originally for stdout pipe(),
}

void restore_normal_stream_behaviour(int saved_stdin, int saved_stdout)
{
    /* restore normal steam behavior */
    dup2(saved_stdin, STDIN_FILENO);
    close(saved_stdin);
    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdout);
}

int prototype(char *first_cmd, char **first, char *second_cmd, char **second, int second_to_stdout)
{
    int status;
    int rc = 0;
    char *env[] = {NULL, NULL, NULL};
    /* make saves of stdout and stdin file descriptors */
    int saved_stdin = dup(STDIN_FILENO);
    int saved_stdout = dup(STDOUT_FILENO);

    int pipefd[2];
    if (pipe(pipefd) == -1)
       return -1;

    int pid = fork();
    if (pid == 0) {
        /* close read end of pipe as it is not used */
        close(pipefd[0]);
        /* redirect stdout into write end of pipe */
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            restore_normal_stream_behaviour(saved_stdin, saved_stdout);
            return -1;
        }
        rc = execve(first_cmd, first, env);
        rc == -1 ? exit(EXIT_FAILURE) : exit(EXIT_SUCCESS);
    }

    /* guarantees child process is executed first */
    waitpid(pid, &status, 0);

    /* parent process */
    if (status != 0) {
        restore_normal_stream_behaviour(saved_stdin, saved_stdout);
        return 1;
    }


    /* redirect stdin to read from pipe */
    if (dup2(pipefd[0], STDIN_FILENO) == -1) {
        restore_normal_stream_behaviour(saved_stdin, saved_stdout);
        return -1;
    }

    if (second_to_stdout)
        dup2(pipefd[1], STDOUT_FILENO);

    /* close write end of pipe as it is not used */
    close(pipefd[1]);

    /* fork process second time */
    pid = fork();
    /* this goes to stdin, so we don't need to redirect any output streams */
    if (pid == 0) {
        int rc = execve(second_cmd, second, env);
        rc == -1 ? exit(EXIT_FAILURE) : exit(EXIT_SUCCESS);
    }

    waitpid(pid, &status, 0);

    /* close read end of pipe as we are done reading */
    close(pipefd[1]);
    restore_normal_stream_behaviour(saved_stdin, saved_stdout);
    return rc != 0;
}


int main()
{
    char *first_cmd = (char *) malloc(sizeof(char) * 1024);
    char *first_args = (char *) malloc(sizeof(char) * 1024);
    char **first = (char **) malloc(sizeof(char *) * 2);
    first[0] = first_cmd;
    first[1] = first_args;
    strcpy(first_cmd, "/bin/ls");
    strcpy(first_args, "-la");

    char *second_cmd = (char *) malloc(sizeof(char) * 1024);
    char *second_args = (char *) malloc(sizeof(char) * 1024);
    char **second = (char **) malloc(sizeof(char *) * 2);
    second[0] = second_cmd;
    second[1] = second_args;
    strcpy(second_cmd, "/bin/grep");
    strcpy(second_args, ".h");

    

    prototype(first_cmd, first, second_cmd, second, 1);


    printf("stdout works as expected. Type in char>");
    int a = getc(stdin);
    printf("\nstdin works as expcted: %c\n", a);
}

