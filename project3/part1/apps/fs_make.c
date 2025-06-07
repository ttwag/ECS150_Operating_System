#include <stdio.h>
#include <libc.h> // perror()
#include <sys/stat.h> // fstat()
#include <fcntl.h> // open()
#include <unistd.h> // lseek(), read(), write()
#include <stddef.h> // size_t

#define BLOCK_SIZE 4096

int main(int argc, const char *argv[]) {
    if (argc > 3) {
        printf("Error: too many arguments\n");
        exit(1);
    }
    else if (argc == 1 || argc == 2) {
        printf("Error: tell me the disk name and number of blocks\n");
        exit(1);
    }

    const char *file_name = argv[1];
    const int num_blk = atoi(argv[2]); 
    
    // open file
    int fd = open(file_name, O_CREAT | O_TRUNC | O_RDWR, S_IRWXU);
    if (fd < 0) {
        perror("Error opening file");
        exit(1);
    }
    
    // write 0s
    const char block[BLOCK_SIZE] = {};
    for (int i = 0; i < num_blk; i++) {
        if (write(fd, block, BLOCK_SIZE) < 0) {
            perror("Error writing file");
            exit(1);
        }
    }

    close(fd);

    printf("Successfully formatted disk: %s\n", file_name);
    return 0;
}

