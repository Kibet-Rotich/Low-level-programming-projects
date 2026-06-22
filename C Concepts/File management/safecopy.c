#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h> 

#define BUFFER_SIZE 4096

int main(int argc, char* argv[]) {

    int fd_source = -1;
    int fd_dest = -1;
    char* buffer = NULL;
    int exit_status = 0; 

    if (argc != 3) {
        fprintf(stderr, "Usage: %s [src_file] [dest_file]\n", argv[0]);
        return 1; 
    }

    buffer = (char*)malloc(BUFFER_SIZE);
    if (buffer == NULL) {
        perror("Memory allocation failed");
        return 1; 
    }

    fd_source = open(argv[1], O_RDONLY);
    if (fd_source == -1) {
        perror("Failed to open source file");
        exit_status = 1;
        goto cleanup; 
    }


    fd_dest = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_dest == -1) {
        perror("Failed to open destination file");
        exit_status = 1;
        goto cleanup; 
    }


    while (1) {

        ssize_t bytes_read = read(fd_source, buffer, BUFFER_SIZE);

        if (bytes_read == 0) {
            printf("Safecopy complete: %s -> %s\n", argv[1], argv[2]);
            break; // Success! Fall down into the cleanup block naturally.
        } 
        
        if (bytes_read == -1) {
            perror("Fatal error reading source chunk");
            exit_status = 1;
            goto cleanup;
        }

        // Push strictly the exact number of valid bytes scooped up
        ssize_t bytes_written = write(fd_dest, buffer, bytes_read);
        if (bytes_written == -1) {
            perror("Fatal error writing to destination");
            exit_status = 1;
            goto cleanup;
        }
    }


cleanup:
    if (buffer != NULL) {
        free(buffer);
    }
    if (fd_source >= 0) {
        close(fd_source);
    }
    if (fd_dest >= 0) {
        close(fd_dest);
    }

    return exit_status;
}