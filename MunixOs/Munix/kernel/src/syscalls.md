# SysCalls for MunixOs

## Syscalls Implemented
- [x] `wait();`
- [x] `getPid();`
- [x] `kill(int pid);` 
- [x] `ipc_send(int pid, msg_t *message);`
- [x] `ipc_receive(msg_t *message);`
- [x] `awake(int pid);`
- [x] `spawn(uint32_t ram_amount, void *blob, uint32_t length, uint32_t start_pos);` 
- [x] `open(char *name);`
- [x] `read(int fd, void *buffer, size_t size);`
- [x] `write(int fd, void *buffer, size_t size);`
- [x] `extend(int fd);`
- [x] `remove(char *name);`
- [x] `register_mem(int how_many_pages);`
- [ ] `who();`
- [x] `whoami();`
- [x] `release_mem();`
- [x] `change_my_name(char *new_name);`
- [x] `close(int fd);`
- [ ] `touch(char *name);`
- [ ] `cd(char *name);`
- [ ] `pwd();`
- [x] `openg(int fd);`
- [x] `exit();`
- [x] `searchPid(char *name);`

The `int` prefix for MunixOs is `0x80`.

## `wait();`
set eax 0x00

put's the job to sleep

## `getPid();`
set eax 0x01

store's in eax the program's pid

## `kill(int pid);`
set eax 0x02

set ebx to the pid u want to kill

kills the pid

## `ipc_send(int pid, msg_t *message);`
set eax to 0x03

set ebx to pid

set ecx to the pointer

set edx to 1 if the msg is from program's memory or 0 if it's a far pointer

```c
// messages
typedef struct {
    int pid; // the one who sent this message
    uint8_t data[124]; // it isn't 128 because i wanted the messages were 128 bytes
} msg_t;

```

## `ipc_receive(msg_t *message);`
set eax to 0x04

set ebx to message ptr

set ecx to 1 if to program's memory or 0 if to far pointer

## `awake(int pid);`
set eax to 0x05

set ebx to pid

## `spawn(uint32_t ram_amount, void *blob, uint32_t length, uint32_t start_pos);`
*note that this call will use the caller's code segment and data segment*

set eax to 0x06

set ebx to the ram amount

set ecx to the blob pointer

set edx to the blob length

set edi to the position where code starts

this will return to eax the pid if successful or -1 if an error happens

# NOTE: AHEAD IF A INTERRUPT ASK FOR A BUFFER IS RELATIVE TO THE FAR POINTER

## `open(char *name);` 
*note: this is relative to the main partition root, new symbols will be added*

set eax to 0x07

set ebx to name ptr (you must add a \0, anyways the limit is 4096 charachters)

the descriptor will be stored in eax, it the descriptor doesn't exist this will return -1 (also returns -1 if there isn't enough space for fd's)

## `read(int fd, void *buffer, size_t size);`
set eax to 0x08

set ebx to the file descriptor (fd)

set ecx to the destination buffer pointer in user memory

set edx to the size in bytes to read from the file

stores the amount of read bytes in eax (or -1 on error)

## `write(int fd, void *buffer, size_t size);`
set eax to 0x09

set ebx to the file descriptor (fd)

set ecx to the source buffer pointer in user memory

set edx to the size in bytes to write to the file

stores the amount of written bytes in eax (or -1 on error)

# `extend(int fd);`
set eax to 0x0A

set ebx to the file descriptor (fd)

allocates and appends a new block to the file chain on the MFS partition

## `remove(char *name);`
set eax to 0x0B

set ebx to the file name or route pointer to delete

removes the file entry and clears its block chain from the disk

if error returns -1, if no problem returns 0

## `openg(int fd);`
set eax to 0x0C

set ebx to the fd

gets a global fd, returns your fd

## `register_mem(int how_many_pages);`

set eax to 0x0D

set ebx to the amount of pages you want

this returns you a far pointer to a new memory region, returns NULL if error

## `who();`

set eax to 0x0F

set ebx to ptr to a 16kb memory space

this will write in ebx a lot of info of the kernel, it's 16kb because it will be huge

## `whoami();`

set eax to 0x10

this returns you where your memory starts relative to the far pointer (to eax)

## `release_mem();`

set eax to 0x11

this releases all the mem you got by register_mem, returns 0 if ok, and -1 if nothing to release

## `change_my_name(char *new_name);`

set eax to 0x12

set ebx to the name pointer

returns 0 if ok, -1 if error encountered

*Note: max length is 128 bytes*

## `close(int fd);` 

set eax to 0x13

set ebx to the file descriptor

gives -1 if the fd didn't exist

## `remove(char *name);`

set eax t0 0x14

set ebx to the ptr to the name, IT HAS TO BE FROM FS

## `touch(char *name);`

set eax to 0x15

set ebx to the name ptr, max size is 256 characters (must end with \0)

returns 0 if no error

returns -1 if the file exists

return -2 if there isn't enough space (idk how i'm going to make this)

## `cd(char *name);`
*NOTE: Only one level of changed*

set eax to 0x16

set ebx to the name ptr max size is 256 charachters (must end with \0)

return 0 if no problem

if the directory doesn't exist returns -1

if that's a file returns -2

(the kernel will move to where the program is to verify that file exist)

## `exit();`

set eax to 0x17

kills the program itself


## `pwd();`

set eax to 0x18

returns a pointer to a string on the user data space containing the program path.

## `searchPid(char *name);`

set eax to 0x19

set ebx to name ptr, max length is of 128 bytes

returns the pid of the process, if doesn't exist returns -1
