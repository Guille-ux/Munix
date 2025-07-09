#include "common.h"
#include "mbtype.h"

int k_isspace(int c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v');
}

int k_isalpha(int c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

int k_isdigit(int c) {
    return (c >= '0' && c <= '9');
}

int k_isalnum(int c) {
    return k_isalpha(c) || k_isdigit(c);
}
