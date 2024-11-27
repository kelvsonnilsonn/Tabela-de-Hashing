#include <stdio.h>
#include <math.h>
#define N 57

int hashing(char placa[]) {
    int n1 = 0, n2 = 0;

    // Parte alfabética (AAA)
    n1 = (placa[0] - 'A') * 26 * 26 +
         (placa[1] - 'A') * 26 +
         (placa[2] - 'A');

    // Parte numérica (1234)
    n2 = (placa[3] - '0') * 1000 +
         (placa[4] - '0') * 100 +
         (placa[5] - '0') * 10 +
         (placa[6] - '0');

    // Índice final
    return (n1 + n2) % N;
}

int main() {
    int numeros = hashing("AES3045");
    printf("%d\n", numeros); // Corrigido
}
