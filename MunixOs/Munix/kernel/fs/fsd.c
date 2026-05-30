#include "fsd.h"
#include "../include/memory.h"
#include "../include/libcs2.h"
#include <stdint.h>

extern char _kernel_fds_start;
extern char _kernel_fds_end;

fd_t *kernel_fds = &_kernel_fds_start;
uint32_t kernel_fds_size = 0;

void initFd() {
	kernel_fds_size = ((uint32_t)&_kernel_fds_end - (uint32_t)&kernel_fds_start);
	memset(&_kernel_fds_start, 0, kernel_fds_size);
}

// a parte de buscar un espacio nuevo tmb busca si ya se abrio el archivo
int createFd(explorer_t *explorer, char *name) {
	int fid = -1;
	bool yet_exist=false;
	for (int i =0; i < kernel_fds_size / sizeof(fd_t);i++) {
		if (kernel_fds[i].ref_count == 0 || (kernel_fds[i].ref_count > 0 && strcmp(kernel_fds[i].file.name, name)==0)) {
			fid = i;
			kernel_fds[i].ref_count++;
			if (kernel_fds[i].ref_count > 1) {
				yet_exist = true;
			}
			break;
		}
	}

	if (yet_exist) {
		return fid;
	}

	if (fid==-1) {
		return -1;
	}

	fd_t *filed = &kernel_fds[fid];
	explorer->new_fd(explorer, filed, name);
	
	
	return fid;
}

// básicamente quitamos una refenrencia, xq asi no destruimos el archivo
// si estaba en uso por varios procesos
// si ya nadie lo usaba ps bno ya sabras q hace, no?
// LO LIMPIA
int removeFd(int fid) {
	if (kernel_fds[fid].ref_count == 0) {
		return -1;
	}
	
	kernel_fds[fid].ref_count--;
	
	if (kernel_fds[fid].ref_count <= 0) { // si ya nadie lo usa
					      // se limpia
		memset(&kernel_fds[fid], 0, sizeof(fd_t));
	}

	return 0;
}

fd_t *getFd(int fid) {
	return &kernel_fds[fid];
}
