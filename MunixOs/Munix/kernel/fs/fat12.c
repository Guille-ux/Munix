#include "fat12.h"


#define kmalloc(size) (stdmem_interface.kmalloc((size)))
#define kfree(ptr) (stdmem_interface.kfree((ptr)))

int16_t getFat12Entry(uint32_t cluster, partition_t *partition) {
	fat12_bpb_t *bpb = fat12_get_bpb(partition);
	if (cluster < 2 || cluster > fat12MaxClusters(bpb, partition)) {
		stdlog_interface.append("[FAT12]: Invalid Cluster!");
		return -1;
	}
	
	uint32_t cluster_offset = cluster + (cluster / 2); 
	// recordemos que usar decimales esta prohibido
	// todavia no tenemos FPU inicializada, todavia...
	uint32_t sector_size = bpb->BytesPerSector;
	uint32_t sectorInFat = cluster_offset / sector_size;
	uint32_t byte_offset = cluster_offset % sector_size;
	// muchos calculos son innecesarios en mi caso, pero por si acaso

	uint32_t relative_addr = sectorInFat + bpb->ReservedSectors;

	// averiguar cuantos sectores deberiamos leer
	// uso un uint32_t porque es lo que usa partition_read
	uint32_t n_sec;
	if (byte_offset == (sector_size -1 )) {
		// aham! en este caso leeremos 2 sectores
		stdlog_interface.append("[FAT12]: Reading 2 Sectors");
		n_sec=2;
	} else {
		stdlog_interface.append("[FAT12]: Reading 1 Sector");
		n_sec=1;
	}
	// asignar memoria para el buffer
	uint8_t *buffer = (uint8_t*)kmalloc(sizeof(uint8_t)*sector_size*n_sec);
	lba_t lba = {.lo=relative_addr};
	readPartition(partition, buffer, lba, n_sec);
	uint16_t entry;
	if (cluster & 1) { // más rápido que modulo, esto dice si es IMPAR
		// si es impar, 4 bits primero, 8 bits despues
		entry = (buffer[byte_offset] & 0x0F) << 4;
		entry |= buffer[byte_offset+1] << 4;
	} else { // si es par, 8 bits primero, 4 bits despues
		entry = buffer[byte_offset];
		entry |= (buffer[byte_offset+1] & 0x0F) << 8;
	}

	// liberar al final de todo
	kfree(buffer);
	kfree(bpb);

	// devolver la entrada
	return entry;
}

fat12_bpb_t *fat12_get_bpb(partition_t *partition) {
	fat12_bpb_t *bpb = (fat12_bpb_t*)kmalloc(sizeof(fat12_bpb_t));
	readPartition(partition, bpb, uint64_2_lba(0), 1);
}

uint32_t fat12MaxClusters(fat12_bpb_t *bpb, partition_t *partition) {
	uint32_t total_sectors = partition->total_sectors & (~((uint32_t)0));
	total_sectors -= bpb->ReservedSectors;
	total_sectors -= bpb->NumTables * bpb->SectorsPerFat;
	total_sectors -= bpb->RootEntries * FAT12_ENTRY_SIZE;
	uint32_t max_clusters = total_sectors / bpb->SectorsPerCluster;
	return max_clusters;
}

static inline uint64_t cluster2addr(uint32_t cluster, fat12_bpb_t *bpb, partition_t *partition) {
	uint32_t cluster_sec = fat12MaxClusters(bpb, partition) * bpb->SectorsPerCluster;
	return partition->total_sectors - cluster_sec + cluster * bpb->SectorsPerCluster;
}

int fat12ReadClusterChain(uint32_t first_cluster, void *buffer, uint32_t max_size, uint32_t *read, partition_t *partition) {
	uint32_t current_cluster = first_cluster;
	fat12_bpb_t *bpb = fat12_get_bpb(partition);
	lba_t lba;
	*read = 0;
	void *b = buffer;
	while (!FAT12_IS_END(current_cluster)) {
		lba = uint64_2_lba(cluster2addr(current_cluster, bpb, partition));
		uint32_t size = bpb->SectorsPerCluster;
		readPartition(partition, b, lba, size);
		*read += size*bpb->BytesPerSector;
		b += size*bpb->BytesPerSector;
		int tmp = getFat12Entry(current_cluster, partition);
		if (tmp < 1 || tmp == FAT12_BAD_CLUSTER) {
			kfree(bpb);
			return -1;
		}
		uint32_t current_cluster = tmp;
	}
	kfree(bpb);
	return 0;
}

int fat12SetEntry(void *buffer, uint16_t cluster, uint32_t byte_offset, uint16_t new_val) {
	uint8_t *b = buffer;
	if (cluster & 1) {
		b[byte_offset] = b[byte_offset] & 0xF0 | (new_val & 0x0F) << 4;
		b[byte_offset+1] = (new_val >> 4) & 0xFF;
	} else {
		b[byte_offset] = new_val & 0xFF;
		b[byte_offset+1] = (b[byte_offset+1] & 0xF0) | (new_val >> 8) & 0x0F;
	}
	return 0;
}

