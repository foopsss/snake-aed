//////////////////////
///// Librerías. /////
//////////////////////

// Para poder usar printf y scanf.
#include <stdio.h>

// Para poder realizar llamadas a system.
#include <stdlib.h>

// Para funciones específicas de Windows, tales
// como GetStdHandle(), SetConsoleCursorPosition(),
// kbhit() y getch().
#include <windows.h>
#include <conio.h>

///////////////////////
///// Miscelánea. /////
///////////////////////

// Constantes para definir la velocidad del juego.
// Básicamente, cada ciclo cuenta 30 milisegundos hasta llegar al
// valor de SPEED (también en milisegundos), lo que equivale a un
// ciclo de juego.
#define FRAMERATE    30
#define SPEED        450

// Constantes correspondientes a los signos a utilizar a la hora de dibujar
// el tablero, la serpiente y la manzana.
#define ASTERISCO    42
#define O_MAYUSCULA  79
#define ESQ_SUP_IZQ  201
#define ESQ_SUP_DER  187
#define ESQ_INF_IZQ  200
#define ESQ_INF_DER  188
#define BARRA_VER    186
#define BARRA_HOR    205

// Constantes correspondientes a las teclas que el usuario puede presionar
// durante el juego.
#define ESCAPE       27
#define ARRIBA       72
#define IZQUIERDA    75
#define DERECHA      77
#define ABAJO        80

// Formato de un registro de tipo "coordenadas".
// Este formato lo vamos a utilizar para definir
// un registro con las coordenadas de la manzana,
// y otro con las coordenadas en las que se va a
// ir moviendo la serpiente.
struct coordenadas {
    int x;
    int y;
};

// Archivo de puntos, en el que vamos a almacenar
// la puntuación de los jugadores.
FILE* puntos;

// Contadores para utilizar en varios ciclos "for".
int i, j;

// hConsole es una variable del tipo HANDLE provisto
// en <windows.h>, que nos permite adueñarnos de la
// ventana de la consola y modificar sus propiedades.
HANDLE hConsole;

