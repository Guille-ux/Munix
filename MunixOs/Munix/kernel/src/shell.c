#include "../include/libcs2.h"
#include "../include/shell.h"
#include "../include/memory.h"
#include "../disk/diski.h"
#include "../disk/disk_manager.h"
#include "../partitions/partition_mng.h"
#include "../mbash/lexer.h"
#include "../mbash/parser.h"
#include "../mbash/eval.h"
#include "../minim/minim.h"
#include "../mbash/modules.h"
#include "../fs/fs.h"
#include <stdint.h>
#include <stdbool.h>

bool send=false;
bool backspace=false;
bool shell_event=false;
char *shell_prompt="~ MunixOs ~> ";
char shell_buffer[SHELL_BUFFER_SIZE];
uint32_t shell_index=0;

/*
 * Módulos de la Shell
 */

/*
 * algunas funciones seran específicas para el usuario, al final
 * es un sistema de módulos, cambiarlos no deberia ser tan difícil
 */

ShellValue clear_handler(ASTNode *stmt, EvalCtx *ctx) {
	stdout_interface.clear(stdout_interface.default_color);

	return newNumVal(0);
}

ShellValue lsblk_handler(ASTNode *stmt, EvalCtx *ctx) {
	kprintf("-----------------------\n");
	kprintf("N. Disks : %d\n", system_disk_count);
	kprintf("N. Partitions : %d\n", kpartition_manager.partitions_count);
	for (int i=0;i<system_disk_count;i++) {
		kprintf("\t Disk N. %d\n", i);
		for (int b = 0;b<kpartition_manager.partitions_count;b++) {
			if (kpartition_manager.partitions[b].parent_disk==&system_disks[i]) {
				partition_t *partition = &kpartition_manager.partitions[b];
				kprintf("\t\t -> Partition N. %d\n", b);
				kprintf("\t\t   -> type: ");
				switch (partition->fs_type) {
					case FS_TYPE_FAT12:
						kprintf("FAT12\n");
						break;
					case FS_TYPE_FAT16_LESS_32MB:
					case FS_TYPE_FAT16_GREATER_32MB:
						kprintf("FAT16\n");
						break;
					case FS_TYPE_FAT32_LBA:
					case FS_TYPE_FAT32_CHS:
						kprintf("FAT32\n");
						break;
					case FS_TYPE_NTFS:
						kprintf("NTFS\n");
						break;
					case FS_TYPE_LINUX:
						kprintf("Linux\n");
						break;
					case FS_TYPE_LINUX_SWAP:
						kprintf("Linux Swap\n");
						break;
					case FS_TYPE_MFS:
						kprintf("MunixFileSystem\n");
						break;
					default:
						kprintf("Unknown Type\n");
				}
			}
		}
	}
	kprintf("-----------------------\n");

	return newNumVal(0);
}

ShellValue remount_handler(ASTNode *stmt, EvalCtx *ctx) {
	if (stmt->data.command_call.argc < 1) {
		kprintf("To Few Args\n");
		return newNumVal(-1);
	}
	if (stmt->data.command_call.args[0]->type != NODE_NUM_LIT) {
		kprintf("Error\n");
		return newNumVal(-1);
	}
	kprintf("Remounting...\n");
	partitionManagerRegisterMemory(&kpartition_manager, stmt->data.command_call.args[0]->data.number_lit);
	for (int i=0;i<system_disk_count;i++) {
		partitionManagerScanDisk(&kpartition_manager, &system_disks[i]);
	}	

	return newNumVal(0);
}

ShellValue setfs_handler(ASTNode *stmt, EvalCtx *ctx) {
	if (stmt->data.command_call.argc < 3) return newNumVal(-1);
	ASTNode **args = stmt->data.command_call.args;
	ShellValue a = evalExpr(args[0], ctx);
	ShellValue b = evalExpr(args[1], ctx);
	ShellValue c = evalExpr(args[2], ctx);
	
	if (a.type != VAL_NUMBER ||
	    b.type != VAL_NUMBER ||
	    c.type != VAL_NUMBER) {
		ref_del(a);
		ref_del(b);
		ref_del(c);
		return newNumVal(-1);
	}

	explorer_t *explorer = (explorer_t *)kmalloc(sizeof(explorer_t));

	// de momento solo soportara MunixFileSystem
	mfs_init_explorer(&kpartition_manager.partitions[a.as.num], explorer, b.as.num, c.as.num);

	ref_del(a);
	ref_del(b);
	ref_del(c);
	return newNumVal((long)explorer);
}

/*
 * Fin de los módulos de la Shell
 */

void push_to_buffer(char c) {
    if (!(shell_index<SHELL_BUFFER_SIZE)) return;
    shell_buffer[shell_index++]=c;
    shell_buffer[shell_index]='\0';
}

void pop_from_buffer() {
    if (shell_index==0) return;
    shell_buffer[--shell_index]='\0';
}

void shell_update() {
    kprintf("\r");
    for (int i=1;i<stdout_interface.get_max_x()/8;i++) {
        kprintf("\t");
    }
    kprintf("\r%s %s", (const char*)shell_prompt, (const char*)shell_buffer);
}

#define MAX_TOKENS 64
Token shell_tokens[64];

int shellEntry() {
	EvalCtx *global_ctx = newShellCtx();
	
	/* registrar módulos */
	register_module("clear", clear_handler);
	register_module("lsblk", lsblk_handler);
	register_module("remount", remount_handler);
	register_module("setfs", setfs_handler);

	global_ctx->command_handler=multimodule_handler;
	Token *t_buff = shell_tokens;
	shell_update();
	__asm__ volatile("sti"); // por si habian sido interrumpidas
	while (1) {
		if (shell_event) {
			push_to_buffer(final_character);
			shell_update();
			shell_event=false;
		} else if (backspace) {
			pop_from_buffer();
			shell_update();
			backspace=false;
		} else if (send) {
			kprintf("\n");
			
			// Análisis Léxico
			lexer_init(shell_buffer);
			lexen(t_buff, MAX_TOKENS);
			
			// Parseo
			parser_init(t_buff);
			ASTNode *tree = parse();

			//liberar tokens
			//free_tokens(t_buff, MAX_TOKENS);

			// Evaluación
			eval(tree, global_ctx);

			// Liberar Memoria del AST
			parser_free_ast(tree);
			free_tokens(t_buff, MAX_TOKENS);
			
			// Reiniciar flags e index
			send=false;
			shell_index=0;

			// Reiniciar el buffer
			memset(shell_buffer, 0, sizeof(char)*SHELL_BUFFER_SIZE);
			shell_update(); // Actualizar Prompt
		}
		__asm__ volatile("hlt"); // estado de bajo consumo 
	}
	return 0;
}
