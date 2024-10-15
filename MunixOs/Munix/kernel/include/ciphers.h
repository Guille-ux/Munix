//ciphers.h
char *xorer(char *str, int length, int key) {
    for (int i = 0; i < length; i++) {
        str[i] = str[i] ^ (key % 128); // Aplicamos XOR con el valor de la clave
    }
    return str; // Devolvemos el puntero original
}
