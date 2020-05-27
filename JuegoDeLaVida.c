#include "JuegoDeLaVida.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Juego.h"
#include "Estado.h"

#define ARGUMENTOS_EJECUTANDO_CON_NOMBRE_SALIDA 7
#define ARGUMENTOS_MODO_MANUAL 6
#define ARGUMENTOS_EJECUTANDO_SIN_NOMBRE_SALIDA 5
#define ARGUMENTOS_MODO_AYUDA 2
#define LARGO_MAXIMO_NOMBRE_ARCHIVO_SALIDA 20
#define CHARS_PARA_CANTIDAD_DE_PASOS 3
#define CHARS_EXTENSION_PBM 4
#define CANTIDAD_ARGUMENTOS_CON_NUMERO 3

#define MIN_CANTIDAD_ARGUMENTOS ARGUMENTOS_EJECUTANDO_SIN_NOMBRE_SALIDA
#define MAX_CANTIDAD_ARGUMENTOS ARGUMENTOS_EJECUTANDO_CON_NOMBRE_SALIDA

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

#define MIN_CHAR_NUMERO 49
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

void _escalarCelda(FILE* archivo, char estado) {
    for (int i = 0; i < 30; ++i) {
        fprintf(archivo, "%c ", estado);
    }
}


bool _stringsSonIguales(char* string1, char* string2){
  if (strlen(string1) != strlen(string2)) {
    return false;
  }
  return !strcmp(string1, string2);
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
    fprintf(archivo, "P1\n%d %d\n", juego->tam_j*30, juego->tam_i*30);
    for (size_t i = 0; i < juego->tam_i; i++) {
        for (int k = 0; k < 30; ++k) {
            for (size_t j = 0; j < juego->tam_j; j++) {
                if (juego->tablero[j + i * juego->tam_j] == PRENDIDO) {
                    _escalarCelda(archivo, PRENDIDO);
                } else {
                    _escalarCelda(archivo, APAGADO);
                }
            }
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
      fprintf(stderr, "Falló la alocacion de memoria\n");
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
    *quiereEditar = true;
    switch (input) {
        case CHAR_PROXIMO_TURNO:
            *siguienteTurno = true;
            break;
        case CHAR_MOVERSE_IZQUIERDA:
            juegoMoverCursorIzquierda(juego);
            break;
        case CHAR_MOVERSE_DERECHA:
            juegoMoverCursorDerecha(juego);
            break;
        case CHAR_MOVERSE_ARRIBA:
            juegoMoverCursorArriba(juego);
            break;
        case CHAR_MOVERSE_ABAJO:
            juegoMoverCursorAbajo(juego);
            break;
        case CHAR_PRENDER_CELULA:
            juegoPrenderCelda(juego);
            break;
        case CHAR_APAGAR_CELULA:
            juegoApagarCelda(juego);
            break;
        default:
            *quiereEditar = false;
    }
    printf("\n");
    system ("/bin/stty cooked");
}

int _ejecutarJuego(FILE* posiciones_iniciales, int tam_i, int tam_j, int cantidad_de_pasos,
    char* nombre_archivo_de_salida) {

  Juego_t juego;

  int estado_de_programa = juegoCrear(&juego, posiciones_iniciales, tam_i, tam_j);

  if (estado_de_programa != EXITO) {
    //_mostrarError(estado_de_programa);
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
  for (size_t i = 0; i < largo; i++) {
    if ((string[i] < MIN_CHAR_NUMERO) || ((string[i] > MAX_CHAR_NUMERO))) {
      return false;
    }
  }
  return true;
}

bool _sonNumerosValidos(char** args, int cantidad_args_con_numero){
  for (int i = 0; i < cantidad_args_con_numero; i++) {
    if(!_esNumero(args[i], strlen(args[i]))) {
        return false;
    }
  }
  return true;
}

bool _sonArgumentosValidos(char** args, int cantidad_args){
  if ((cantidad_args < MIN_CANTIDAD_ARGUMENTOS) ||
                                      cantidad_args > MAX_CANTIDAD_ARGUMENTOS) {
    return false;
  }
  if (!_sonNumerosValidos(args + 1, CANTIDAD_ARGUMENTOS_CON_NUMERO)) {
    return false;
  }
  return true;
}


int juegoDeLaVidaEjecutar(char** args, int cantidad_args) {
  int estado_de_programa = EXITO;

  if (!_sonArgumentosValidos(args, cantidad_args)) {
    _mostrarError(ARGUMENTOS_ERRONEOS);
    return ARGUMENTOS_ERRONEOS;
  }

  FILE* posiciones_iniciales = fopen(args[INDICE_ARCHIVO_DE_ENTRADA], "r");
  if(!posiciones_iniciales) {
    _mostrarError(ERROR_APERTURA_ARCHIVO);
    return ERROR_APERTURA_ARCHIVO;
  }

  switch (cantidad_args) {
    case ARGUMENTOS_EJECUTANDO_CON_NOMBRE_SALIDA:
      if (_stringsSonIguales(args[5], "-o")) {
        estado_de_programa = _ejecutarJuego(posiciones_iniciales,
          atoi(args[INDICE_CANTIDAD_FILAS]), atoi(args[INDICE_CANTIDAD_COLUMNAS]),
          atoi(args[INDICE_CANTIDAD_DE_TURNOS]), args[INDICE_ARCHIVO_DE_SALIDA]);
      } else {
        estado_de_programa = ARGUMENTOS_ERRONEOS;
      }
      break;

    case ARGUMENTOS_EJECUTANDO_SIN_NOMBRE_SALIDA:
      estado_de_programa = _ejecutarJuego(posiciones_iniciales,
            atoi(args[INDICE_CANTIDAD_FILAS]),atoi(args[INDICE_CANTIDAD_COLUMNAS]),
            atoi(args[INDICE_CANTIDAD_DE_TURNOS]), args[INDICE_ARCHIVO_DE_ENTRADA]);
      break;

    case ARGUMENTOS_MODO_AYUDA:
      if (_stringsSonIguales(args[1], "-h")) {
        mostrarAyuda();
      } else {
        estado_de_programa = ARGUMENTOS_ERRONEOS;
      }
      break;
    case ARGUMENTOS_MODO_MANUAL:
      if (_stringsSonIguales(args[5], "-manual")){

      } else {
        estado_de_programa = ARGUMENTOS_ERRONEOS;
      }
    }

  fclose(posiciones_iniciales);
  _mostrarError(estado_de_programa);
  return estado_de_programa;
}
