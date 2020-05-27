#include "Juego.h"
#include "Estado.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*i es FILA, j es COLUMNA*/

#define CANTIDAD_MINIMA_SUPERVIVENCIA 2
#define CANTIDAD_MAXIMA_SUPERVIVENCIA 3
#define CANTIDAD_PARA_NACER 3


int obtenerIndiceMatriz(int tam_j, unsigned int i, unsigned int j);

unsigned int vecinos(unsigned char* a, unsigned int i, unsigned int j, unsigned int M, unsigned int N);

/*
int obtenerIndiceMatriz(int tam_j, unsigned int i, unsigned int j) {
  return j + i * tam_j;
}

//Recibe la coordenada de una celda y devuelve la coordenada que le corresponde
//en la tablero toroidal
Coordenada_t obtenerCoordenadaToroidal(int i, int j, unsigned int tam_i, unsigned int tam_j) {
  Coordenada_t coordenada;
  coordenada.i = i;
  coordenada.j = j;

  if (i >= tam_i) {
    coordenada.i = 0;
  } else if (i < 0) {
    coordenada.i = tam_i - 1;
  }

  if (j >= tam_j) {
    coordenada.j = 0;
  }else if (j < 0) {
    coordenada.j = tam_j - 1;
  }
  return coordenada;
}
*/
bool juegoEstaElCursor(Juego_t* juego, int i, int j) {
    return ( (i == juego->posicionCursor_i)  && (j == juego->posicionCursor_j) );
}

void juegoMoverCursorIzquierda(Juego_t* juego) {
    if (juego->posicionCursor_j > 0) {
        juego->posicionCursor_j--;
    }
}

void juegoMoverCursorDerecha(Juego_t* juego) {
    if (juego->posicionCursor_j < (juego->tam_j - 1)) {
        juego->posicionCursor_j++;
    }
}

void juegoMoverCursorArriba(Juego_t* juego) {
    if (juego->posicionCursor_i > 0) {
        juego->posicionCursor_i--;
    }
}

void juegoMoverCursorAbajo(Juego_t* juego) {
    if (juego->posicionCursor_i < (juego->tam_i - 1)) {
        juego->posicionCursor_i++;
    }
}

void juegoPrenderCelda(Juego_t* juego) {
    int posicion = obtenerIndiceMatriz(juego->tam_j, juego->posicionCursor_i, juego->posicionCursor_j);
    juego->tablero[posicion] = PRENDIDO;
}

void juegoApagarCelda(Juego_t* juego) {
    int posicion = obtenerIndiceMatriz(juego->tam_j, juego->posicionCursor_i, juego->posicionCursor_j);
    juego->tablero[posicion] = APAGADO;
}
/*
//a es un puntero a la posicion 0,0 de la tablero (es el array de la matriz), M es tamanio fila, N es tamanio columna
unsigned int vecinos(unsigned char *a, unsigned int i, unsigned int j, unsigned int M, unsigned int N) {
  unsigned int vecinos_encendidos = 0;
  Coordenada_t coordenadaVecino;
  int posVecino = 0;
  for (int k = -1; k < 2; k++) {
    for (int l = -1; l < 2; l++) {

      if (k != 0 || l != 0) {
        //REVISAR SI ESTO ANDA
        coordenadaVecino = obtenerCoordenadaToroidal(i + k, j + l, M, N);
        posVecino = obtenerIndiceMatriz(N, coordenadaVecino.i, coordenadaVecino.j);
        if (a[posVecino] == PRENDIDO) {
            vecinos_encendidos++;
        }
      }

    }
  }
  return vecinos_encendidos;
}
*/
/*Actualiza la celda en la nueva tablero*/
static void _actualizarCelda(Juego_t* juego, unsigned char* tablero_siguiente, int i, int j) {
  int cant_vecinos = vecinos(juego->tablero, i, j, juego->tam_i, juego->tam_j); //hay que implementar esto
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

int _encenderCeldasIniciales(Juego_t* juego, FILE* posiciones_iniciales) { //PREGUNTAR SI PUEDE VENIR CON ERRORES EL ARCHIVO
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

int juegoCrear(Juego_t* juego, FILE* posiciones_iniciales, int tam_i, int tam_j){
  juego->tablero = malloc(tam_i * tam_j * sizeof(char));
  if (!juego->tablero) {
    return ERROR_DE_MEMORIA;
  }
  memset(juego->tablero, APAGADO, tam_i * tam_j * sizeof(char));
  juego->tam_i = tam_i;
  juego->tam_j = tam_j;
  juego->posicionCursor_i = 0;
  juego->posicionCursor_j = 0;

  return _encenderCeldasIniciales(juego, posiciones_iniciales);
}

void juegoDestruir(Juego_t* juego){
  free(juego->tablero);
}


//PREGUNTAR SI EN EL ARCHIVO LA tablero EMPIEZA EN 0,0 O EN 1,1
bool juegoAgregarCelda(Juego_t* juego, unsigned int i, unsigned int j){
  if ((i >= juego->tam_i) || (j >= juego->tam_j)) {
    return false;
  }
  juego->tablero[obtenerIndiceMatriz(juego->tam_j, i, j)] = PRENDIDO;
  return true;
}


void juegoAvanzarEstado(Juego_t* juego){
  unsigned char* tablero_siguiente = malloc(juego->tam_i * juego->tam_j * sizeof(char));
  memset(tablero_siguiente, APAGADO, juego->tam_i * juego->tam_j * sizeof(char));

  //COMPLETAR IMPLEMENTACION
  for (size_t i = 0; i < juego->tam_i; i++) {
    for (size_t j = 0; j < juego->tam_j; j++) {
      _actualizarCelda(juego, tablero_siguiente, i, j);
    }
  }
  //LJDFALJKJDNSDKABFNLAKJFNSKDVNLKSNDAD
  //ASN KAJDSNF.MAN-AKDF-ALFN.ANF-ÑAKFMA.,NDFASÑD
  free(juego->tablero);
  juego->tablero = tablero_siguiente;
}


const unsigned char* juegoObtenerEstadoActual(Juego_t* juego){
  return juego->tablero;
}
