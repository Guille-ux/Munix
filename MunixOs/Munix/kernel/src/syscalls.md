# SysCalls for MunixOs

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
