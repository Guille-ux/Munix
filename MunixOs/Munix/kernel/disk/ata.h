#ifndef ATA_DRIVERS_H
#define ATA_DIRVERS_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "../include/libcs2.h"

/*
 * Vale, supuestamente me sera más fácil hacerlo con una interfaz de disco
 *
 */

typedef enum {
	ATADEV_UNKNOWN,
	ATADEV_ATA,
	ATADEV_ATAPI,
	ATADEV_SATAPI,
	ATADEV_SATA,
} AtaDevType;

typedef struct {
	AtaDevType type;
	bool lba48_supported;
	uint8_t drv;
	uint8_t channel;
	uint16_t io_base;
	uint16_t control_base;
	uint64_t total_sectors;
	uint16_t identify_data[256];
} ata_device_t;

/*
 * Ok, esto son las bases para cada cosa, y tambien los irq
 *
 */

#define ATA_PRIMARY_BASE 0x1F0
#define ATA_SECONDARY_BASE 0x170

#define ATA_PRIMARY_CONTROL_BASE 0x3F6
#define ATA_SECONDARY_CONTROL_BASE 0x376

#define ATA_PRIMARY_IRQ 14
#define ATA_SECONDARY_IRQ 15

/*
 * Muy bien, ahora toca el comando identify, o eso dice osdev
 *
 */

#define ATA_SELECT_MASTER 0xA0
#define ATA_SELECT_SLAVE 0xB0
#define ATA_IDENTIFY_COMMAND 0xEC

/*
 * Importante! primero se selecciona disco en el io + 6 (eso esta explicado
 * más adelate), justo despues pones los registros lba a 0
 * luego escribes el comando identify (0xEC) en io + 7 (tambien
 * explicado más adelante) y finalmente (entre comillas)
 * se lee el status register en io + 7
 * si en el status aparece 0 entonces el disco no existe
 * en cambio, para cualquier otro valor se debe esperar hasta que el bit 7
 * del status register
 */

/*
 * Offsets!
 * wadkshjksdjn
 * son los offsets para cada cosa
 * se usa la base, pero los offsets son iguales para ambos
 * pero oye, se usan las base:
 *  -> Canal Primario : ATA_PRIMARY_BASE
 *  -> Canal Secundario : ATA_SECONDARY_BASE
 *
 */

#define ATA_DATA_REG 0
#define ATA_ERROR_REG 1 // read only
#define ATA_FEATURE_REG 1 // write only
#define ATA_SECTOR_COUNT_REG 2
#define ATA_SECTOR_NUM_REG 3 // LBA lo
#define ATA_CYLINDER_LO_REG 4 // LBA mid
#define ATA_CYLINDER_HI 5 // LBA hi
#define ATA_DRIVE_HEAD_REG 6 // it's for select a head or drive, idk
#define ATA_STATUS_REG 7 // read only
#define ATA_COMMAND_REG 7 // write only

/*
 * Ok, esos fueron para la base, los siguientes son de los registros de
 * control
 * y no te preocupes
 * QUEDAN MUCHOS MÁS
 *
 */

#define ATA_ALTERNATE_STATUS_REG 0 // read only
#define ATA_DEVICE_CONTROL_REG 0 // write only
#define ATA_DRIVE_ADDRESS_REG 1

/*
 * Vale, ya hice los de control
 * ahora tocan los de error
 * y ni siquiera se cual es el puerto base
 * supongo que sera para cuando lea el error register ¿no?
 * si, son para cuando lea el error reg, voy a hacer funciones auxiliares, no
 * se que hago, creo que si, voy a hacer un struct que represente los errores
 *
 */

#define ATA_AMNF_ORDER 0
#define ATA_TKZNF_ORDER 1
#define ATA_ABRT_ORDER 2
#define ATA_MCR_ORDER 3
#define ATA_IDNF_ORDER 4
#define ATA_MC_ORDER 5
#define ATA_UNC_ORDER 6
#define ATA_BBK_ORDER 7

