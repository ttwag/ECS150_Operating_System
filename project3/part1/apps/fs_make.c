#include <stdio.h>
#include <stdlib.h> //atoi() and exit()
#include <libc.h> // perror()
#include <sys/stat.h> // fstat()
#include <fcntl.h> // open()
#include <unistd.h> // lseek(), read(), write()
#include <stddef.h> // size_t

#define BLOCK_SIZE 4096
#define FAT_EOC 0xFFFF

int main(int argc, const char *argv[]) {
    if (argc > 3) {
        printf("Error: too many arguments\n");
        exit(1);
    }
    else if (argc == 1 || argc == 2) {
        printf("Error: tell me the disk name and number of blocks\n");
        exit(1);
    }
    int data_block = atoi(argv[2]);
    if (data_block % 2 != 0) {
        printf("Number of data block should be multiples of 2\n");
        exit(1);
    }
    else if (data_block > 8192) {
        printf("Number of blocks exceeds current limit: 8192 blocks\n");
        exit(1);
    }
    const char *file_name = argv[1];
    const uint16_t num_data_blk = data_block;
    const uint8_t num_fat_blk = (num_data_blk * 2 + BLOCK_SIZE  - 1 ) / BLOCK_SIZE;
    const uint16_t num_total_blk = 2 + num_data_blk + num_fat_blk;

    const uint16_t root_dir_index = 1 + num_fat_blk;
    const uint16_t data_index = root_dir_index + 1;
    const uint8_t empty = 0;
    const uint8_t null_char = '\0';
    const uint8_t zero_buf[BLOCK_SIZE] = {};

    // open file
    int fd = open(file_name, O_CREAT | O_TRUNC | O_RDWR, S_IRWXU);
    if (fd < 0) {
        perror("Error opening file");
        exit(1);
    }
 
    // initialize super block
    write(fd, "ECS150FS", 8);
    write(fd, &num_total_blk, 2);
    write(fd, &root_dir_index, 2);
    write(fd, &data_index, 2);
    write(fd, &num_data_blk, 2);
    write(fd, &num_fat_blk, 1);
    write(fd, zero_buf, 4079);

    // initialize FAT block
    uint16_t fat_eoc = 0xFFFF;
    write(fd, &fat_eoc, 2);
    // initialize the rest to 0
    for (int i = 0; i < num_fat_blk * BLOCK_SIZE - 2; i++) {
        if (write(fd, &empty, 1) < 0) {
            perror("Error writing file");
            exit(1);
        }
    }

    // initialize root directory block
    for (int i = 0; i < 128; i++) {
        write(fd, &null_char, 1);
        write(fd, zero_buf, 31);
    }

    // initialize data blocks

    for (int i = 0; i < num_data_blk; i++) {
        if (write(fd, zero_buf, BLOCK_SIZE) < 0) {
            perror("Error writing file");
            exit(1);
        }
    }
    
    struct stat file_stat = {};
    fstat(fd, &file_stat);
    printf("\nSuccessfully formatted disk: %s\nNumber of Blocks: %lld\n\n", file_name, file_stat.st_size / BLOCK_SIZE);
    
    close(fd);
    return 0;
}

