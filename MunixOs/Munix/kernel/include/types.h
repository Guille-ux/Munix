//types.h

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
