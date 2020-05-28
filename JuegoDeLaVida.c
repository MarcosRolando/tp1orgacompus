#include "JuegoDeLaVida.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Juego.h"
#include "Estado.h"
#include "Cursor.h"

#define ESCALA_CELULA_PBM 30

#define ARGUMENTOS_EJECUTANDO_CON_NOMBRE_SALIDA 7
#define ARGUMENTOS_MODO_MANUAL 6
#define ARGUMENTOS_EJECUTANDO_SIN_NOMBRE_SALIDA 5
#define ARGUMENTOS_MODO_CONSULTA 2
#define LARGO_MAXIMO_NOMBRE_ARCHIVO_SALIDA 20
#define CHARS_PARA_CANTIDAD_DE_PASOS 5
#define CHARS_EXTENSION_PBM 4
#define CANTIDAD_ARGUMENTOS_CON_NUMERO 3
#define CANTIDAD_DE_MODOS 4

#define CURSOR_CELDA_APAGADA  "+ "
#define CURSOR_CELDA_PRENDIDA "0 "

#define INDICE_CANTIDAD_DE_TURNOS 1
#define INDICE_CANTIDAD_FILAS 2
#define INDICE_CANTIDAD_COLUMNAS 3
#define INDICE_ARCHIVO_DE_ENTRADA 4
#define INDICE_MODO_DE_JUEGO 5
#define INDICE_ARCHIVO_DE_SALIDA 6

#define FFMPEG_BUFFER_LENGTH 42

#define CHAR_PROXIMO_TURNO 'n'
#define CHAR_MOVERSE_IZQUIERDA 'a'
#define CHAR_MOVERSE_DERECHA 'd'
#define CHAR_MOVERSE_ARRIBA 'w'
#define CHAR_MOVERSE_ABAJO 's'
#define CHAR_PRENDER_CELULA 'p'
#define CHAR_APAGAR_CELULA 'o'

#define MIN_CHAR_NUMERO 48
#define MAX_CHAR_NUMERO 57

#define PBM_CELDA_PRENDIDA '0'
#define PBM_CELDA_APAGADA '1'

void mostrarAyuda() {
    printf("Uso:\n"
           "  conway -h\n"
           "  conway -V\n"
           "  conway i M N inputfile [-o outputprefix]\n"
           "Opciones:\n"
           "  -h, --help Imprime este mensaje.\n"
           "  -V, --version Da la versión del programa.\n"
           "  -o Prefijo de los archivos de salida.\n"
           "  -manual Prefijo para modo de edición del tablero en ejecución.\n"
           "Ejemplos:\n"
           "  conway 10 20 20 glider -o estado\n"
           "  Representa 10 iteraciones del Juego de la Vida en una matriz de "
           "20x20,\n"
           "  con un estado inicial tomado del archivo ‘‘glider’’.\n"
           "  Los archivos de salida se llamarán estado_n.pbm.\n"
           "  Si no se da un prefijo para los archivos de salida,\n"
           "  el prefijo será el nombre del archivo de entrada.\n"
           "  Si en vez de -o se escribe -manual y no se agrega el prefijo de "
           "los\n"
           "  archivos de salida entonces se correrá el programa en modo "
           "manual.\n");
}

void mostrarVersion() {
  printf("Versión 1.0\n");
}

int min(int i, int j) {
  int min = i;
  if (min > j) {
    min = j;
  }
  return min;
}

/* Compara los dos strings que recibe y devuelve true si son iguales */
bool _stringsSonIguales(const char* string1, const char* string2) {
  if (strlen(string1) != strlen(string2)) {
    return false;
  }
  return !strcmp(string1, string2);
}


void _escalarCelda(FILE* archivo, char estado) {
    for (int i = 0; i < ESCALA_CELULA_PBM; ++i) {
        fprintf(archivo, "%c ", estado);
    }
}