void colorear_fondo() {
    // Ponemos el color de fondo de la consola en negro.
    //
    // Los valores del tipo "FOREGROUND..."/"BACKGROUND..."
    // se tratan de canales que nos permiten elegir
    // combinaciones de colores en un formato RGB.
    //
    // En este caso, no necesitamos especificar valores
    // de tipo "BACKGROUND...", porque especificar los
    // tres canales para el color del texto pone el
    // color negro por defecto como el color del fondo.
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void trazar_linea(int longitud) {
    // Función simple para imprimir líneas.
    // Lo usamos para nuestra interfaz gráfica.
    for (i = 0; i < longitud - 1; i++) {
        printf("%c", BARRA_HOR);
    }
    printf("%c\n", BARRA_HOR);
}

void presione_tecla() {
    // getch() nos permite detener el programa hasta
    // recibir un input.
    printf("Presione una tecla para continuar...");
    getch();
}

void ocultar_cursor_consola(int flag) {
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 100;

    // En función del valor de la propiedad
    // bVisible, el cursor va a mostrarse o
    // no por pantalla.
    if (flag == 0) {
        cursorInfo.bVisible = FALSE;
    } else {
        cursorInfo.bVisible = TRUE;
    }

    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void gotoxy(int x, int y) {
    // Le restamos 1 a las coordenadas porque, de lo contrario, gotoxy nos
    // deja una posición por delante de donde realmente esperamos estar
    // cuando especificamos un par [X; Y] en los parámetros.
    COORD c;
    c.X = x - 1;
    c.Y = y - 1;

    SetConsoleCursorPosition(hConsole, c);
}

//////////////////////////////////////////
///// Cosas relacionadas al tablero. /////
//////////////////////////////////////////

// Dimensiones del tablero.
#define filas 25
#define columnas 80

// Posición inicial del tablero en pantalla.
// Estas constantes definen a partir de dónde se empieza
// a mostrar el tablero.
#define pos_inicial_x 5
#define pos_inicial_y 5

// Estructura correspondientes al tablero.
char tablero[filas][columnas];

void cargar_tablero() {
    for (i = 0; i < filas; i++) {
        for (j = 0; j < columnas; j++) {
            tablero[i][j] = ' ';
        }
    }
}

void mostrar_tablero(int pos_x, int pos_y) {
    cargar_tablero();
    gotoxy(pos_x, pos_y);

    for (i = 0; i < filas + 2; i++) {
        if (i == 0 || i == filas + 1) {
            // Estamos trabajando en uno de los bordes
            // del tablero.
            if (i == 0) {
                printf("%c", ESQ_SUP_IZQ);
            } else {
                printf("%c", ESQ_INF_IZQ);
            }

            for (j = 0; j < columnas; j++) {
                printf("%c", BARRA_HOR);
            }

            if (i == 0) {
                printf("%c", ESQ_SUP_DER);
            } else {
                printf("%c", ESQ_INF_DER);
            }
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
                printf("%c", tablero[i - 1][j]);
            }

            printf("%c\n", BARRA_VER);
        }

        pos_y = pos_y + 1;
        gotoxy(pos_x, pos_y);
    }
}

//////////////////////////////////////////////
///// Cosas relacionadas a la serpiente. /////
//////////////////////////////////////////////

// Formato de la lista que define a la serpiente.
// Tipo de dato personalizado, de nombre "serpiente".
typedef struct nodo {
    int tipo; // 0 = cabeza, 1 = cuerpo
    int x;
    int y;
    struct nodo* prox;
} serpiente;

// Tamaño de un nodo de tipo "serpiente".
#define serp_size sizeof(serpiente)

// Cabeza de la serpiente.
serpiente* cabeza;

// Variables a utilizar para manejar la serpiente.
int pos_ant_x_cola;
int pos_ant_y_cola;

void crear_cabeza_serpiente() {
    // Reservamos memoria para el primer elemento
    // de nuestra serpiente, la cabeza.
    cabeza = NULL;
    cabeza = (serpiente*) malloc(serp_size);

    // Le asignamos valores a los elementos de
    // su nodo.
    cabeza -> tipo = 0;
    cabeza -> x = pos_inicial_x + columnas / 2;
    cabeza -> y = pos_inicial_y + filas / 2;
    cabeza -> prox = NULL;
}

void alargar_serpiente(serpiente *serp, int pos_x, int pos_y) {
    // Debemos llegar hasta la cola de la serpiente
    // para ubicar un nuevo elemento al final.
    if (serp -> prox == NULL) {
        serp -> prox = NULL;
        serp -> prox = (serpiente*) malloc(serp_size);
        serp -> prox -> tipo = 1;
        serp -> prox -> x = pos_x;
        serp -> prox -> y = pos_y;
        serp -> prox -> prox = NULL;    
    } else {
        alargar_serpiente(serp -> prox, pos_x, pos_y);
    }
}

void mostrar_serpiente(serpiente *serp) {
    // En función del tipo del elemento, vamos a elegir un color
    // para la parte de la serpiente.
    if (serp -> tipo == 0) {
        // Azul para la cabeza de la serpiente.
        SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | FOREGROUND_BLUE);
    } else if (serp -> tipo == 1) {
        // Verde para el cuerpo.
        SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
    }

    gotoxy(serp -> x, serp -> y);
    printf("%c", ASTERISCO);

    if (serp -> prox != NULL) {
        mostrar_serpiente(serp -> prox);
    } else {
        // Con esto borramos los espacios en los que la serpiente
        // ya no se encuentra tras haberse desplazado.
        gotoxy(pos_ant_x_cola, pos_ant_y_cola);
        printf(" ");
    }
}

