#include <stdio.h>
#include "misc-func.h"

int main() {
    char eleccion;
    colorear_fondo();

    // Leemos un carácter porque entonces a la hora de compararlo no vamos
    // a crashear todo el programa si un usuario introduce algo que no
    // corresponde, o que no tiene el mismo tipo de dato, si trabajamos con
    // un entero.
    while (eleccion != '3') {
        clrscr();
        printf("Bienvenido al Snake!\n");
        printf("Este es el menu principal, cuenta con las siguientes opciones: \n");
        printf("--------------------------------------------------------------\n");
        printf("1. Jugar.\n");
        printf("2. Creditos.\n");
        printf("3. Salir.\n");
        printf("\n");
        printf("Introduzca una opcion: ");
        scanf(" %c", &eleccion);

        switch(eleccion) {
        case 1:
            // empezar_juego();
        case 2:
            // creditos();
            break;
        case 3:
            break;
        }
    }

    return 0;
}

