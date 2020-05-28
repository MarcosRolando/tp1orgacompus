#include "Juego.h"
#include "Estado.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "CharsCelulas.h"
#include "Coordenada.h"

/*i es FILA, j es COLUMNA*/

#define CANTIDAD_MINIMA_SUPERVIVENCIA 2
#define CANTIDAD_MAXIMA_SUPERVIVENCIA 3
#define CANTIDAD_PARA_NACER 3

int obtenerIndiceMatriz(int tam_j, unsigned int i, unsigned int j);

unsigned int vecinos(unsigned char* a, unsigned int i, unsigned int j, unsigned int M, unsigned int N);

Coordenada_t obtenerCoordenadaToroidal(int i, int j, unsigned int tam_i, unsigned int tam_j);

void juegoPrenderCelda(Juego_t* juego, int i, int j) {
    int posicion = obtenerIndiceMatriz(juego->tam_j, i, j);
    juego->tablero[posicion] = PRENDIDO;
}


void juegoApagarCelda(Juego_t* juego, int i, int j) {
    int posicion = obtenerIndiceMatriz(juego->tam_j, i, j);
    juego->tablero[posicion] = APAGADO;
}

/* Actualiza la celda en la nueva tablero */
static void _actualizarCelda(Juego_t* juego, unsigned char* tablero_siguiente, int i, int j) {
  int cant_vecinos = vecinos(juego->tablero, i, j, juego->tam_i, juego->tam_j);
  if ( (cant_vecinos < CANTIDAD_MINIMA_SUPERVIVENCIA) || (cant_vecinos > CANTIDAD_MAXIMA_SUPERVIVENCIA) ) {
    tablero_siguiente[obtenerIndiceMatriz(juego->tam_j, i, j)] = APAGADO;
  } else if (cant_vecinos == CANTIDAD_PARA_NACER) {
    tablero_siguiente[obtenerIndiceMatriz(juego->tam_j, i, j)] = PRENDIDO;
  } else if ((cant_vecinos == CANTIDAD_MINIMA_SUPERVIVENCIA) &&
             (juego->tablero[obtenerIndiceMatriz(juego->tam_j, i, j)] == PRENDIDO)) {
    tablero_siguiente[obtenerIndiceMatriz(juego->tam_j, i, j)] = PRENDIDO;
  } else {
    tablero_siguiente[obtenerIndiceMatriz(juego->tam_j, i, j)] = APAGADO;
  }
}

/* Enciende las celulas en las posiciones que indica el archivo "posiciones_iniciales".
Retorna un codigo de error si hay algun problema prendiendo alguna celula y EXITO
en caso contrario */
int _encenderCeldasIniciales(Juego_t* juego, FILE* posiciones_iniciales) {
  unsigned int i_coord = 0, j_coord = 0;
  int read;
  read = fscanf(posiciones_iniciales, "%u %u\n", &i_coord, &j_coord);
  while (read == 2) {
    if(!juegoAgregarCelda(juego, i_coord, j_coord)) return POSICIONES_INVALIDAS;
    read = fscanf(posiciones_iniciales, "%u %u\n", &i_coord, &j_coord);
  }
  if (!feof(posiciones_iniciales)) return POSICIONES_INVALIDAS;
  return EXITO;
}

/* Crea el tablero del juego */
int juegoCrear(Juego_t* juego, FILE* posiciones_iniciales, int tam_i, int tam_j){
  juego->tablero = malloc(tam_i * tam_j * sizeof(char));
  if (!juego->tablero) {
    return ERROR_DE_MEMORIA;
  }
  memset(juego->tablero, APAGADO, tam_i * tam_j * sizeof(char));
  juego->tam_i = tam_i;
  juego->tam_j = tam_j;

  return _encenderCeldasIniciales(juego, posiciones_iniciales);
}

/* Libera los recursos del juego */
void juegoDestruir(Juego_t* juego){
  free(juego->tablero);
}

/* Retorna true si prende la celula en la posicion fila i y columna j.
Si trato de prender una celula en una posicion invalida devuelve false */
bool juegoAgregarCelda(Juego_t* juego, unsigned int i, unsigned int j){
  if ((i >= juego->tam_i) || (j >= juego->tam_j)) {
    return false;
  }
  juego->tablero[obtenerIndiceMatriz(juego->tam_j, i, j)] = PRENDIDO;
  return true;
}

/* Actualiza el tablero */
void juegoAvanzarEstado(Juego_t* juego){
  unsigned char* tablero_siguiente = malloc(juego->tam_i * juego->tam_j * sizeof(char));
  memset(tablero_siguiente, APAGADO, juego->tam_i * juego->tam_j * sizeof(char));
  for (size_t i = 0; i < juego->tam_i; i++) {
    for (size_t j = 0; j < juego->tam_j; j++) {
      _actualizarCelda(juego, tablero_siguiente, i, j);
    }
  }
  free(juego->tablero);
  juego->tablero = tablero_siguiente;
}

/* Devuelve el tablero del juego */
const unsigned char* juegoObtenerEstadoActual(Juego_t* juego){
  return juego->tablero;
}
