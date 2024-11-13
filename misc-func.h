// misc-func es una librería para almacenar funciones varias que usamos con
// diversos propósitos. Varias de estas funciones están escritas con directivas
// de proprocesador porque no todos los miembros en el grupo trabajamos con
// el mismo sistema operativo, así que requerimos de ellas para poder crear
// algo que funcione en todas las plataformas.

#if defined(_WIN32)
#include <stdlib.h>
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
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
        // En Windows, para poder hacer ciertas acciones por pantalla necesitamos
        // "adueñarnos" de la terminal (en caso de que trabajemos con CMD.exe).
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_BLUE);
    #elif defined(__linux__)
        printf("\033[1m");
        printf("\033[44m");
    #endif
}

void esperar(int segundos) {
    // La función Sleep, tal como está definida en "windows.h", toma un
    // tiempo en milisegundos. Tampoco tiene el mismo nombre que en Linux.
    #if defined(_WIN32)
        Sleep(segundos * 1000);
    #elif defined(__linux__)
        sleep(segundos);
    #endif
}