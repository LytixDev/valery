#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
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

int test_exec(char *path, char *args)
{
    int status;
    int return_code = 0;
    char *args_cpy = args;
    /* env unused in this case */
    char *env[] = {NULL, NULL, NULL};
    /* args being an empty string results in undefined behavior */
    if (strcmp(args_cpy, "") == 0)
        args_cpy = NULL;

    char *full[] = {path, args_cpy};

    pid_t new_pid = fork();
    if (new_pid == CHILD_PID) {
        return_code = execve(path, full, env);
        return_code == -1 ? exit(EXIT_FAILURE) : exit(EXIT_SUCCESS);
    }

    waitpid(new_pid, &status, 0);

    return status != 0;
}


int test()
{
    int fd[2];
    int status;

    if (pipe(fd) == -1)
        return 1;

    int id = fork();

    if (id == 0) {
        close(fd[0]);
        int x;
        printf("Input a number: ");
        scanf("%d", &x);

        if (write(fd[1], &x, sizeof(int)) == -1) {
            close(fd[1]);
            exit(EXIT_FAILURE);
        }

        close(fd[1]);
        exit(EXIT_SUCCESS);

    } else {
        close(fd[1]);
        int y;
        if (read(fd[0], &y, sizeof(int)) != -1) {
            close(fd[0]);
            printf("Read from child process %d\n", y);
        }
        close(fd[0]);
    }

    waitpid(id, &status, 0);
 
    return 0;
}


int main()
{
    char buf[1024] = "/bin/ls -la | /bin/grep *.c";
    struct tokens_t *tokens = new_tokens_t();
    tokenize(tokens, buf);
    char path[1024] = "/bin/ls";
    char args[1024] = "-la";

    test_exec(path, args);
}

//https://stackoverflow.com/questions/29154056/redirect-stdout-to-a-file
