// misc-func es una librería para almacenar funciones varias que usamos con
// diversos propósitos. Varias de estas funciones están escritas con directivas
// de proprocesador porque no todos los miembros en el grupo trabajamos con
// el mismo sistema operativo, así que requerimos de ellas para poder crear
// algo que funcione en todas las plataformas.

#if defined(_WIN32)
#include <stdlib.h>
#include <windows.h>
#endif

void clrscr() {
    #if defined(_WIN32)
        system("cls");
    #elif defined(__linux__)
        printf("\e[1;1H\e[2J");
    #endif
}

void colorear_fondo() {
    #if defined(_WIN32)
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_BLUE);
    #elif defined(__linux__)
        printf("\033[1m");
        printf("\033[44m");
    #endif
}
