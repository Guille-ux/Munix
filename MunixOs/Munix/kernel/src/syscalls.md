# SysCalls for MunixOs

## Syscalls Implemented
- [x] `wait();`
- [x] `getPid();`
- [x] `kill(int pid);` 
- [x] `ipc_send(int pid, msg_t *message);`
- [x] `ipc_receive(msg_t *message);`
- [x] `awake(int pid);`
- [x] `spawn(uint32_t ram_amount, void *blob, uint32_t length, uint32_t start_pos);` 
- [ ] `open(char *name);`
- [ ] `read(int fd, void *buffer, size_t size);`
- [ ] `write(int fd, void *buffer, size_t size);`
- [ ] `extend(int fd);`
- [ ] `remove(char *name);`
- [ ] `getch();`
- [ ] `register_mem(int how_many_pages);`
- [ ] `who();`
- [ ] `whoami();`
- [ ] `release_mem();`
- [ ] `change_my_name(char *new_name);`
- [ ] `close(int fd);`

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

this will return to eax 0 if successful or -1 if an error happens

# NOTE: AHEAD IF A INTERRUPT ASK FOR A BUFFER IS RELATIVE TO THE FAR POINTER

## `open(char *name);` 
*note: this is relative to the main partition root, new symbols will be added*

set eax to 0x07
set ebx to name ptr (you must add a \0, anyways the limit is 4096 charachters)

the descriptor will be stored in eax, it the descriptor doesn't exist this will return -1

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

## `getch();`
set eax to 0x0C
fetches an ASCII character from the kernel's circular keyboard buffer
the ASCII value will be stored in eax, if none 0 will be stored

## `register_mem(int how_many_pages);`

set eax to 0x0D
set ebx to the amount of pages you want
this returns you a far pointer to a new memory region, returns -1 if error

## `who();`

set eax to 0x0F
set ebx to ptr to a 16kb memory space

this will write in ebx a lot of info of the kernel, it's 16kb because it will be huge

## `whoami();`

set eax to 0x10

this returns you where your memory starts relative to the far pointer (to eax)

## `release_mem();`

set eax to 0x11

this releases all the mem you got by register_mem

## `change_my_name(char *new_name);`

set eax to 0x12
set ebx to the name pointer

*Note: max length is 128 bytes*

## `close(int fd);` 

set eax to 0x13
set ebx to the file descriptor

gives -1 if the fd didn't exist
