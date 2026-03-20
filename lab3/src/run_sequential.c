#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    pid_t child_pid = fork();

    if (child_pid == 0) {
        char *args[] = {"./sequential_min_max", "42", "100", NULL};
        
        execv("./sequential_min_max", args);

        perror("execv failed");
        exit(1);
    } else if (child_pid > 0) {
        wait(NULL);
        printf("Sequential min_max finished execution.\n");
    } else {
        perror("fork failed");
        return 1;
    }

    return 0;
}