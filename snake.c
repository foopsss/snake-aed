#include "juego.h"

///// Variables globales a utilizar en el juego. /////

// Puntaje del jugador.
int puntaje = 0;

// Longitud de la serpiente. Considera la cabeza de la
// serpiente dentro del total.
int longitud = 1;

///// Funciones. /////

void cargar_puntuacion() {
    // String para almacenar el nombre del jugador luego
    // de que termine la partida.
    char nombre_jugador[100];

    system("cls");

    trazar_linea(110);
    printf("Perdiste.\n");
    printf("Recorda que no debes tocar los bordes del tablero ni hacer que la serpiente colisione consigo misma.\n");

    trazar_linea(110);
    printf("DATOS FINALES DE TU PARTIDA: \n");
    printf("Puntaje: %d puntos\n", puntaje);
    printf("Longitud de la serpiente: %d unidad/es\n", longitud);
    printf("\n");
    printf("Estos datos van a ser grabados en un archivo llamado 'puntuaciones.txt', el cual se va a encontrar en la\n");
    printf("misma carpeta que el ejecutable de este juego.\n");
    
    trazar_linea(110);
    printf("Antes de eso, por favor decinos como te llamas (100 caracteres como maximo).\n\n");
    printf("Nombre: ");
    scanf("%s", nombre_jugador);
    puntos = fopen("puntuaciones.txt", "a");

    if (puntos == NULL) {
        // Debemos crear el archivo de puntuaciones.
        puntos = fopen("puntuaciones.txt", "w");
    }

    if (longitud == 1) {
        // NITPICK: si la serpiente solo tiene 1 unidad, usemos la escritura correcta.
        fprintf(puntos, "Nombre: %s - Puntuacion: %d puntos - Longitud de la serpiente: %d unidad", nombre_jugador, puntaje, longitud);
    } else if (longitud > 1) {
        fprintf(puntos, "Nombre: %s - Puntuacion: %d puntos - Longitud de la serpiente: %d unidades", nombre_jugador, puntaje, longitud);
    }

    fprintf(puntos, "\n");
    fclose(puntos);

    trazar_linea(110);
    printf("A continuacion, vas a ser devuelto/a al menu principal. Desde ahi podes volver a jugar, o revisar las\n");
    printf("puntuaciones de partidas previas en la seccion 'Puntuaciones'.\n");
    
    trazar_linea(110);
    presione_tecla();
}

void limpiar_lista(serpiente *nodo) {
    serpiente* aux;

    while (nodo != NULL) {
        aux = nodo;
        nodo = nodo -> prox;
        free(aux);
    }
}

void jugar() {
    // Variables para controlar si el juego debe terminar o no, si
    // si el usuario presionó una tecla, y para llevar la cuenta
    // del tiempo que pasa hasta que se alcanza el límite de tiempo
    // para realizar una acción con la serpiente.
    int game_over = 0;
    char tecla_presionada = 0;
    int tiempo = 0;

    // Variable para controlar si la serpiente está "digiriendo"
    // una manzana o no.
    int digestion = 0;

    // Este registro nos va a indicar la dirección en la
    // cual se debe mover la serpiente cuando el usuario
    // presiona una tecla.
    //
    // Lo inicializamos de manera tal que la serpiente por
    // defecto se mueva hacia arriba.
    struct coordenadas direccion = {0, -1};

    // Limpiamos la pantalla y ocultamos el cursor de la
    // consola para que se vea un poco mejor el juego.
    system("cls");
    ocultar_cursor_consola(0);

    // Mostramos el tablero, creamos la serpiente y la
    // manzana, y los mostramos por pantalla.
    mostrar_tablero(pos_inicial_x, pos_inicial_y);
    crear_cabeza_serpiente();
    crear_manzana();
    mostrar_serpiente(cabeza);
    mostrar_manzana();

    while (game_over != 1) {
        if (kbhit()) {
            // Obtenemos la tecla presionada por el usuario.
            tecla_presionada = getch();

            // En función de la tecla presionada, determinamos la
            // dirección de la serpiente.
            switch(tecla_presionada) {
            case ARRIBA:
                direccion.x = 0;
                direccion.y = -1;
                break;
            case ABAJO:
                direccion.x = 0;
                direccion.y = 1;
                break;
            case DERECHA:
                direccion.x = 1;
                direccion.y = 0;
                break;
            case IZQUIERDA:
                direccion.x = -1;
                direccion.y = 0;
                break;
            default:
                break;
            }
        }

        // Acumulamos los milisegundos que pasan hasta que se llegue
        // al límite de tiempo para realizar un movimiento.
        //
        // Cuando se supera el límite, movemos la serpiente en la
        // dirección establecida, sea la que eligió el usuario, o la
        // dirección por defecto.
        Sleep(FRAMERATE);
        tiempo += FRAMERATE;

        if (tiempo >= SPEED) {
            mostrar_serpiente(cabeza);
            mostrar_manzana();

            // Mostramos el puntaje y la longitud alcanzada por la serpiente
            // a medida que el jugador come manzanas.
            gotoxy(pos_inicial_x, pos_inicial_y - 3);
            printf("Puntaje: %d puntos", puntaje);
            gotoxy(pos_inicial_x, pos_inicial_y - 2);
            printf("Longitud de la serpiente: %d unidad/es", longitud);

            mover_serpiente(cabeza, cabeza -> x + direccion.x, cabeza -> y + direccion.y);
            game_over = colision_tablero() + colision_serpiente(cabeza -> prox) + (tecla_presionada == ESCAPE);

            if (digestion == 0) {
                // No hay una manzana en digestión, así que comprobamos
                // si la serpiente comió otra manzana o no. Entrar en el
                // siguiente bucle indicaría que chocamos con otra manzana.
                if (colision_manzana(cabeza) != 0) {
                    puntaje += 10;
                    longitud += 1;
                    digestion = 1;
                }
            } else {
                // Una vez que se termine digerir la manzana, le añadimos
                // otro elemento a la serpiente, y ponemos una nueva
                // manzana en el tablero.
                alargar_serpiente(cabeza, manzana.x, manzana.y);
                crear_manzana();
                digestion = 0;
            }

            // Reiniciamos el tiempo para volver a realizar el ciclo
            // de actualización.
            tiempo = 0;
        }
    }

    // Luego de una pequeña pausa, le pedimos al usuario que nos
    // proporcione algo de información para poder registrar su
    // puntaje.
    //
    // A partir de acá, ya no es necesario ocultar el cursor.
    Sleep(5000);
    ocultar_cursor_consola(1);
    cargar_puntuacion();

    // Reseteamos los contadores.
    puntaje = 0;
    longitud = 1;

    // Una vez que el juego termine, limpiamos la memoria ocupada
    // por los elementos de la serpiente.
    limpiar_lista(cabeza);
}

