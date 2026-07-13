#include <iostream>
#include <sched.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <unistd.h>

const size_t STACK_SIZE = 1024 * 1024;

int container_main(void* arg) {
    std::cout << "[Container] Activating filesystem isolation...\n";
    
    // 1. Jail the process inside our extracted directory layout
    if (chroot("/home/rotich-kibet/mini_rootfs") != 0) {
        perror("chroot failed");
        return 1;
    }
    
    // 2. Move our working directory to the new virtual root
    if (chdir("/") != 0) {
        perror("chdir failed");
        return 1;
    }

    // 3. Mount a completely clean, isolated proc filesystem for ps aux to read
    // target: "/proc", filesystemtype: "proc"
    if (mount("proc", "/proc", "proc", 0, NULL) != 0) {
        perror("proc mount failed");
        return 1;
    }

    std::cout << "[Container] Booting isolated interactive shell...\n";
    sethostname("isolated-sandbox", 16);

    char* container_args[] = { (char*)"/bin/bash", NULL };
    execv(container_args[0], container_args);

    perror("execv failed");
    return 1;
}

int main() {
    std::cout << "[Host] Spawning isolated namespace container...\n";

    char* stack = new char[STACK_SIZE];

    // Spawning with PID namespace and Mount (NS) namespace boundaries activated
    pid_t child_pid = clone(
        container_main, 
        stack + STACK_SIZE, 
        CLONE_NEWPID | CLONE_NEWNS | SIGCHLD, 
        NULL
    );

    if (child_pid == -1) {
        perror("clone failed");
        delete[] stack;
        return 1;
    }

    waitpid(child_pid, NULL, 0);
    
    std::cout << "[Host] Container destroyed.\n";
    delete[] stack;
    return 0;
}