void _escalarFila(Juego_t* juego, unsigned int i, FILE* archivo) {
  for (int j = 0; j < juego->tam_j; j++) { /*Itera las columnas del tablero*/
      if (juego->tablero[j + i * juego->tam_j] == PRENDIDO) {
          _escalarCelda(archivo, PBM_CELDA_PRENDIDA); /*Imprime ESCALA_CELULA_PBM veces*/
      } else {
          _escalarCelda(archivo, PBM_CELDA_APAGADA); /*Imprime ESCALA_CELULA_PBM veces*/
      }
  }
}

void _imprimirMatrizArchivo(Juego_t* juego, int paso, char* prefijo_archivo_de_salida) {
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
             "_%03d", paso + 1);
    strcat(nombre_archivo_salida, ".pbm");
    printf("Grabando %s\n", nombre_archivo_salida);
    FILE *archivo = fopen(nombre_archivo_salida, "w");
    fprintf(archivo, "P1\n%d %d\n", juego->tam_j*ESCALA_CELULA_PBM, juego->tam_i*ESCALA_CELULA_PBM);
    for (int i = 0; i < juego->tam_i; i++) { /*Itera las filas del tablero*/
        for (int k = 0; k < ESCALA_CELULA_PBM; ++k) { /*Repite la fila ESCALA_CELULA_PBM veces*/
          _escalarFila(juego, i, archivo);
        }
    }
    fclose(archivo);
}

void _imprimirComandos(Juego_t* juego, int paso, int cantidad_de_pasos) {
  printf("Turno %d/%d\n\n", paso + 1, cantidad_de_pasos);
  printf("Arriba: w\n");
  printf("Abajo: s\n");
  printf("Izquierda: a\n");
  printf("Derecha: d\n");
  printf("Encender celda: p\n");
  printf("Apagar celda: o\n");
  printf("Siguiente turno: n\n");
}

void _imprimirMatrizManual(Juego_t* juego, Cursor_t* cursor, int paso, bool quiereEditar,
                                                              int cantidad_de_pasos) {
    for (size_t i = 0; i < juego->tam_i; i++) {
        for (size_t j = 0; j < juego->tam_j; j++) {
            if (cursorEstaEnPosicion(cursor, i, j) && quiereEditar) {
                if (juego->tablero[j + i * juego->tam_j] == APAGADO) {
                    printf(CURSOR_CELDA_APAGADA);
                } else {
                    printf(CURSOR_CELDA_PRENDIDA);
                }
            } else {
                printf("%c ", juego->tablero[j + i * juego->tam_j]);
            }
        }
        printf("\n");
    }
    _imprimirComandos(juego, paso, cantidad_de_pasos);
}

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

void _inputUsuario(Juego_t* juego, Cursor_t* cursor, bool* siguienteTurno, bool* quiereEditar) {
    printf("\n");
    system ("/bin/stty raw");
    int input = getchar();
    *quiereEditar = true;
    switch (input) {
        case CHAR_PROXIMO_TURNO:
            *siguienteTurno = true;
            break;
        case CHAR_MOVERSE_IZQUIERDA:
            cursorMoverIzquierda(cursor);
            break;
        case CHAR_MOVERSE_DERECHA:
            cursorMoverDerecha(cursor, juego->tam_j);
            break;
        case CHAR_MOVERSE_ARRIBA:
            cursorMoverArriba(cursor);
            break;
        case CHAR_MOVERSE_ABAJO:
            cursorMoverAbajo(cursor, juego->tam_i);
            break;
        case CHAR_PRENDER_CELULA:
            juegoPrenderCelda(juego, cursor->posicionCursor_i,
                                                    cursor->posicionCursor_j);
            break;
        case CHAR_APAGAR_CELULA:
            juegoApagarCelda(juego, cursor->posicionCursor_i,
                                                    cursor->posicionCursor_j);
            break;
        default:
            *quiereEditar = false;
    }
    printf("\n");
    system ("/bin/stty cooked");
}