int16_t RemoveFat12Entry(uint32_t cluster, partition_t *partition) {
	fat12_bpb_t *bpb = fat12_get_bpb(partition);
	if (cluster < 2 || cluster > fat12MaxClusters(bpb, partition)) {
		stdlog_interface.append("[FAT12]: Invalid Cluster!");
		return -1;
	}
	
	uint32_t cluster_offset = cluster + (cluster / 2); 
	// recordemos que usar decimales esta prohibido
	// todavia no tenemos FPU inicializada, todavia...
	uint32_t sector_size = bpb->BytesPerSector;
	uint32_t sectorInFat = cluster_offset / sector_size;
	uint32_t byte_offset = cluster_offset % sector_size;
	// muchos calculos son innecesarios en mi caso, pero por si acaso

	uint32_t relative_addr = sectorInFat + bpb->ReservedSectors;

	// averiguar cuantos sectores deberiamos leer
	// uso un uint32_t porque es lo que usa partition_read
	uint32_t n_sec;
	if (byte_offset == (sector_size -1 )) {
		// aham! en este caso leeremos 2 sectores
		stdlog_interface.append("[FAT12]: Reading 2 Sectors");
		n_sec=2;
	} else {
		stdlog_interface.append("[FAT12]: Reading 1 Sector");
		n_sec=1;
	}
	// asignar memoria para el buffer
	uint8_t *buffer = (uint8_t*)kmalloc(sizeof(uint8_t)*sector_size*n_sec);
	lba_t lba = {.lo=relative_addr};
	readPartition(partition, buffer, lba, n_sec);

	uint16_t entry;
	if (cluster & 1) { // más rápido que modulo, esto dice si es IMPAR
		// si es impar, 4 bits primero, 8 bits despues
		entry = (buffer[byte_offset] & 0x0F) << 4;
		entry |= buffer[byte_offset+1] << 4;
	} else { // si es par, 8 bits primero, 4 bits despues
		entry = buffer[byte_offset];
		entry |= (buffer[byte_offset+1] & 0x0F) << 8;
	}
	uint16_t new_entry = 0x000;
	
	fat12SetEntry(buffer, cluster, byte_offset, new_entry); 
	writePartition(partition, buffer, lba, n_sec);

	// liberar al final de todo
	kfree(buffer);
	kfree(bpb);

	// devolver la entrada
	return entry;
}

int fat12RemoveClusterChain(uint32_t first_cluster, partition_t *partition) {
	uint32_t current_cluster = first_cluster;
	fat12_bpb_t *bpb = fat12_get_bpb(partition);
	while (!FAT12_IS_END(current_cluster)) {
		int tmp = RemoveFat12Entry(current_cluster, partition);
		if (tmp < 1 || tmp == FAT12_BAD_CLUSTER) {
			kfree(bpb);
			return -1;

		}
		uint32_t current_cluster = tmp;
	}
	kfree(bpb);
	return 0;
}

int fat12AllocClusters(partition_t *partition, uint32_t n, uint32_t *first_cluster) {
	fat12_bpb_t *bpb = fat12_get_bpb(partition);
	if (n == 0) {
		if (first_cluster != NULL) {
			*first_cluster = 0;
		}
		return 0;
	}
	uint32_t first_allocated=0;
	uint32_t last_allocated=0;
	uint32_t allocated_count=0;
	
	stdlog_interface.append("[FAT12]: Allocating Clusters...");

	uint32_t max_clusters = fat12MaxClusters(bpb, partition);
	for (uint32_t i=2; i <= max_clusters && allocated_count < n;i++) {
		uint16_t entry = getFat12Entry(i, partition);
		if (entry != FAT12_FREE_CLUSTER) {
			continue;
		}
		if (first_allocated == 0) {
			first_allocated = i;
		} else {
			fat12SetCluster(partition, bpb, last_allocated, i);
		}
		last_allocated = i;
		allocated_count++;
	}
	// añadir EOC (end of chain)
	fat12SetCluster(partition, bpb, last_allocated, 0xFFF);
	*first_cluster = first_allocated;
	// comprobar errores
	if (allocated_count < n) {
		fat12RemoveClusterChain(first_allocated, partition);
		return -1;
	}
	return 0;
}

