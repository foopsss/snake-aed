#if defined(_WIN32)
#include <windows.h>
#endif

#define SIGNO_MAS 053
#define GUION 055
#define BARRA_VER 124

void gotoxy(int x, int y) {
    // Tómese en cuenta que los ejes están "invertidos" a la hora de trabajar
    // con terminales. Por lo tanto, el borde superior es nuestro eje X y el
    // borde izquierdo de la ventana es el eje Y.
    #if defined(_WIN32)
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        COORD c;
        c.X = x;
        c.Y = y;

        SetConsoleCursorPosition(hConsole, c);
    #elif defined(__linux__)
        printf("%c[%d;%dH", 0x1B, y, x);
    #endif
}
