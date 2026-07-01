#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <program_to_supervise>\n", argv[0]);
        return 1;
    }

    while (1) {
        int status;
        int pipefd[2];

        if (pipe(pipefd) != 0) {
            perror("Fatal: Failed to instantiate IPC pipe");
            return 1;
        }

        pid_t pid = fork();


        if (pid == 0) {
            close(pipefd[0]); // Close read end
            dup2(pipefd[1], STDOUT_FILENO);

            execvp(argv[1], &argv[1]);


            perror("Lazarus: Failed to execute target binary");
            exit(127); 
        }

        close(pipefd[1]); // Close write end immediately

        // Allocate a safe, static 1KB buffer on the Stack.
        char buffer[1024]; 
        ssize_t bytes_read;


        while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
            
            buffer[bytes_read] = '\0'; 
            printf("[LAZARUS] %s", buffer);
            fflush(stdout); 
        }

        close(pipefd[0]); 

        waitpid(pid, &status, 0);


        if (WIFEXITED(status)) {
            printf("[LAZARUS AUTOPSY] Process exited voluntarily with code: %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            int sig = WTERMSIG(status);
            if (sig == 11) {
                printf("[LAZARUS AUTOPSY] Process suffered a Segmentation Fault (Signal 11)\n");
            } else {
                printf("[LAZARUS AUTOPSY] Process killed by external Signal: %d\n", sig);
            }
        }

        printf("\n[LAZARUS] Resurrecting target in 2 seconds...\n-----------------------------\n");
        sleep(2);
    }

    return 0;
}