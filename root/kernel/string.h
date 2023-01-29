int strcmp(const char *str1, const char *str2) {
  int i = 0;
  while (str1[i] != '\0' && str2[i] != '\0') {
    if (str1[i] != str2[i]) {
      return str1[i] - str2[i];
    }
    i++;
  }
  return str1[i] - str2[i];
}

char *strncat(char *dest, const char *src, size_t n) {
size_t dest_len = strlen(dest);
size_t i;

for (i = 0; i < n && src[i] != '\0'; i++) {
dest[dest_len + i] = src[i];
}
dest[dest_len + i] = '\0';

return dest;
}

