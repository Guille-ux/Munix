#include "../keyboard/keyboard.h"
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
#include "../fs/ifat.h"
#include <stdint.h>
#include <stdbool.h>



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

ShellValue pwd_handler(ASTNode *stmt, EvalCtx *ctx) {
	if (stmt->data.command_call.argc < 1) return newNumVal(-1);
	ASTNode **args = stmt->data.command_call.args;
	ShellValue a = evalExpr(args[0], ctx);
	if (a.type!=VAL_NUMBER) {
		ref_del(a);
		return newNumVal(-1);
	}

	explorer_t *explorer = (explorer_t*)a.as.num;

	kprintf("%s\n", explorer->path);

	ref_del(a);
	return newStrVal(explorer->path);
}

ShellValue ref_info_handler(ASTNode *stmt, EvalCtx *ctx) {
	if (stmt->data.command_call.argc < 1) return newNumVal(-1);
	ShellValue a = evalExpr(stmt->data.command_call.args[0], ctx);
	if (a.type != VAL_NUMBER) {
		ref_del(a);
		return newNumVal(-1);
	}

	Var *var = getShellVarEntry(ctx, a.as.num);
	ShellValue val = var->val;
	uint32_t n_refs=0;
	if (val.type == VAL_STRING) {
		Obj *obj = (Obj*)((size_t)val.as.str - sizeof(Obj));
		n_refs = obj->refc;
		kprintf("ref count: %d \n", n_refs);
	} else {
		kprintf("Unknown!\n");
	}


	ref_del(a);
	return newNumVal(n_refs);
}

static uint8_t buff[512];

ShellValue ls_handler(ASTNode *stmt, EvalCtx *ctx) {
	if (stmt->data.command_call.argc < 1) return newNumVal(-1);
	ASTNode **args = stmt->data.command_call.args;

	ShellValue a = evalExpr(args[0], ctx);
	if (a.type != VAL_NUMBER) {
		ref_del(a);
		return newNumVal(-1);
	}

	explorer_t *explorer = (explorer_t*)a.as.num;

	mfs_dir_header_t* header = (mfs_dir_header_t*)(*explorer->cwd);
	mfs_entry_t *entries = (mfs_entry_t*)((size_t)(*explorer->cwd)+sizeof(mfs_dir_header_t));

	memset(buff, 0, 512);
	buff[0]='\0';

	kprintf("num entries -> %d\n", header->num_entries);
	for (uint32_t i=0;i<header->num_entries-1;i++) {
		if (entries[i].first_block == IFAT_TOMBSTONE) continue;
		if (entries[i].first_block == IFAT_FREE_BLOCK) break;
		
		kprintf("%s\n", entries[i].name);
		strncat(buff, entries[i].name, 511);
	}

	ref_del(a);
	ShellValue ret = newStrVal(buff);
	return ret;
}

ShellValue mkdir_handler(ASTNode *stmt, EvalCtx *ctx) {
	if (stmt->data.command_call.argc < 2) return newNumVal(-1);
	ASTNode **args = stmt->data.command_call.args;

	ShellValue a = evalExpr(args[0], ctx);
	if (a.type != VAL_NUMBER) {
		ref_del(a);
		return newNumVal(-1);
	}
	ShellValue b = evalExpr(args[1], ctx);
	if (b.type != VAL_STRING) {
		ref_del(a);
		ref_del(b);
		return newNumVal(-1);
	}

	explorer_t *explorer = (explorer_t*)a.as.num;

	int result = explorer->mkdir(explorer, b.as.str);

	if (result != 0) {
		return newNumVal(-1);
	}

	ref_del(a);
	ref_del(b);

	return newNumVal(0);
}

ShellValue rm_handler(ASTNode *stmt, EvalCtx *ctx) {
	if (stmt->data.command_call.argc < 2) return newNumVal(-1);

	ASTNode **args = stmt->data.command_call.args;

	ShellValue a = evalExpr(args[0], ctx);
	if (a.type != VAL_NUMBER) {
		ref_del(a);
		return newNumVal(-1);
	}
	ShellValue b = evalExpr(args[1], ctx);
	if (b.type != VAL_STRING) {
		ref_del(a);
		ref_del(b);
		return newNumVal(-1);
	}

	explorer_t *explorer = (explorer_t*)a.as.num;
	
	// aqui deberia hacer código para ir borrando todos los archivos recursivamente

	ref_del(a);
	ref_del(b);
	return newNumVal(0);
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
	register_module("pwd", pwd_handler);
	register_module("ref_count", ref_info_handler);
	register_module("ls", ls_handler);
	register_module("mkdir", mkdir_handler);
	register_module("rm", rm_handler); // RECURSIVO
	//register_module("cd", cd_handler);
	//register_module("mod", mod_handler);
	//register_module("chmod", chmod_handler);
	//register_module("stat", stat_handler);
	//register_module("chstat", chstat_handler);
	//register_module("touch", touch_handler);
	//register_module("cat", cat_handler);
	//register_module("write", write_handler);
	//register_module("clean", clean_handler);
	

	uint32_t max_x = stdout_interface.get_max_x();
	uint32_t max_y = stdout_interface.get_max_y();
	

	global_ctx->command_handler=multimodule_handler;
	Token *t_buff = shell_tokens;
	shell_update();
	__asm__ volatile("sti"); // por si habian sido interrumpidas
	while (1) {
		uint16_t key = kgetchar();
		if (key != 0) {	
			if (key == '\b') {
				uint32_t ccur = stdout_interface.get_cur_y() * max_x + stdout_interface.get_cur_x();
				ccur--;
				size_t tmp_x = ccur % max_x;
				size_t tmp_y = (ccur - ( ccur % max_x)) / max_x;
				stdout_interface.setcur(tmp_x, tmp_y);
				stdout_interface.putchar(tmp_x, tmp_y, ' ', stdout_interface.default_color);
				
			} else if (key == '\n') {
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

				// Reiniciar el buffer
				memset(shell_buffer, 0, sizeof(char)*SHELL_BUFFER_SIZE);
				shell_update(); // Actualizar Prompt
			} else {
				if (SPECIAL_KEY & key) {
					continue;
				}
				shell_putc(key);
			}
		}
		__asm__ volatile("hlt"); // estado de bajo consumo 
	}
	return 0;
}

void shell_putc(char c) {
    size_t x = stdout_interface.get_cur_x();
    size_t y = stdout_interface.get_cur_y();
    size_t max_x = stdout_interface.get_max_x();

    
    stdout_interface.putchar(x, y, c, stdout_interface.default_color);

    
    x++;
    if (x >= max_x) { 
        x = 0;        
        y++;          
    }

    
    stdout_interface.setcur(x, y);
}
