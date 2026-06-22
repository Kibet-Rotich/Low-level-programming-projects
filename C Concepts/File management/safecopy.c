#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(int argc, char* argv[]){
    if(argc != 3){
        printf("usage ./safecopy [src file] [dest file]");
        return -1;
    }
    char* buffer = (char*)malloc(1024*4);

    if (buffer == NULL){
        printf("cannot allocate memory\n");
        return -1;
    }


    int fd_source = open(argv[1], O_RDONLY);
    printf("fdsource == %d \n", fd_source);
    if(fd_source == -1){
        printf("Cannot open source file.\n");
        return -1;
    }

    int fd_dest = open(argv[2], O_WRONLY|O_CREAT | O_TRUNC,0644);
    printf("fd_dest == %d \n", fd_dest);
    if(fd_dest==-1){
        printf("cannot open dest file\n");
        return -1;
    }

    while(1){
        int bytes_read = read(fd_source,buffer,4096);

        if(bytes_read == 0){
            printf("copy process completed or file is empty\n");
            break;
        }else if(bytes_read == -1){
            printf("error encountered while copying src file\n");
            goto cleanup;

        }else{
            int byteswritten = write(fd_dest,buffer,bytes_read);
            if(byteswritten == -1){
                printf("error occured while pasting to dest file\n");
                goto cleanup;
            }
        }

        

    }
    cleanup:
        free(buffer);
        close(fd_dest);
        close(fd_source);
        return 0;

    
    
}