void _procesarMatrizManual(Juego_t* juego, Cursor_t* cursor, int paso_actual,
                                                      int cantidad_de_pasos) {
  bool siguienteTurno = false;
  bool quiereEditar = false;
  while (!siguienteTurno) {
    system("clear");
    _imprimirMatrizManual(juego, cursor, paso_actual, quiereEditar, cantidad_de_pasos);
    _inputUsuario(juego, cursor, &siguienteTurno, &quiereEditar);
  }
}


int _ejecutarJuego(FILE* posiciones_iniciales, int tam_i, int tam_j, int cantidad_de_pasos,
                        char* nombre_archivo_de_salida, bool es_modo_manual) {

  Juego_t juego;
  Cursor_t cursor;
  cursorCrear(&cursor);
  printf("Leyendo estado inicial...\n");
  int estado_de_programa = juegoCrear(&juego, posiciones_iniciales, tam_i, tam_j);

  if (estado_de_programa != EXITO) {
    juegoDestruir(&juego);
    return estado_de_programa;
  }

  for (int i = 0; i < cantidad_de_pasos; ++i) {
    if (es_modo_manual) {
      _procesarMatrizManual(&juego, &cursor, i, cantidad_de_pasos);
    } else {
      _imprimirMatrizArchivo(&juego, i, nombre_archivo_de_salida);
    }
    juegoAvanzarEstado(&juego);
  }

  if (!es_modo_manual) printf("Listo\n");
  cursorDestruir(&cursor);
  juegoDestruir(&juego);
  return EXITO;
}


bool _esNumeroPositivo(const char* string, int largo) {
  if (_stringsSonIguales(string, "0")) {
    return false;
  }
  for (size_t i = 0; i < largo; i++) {
    if ((string[i] < MIN_CHAR_NUMERO) || ((string[i] > MAX_CHAR_NUMERO))) {
      return false;
    }
  }
  return true;
}

bool _sonNumerosValidos(char** args, int cantidad_args_con_numero) {
  for (int i = 0; i < cantidad_args_con_numero; i++) {
    if(!_esNumeroPositivo(args[i], strlen(args[i]))) {
        return false;
    }
  }
  return true;
}

bool _sonArgumentosValidos(char** args, int cantidad_args) {
  const int cantidades_argumentos[CANTIDAD_DE_MODOS] = {
                  ARGUMENTOS_EJECUTANDO_CON_NOMBRE_SALIDA,
                  ARGUMENTOS_MODO_MANUAL, ARGUMENTOS_EJECUTANDO_SIN_NOMBRE_SALIDA,
                  ARGUMENTOS_MODO_CONSULTA};
  bool cantidad_es_valida = false;
  for (int i = 0; i < CANTIDAD_DE_MODOS; i++) {
    if (cantidades_argumentos[i] == cantidad_args) {
      cantidad_es_valida = true;
      break;
    }
  }
  if (!cantidad_es_valida) return false;
  if (cantidad_args != ARGUMENTOS_MODO_CONSULTA) {
      return (_sonNumerosValidos(args + 1, CANTIDAD_ARGUMENTOS_CON_NUMERO));
  }
  return true;
}

void _generarVideoFFMPEG(char* nombre_archivos) {
  int bytes_nombre_archivo = min(strlen(nombre_archivos),
                                   LARGO_MAXIMO_NOMBRE_ARCHIVO_SALIDA);
  int tamanio = FFMPEG_BUFFER_LENGTH + bytes_nombre_archivo + 1;
  char comando_video[tamanio];
  memset(comando_video, 0, tamanio);
  char* primera_parte_comando = "ffmpeg -framerate 1 -i ";
  int chars_copiados = snprintf(comando_video, strlen(primera_parte_comando) + 1,
                                                  "%s", primera_parte_comando);
  snprintf(comando_video + chars_copiados,bytes_nombre_archivo + 1, "%s",
          nombre_archivos);
  chars_copiados += bytes_nombre_archivo;
  char* segunda_parte_comando = "_%03d.pbm juego.avi";
  snprintf(comando_video + chars_copiados, strlen(segunda_parte_comando) + 1,
                                                  "%s", segunda_parte_comando);
  system(comando_video);
}

