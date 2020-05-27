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
#define CANTIDAD_ARGUMENTOS_CON_NUMERO 3

#define INDICE_CANTIDAD_DE_TURNOS 1
#define INDICE_CANTIDAD_FILAS 2
#define INDICE_CANTIDAD_COLUMNAS 3
#define INDICE_ARCHIVO_DE_ENTRADA 4
#define INDICE_ARCHIVO_DE_SALIDA 6

#define CHAR_PROXIMO_TURNO 'n'
#define CHAR_MOVERSE_IZQUIERDA 'a'
#define CHAR_MOVERSE_DERECHA 'd'
#define CHAR_MOVERSE_ARRIBA 'w'
#define CHAR_MOVERSE_ABAJO 's'
#define CHAR_PRENDER_CELULA 'p'
#define CHAR_APAGAR_CELULA 'o'

#define MIN_CHAR_NUMERO 48
#define MAX_CHAR_NUMERO 57

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


void _imprimirMatriz(Juego_t* juego, int paso, char* prefijo_archivo_de_salida) {
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
    fprintf(archivo, "P1\n%d %d\n", juego->tam_j, juego->tam_i);
    for (size_t i = 0; i < juego->tam_i; i++) {
        for (size_t j = 0; j < juego->tam_j; j++) {
            fprintf(archivo, "%c ", juego->tablero[j + i * juego->tam_j]);
        }
    }
    fclose(archivo);
}

/*
void _imprimirMatriz(Juego_t* juego, int paso, bool quiereEditar) {
    for (size_t i = 0; i < juego->tam_i; i++) {
        for (size_t j = 0; j < juego->tam_j; j++) {
            if (juegoEstaElCursor(juego, i, j) && quiereEditar) {
                if (juego->tablero[j + i * juego->tam_j] == APAGADO) {
                    printf("+ ");
                } else {
                    printf("0 ");
                }
            } else {
                printf("%c ", juego->tablero[j + i * juego->tam_j]);
            }
        }
        printf("\n");
    }
}
*/
void _mostrarError(int error) {
  switch (error) {
    case ERROR_DE_MEMORIA:
      fprintf(stderr, "Fallo la alocacion de memoria\n");
      break;
    case POSICIONES_INVALIDAS:
      fprintf(stderr, "Formato del archivo inválido\n");
      break;
    case ARGUMENTOS_ERRONEOS:
      fprintf(stderr, "Argumentos inválidos\n");
      break;
    case ERROR_APERTURA_ARCHIVO:
      fprintf(stderr, "No se pudo abrir el archivo\n");
      break;
  }
}

void _inputUsuario(Juego_t* juego, bool* siguienteTurno, bool* quiereEditar) {
    printf("\n");
    system ("/bin/stty raw");
    int input = getchar();

    switch (input) {
        case CHAR_PROXIMO_TURNO:
            *siguienteTurno = true;
            break;
        case CHAR_MOVERSE_IZQUIERDA:
            juegoMoverCursorIzquierda(juego);
            *quiereEditar = true;
            break;
        case CHAR_MOVERSE_DERECHA:
            juegoMoverCursorDerecha(juego);
            *quiereEditar = true;
            break;
        case CHAR_MOVERSE_ARRIBA:
            juegoMoverCursorArriba(juego);
            *quiereEditar = true;
            break;
        case CHAR_MOVERSE_ABAJO:
            juegoMoverCursorAbajo(juego);
            *quiereEditar = true;
            break;
        case CHAR_PRENDER_CELULA:
            juegoPrenderCelda(juego);
            *quiereEditar = true;
            break;
        case CHAR_APAGAR_CELULA:
            juegoApagarCelda(juego);
            *quiereEditar = true;
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
    //bool siguienteTurno = false;
    //bool quiereEditar = false;
    _imprimirMatriz(&juego, i, nombre_archivo_de_salida);
    /*
    while (!siguienteTurno) {
        system("clear");
        _imprimirMatriz(&juego, i, quiereEditar);
        _inputUsuario(&juego, &siguienteTurno, &quiereEditar);
    }
    */
    juegoAvanzarEstado(&juego);
  }
  juegoDestruir(&juego);
  return EXITO;
}


bool _esNumero(const char* string, int largo){
  for (size_t i = 0; i < len; i++) {
    if ((string[i] < MIN_CHAR_NUMERO) || ((string[i] > MAX_CHAR_NUMERO))) {
      return false;
    }
  }
  return true;
}

bool _sonArgumentosValidos(char** args, int cantidad_args){
  for (int i = 0; i < cantidad_args; i++) {
    if(!_esNumero(args[i], strlen(args[i])){
        return false;
    }
  }
  return true;
}


int juegoDeLaVidaEjecutar(char** args, int cantidad_args) {
  int estado_de_programa = EXITO;
  if (!_sonArgumentosValidos(args + 1, CANTIDAD_ARGUMENTOS_CON_NUMERO)) {
    //fprintf(stderr, "Argumentos inválidos\n");
    return ARGUMENTOS_ERRONEOS;
  }

  FILE* posiciones_iniciales = fopen(args[INDICE_ARCHIVO_DE_ENTRADA], "r");
  if(!posiciones_iniciales) {
    fprintf(stderr, "No se pudo abrir el archivo\n");
    return ERROR_APERTURA_ARCHIVO;
  }

  switch (cantidad_args) {
    case ARGUMENTOS_EJECUTANDO_CON_NOMBRE_SALIDA:
      estado_de_programa = _ejecutarJuego(posiciones_iniciales,
            atoi(args[INDICE_CANTIDAD_FILAS]), atoi(args[INDICE_CANTIDAD_COLUMNAS]),
            atoi(args[INDICE_CANTIDAD_DE_TURNOS]), args[INDICE_ARCHIVO_DE_SALIDA]);

    case ARGUMENTOS_EJECUTANDO_SIN_NOMBRE_SALIDA:
      estado_de_programa = _ejecutarJuego(posiciones_iniciales,
        atoi(args[INDICE_CANTIDAD_FILAS]),atoi(args[INDICE_CANTIDAD_COLUMNAS]),
        atoi(args[INDICE_CANTIDAD_DE_TURNOS]), args[INDICE_ARCHIVO_DE_ENTRADA]);

    case ARGUMENTOS_MODO:
      if (!strncmp(args[1], "-h", 2)) mostrarAyuda();
      break;
    default:
      //fprintf(stderr, "Argumentos inválidos\n");
      estado_de_programa = ARGUMENTOS_ERRONEOS;
  }
  fclose(posiciones_iniciales);
  _mostrarError(estado_de_programa);
  return estado_de_programa;
}