void mover_serpiente(serpiente *serp, int pos_x, int pos_y) {
    // Con este método movemos la serpiente de atrás para adelante.
    // Es decir, desde la cola hacia la cabeza. Esto significa que,
    // por un breve momento, habrá dos elementos en la misma posición,
    // aunque no es algo problemático porque el siguiente elemento
    // se mueve de manera casi inmediata.
    if (serp -> prox != NULL) {
        // Estamos en la cabeza o en una posición intermedia, debemos
        // avanzar hasta el último elemento.
        //
        // Nótese que en cada llamada, al próximo elemento le pasamos
        // la posición del elemento actual, a la cual debe moverse el
        // primero.
        mover_serpiente(serp -> prox, serp -> x, serp -> y);
    } else {
        // Estamos en la cola, resguardamos su posición.
        // Con las llamadas a la función que se encuentran apiladas, a
        // medida que vayamos desapilándolas vamos a ir cambiando la
        // posición de la cola y todos los elementos que le siguen.
        pos_ant_x_cola = serp -> x;
        pos_ant_y_cola = serp -> y;
    }

    serp -> x = pos_x;
    serp -> y = pos_y;
}

int colision_tablero() {
    int colision = 0;

    // Detectamos la colisión con las filas y columnas del tablero.
    // Lo hacemos de forma tal que, cuando la serpiente intente avanzar
    // hacia la posición de una fila o columna, se corte el juego.
    if (cabeza -> x == pos_inicial_x || cabeza -> x == pos_inicial_x + columnas + 1) {
        colision += 1;
    }

    if (cabeza -> y == pos_inicial_y || cabeza -> y == pos_inicial_y + filas + 1) {
        colision += 1;
    }

    return colision;
}

int colision_serpiente(serpiente *serp) {
    if (serp != NULL) {
        // Para cada elemento, controlamos si sus coordenadas
        // coinciden o no con las coordenadas de la cabeza. En
        // caso de que esto suceda, la serpiente ha colisionado
        // consigo misma.
        if (cabeza -> x == serp -> x && cabeza -> y == serp -> y) {
            return 1;
        } else if (serp -> prox != NULL) {
            colision_serpiente(serp -> prox);
        } else {
            return 0;
        }
    } else {
        // Como nosotros le pasamos cabeza -> prox a la
        // función, salir por esta condición implica que
        // la serpiente no creció y solo tiene su cabeza.
        return 0;
    }
}

///////////////////////////////////////////
///// Cosas relacionadas a la comida. /////
///////////////////////////////////////////

// Registro correspondiente a la manzana.
struct coordenadas manzana;

int colision_manzana(serpiente *serp) {
    // Necesitamos que esta variable mantenga su
    // valor a lo largo de las llamadas recursivas
    // de un mismo bucle, así que la definimos como
    // "static".
    static int colision = 0;

    if (serp == cabeza) {
        // Estamos comenzando un nuevo bucle de
        // comparaciones, así que limpiamos el
        // valor de la variable colision.
        colision = 0;
    }

    // Comparamos la posición ocupada por una parte de la serpiente
    // con la posición cargada al azar para la manzana.
    colision += (serp -> x == manzana.x) && (serp -> y == manzana.y);

    if (colision == 0) {
        // Si no hubo colisión con la parte tratada de la serpiente,
        // comparamos con las otras partes.
        if (serp -> prox != NULL) {
            colision_manzana(serp -> prox);
        }
    }

    // El "0" significa que no hubo una colisión con
    // ninguna parte de la serpiente, mientras que
    // un "1" indica lo opuesto.
    return colision;
}

int random(int lim_sup) {
    // Devuelve un número al azar, que no debe ser
    // superior a una cota pasada como parámetro.
    return rand() % lim_sup;
}

void crear_manzana() {
    // Le asignamos coordenadas al azar a la manzana, de
    // manera tal que aparezca en cualquier lugar dentro
    // del tablero.
    manzana.x = pos_inicial_x + random(columnas) + 1;
    manzana.y = pos_inicial_y + random(filas) + 1;

    if (colision_manzana(cabeza) != 0) {
        // Si la posición asignada resulta ser una posición
        // en la que se encuentra alguna parte de la serpiente,
        // debemos repetir el proceso.
        crear_manzana();
    }
}

void mostrar_manzana() {
    // Establecemos como rojo el color del carácter a mostrar, que sería
    // nuestra manzana. Luego lo imprimimos.
    SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | FOREGROUND_RED);
    gotoxy(manzana.x, manzana.y);
    printf("%c", O_MAYUSCULA);

    // Devolvemos el color de los caracteres al valor normal.
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}