#define DISK_BUFF 256//BUFFER DISK length in uint16_t

//status
#define ATA_SR_BSY     0x80    // Busy
#define ATA_SR_DRDY    0x40    // Drive ready
#define ATA_SR_DF      0x20    // Drive write fault
#define ATA_SR_DSC     0x10    // Drive seek complete
#define ATA_SR_DRQ     0x08    // Data request ready
#define ATA_SR_CORR    0x04    // Corrected data
#define ATA_SR_IDX     0x02    // Index
#define ATA_SR_ERR     0x01    // Error

//errors
#define ATA_ER_BBK      0x80    // Bad block
#define ATA_ER_UNC      0x40    // Uncorrectable data
#define ATA_ER_MC       0x20    // Media changed
#define ATA_ER_IDNF     0x10    // ID mark not found
#define ATA_ER_MCR      0x08    // Media change request
#define ATA_ER_ABRT     0x04    // Command aborted
#define ATA_ER_TK0NF    0x02    // Track 0 not found
#define ATA_ER_AMNF     0x01    // No address mark

//commands
#define ATA_CMD_READ_PIO          0x20
#define ATA_CMD_READ_PIO_EXT      0x24
#define ATA_CMD_READ_DMA          0xC8
#define ATA_CMD_READ_DMA_EXT      0x25
#define ATA_CMD_WRITE_PIO         0x30
#define ATA_CMD_WRITE_PIO_EXT     0x34
#define ATA_CMD_WRITE_DMA         0xCA
#define ATA_CMD_WRITE_DMA_EXT     0x35
#define ATA_CMD_CACHE_FLUSH       0xE7
#define ATA_CMD_CACHE_FLUSH_EXT   0xEA
#define ATA_CMD_PACKET            0xA0
#define ATA_CMD_IDENTIFY_PACKET   0xA1
#define ATA_CMD_IDENTIFY          0xEC

//atapi and i don't know

#define      ATAPI_CMD_READ       0xA8
#define      ATAPI_CMD_EJECT      0x1B
#define ATA_IDENT_DEVICETYPE   0
#define ATA_IDENT_CYLINDERS    2
#define ATA_IDENT_HEADS        6
#define ATA_IDENT_SECTORS      12
#define ATA_IDENT_SERIAL       20
#define ATA_IDENT_MODEL        54
#define ATA_IDENT_CAPABILITIES 98
#define ATA_IDENT_FIELDVALID   106
#define ATA_IDENT_MAX_LBA      120
#define ATA_IDENT_COMMANDSETS  164
#define ATA_IDENT_MAX_LBA_EXT  200

#define IDE_ATA        0x00
#define IDE_ATAPI      0x01

#define ATA_MASTER     0x00
#define ATA_SLAVE      0x01

//ATA OTHER THINGS
#define ATA_PRIMARY_IO 0x1F0
#define ATA_CONTROL_REG 0x3F6
#define ATA_REG_DEVICE 0x06
#define LBA_LOW 0x03
#define LBA_MID 0x04
#define LBA_HI 0x05
#define ATA_CMD_OF 0x07

//My structures for lba

typedef struct {
	uint8_t low;
	uint8_t mid;
	uint8_t hi;
} Lba;

void ata_select() {
	outb(ATA_REG_DEVICE + ATA_PRIMARY_IO, 0xE0); // select the master disk
}
void ata_config_ns(uint16_t ns) {
	outb(ATA_PRIMARY_IO + 0x02, ns); //0x02 = ata number of sector select
}

void ata_conf_lba(Lba lba) {
	outb(ATA_PRIMARY_IO+LBA_LOW, lba.low);
	outb(ATA_PRIMARY_IO+LBA_MID, lba.mid);
	outb(ATA_PRIMARY_IO+LBA_HI, lba.hi);
}

void ata_wait_busy() {
	while (inb(ATA_PRIMARY_IO + ATA_CONTROL_REG) & ATA_SR_BSY);
}

void ata_wait_ready() {
	while (!inb(ATA_PRIMARY_IO + ATA_CONTROL_REG) & ATA_SR_DRQ);
}

void ata_read(Lba lba, uint16_t *buffer, uint16_t ns) {
	ata_select();
	ata_config_ns(ns);
	ata_conf_lba(lba);
	ata_wait_busy();
	ata_wait_ready();
	outb(ATA_PRIMARY_IO + ATA_CMD_OF, ATA_CMD_READ_PIO);
	ata_wait_busy();
	ata_wait_ready();
	for (int i = 0; i < 256*ns; i++) {
		uint16_t data = inw(ATA_PRIMARY_IO);
		buffer[i] = data;
	}
}
void ata_write(Lba lba, uint16_t *buffer, uint16_t ns) {
	ata_select();
	ata_config_ns(ns);
	ata_conf_lba(lba);
	ata_wait_busy();
	ata_wait_ready();
	outb(ATA_PRIMARY_IO + ATA_CMD_OF, ATA_CMD_WRITE_PIO);
	ata_wait_busy();
	ata_wait_ready();
	for (int i = 0; i < ns*256; i++) {
		outw(ATA_PRIMARY_IO, buffer[i]);
	ata_wait_busy();
	}
}
Lba int_lba(uint32_t address) {
	Lba lba;
	lba.low = address & 0xFF;
	lba.mid = (address >> 8) & 0xFF;
	lba.hi = (address >> 16) & 0xFF;
	return lba;
}
void read_disk(uint16_t *buffer, uint32_t lba) {
	Lba address = int_lba(lba);
	uint16_t ns = DISK_BUFF / 256;
	ata_read(address, buffer, ns);
}
void write_disk(uint16_t *buffer, uint32_t lba) {
        Lba address = int_lba(lba);
        uint16_t ns = DISK_BUFF / 256;
        ata_write(address, buffer, ns);
}

