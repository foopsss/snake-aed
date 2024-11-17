#if defined(_WIN32)
#include <windows.h>
#endif

// Constantes para algunos signos a utilizar a la hora de dibujar el tablero.
#define SIGNO_MAS 053
#define GUION 055
#define BARRA_VER 124

// Dimensiones del tablero.
#define filas 25
#define columnas 40

char tablero[filas][columnas];
int i, j;

void gotoxy(int x, int y) {
    #if defined(_WIN32)
        // En Windows, para poder hacer ciertas acciones por pantalla necesitamos
        // "adueñarnos" de la terminal (en caso de que trabajemos con CMD.exe).
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        COORD c;
        c.X = x;
        c.Y = y;

        SetConsoleCursorPosition(hConsole, c);
    #elif defined(__linux__)
        // Para esta parte estamos usando un pequeño "hack", lamentablemente.
        // En nuestras pruebas, este gotoxy nos deja una unidad menos de donde
        // realmente queremos estar, tanto en X como en Y, por lo que se hace
        // necesario sumarle 1 a las magnitudes indicadas con los parámetros.
        printf("%c[%d;%dH", 0x1B, y + 1, x + 1);
    #endif
}

void cargar_tablero() {
    for (i = 0; i < filas; i++) {
        for (j = 0; j < columnas; j++) {
            tablero[i][j] = '.';
        }
    }
}

void mostrar_tablero() {
    // pos_y indica en donde queremos empezar parados en el eje Y.
    // Debido a que luego la función no trabaja correctamente si
    // utilizamos más de un gotoxy, toca usar esta manera donde
    // inicializamos la variable con una unidad menos de la cual
    // realmente queremos.
    //
    // Luego, la incrementamos inmediatamente entrados a nuestro
    // primer ciclo, de manera que vamos a trabajar con el valor
    // correcto tanto en la primera iteración como en iteraciones
    // posteriores. Poco adecuado, pero necesario.
    int pos_y = 4;

    for (i = 0; i < filas + 2; i++) {
        pos_y = pos_y + 1;
        gotoxy(33, pos_y);

        if (i == 0 || i == filas + 1) {
            // Estamos trabajando en uno de los bordes
            // del tablero.
            printf("%c", SIGNO_MAS);

            for (j = 0; j < columnas; j++) {
                printf(" %c", GUION);
            }

            printf(" %c\n", SIGNO_MAS);
        } else {
            // Estamos trabajando en uno de los puntos
            // intermedios del tablero.
            printf("%c", BARRA_VER);

            for (j = 0; j < columnas; j++) {
                // La matriz "tablero" está cargada
                // desde cero, y si usamos el "i" como
                // viene, nos estaríamos perdiendo una
                // fila de la matriz por empezar a mostrar
                // desde i = 1.
                printf(" %c", tablero[i - 1][j]);
            }

            printf(" %c\n", BARRA_VER);
        }
    }
}
