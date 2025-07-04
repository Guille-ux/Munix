/* This program is free software: you can redistribute it and/or modify
/* it under the terms of the GNU General Public License as published by
/* the Free Software Foundation, either version 3 of the License, or
/* (at your option) any later version.
/* This program is distributed in the hope that it will be useful,
/* but WITHOUT ANY WARRANTY; without even the implied warranty of
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
/* GNU General Public License for more details.
/* You should have received a copy of the GNU General Public License
/* along with this program. If not, see <https://www.gnu.org/licenses/>.
/* Copyright (c) 2025 Guillermo Leira Temes
/* */

//types.h

#ifndef _TYPES_H
#define _TYPES_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

//definiendo tipos importantes
/*
typedef unsigned char bool;
#define true 1
#define false 0
*/
//-------------------------------------------------//
// toca definir uint
//-------------------------------------------------//




//oh, NULL
/*
#define NULL ( (void *) 0)
*/
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

int len(const char *string);
int strcmp(const char *str1, const char *str2);
int inter(char *text);
void sprint(char *buffer, int nmum);
int ccmp(char *str1, char *str2, int leng);
int sccmp(char *str1, char *str2, unsigned int leng, unsigned int begin);
int cinter(char *text, int len, int begin);

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
    unsigned int unum;
    int i = 0;
    int is_nega = 0;
    char temp[BUFFER_SIZE];

    if (nmum == 0) {
        buffer[i++] = '0';
        buffer[i++] = '\n';
        return;
    }
    if (nmum < 0) {
        is_nega = 1;
        unum = (unsigned int)(-(nmum + 1)) + 1;
    } else {
        unum = (unsigned int)nmum;
    }
    while (unum > 0) {
        temp[i++] = (unum % 10) + '0';
        unum /= 10;
    }

    if (is_nega) {
        temp[i++] = '-';
    }
    for (int j = 0; j < i; j++) {
        buffer[j] = temp[i - (j + 1)];
    }
    buffer[i++] = '\n';
}

int ccmp(char *str1, char *str2, int leng) {
         if (len((const char*)str1) > leng) {
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
int sccmp(char *str1, char *str2, unsigned int leng, unsigned int begin) {
         if (len((const char*)str1) > leng) {
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
	while (text[i + begin] <= '9' && text[i + begin] >= '0' || i >= len) {
		result = result*10 + text[i + begin]-'0';
		i++;
	}
	return result;
}

#endif
