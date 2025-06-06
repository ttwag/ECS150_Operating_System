#include <sys/stat.h> // fstat()
#include <fcntl.h> // open()
#include <unistd.h> // lseek(), read(), write()
#include <stddef.h> // size_t
#include "disk.h"

// byte size of each block
#define BLOCK_SIZE 4096

// the file descriptor
static int fd = -1;

int block_disk_open(const char *disk_filename) {
    if (!disk_filename) return -1;
    fd = open(disk_filename, O_RDWR);
    if (fd == -1) {
        perror("Error opening file");
        return -1;
    }
    return 0;
}

int block_disk_close(void) {
    if (fd == -1) return -1;
    close(fd);

    // reset the file descriptor
    fd = -1;
    return 0;
}

int block_read(size_t block_num, void *buf) {
    if (fd == -1) return -1;
    if (block_num >= block_disk_count()) return -1;
    if (!buf) return -1;
    
    // seek to the byte offset, then write
    size_t byte_offset = block_num * BLOCK_SIZE;
    if (lseek(fd, byte_offset, SEEK_SET) == -1) {
        perror("Error seeking");
        return -1;
    }
    ssize_t bytes_read = read(fd, buf, BLOCK_SIZE);
    if (bytes_read == -1 || bytes_read != BLOCK_SIZE) {
        perror("Error reading a block");
        return -1;
    }
    return 0;
}

int block_write(size_t block_num, const void *buf) {
    if (fd == -1) return -1;
    if (block_num >= block_disk_count()) return -1;
    if (!buf) return -1;

    // seek to the byte offset, then write
    size_t byte_offset = block_num * BLOCK_SIZE;
    if (lseek(fd, byte_offset, SEEK_SET) == -1) {
        perror("Error seeking");
        return -1;
    }
    ssize_t bytes_written = write(fd, buf, BLOCK_SIZE);
    if (bytes_written == -1 || bytes_written != BLOCK_SIZE) {
        perror("Error writing a block");
        return -1;
    }
    return 0;
}

int block_disk_count(void) {
    // check if a disk is open
    if (fd == -1) return -1;

    // get file size in bytes
    struct stat file_stat = {};
    if (fstat(fd, &file_stat) == -1) {
        perror("Cannot get file size");
        return -1;
    }
    
    if (file_stat.st_size % BLOCK_SIZE != 0) return -1; // corrupted
    
    // number of blocks is file size divided by block size
    return file_stat.st_size / BLOCK_SIZE;
}