void instrucciones() {
    system("cls");
    trazar_linea(110);
    printf("Para jugar al Snake deben respetarse 2 reglas basicas: \n");
    printf("\n");
    printf("1. No deben tocarse los bordes del tablero.\n");
    printf("2. La serpiente no debe colisionar consigo misma.\n");
    printf("\n");
    printf("Si haces cualquiera de estas dos cosas, la serpiente va a morir!\n");
    printf("Trata de comer todas las manzanas que puedas para que la serpiente se vuelva mas larga.\n");

    trazar_linea(110);
    printf("Por defecto, la serpiente se mueve hacia arriba.\n");
    printf("Para cambiar la direccion, se deben utilizar las flechas del teclado.\n");

    trazar_linea(110);
    printf("SIMBOLOS\n");
    printf("--------\n");

    // Cabeza de la serpiente.
    printf("- ");
    SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | FOREGROUND_BLUE);
    printf("%c", ASTERISCO);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    printf(": ");
    printf("caracter correspondiente a la cabeza de la serpiente.\n");

    // Cuerpo de la serpiente.
    printf("- ");
    SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
    printf("%c", ASTERISCO);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    printf(": ");
    printf("caracter correspondiente a las unidades que conforman el cuerpo de la serpiente.\n");

    // Manzana.
    printf("- ");
    SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | FOREGROUND_RED);
    printf("%c", O_MAYUSCULA);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    printf(": ");
    printf("caracter correspondiente a las manzanas que debe comer la serpiente.\n");

    trazar_linea(110);
    presione_tecla();
}

void puntuaciones() {
    char ventana[200];

    system("cls");
    puntos = fopen("puntuaciones.txt", "r");
    trazar_linea(110);

    if (puntos == NULL) {
        printf("No hay puntuaciones cargadas.\n");
        printf("Luego de jugar algunas partidas vas a poder ver tus puntuaciones en el menu.\n");
    } else {
        // Si el archivo existe, lo recorremos línea por línea
        // e imprimimos cada una de ellas, ya que ahí se encuentran
        // los puntajes de cada jugador.
        while(fgets(ventana, 100, puntos) != NULL) {
            printf("%s", ventana);
        }
        fclose(puntos);
    }

    trazar_linea(110);
    presione_tecla();
}

int main() {
    // Cambiamos el color de fondo de la terminal a negro.
    colorear_fondo();

    // En Windows, para poder hacer ciertas acciones por pantalla necesitamos
    // "adueñarnos" de la terminal (en caso de que trabajemos con CMD.exe).
    //
    // Para ello, cargamos el HANDLE que hemos definido en "juego.h", y que
    // vamos a utilizar muchas veces a lo largo del programa.
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    char eleccion;

    while (eleccion != '4') {
        // Limpiamos la pantalla y ocultamos el cursor.
        system("cls");
        ocultar_cursor_consola(0);
        
        // Mostramos el menú principal.
        trazar_linea(80);
        printf("Bienvenido/a al ");
        SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
        printf("Snake");
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        printf("!\n");

        trazar_linea(80);
        printf("Esta es nuestra version del juego, producida por los siguientes estudiantes: \n");
        printf("(*) Benchat Parra, Lucas Ivan\n");
        printf("(*) Fedorchuk, Francisco Nicolas\n");
        printf("(*) Romaniuk, Lucas Genaro\n");
        printf("(*) Yaya, Franco Gabriel\n");

        trazar_linea(80);
        printf("Cuenta con las siguientes opciones: \n");
        printf("\n");
        printf("1. Jugar.\n");
        printf("2. Instrucciones de juego.\n");
        printf("3. Puntuaciones de partidas pasadas.\n");
        printf("4. Salir.\n");
        printf("\n");
        printf("Presione el boton correspondiente a la opcion que desee elegir.");
        eleccion = getch();

        switch(eleccion) {
        case '1':
            jugar();
            break;
        case '2':
            instrucciones();
            break;
        case '3':
            puntuaciones();
            break;
        case '4':
            break;
        }
    }

    return 0;
}
