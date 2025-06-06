#ifndef DISK_H
#define DISK_H

#include <stddef.h>

// the size of a block is 4096 byte

/*
 * block_disk_open - open the disk
 * @filename: name of the disk
 * Return: 0 if success, -1 if failure
 */
int block_disk_open(const char *disk_filename);

/*
 * block_disk_close - close the disk
 * Return: 0 if success, -1 if failure
 */
int block_disk_close(void);

/*
 * block_read - read a block in the disk
 * @block_num: block offset
 * @buf: buffer that would receive a copy of the block
 * Return: 0 if success, -1 if failure
 */
int block_read(size_t block_num, void *buf);

/*
 * block_write - read a block in the disk
 * @block_num: block offset
 * @buf: buffer whose contents will be written back to the block
 * Return: 0 if success, -1 if failure
 */
int block_write(size_t block_num, const void *buf);

/*
 * block_disk_count - returns the number of blocks in a disk
 * Return: number of blocks, -1 if failure
 */
int block_disk_count(void);

#endif //DISK_H




/*
Step 1: Start with the Header File (disk.h)
First, think about what this module needs to provide:

A way to open/close a virtual disk file
A way to read/write blocks of data
A way to find out how many blocks exist

Questions for you:

What should the block size be? (Check the project specs)
What functions do you need to declare?
What should each function return on success vs failure?

Try writing the header file first with:

Include guards
A #define for block size
Function declarations with good documentation

Step 2: Think About Global State
For the implementation (disk.c), consider:

How will you keep track of which disk file is currently open?
What information do you need to store about the open disk?

Hint: You'll probably need at least:

A file descriptor
The number of blocks in the disk

Step 3: Implement block_disk_open()
This function should:

Check if a disk is already open
Open the file
Figure out how many blocks it contains
Store this information for later use

Questions to think about:

Which system call opens a file?
How can you find the size of a file? (Look up fstat())
How do you calculate blocks from file size?
What if the file size isn't a multiple of block size?

Step 4: Implement block_disk_close()
This should:

Close the file
Reset your global state

Question: What should you return if no disk is open?
Step 5: Implement the Read/Write Functions
For both block_read() and block_write():

Validate inputs (is a disk open? is block number valid?)
Calculate the byte offset in the file
Seek to that position
Read or write exactly one block

Key questions:

How do you convert a block number to a byte offset?
Which system call moves the file position?
What should you do if read/write doesn't transfer the full block?
*/