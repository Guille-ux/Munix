//types.h


//COLORS
#define BLACK 0x00
#define BLUE 0x01
#define GREEN 0x02
#define CYAN 0x03
#define RED 0x04
#define MAGENTA 0x05
#define BROWN 0x06
#define WHITE 0x07
#define GRAY 0x08
#define LIGHT_BLUE 0x09
#define LIGHT_GREEN 0x0A
#define LIGHT_CYAN 0x0B
#define LIGHT_RED 0x0C
#define LIGHT_MAGENTA 0x0D
#define YELLOW 0x0E
#define WHITE 0x0F

//STRUCTS AND MORE
typedef struct {
	uint8_t background;
	uint8_t text;
} Color;

typedef struct {
	int result;
	int nseed;
} Random;

typedef struct {
	int pos;
	int posc;
} Pos;
typedef struct {
	char *text;
	int len;
} String;

int len(const char *string) {
    int length = 0;
    unsigned char ch;
    while ((ch = string[length]) != '\n') {
        length++;
    }

    return length;
}
int strcmp(const char *str1, const char *str2) {
	if (len(str1) != len(str2)) {
		return 0;
	} else {
		for (int i = 0; i < len(str1); i++) {
			if (str1[i] != str2[i]) {
				return 0;
			}
		}
	}
	return 1;
}
int inter(char *text) {
	int i = 0;
	int result;
	int sign;
	while (text[i] == ' ') {
		i++;
	}
	if (text[i] > '9' || text[i] < '0') {
		return 0;
	} else if (text[i] == '-') {
		sign = -1;
		i++;
	} else if (text[i] == '+') {
		sign = 1;
		i++;
	} else {
		sign = 1;
	}
	while (text[i] <= '9' && text[i] >= '0') {
		result = result*10 + text[i]-'0';
		i++;
	}
	return result;
}
void sprint(char *buffer, int nmum) {
	int num = nmum*10;
	int i = 0;
	int is_nega = 0;
	int nlen = 0;
	char m[BUFFER_SIZE];
	if (num < 0) {
		is_nega = 1;
	}
	while (num != 0) {
		m[i] = num % 10 + '0';
		i++;
		num /= 10;
	}
	if (is_nega == 1) {
		m[i] = '-';
	}
	for (int x = i; x > 0; x--) {
		buffer[x-1] = m[x];
	}
	buffer[i++] = '\n';
}
int ccmp(char *str1, char *str2, int leng) {
         if (len(*str1 ) > leng) {
                for (int i = 0; i < leng; i++) {
                        if (str1[i] != str2[i]) {
                                return 0;
                        }
                }
        } else {
		return 0;
	}
        return 1;

}
int sccmp(char *str1, char *str2, int leng, int begin) {
         if (len(*str1 ) > leng) {
                for (int i = 0; i < leng; i++) {
                        if (str1[i + begin] != str2[i]) {
                                return 0;
                        }
                }
        } else {
		return 0;
	}
        return 1;

}
int cinter(char *text, int len, int begin) {
	int i = 0;
	int result;
	int sign;
	while (text[i + begin] == ' ') {
		i++;
	}
	if (text[i + begin] > '9' || text[i + begin] < '0') {
		return 0;
	} else if (text[i + begin] == '-') {
		sign = -1;
		i++;
	} else if (text[i + begin] == '+') {
		sign = 1;
		i++;
	} else {
		sign = 1;
	}
	while (text[i + begin] <= '9' && text[i + begin] >= '0' || i => len) {
		result = result*10 + text[i + begin]-'0';
		i++;
	}
	return result;
}
