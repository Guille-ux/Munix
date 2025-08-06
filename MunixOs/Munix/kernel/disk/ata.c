#include "ata.h"
#include "../include/memory.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

static inline bool assertAtaErr(uint8_t errOut, int order) {
	return ((errOut & (1 << order)) != 0) ? true : false;
}

ATAError wtfAtaErrors(uint8_t errOut) {
	ATAError errors;
	errors.amnf = assertAtaErr(errOut, ATA_AMNF_ORDER);
	errors.tkznf = assertAtaErr(errOut, ATA_TKZNF_ORDER);
	errors.abrt = assertAtaErr(errOut, ATA_ABRT_ORDER);
	errors.mcr = assertAtaErr(errOut, ATA_MCR_ORDER);
	errors.idnf = assertAtaErr(errOut, ATA_IDNF_ORDER);
	errors.mc = assertAtaErr(errOut, ATA_MC_ORDER);
	errors.unc = assertAtaErr(errOut, ATA_UNC_ORDER);
	errors.bbk = assertAtaErr(errOut, ATA_BBK_ORDER);
	return errors;
}

DriveHeadRegCfg byte2driveHeadCfg(uint8_t byte) {
	DriveHeadRegCfg cfg;
	cfg.lba = ((byte & (1 << 6)) != 0) ? true : false;
	cfg.drv = (byte & (1 << 4)==(1 << 4)) ? 1:0;
	cfg.idk = ( (byte & 1) | (byte & 2) | (byte & (1 << 2)));
	return cfg;
}

uint8_t driveHeadCfg2byte(DriveHeadRegCfg cfg) {
	uint8_t byte=((1 << 7) | (1 << 5));
	byte |= ((cfg.idk & 1)  | (cfg.idk & 2) | (cfg.idk & (1 << 2)));
	byte |= ((1 << 4) & (cfg.drv << 4));
	byte |= (((cfg.lba) ? 1:0) << 6);

	return byte;
}

static inline void mdelay(ata_device_t *device) {
	for (int i=0;i<15;i++) { // espera de 420ns
		inb(device->control_base);
	}
}

bool ata_identify_device(ata_device_t *device) {
	ataSoftReset(device);
	mdelay(device);
	DriveHeadRegCfg cfg = (DriveHeadRegCfg){.idk=0, .drv=device->drv, .lba=false};
	outb(device->io_base+6, driveHeadCfg2byte(cfg));
	mdelay(device);

	/*
	 * Poner los lba a 0
	 *
	 */

	uint8_t cl=inb(device->io_base+4);
	uint8_t ch=inb(device->io_base+5);

	if (cl==0x14 && ch==0xEB) {
		kprintf("[DISK]: ATA Device Detected \n\t -> Channel : %d\n\t -> Drive : %d\n\t -> Device Type : ATAPI Device\n", device->channel, device->drv);
		device->type = ATADEV_ATAPI;
		return false; // no hemos encontrado un ata pio
	}
	else if (cl==0x69 && ch ==0x96) {
		kprintf("[DISK]: ATA Device Detected \n\t -> Channel : %d\n\t -> Drive : %d\n\t -> Device Type : SATAPI Device\n", device->channel, device->drv);
		device->type = ATADEV_SATAPI;
		return false;
	}
	else if (cl == 0 && ch == 0) {
		kprintf("[DISK]: ATA Device Detected \n\t -> Channel : %d\n\t -> Drive : %d\n\t -> Device Type : PATA Device\n", device->channel, device->drv);
		device->type = ATADEV_ATA;
	}
	else if (cl == 0x3C && ch == 0xC3) {
		kprintf("[DISK]: ATA Device Detected \n\t -> Channel : %d\n\t -> Drive : %d\n\t -> Device Type : SATA Device\n", device->channel, device->drv);
		device->type = ATADEV_SATA;
		return false;
	} else {
		kprintf("[DISK]: Unknown ATA Device!\n");
		device->type = ATADEV_UNKNOWN;
		return false;
	}

	for (int i=3;i<6;i++) {
		outb(device->io_base+i, 0);
		mdelay(device);
	}

	/*
	 * Escribir comando identify
	 *
	 */

	outb(device->io_base + 7, ATA_IDENTIFY_COMMAND); // 0xEC
	mdelay(device);
	
	uint8_t status = inb(device->io_base + 7);
	if (status == 0) {
		return false; // el disco no existe
	}
	ata_wait_busy(device);
	mdelay(device);

	cl=inb(device->io_base + 4);
	ch=inb(device->io_base + 5);
	if (ch!=0 || cl!=0) {
		kprintf("[DISK]: The Disk isn't an ata-pio disk!\n");
		return false;
	}

	while ((inb(device->control_base) & 1)==0 && (inb(device->control_base) & (1 << 3))==0);

	if (inb(device->control_base) & 1) return false; // oh no, un maldito error
	for (int i=0;i<256;i++) {
		device->identify_data[i]=inw(device->io_base);
	}
	return true;
}

