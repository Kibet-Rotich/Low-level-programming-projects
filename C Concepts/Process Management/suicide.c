// suicide.c - Our unstable test subject
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main() {
    srand(time(NULL));
    printf("Server booting up... PID: %d\n", getpid());
    sleep(1);
    printf("Connecting to database...\n");
    sleep(2);

    int doom = rand() % 3;
    if (doom == 0) {
        printf("CRITICAL: Database disconnected!\n");
        exit(42); // Normal exit, but with a failure error code
    } else if (doom == 1) {
        printf("Simulating bad pointer write...\n");
        int *p = NULL; *p = 10; // Guaranteed Segmentation Fault (Signal 11)
    } else {
        printf("Working normally...\n");
        sleep(2);
        abort(); // Hardware Abort (Signal 6)
    }
    return 0;
}