#ifndef INITIALIZATION
#define INITIALIZATION

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../include/handler.h"
#include "../include/libcs2.h"
#include "../include/memory.h"
#include "../disk/disk_manager.h"
#include "../disk/general.h"
#include "../disk/diski.h"
#include "../pci/pci.h"
#include "../mbash/lexer.h"
#include "../mbash/parser.h"
#include "../mbash/eval.h"
#include "../minim/minim.h"
#include "../partitions/mbr.h"
#include "../partitions/partitions.h"
#include "../partitions/partition_mng.h"
#include "../include/gdt.h"
#include "../include/idt.h"
#include "../include/timer.h"
#include "../include/pic.h"
#include "../include/low_level.h"
#include "../include/buddy_alloc.h"
#include "../include/isr.h"
#include "../multiboot/multiboot.h"
#include "../paging/paging32.h"
#include "../memory/bitmap.h"
#include "../handlers/kb_handler.h"
#include "../keyboard/keyboard.h"

#define MAX_LOG_LEN 33
#define MAX_LOGS 512

#define N_HANDLES 10
extern handle_t kernel_handles[N_HANDLES];

extern char *log[MAX_LOGS];
#define ALL_SIZE 1024*1024*64  // 64MB heap size

extern free_node *my_free_list[64];
extern size_t size;
extern char heap_start[ALL_SIZE];
extern int mini_order; 
extern int maxi_order;

void kernel_init();
void init_disk_subsystem();
void scanByDetectedDisks();

#endif