int _ejecutarComando(char** args, int cantidad_args, FILE* posiciones_iniciales) {
  int estado_de_programa = EXITO;
  switch (cantidad_args) {
    case ARGUMENTOS_EJECUTANDO_CON_NOMBRE_SALIDA:
      if (_stringsSonIguales(args[5], "-o")) {
        estado_de_programa = _ejecutarJuego(posiciones_iniciales,
          atoi(args[INDICE_CANTIDAD_FILAS]), atoi(args[INDICE_CANTIDAD_COLUMNAS]),
          atoi(args[INDICE_CANTIDAD_DE_TURNOS]), args[INDICE_ARCHIVO_DE_SALIDA], false);
        if (estado_de_programa == EXITO) _generarVideoFFMPEG(args[INDICE_ARCHIVO_DE_SALIDA]);
      } else {
        estado_de_programa = ARGUMENTOS_ERRONEOS;
      }
      break;

    case ARGUMENTOS_EJECUTANDO_SIN_NOMBRE_SALIDA:
      estado_de_programa = _ejecutarJuego(posiciones_iniciales,
            atoi(args[INDICE_CANTIDAD_FILAS]),atoi(args[INDICE_CANTIDAD_COLUMNAS]),
            atoi(args[INDICE_CANTIDAD_DE_TURNOS]), args[INDICE_ARCHIVO_DE_ENTRADA], false);
      if (estado_de_programa == EXITO) _generarVideoFFMPEG(args[INDICE_ARCHIVO_DE_ENTRADA]);
      break;

    case ARGUMENTOS_MODO_CONSULTA:
      if ((_stringsSonIguales(args[1], "-h")) ||
          (_stringsSonIguales(args[1], "--help"))) {
        mostrarAyuda();
      } else if ((_stringsSonIguales(args[1], "-V")) ||
          (_stringsSonIguales(args[1], "--version"))) {
          mostrarVersion();
      } else {
        estado_de_programa = ARGUMENTOS_ERRONEOS;
      }
      break;
    case ARGUMENTOS_MODO_MANUAL:
      if (_stringsSonIguales(args[INDICE_MODO_DE_JUEGO], "-manual")) {
        estado_de_programa = _ejecutarJuego(posiciones_iniciales,
              atoi(args[INDICE_CANTIDAD_FILAS]),atoi(args[INDICE_CANTIDAD_COLUMNAS]),
              atoi(args[INDICE_CANTIDAD_DE_TURNOS]), args[INDICE_ARCHIVO_DE_ENTRADA], true);
      } else {
        estado_de_programa = ARGUMENTOS_ERRONEOS;
      }
    }
    return estado_de_programa;
}


int juegoDeLaVidaEjecutar(char** args, int cantidad_args) {
  int estado_de_programa = EXITO;
  FILE* posiciones_iniciales = NULL;

  if (!_sonArgumentosValidos(args, cantidad_args)) {
    _mostrarError(ARGUMENTOS_ERRONEOS);
    return ARGUMENTOS_ERRONEOS;
  }

  if (cantidad_args != ARGUMENTOS_MODO_CONSULTA) {
      posiciones_iniciales = fopen(args[INDICE_ARCHIVO_DE_ENTRADA], "r");
      if(!posiciones_iniciales) {
          _mostrarError(ERROR_APERTURA_ARCHIVO);
          return ERROR_APERTURA_ARCHIVO;
      }
  }

  estado_de_programa = _ejecutarComando(args, cantidad_args, posiciones_iniciales);

  if (posiciones_iniciales != NULL) fclose(posiciones_iniciales);
  _mostrarError(estado_de_programa);
  return estado_de_programa;
}