AtaDevType ata_detect_device(ata_device_t *device) {
	ataSoftReset(device);
	mdelay(device);
	DriveHeadRegCfg cfg = (DriveHeadRegCfg){.idk=0, .drv=device->drv, .lba=false};
	outb(device->io_base+6, driveHeadCfg2byte(cfg));
	mdelay(device);

	/*
	 * Poner los lba a 0
	 *
	 */

	uint8_t cl=inb(device->io_base+4);
	uint8_t ch=inb(device->io_base+5);

	if (cl==0x14 && ch==0xEB) {
		kprintf("[DISK]: ATA Device Detected \n\t -> Channel : %d\n\t -> Drive : %d\n\t -> Device Type : ATAPI Device\n", device->channel, device->drv);
		device->type = ATADEV_ATAPI;
		return ATADEV_ATAPI;
	}
	else if (cl==0x69 && ch ==0x96) {
		kprintf("[DISK]: ATA Device Detected \n\t -> Channel : %d\n\t -> Drive : %d\n\t -> Device Type : SATAPI Device\n", device->channel, device->drv);
		device->type = ATADEV_SATAPI;
		return ATADEV_SATAPI;
	}
	else if (cl == 0 && ch == 0) {
		kprintf("[DISK]: ATA Device Detected \n\t -> Channel : %d\n\t -> Drive : %d\n\t -> Device Type : PATA Device\n", device->channel, device->drv);
		device->type = ATADEV_ATA;
		return ATADEV_ATA;
	}
	else if (cl == 0x3C && ch == 0xC3) {
		kprintf("[DISK]: ATA Device Detected \n\t -> Channel : %d\n\t -> Drive : %d\n\t -> Device Type : SATA Device\n", device->channel, device->drv);
		device->type = ATADEV_SATA;
		return ATADEV_SATA;
	} else {
		kprintf("[DISK]: Unknown ATA Device!\n");
		device->type = ATADEV_UNKNOWN;
		return ATADEV_UNKNOWN;
	}
}
#define ATA_NIEN 2 // algo para que pare de molestar con interrupts
#define ATA_SOFT_RESET (1 << 2) 

void ataSoftReset(ata_device_t *device) {
	outb(device->control_base, ATA_NIEN | ATA_SOFT_RESET);
	mdelay(device);
	mdelay(device);
	mdelay(device);
	mdelay(device);

	outb(device->control_base, ATA_NIEN);
	mdelay(device);
	mdelay(device);
	mdelay(device);
	mdelay(device);

	ata_wait_busy(device);
}

void parseAtaIdentify(ata_device_t *device) {
	kprintf("[DISK]: Parsing ATA Identify...\n");
	if (device->identify_data[83] & (1 << 10)) {
		kprintf("[DISK]: LBA48 is Supported\n");
		kprintf("[DISK]: Reading Disk Capacity...\n");
		device->lba48_supported=true;
		// si tiene lba48 la cantidad de sectores
		// se lee de forma diferente (en otros bytes)
		uint64_t a=device->identify_data[100];
		uint64_t b=device->identify_data[101];
		uint64_t c=device->identify_data[102];
		uint64_t d=device->identify_data[103];
		device->total_sectors = a | (b << 16) | (c << 32) | (d << 48);
	} else {
		kprintf("[DISK]: LBA48 not Supported\n");
		kprintf("[DISK]: Reading Disk Capacity...\n");
		device->lba48_supported=false;
		uint16_t a=device->identify_data[60];
		uint32_t b=device->identify_data[61];
		device->total_sectors = a | (b << 16);
	}
}

