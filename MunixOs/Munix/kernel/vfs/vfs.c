#include "vfs.h"
#include "../include/libcs2.h"
#include "../fs/fsd.h"

// TODO: IMPLEMENT TAR FUNCS

vfs_t kernel_vfs;

int initKernelVFS_EXP(vfs_type type, explorer_t *expfs) {
	if (expfs==NULL) return -1;

	kernel_vfs.type = EXPLORER_FS;
	kernel_vfs.fs.expfs = expfs;

	// no habia mucho más q hacer

	return 0;
}

// ahora las funciones complejas de verdad, primero ucd
// más adelante a parte de esto, los procesos tmb seran capaces de hacer otras cosas

int ucd(const char *names) {
	char *name = strdup(names);
	if (kernel_vfs.type == TAR_FS) {
		// de momento nada
	} else if (kernel_vfs.type == EXPLORER_FS) {
		// aqui hay q parsear, esta función servira de base para muchas otras
		char delimiters[] = "/";
		char *chain = kernel_vfs.fs.expfs->path;
		char *token = strtok(chain, delimiters);

		while (token!=NULL) {
			int result = kernel_vfs.fs.expfs->cd(kernel_vfs.fs.expfs, token);
			if (result == -1) {
				return -1; // vamos, q la ruta es incorrecta
			}
			token = strtok(NULL, delimiters);
		}
		// si salimos del bucle es xq era null

	}
	free(name);
	return 0;
}

int ufd(const char *names) {
	char *name = strdup(names);
	if (kernel_vfs.type == TAR_FS) {
		// idk
	} else if (kernel_vfs.type == EXPLORER_FS) {
		char *name_start = strrchr(name, "/");
		int index = name_start - name;
		name[index++] = '\0';
		// ahora llamamos a ucd
		int result = ucd(name); // si ya se q ahora hay 2 copias en memoria
		if (result != 0) {
			return -1;
		}
		
		createFd(kernel_vfs.fs.expfs, name_start);
		
		ucd("/"); // para volver al root, si el programa estaba en otra parte, ps ya volvera el solo
		
	}
	free(name);
	return 0;
}
