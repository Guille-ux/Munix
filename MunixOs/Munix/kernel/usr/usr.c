#include "usr.h"

extern char _kernel_identities_start;
extern char _kernel_identities_end;

identity_t *kernel_identities = &_kernel_identities_start;
uint32_t kernel_identities_size = 0;

void initIdentities() {
	kernel_identities_size = ((uint32_t)&_kernel_identities_end - (uint32_t)&kernel_identities_start);
	memset(&_kernel_identities_start, 0, kernel_identities_size);
}

int_t createIdentity(permission_lvl_t lvl, bool is_group, char *name) {
	int iid = -1;
	for (int i =0; i < kernel_identities_size / sizeof(identity_t);i++) {
		if (kernel_identities[i].used == false) {
			iid = i;
			kernel_identities[i].used = true;
			break;
		}
	}

	if (iid==-1) {
		return -1;
	}

	identity_t *identity = &kernel_identities[iid];

	if (name != NULL) {
		strncpy(identity.name, name, IDENTITY_NAME_SIZE);
	}
	identity.is_group = is_group
	identity.level = lvl;
	
	return iid;
}

int removeIdentity(int iid) {
	if (kernel_identity[iid].used == false) {
		return -1;
	}
	
	kernel_identity[iid].used = false;

	return 0;
}

identity_t *getIdentity(int iid) {
	return &kernel_identities[iid];
}