typedef struct {
	bool amnf; // Addres Mark Not Found -> AMNF
	bool tkznf; // TracK Zero Not Found -> TKZNF
	bool abrt; // ABoRT -> ABRT
	bool mcr; // Media Change Request -> MCR
	bool idnf; // ID Not Found -> IDNF
	bool mc; // Media Changed
	bool unc; // UNCorrectable data error -> UNC
	bool bbk; // Bad BlocK detected -> BBK
} ATAError;

ATAError wtfAtaErrors(uint8_t errOut);

/*
 * Muy Bien, ahora es algo similar pero para los head registers
 * (io + 6)
 *
 */

typedef struct {
	uint8_t idk; // in chs address -> bits 0-3 of the head, in LBA 28 is 24 - 27 bits
	uint8_t drv; // drive number, only 1 bit
	bool lba; // chs if clear or lba if set, only 1 bit
} DriveHeadRegCfg; // nombre mu largo

DriveHeadRegCfg byte2driveHeadCfg(uint8_t byte);
uint8_t driveHeadCfg2byte(DriveHeadRegCfg cfg);

/*
 * Ok, ahora toca hacer algo similar pero con el puerto de status
 * he mirado y resulta que tambien se puede leer lo mismo que se lee en
 * io + 7 (Status Register) en el Alternate Status Register, que hace lo mismo
 * sin afectar a las interrupciones
 *
 */

#define ATA_STATUS_REG_ERR 1
#define ATA_STATUS_REG_IDX 2 // no nos importa (porque siempre es 0), pero da igual
#define ATA_STATUS_REG_CORR (1 << 2) // lo mismo, siempre es 0
#define ATA_STATUS_REG_DRQ (1 << 3) // cuando tiene cosas que enviar o recibir
#define ATA_STATUS_REG_SRV (1 << 4) // overlaped bla bla, no se lo que es
#define ATA_STATUS_REG_DF (1 << 5) // un drive fault, que no da un err, raro
#define ATA_STATUS_REG_RDY (1 << 6) // a ver, rdy me suena a ready, ¿no?
#define ATA_STATUS_REG_BSY (1 << 7) // bueno, bsy == busy, pero se supone
				    // que lo demás no tiene significado
				    // cuando bsy (o busy) esta a 1
/*
 * Registro de control del dispositivo
 * parece útil, pero creo que no lo es a menos para mi causa
 * o si, no se si me voy a cargar el disco, quien sabe?
 *
 */
#define _2SEC_ON_NANO_SEC 2*1000*1000*1000

static inline bool ata_wait_busy(ata_device_t *device) {
	for (int i = 0;i<15;i++) {//segun osdev hay que usar retrasos de 400ns
		inb(device->control_base);
	}
	uint32_t timeout = _2SEC_ON_NANO_SEC; // 2 segundos en nanosegundos
	while (((inb(device->control_base) & ATA_STATUS_REG_BSY ) && timeout==0) != 0) {
		timeout--;
	}
	if (timeout == 0) return false;
	return true;
}

static inline bool ata_wait_drq(ata_device_t *device) {
	for (int i = 0;i<15;i++) {//segun osdev hay que usar retrasos de 400ns
		inb(device->control_base);
	}
	uint32_t timeout = _2SEC_ON_NANO_SEC / 2; // 2 segundos en nanosegundos
	while (((inb(device->control_base) ^ ATA_STATUS_REG_DRQ ) && timeout==0) != 0) {
		timeout--;
	}
	if (timeout == 0) return false;
	return true;
}

static inline bool ata_wait_ready(ata_device_t *device) {
	for (int i = 0;i<15;i++) {//segun osdev hay que usar retrasos de 400ns
		inb(device->control_base);
	}
	uint64_t timeout = _2SEC_ON_NANO_SEC * 15; // 2 segundos en nanosegundos
	while (((inb(device->control_base) ^ ATA_STATUS_REG_RDY ) && timeout==0) != 0) {
		timeout--;
	}
	if (timeout == 0) return false;
	return true;
}

bool ata_identify_device(ata_device_t *device);
AtaDevType ata_detect_device(ata_device_t *device);
void ataSoftReset(ata_device_t *device);


#endif
