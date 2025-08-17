#ifndef EFIDEF
#define EFIDEF

/*
 * ¡NOTA!
 * el siguiente código fue escrito por mi (Guillermo Leira Temes)
 * basandose en el código de las cabeceras EFI de GNU
 * especificamente de este enlace: https://sourceforge.net/p/gnu-efi/code/ci/master/tree/inc/efidef.h
 * 
 * No deberia haber ningun problema puesto que mi licencia tambien es GPL-3.0
 *
 *



This code is covered by the following agreement:

Copyright (c) 1998-2000 Intel Corporation

Redistribution and use in source and binary forms, with or without modification, are permitted
provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and
the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this list of conditions
and the following disclaimer in the documentation and/or other materials provided with the
distribution.

THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL INTEL BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE. THE EFI SPECIFICATION AND ALL OTHER INFORMATION
ON THIS WEB SITE ARE PROVIDED "AS IS" WITH NO WARRANTIES, AND ARE SUBJECT
TO CHANGE WITHOUT NOTICE.
 
 * ok, esto es suficiente
 * aunque el código lo haya escrito yo, ha sido casí copiando, porque (lo he
 * leido y mientras iba reescribiendolo, es casi copiar, menos que solo
 * lo hago con lo que necesito), de todas formas, esto es la definición de 
 * la interfaz, no deberia de tener ningun problema, de todas formas
 * este código ha sido escrito por mi basandose en GNU-EFI, por lo tanto 
 * agradeceria que se me diese algun credito al portear esto
 * para que no dependa de todas las cabeceras EFI de GNU.
 *
 * - Guillermo Leira Temes 17 de Agosto de 2025 -
 *
 */


#include <stdint.h>
#include <stddef.h>

typedef void *EFI_HANDLE;
typedef void *EFI_EVENT;
typedef uint64_t EFI_LBA;
typedef size_t EFI_STATUS;
typedef size_t EFI_TPL;

/*
 * EFI_GUID
 */

typedef struct {
	uint32_t data1;
	uint16_t data2;
	uint16_t data3;
	uint8_t data4[8];
} EFI_GUID;

/*
 * EFI_TIME
 */

typedef struct {
	uint16_t Year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint8_t pad1;
	uint32_t nanosecond;
	int16_t timezone;
	uint8_t daylight;
	uint8_t pad2;
} EFI_TIME;

// def's for efi_time
#define EFI_TIME_ADJUST_DAYLIGHT 0x01
#define EFI_TIME_IN_DAYLIGHT 0x02

// def's por timezone
#define EFI_UNSPECIFIED_TIMEZONE 0x07FF


/*
 * Network
 */

typedef struct {
	uint8_t addr[4];
} EFI_IPv4;

typedef struct {
	uint8_t addr[16];
} EFI_IPv6;

typedef struct {
	uint8_t addr[32];
} EFI_MAC;

typedef struct {
	uint32_t RecvQueueTimeoutVal;
	uint32_t TrxQueueTimeoutVal;
	uint16_t ProtocolTypeFilter;
	bool EnableUnicastRecv;
	bool EnableMulticastRecv;
	bool EnableBroadcastRecv;
	bool EnablePromiscuousRecv;
	bool FlushQueuesOnReset;
	bool EnableRecvTimestamps;
	bool DisableBackgroundPolling;
} EFI_NETWORK_MANAGED_CONFIG_DATA;

/*
 * Memoria, oh, la memoria
 */

typedef uint64_t EFI_PHYSICAL_ADDRESS;
typedef uint64_t EFI_VIRTUAL_ADDRESS;

typedef enum {
	AllocAnyPages,
	AllocMaxAddr,
	AllocAddr,
	MaxAllocteType
} EFI_ALLOC_TYPE;

typedef enum {
	EfiReservedMemoryType,
	EfiLoaderCode,
	EfiLoaderData,
	EfiBootServicesCode,
	EfiBootServicesData,
	EfiRuntimeServicesCode,
	EfiRuntimeServicesData,
	EfiConventionalMemory,
	EfiUnusableMemory,
	EfiACPIReclaimMemory,
	EfiACPIMemoryNVS,
	EfiMemoryMappedIO,
	EfiMemoryMappedIOPortSpace,
	EfiPalCode,
	EfiPersistentMemory,
	EfiUnacceptedMemoryType,
	EfiMaxMemoryType,
} EFI_MEMORY_TYPE;

// atributos de cacheabilidad
#define EFI_MEM_UC (uint64_t)0x01;
#define EFI_MEM_WC (uint64_t)0x02;
#define EFI_MEM_WT (uint64_t)0x04;
#define EFI_MEM_WB (uint64_t)0x08;
#define EFI_MEM_UCE (uint64_t)0x10;

#endif
