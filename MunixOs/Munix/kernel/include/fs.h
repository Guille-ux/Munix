//fs.h

#define PROTECTED_BLOCKS ((uint32_t)SIZE/512)//size of the os. SIZE / 512
#define MAX_FILES 1000//the max number of files that the filesystem can handle
#define MAX_SECTORS ((uint32_t)8574934592) //near 8  gb of memory

//file headers
#define MNUMBER 0xF6F3//magic number, it identifie a sector(s) as a file
#define NS_POS 1// the position on a sector that tells the number of sectors of the file
#define NAME_POS 3 //position where begins the name
#define DATA_BEGIN 27 // begin of the important data
#define DATA_END_SYMBOL 0xF7F2
//struct of a File

typedef struct {
	uint16_t ns;
	uint32_t begin;
	char name[51];
} File;

File FileTable[MAX_FILES];

char write_block(uint32_t block, uint16_t *data) {
	block += PROTECTED_BLOCKS; //protecting blocks
	write_disk(data, block);
	return 0;
}
void read_block(uint32_t block, uint16_t *data) {
        block += PROTECTED_BLOCKS; //protecting blocks
        read_disk(data, block);
}

uint16_t list_files() {
	uint16_t buffer[DISK_BUFF];
	uint16_t files = 0;
	for (uint32_t i = 0; files < MAX_FILES || i >= MAX_SECTORS; i++) {
		read_block(i, buffer);
		if (buffer[0] == MNUMBER) {
			FileTable[files].ns = buffer[NS_POS];
			FileTable[files].begin = i;
			for (int f = 0; f < 50; f++){
				FileTable[files].name[f] = (uint8_t)buffer[NAME_POS+f];
			}
			FileTable[files].name[51] = '\n';
			i += FileTable[files].ns;
			files++;
		}
	}
	return files;
}

void write_file(uint16_t sectors, uint16_t *data) { //the data needs to finish with '\n'
	uint16_t last_file = list_files() - 1;
	uint32_t pos = FileTable[last_file].ns + FileTable[last_file];
	for (uint16_t sector = 0; sector < sectors; sector++) {
		uint16_t *buffer = data + 256 * sector;
		write_block(pos+sector, buffer);
	}
}

void add_header(uint16_t *data, uint16_t sectors, const char *name) {
    data[0] = MNUMBER;
    data[NS_POS] = sectors;
    for (int i = 0; i < 50; i++) {
        if (name[i] == '\n') {
            data[NAME_POS + i] = 0;
        } else {
    	    data[NAME_POS + i] = (uint8_t)name[i];
	}
    }

    for (int i = NAME_POS + 50; i < DATA_BEGIN; i++) {
        data[i] = 0;
    }
}
void new_content(uint16_t *data, uint16_t size, const char *name) { // data necesita acabar con '\n'
	int is_begin = 1;
	//data necesita medir un multiplo de 256 y estar la info desplazada 53 bytes
	for (int i = 0; i < size; i++) {
		if (is_begin == 1) {
			is_begin = 0;
			add_header(data[0], size, name);
		}
	}
}
int size_of_file(uint16_t **file) { //size in uitn16_t of a matrix file
	int z = 0;
	for (int i = 0; file[z][i] != DATA_END_SYMBOL; i++) {
		if (i > 256) {
			i = 0;
			z += 1;
		}
	return z*256+i;
	}
}

int size(uint16_t *data) { // midiendo tamaños de una forma normal
	int file_size = 0;
	while (data[file_size] != DATA_END_SYMBOL) {
		file_size++;
	}
	return file_size;
}

void write_new_file(const char *name, uint16_t *data) {
	int data_size = size(data);
	uint16_t tmp[data_size + 256];
	for (int i = 0; i < data_size; i++) {
		tmp[i+256] = data[i];
	}
	sectors = data_size / 256 + 256;
	add_header(tmp, sectors, name);
	write_file(sectors, *data);
}

void read_file(const char *name) {
	//work on this
}
int search_file(const char *name) { // returns the number on the File Table
	//work on this also
}

void remove_file(const char *name) { //changes all bits to 0
	//work on this also
}