#include "../include/libcs2.h"
#include "../disk/diski.h"
#include "partitions.h"
#include "../include/memory.h"
#include "../disk/diski.h"
#include <stdint.h>
#include <stddef.h>

void readPartition(partition_t *partition, void *buffer, lba_t where, uint32_t n) {
	disk_t *disk = partition->parent_disk;
	uint64_t tmp = lba2uint64(where);
	if (tmp + n > partition->total_sectors) {
		// ERROR, se sale del limite de la partición
		// en el futuro tendre algo más sofisticado, pero de momento
		// se quedara así
		stdlog_interface.append("[RLBA]: Too Many sectors"); 
		// RLBA = Relative LBA
		return;
	}
	tmp += lba2uint64(partition->lba_start);
	lba_t lba = uint64_2_lba(tmp);
	disk->read(disk, buffer, lba, n);

}
void writePartition(partition_t *partition, void *buffer, lba_t where, uint32_t n) {
	disk_t *disk = partition->parent_disk;
	uint64_t tmp = lba2uint64(where);
	if (tmp + n > partition->total_sectors) {
		// ERROR, se sale del limite de la partición
		// en el futuro tendre algo más sofisticado, pero de momento
		// se quedara así
		stdlog_interface.append("[RLBA]: Too Many Sectors");
		return;
	}
	tmp += lba2uint64(partition->lba_start);
	lba_t lba = uint64_2_lba(tmp);
	disk->write(disk, buffer, lba, n);
}
