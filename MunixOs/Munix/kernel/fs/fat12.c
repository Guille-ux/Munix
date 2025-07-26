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
	if (byte_offset == (sector_size -1 )) {
		// aham! en este caso leeremos 2 sectores
		stdlog_interface.append("[FAT12]: Reading 2 Sectors");
		uint32_t n_sec=2;
	} else {
		stdlog_interface.append("[FAT12]: Reading 1 Sector");
		uint32_t n_sec=1;
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

inline uint64_t cluster2addr(uint32_t cluster, fat12_bpb_t *bpb, partition_t *partition) {
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
		if (tmp < 1 || tmp == FAT12_BAD_CLUSTER) return -1;
		uint32_t current_cluster = tmp;
	}
	kfree(bpb);
	return 0;
}

int fat12SetEntry(void *buffer, uint16_t cluster, uint32_t byte_offset, uint16_t new_val) {
	if (cluster & 1) {
		buffer[byte_offset] = buffer[byte_offset] & 0xF0 | (new_val & 0x0F) << 4;
		buffer[byte_offset+1] = (new_val >> 4) & 0xFF;
	} else {
		buffer[byte_offset] = new_val & 0xFF;
		buffer[byte_offset+1] = (buffer[byte_offset+1] & 0xF0) | (new_val >> 8) & 0x0F;
	}
	return 0;
}

int16 RemoveFat12Entry(uint32_t cluster, partition_t *partition) {
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
	if (byte_offset == (sector_size -1 )) {
		// aham! en este caso leeremos 2 sectores
		stdlog_interface.append("[FAT12]: Reading 2 Sectors");
		uint32_t n_sec=2;
	} else {
		stdlog_interface.append("[FAT12]: Reading 1 Sector");
		uint32_t n_sec=1;
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
		if (tmp < 1 || tmp == FAT12_BAD_CLUSTER) return -1;
		uint32_t current_cluster = tmp;
	}
	kfree(bpb);
	return 0;
}
