#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t child_pid = fork();

    if (child_pid > 0) {
        printf("Parent process (PID: %d) is sleeping for 30 seconds...\n", getpid());
        printf("Check the process list now using: ps -ef | grep zombie\n");
        
        sleep(30); 
        
        printf("Parent is waking up and exiting. Zombie should disappear now.\n");
    } else if (child_pid == 0) {
        printf("Child process (PID: %d) is exiting and becoming a zombie...\n", getpid());
        exit(0);
    } else {
        perror("fork failed");
        return 1;
    }

    return 0;
}