static inline void ataCommon(ata_device_t *device, lba_t lba, uint16_t n) {
	DriveHeadRegCfg cfg;
	outb(device->io_base + 1, 0x00);
	if (device->lba48_supported) {
		cfg.lba = true;
		cfg.drv = device->drv;
		cfg.idk = 0;
		uint8_t drv_cfg = driveHeadCfg2byte(cfg);

		outb(device->io_base + 1, 0x00);
		outb(device->io_base + 2, (n >> 8) & 0xFF);
		outb(device->io_base + 2, n & 0xFF);

		outb(device->io_base + 3, (lba.lo >> 24) & 0xFF);
		outb(device->io_base + 3, (lba.lo) & 0xFF);
		
		outb(device->io_base + 4, (lba.hi) & 0xFF);
		outb(device->io_base + 4, (lba.lo >> 8) & 0xFF);

		outb(device->io_base + 5, (lba.hi >> 8) & 0xFF);
		outb(device->io_base + 5, (lba.lo >> 16) & 0xFF);

		outb(device->io_base + 6, drv_cfg);
	} else {
		cfg.lba=true;
		cfg.drv=device->drv;
		cfg.idk = (lba.lo >> 24) & 0xF;

		uint8_t drv_cfg = driveHeadCfg2byte(cfg);

		outb(device->io_base + 1, (n==256) ? 0 : n); // recuento xD

		outb(device->io_base + 3, lba.lo & 0xFF);
		outb(device->io_base + 4, (lba.lo >> 8) & 0xFF);
		outb(device->io_base + 5, (lba.lo >> 16) & 0xFF);

		outb(device->io_base + 6, drv_cfg);
	}
}

void *ataReadLBA(ata_device_t *device, lba_t lba, void *buffer, uint16_t n) {
	ataSoftReset(device);
	ataCommon(device, lba, n);
	if (device->lba48_supported) {
		outb(device->io_base + 7, 0x24);
	} else {
		outb(device->io_base + 7, 0x20);
	}
	ata_wait_drq(device);
	
	uint8_t *b = buffer;

	for (uint32_t idx = 0; idx < n*256; idx++) {
		uint16_t tmp = inw(device->io_base); // es el puerto de datos
		b[idx*2] = tmp & 0xFF;
		b[idx*2+1] = (tmp >> 8) & 0xFF;
	}
	return buffer;
}

void ataWriteLBA(ata_device_t *device, lba_t lba, void *buffer, uint16_t n) {
	ataSoftReset(device);
	ataCommon(device, lba, n);
	if (device->lba48_supported) {
		outb(device->io_base + 7, 0x34);
	} else {
		outb(device->io_base + 7, 0x30);
	}
	ata_wait_drq(device);
	
	uint8_t *b = buffer;
	
	for (uint32_t idx = 0;idx < n*256;idx++) {
		uint16_t tmp = b[idx*2] | (b[idx*2+1] << 8) ;
		outw(device->io_base, tmp);
	}
}


void ataWriteLBA2(ata_device_t *device, lba_t lba, void *buffer, uint16_t n) {
	if (device->lba48_supported) {
		ataWriteLBA(device, lba, buffer, n);		
		return;
	} else {
		uint64_t tmp=0;
		void *b = buffer;
		while (n > 0) {
			ataWriteLBA(device, lba, b, n & 0xFF);
			tmp = lba2uint64(lba);
			b = (void*)((uintptr_t)b + (n & 0xFF) * 512);
			tmp += (n & 0xFF);
			lba = uint64_2_lba(tmp);
			n -= (n & 0xFF);
		}
	}
}
void ataReadLBA2(ata_device_t *device, lba_t lba, void *buffer, uint16_t n) {
	if (device->lba48_supported) {
		ataReadLBA(device, lba, buffer, n);		
		return;
	} else {
		uint64_t tmp=0;
		void *b = buffer;
		while (n > 0) {
			ataReadLBA(device, lba, b, n & 0xFF);
			tmp = lba2uint64(lba);
			b = (void*)((uintptr_t)b + (n & 0xFF) * 512);
			tmp += (n & 0xFF);
			lba = uint64_2_lba(tmp);
			n -= (n & 0xFF);
		}
	}
}
