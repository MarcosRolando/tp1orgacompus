#include "JuegoDeLaVida.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Juego.h"
#include "Estado.h"

#define ARGUMENTOS_EJECUTANDO_CON_NOMBRE_SALIDA 7
#define ARGUMENTOS_EJECUTANDO_SIN_NOMBRE_SALIDA 5
#define ARGUMENTOS_MODO 2
#define LARGO_MAXIMO_NOMBRE_ARCHIVO_SALIDA 20
#define CHARS_PARA_CANTIDAD_DE_PASOS 3
#define CHARS_EXTENSION_PBM 4


void mostrarAyuda() {
    printf("Uso:\n"
           "conway -h\n"
           "conway -V\n"
           "conway i M N inputfile [-o outputprefix]\n"
           "Opciones:\n"
           "-h, --help Imprime este mensaje.\n"
           "-V, --version Da la versión del programa.\n"
           "-o Prefijo de los archivos de salida.\n"
           "Ejemplos:"
           "conway 10 20 20 glider -o estado\n"
           "Representa 10 iteraciones del Juego de la Vida en una matriz de 20x20,\n"
           "con un estado inicial tomado del archivo ‘‘glider’’.\n"
           "Los archivos de salida se llamarán estado_n.pbm.\n"
           "Si no se da un prefijo para los archivos de salida,\n"
           "el prefijo será el nombre del archivo de entrada.);\n");
}

int min(int i, int j) {
  int min = i;
  if (min > j) {
    min = j;
  }
  return min;
}

/*
<<<<<<< HEAD
void _imprimirMatriz(const unsigned char* matriz, int tam_i, int tam_j, int paso,
                                              char* prefijo_archivo_de_salida) {
=======
void _imprimirMatriz(const unsigned char* matriz, int tam_i, int tam_j, int paso, char* prefijo_archivo_de_salida) {
>>>>>>> 621d25ea59046e7808ee94e805a3b41ab807e1b7
    int tamanio = LARGO_MAXIMO_NOMBRE_ARCHIVO_SALIDA +
                  CHARS_PARA_CANTIDAD_DE_PASOS + CHARS_EXTENSION_PBM + 1;
    char nombre_archivo_salida[tamanio];
    int chars_copiados = min(strlen(prefijo_archivo_de_salida),
                             LARGO_MAXIMO_NOMBRE_ARCHIVO_SALIDA);
    memset(nombre_archivo_salida, 0, tamanio);
    strncat(nombre_archivo_salida, prefijo_archivo_de_salida,
            LARGO_MAXIMO_NOMBRE_ARCHIVO_SALIDA);
    snprintf(nombre_archivo_salida + chars_copiados,
             CHARS_PARA_CANTIDAD_DE_PASOS,
             "%d", paso);
    strcat(nombre_archivo_salida, ".pbm");
    FILE *archivo = fopen(nombre_archivo_salida, "w");
    fprintf(archivo, "P1\n%d %d\n", tam_j, tam_i);
    for (size_t i = 0; i < tam_i; i++) {
        for (size_t j = 0; j < tam_j; j++) {
            fprintf(archivo, "%c ", matriz[j + i * tam_j]);
        }
    }
    fclose(archivo);
}
*/

<<<<<<< HEAD
void _imprimirMatriz(Juego_t* juego, int paso, bool siguienteTurno) {
    for (size_t i = 0; i < juego->tam_i; i++) {
        for (size_t j = 0; j < juego->tam_j; j++) {
            if (juegoEstaElCursor(juego, i, j)) {
                if (juego->tablero[j + i * juego->tam_j] == APAGADO) {
                    printf("+ ");
                } else {
                    printf("0 ");
                }
            } else {
                printf("%c ", juego->tablero[j + i * juego->tam_j]);
            }
=======

void _imprimirMatriz(const unsigned char* matriz, int tam_i, int tam_j, int paso) {
    for (size_t i = 0; i < tam_i; i++) {
        for (size_t j = 0; j < tam_j; j++) {
            printf("%c ", matriz[j + i * tam_j]);
>>>>>>> 621d25ea59046e7808ee94e805a3b41ab807e1b7
        }
        printf("\n");
    }
}

void _mostrarError(int error) {
  switch (error) {
    case ERROR_DE_MEMORIA:
      fprintf(stderr, "Falla la alocacion de memoria\n");
      break;

    case POSICIONES_INVALIDAS:
      fprintf(stderr, "Formato del archivo invalido\n");
      break;
  }
}

void _inputUsuario(Juego_t* juego, bool* siguienteTurno) {
    printf("\n");
    system ("/bin/stty raw");
    int input = getchar();
    switch (input) {
        case 'n':
            *siguienteTurno = true;
            break;
        case 'a':
            juegoMoverCursorIzquierda(juego);
            break;
        case 'd':
            juegoMoverCursorDerecha(juego);
            break;
        case 'w':
            juegoMoverCursorArriba(juego);
            break;
        case 's':
            juegoMoverCursorAbajo(juego);
            break;
        case 'p':
            juegoPrenderCelda(juego);
            break;
        case 'o':
            juegoApagarCelda(juego);
    }
    printf("\n");
    system ("/bin/stty cooked");
}

int _ejecutarJuego(FILE* posiciones_iniciales, int tam_i, int tam_j, int cantidad_de_pasos,
    char* nombre_archivo_de_salida) {

  Juego_t juego;

  int estado_de_programa = juegoCrear(&juego, posiciones_iniciales, tam_i, tam_j);

  if (estado_de_programa != EXITO) {
    _mostrarError(estado_de_programa);
    juegoDestruir(&juego);
    return estado_de_programa;
  }

  for (int i = 0; i < cantidad_de_pasos; ++i) {
    bool siguienteTurno = false;
    //_imprimirMatriz(matriz_actual, tam_i, tam_j, i, nombre_archivo_de_salida);
    while (!siguienteTurno) {
        system("clear");
        _imprimirMatriz(&juego, i, siguienteTurno);
        _inputUsuario(&juego, &siguienteTurno);
    }
    juegoAvanzarEstado(&juego);
  }
  juegoDestruir(&juego);
  return EXITO;
}


int juegoDeLaVidaEjecutar(char** args, int cantidad_args) {

  FILE* posiciones_iniciales = fopen(args[4], "r");
  if(!posiciones_iniciales) {
    fprintf(stderr, "No se pudo abrir el archivo\n");
    return ERROR_APERTURA_ARCHIVO;
  }
  //AGREGAR CASO EN EL QUE NO SE ESCRIBE EL -o Y SU PREFIJO EN LOS ARGUMENTOS
  switch (cantidad_args) {
    case ARGUMENTOS_EJECUTANDO_CON_NOMBRE_SALIDA:
    //CAMBIAR LOS NUMEROS POR CONSTANTES
      return _ejecutarJuego(posiciones_iniciales, atoi(args[2]), atoi(args[3]),
                                                      atoi(args[1]), args[6]);
    case ARGUMENTOS_EJECUTANDO_SIN_NOMBRE_SALIDA:
      return _ejecutarJuego(posiciones_iniciales, atoi(args[2]),atoi(args[3]),
                                                      atoi(args[1]), args[4]);
    case ARGUMENTOS_MODO:
      if (!strncmp(args[1], "-h", 2)) mostrarAyuda();
      break;
    default:
      fprintf(stderr, "Argumentos Erroneos\n");
      return ARGUMENTOS_ERRONEOS;
  }

  //AGREGAR CHEQUEO DE INPUT
  fclose(posiciones_iniciales);
  return EXITO;
}