int fat12SetCluster(partition_t *partition, fat12_bpb_t *bpb, uint32_t cluster, uint32_t new_val) {
	if (cluster < 2 || cluster > fat12MaxClusters(bpb, partition)) {
		stdlog_interface.append("[FAT12]: Invalid Cluster!");
		return -1;
	}
	
	uint32_t cluster_offset = cluster + (cluster / 2); 
	// recordemos que usar decimales esta prohibido
	// todavia no tenemos FPU inicializada, todavia...
	uint32_t sector_size = bpb->BytesPerSector;
	uint32_t sectorInFat = cluster_offset / sector_size;
	uint32_t byte_offset = cluster_offset % sector_size;
	// muchos calculos son innecesarios en mi caso, pero por si acaso

	uint32_t relative_addr = sectorInFat + bpb->ReservedSectors;

	// averiguar cuantos sectores deberiamos leer
	// uso un uint32_t porque es lo que usa partition_read
	uint32_t n_sec;
	if (byte_offset == (sector_size -1 )) {
		// aham! en este caso leeremos 2 sectores
		stdlog_interface.append("[FAT12]: Reading 2 Sectors");
		n_sec=2;
	} else {
		stdlog_interface.append("[FAT12]: Reading 1 Sector");
		n_sec=1;
	}
	// asignar memoria para el buffer
	uint8_t *buffer = (uint8_t*)kmalloc(sizeof(uint8_t)*sector_size*n_sec);
	lba_t lba = {.lo=relative_addr};
	readPartition(partition, buffer, lba, n_sec);

	uint16_t entry;
	if (cluster & 1) { // más rápido que modulo, esto dice si es IMPAR
		// si es impar, 4 bits primero, 8 bits despues
		entry = (buffer[byte_offset] & 0x0F) << 4;
		entry |= buffer[byte_offset+1] << 4;
	} else { // si es par, 8 bits primero, 4 bits despues
		entry = buffer[byte_offset];
		entry |= (buffer[byte_offset+1] & 0x0F) << 8;
	}
	uint16_t new_entry = new_val & 0xFFF;
	
	fat12SetEntry(buffer, cluster, byte_offset, new_entry); 
	writePartition(partition, buffer, lba, n_sec);

	// liberar al final de todo
	kfree(buffer);
	kfree(bpb);

	// devolver la entrada
	return entry;
}

int fat12WriteClusterChain(partition_t *partition, uint32_t first_cluster, void *buffer, uint32_t n) {
	fat12_bpb_t *bpb = fat12_get_bpb(partition);
	uint32_t current_cluster = 0;
	uint32_t wrote = 0;
	lba_t lba;
	void *b = buffer;
	while (wrote < n && !FAT12_IS_END(getFat12Entry(current_cluster & 0xFFFF, partition))) {
		lba = uint64_2_lba(cluster2addr(current_cluster, bpb, partition));
		uint32_t size = bpb->SectorsPerCluster;
		writePartition(partition, b, lba, size);
		wrote += size*bpb->BytesPerSector;
		b += size*bpb->BytesPerSector;
		int tmp = getFat12Entry(current_cluster, partition);
		if (tmp < 1 || tmp == FAT12_BAD_CLUSTER) {
			kfree(bpb);
			return -1;
		}
		uint32_t current_cluster = tmp;
	}
	kfree(bpb);
	return 0;
}

void FAT12listRootDirectory(partition_t *partition, fat12_bpb_t *bpb) {
	lba_t root_dir = FAT12getRootDirLba(partition, bpb);
	uint32_t size = FAT12getRootDirSize(bpb);
	fat12_entry_t *entries = (fat12_entry_t*)kmalloc(sizeof(uint8_t)*size*512);
	readPartition(partition, entries, root_dir, size);
	
	uint32_t c=0;
	kprintf("root: '/'\n");
	while (1 && c < (size*512)/32) {
		if (entries[c].name[0]==0xE5) {
			// xD, no hacer nada
		} else if (entries[c].name[0]==0x00) {
			break;
		} else if (entries[c].attr & FAT12_ATTR_VOLUME_ID) {
			// nada
		} else if (entries[c].attr & FAT12_ATTR_LONG_NAME) {
			// va separado del anterior porque es provisional
		} else if (entries[c].attr & FAT12_ATTR_ARCHIVE){
			char tmp[13];
			tmp[12]='\0';
			memcpy(tmp, entries[c].name, 8);
			memcpy(tmp+9, entries[c].extension, 3);
			tmp[8]='.';
			kprintf("\t -> %s\n", tmp);
		} else if (entries[c].attr & FAT12_ATTR_DIR) {
			char tmp[12];
			tmp[11]='\0';
			memcpy(tmp, entries[c].name, 8);
			memcpy(tmp+8, entries[c].extension, 3)
			kprintf("\t -> %s\n", tmp);
		}
		c++;
	}
}

fat12_entry_t *FAT12listInDirectory(void *buffer, uint32_t n) {
	uint32_t c=0;
	while (1 && c < n) {
		if (entries[c].name[0]==0xE5) {
			// xD, no hacer nada
		} else if (entries[c].name[0]==0x00) {
			break;
		} else if (entries[c].attr & FAT12_ATTR_VOLUME_ID) {
			// nada
		} else if (entries[c].attr & FAT12_ATTR_LONG_NAME) {
			// va separado del anterior porque es provisional
		} else if (entries[c].attr & FAT12_ATTR_ARCHIVE){
			char tmp[13];
			tmp[12]='\0';
			memcpy(tmp, entries[c].name, 8);
			memcpy(tmp+9, entries[c].extension, 3);
			tmp[8]='.';
			kprintf("%s\n", tmp);
		} else if (entries[c].attr & FAT12_ATTR_DIR) {
			char tmp[12];
			tmp[11]='\0';
			memcpy(tmp, entries[c].name, 8);
			memcpy(tmp+8, entries[c].extension, 3)
			kprintf("%s\n", tmp);
		}
		c++;
	}
}
