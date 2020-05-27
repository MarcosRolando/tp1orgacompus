#ifndef _JUEGO_H_
#define  _JUEGO_H_

#include <stdbool.h>
#include <stdio.h>
#include "CharsCelulas.h"

typedef struct {
    int i;
    int j;
} Coordenada_t;

typedef struct {
  unsigned char* tablero;
  int tam_i;
  int tam_j;
  int posicionCursor_i;
  int posicionCursor_j;
} Juego_t;

//Inicializa el objeto Juego
int juegoCrear(Juego_t* juego, FILE* posiciones_iniciales, int tam_i, int tam_j);

//Libera los recursos del objeto Juego
void juegoDestruir(Juego_t* juego);

bool juegoEstaElCursor(Juego_t* juego, int i, int j);

void juegoMoverCursorIzquierda(Juego_t* juego);

void juegoMoverCursorDerecha(Juego_t* juego);

void juegoMoverCursorArriba(Juego_t* juego);

void juegoMoverCursorAbajo(Juego_t* juego);

void juegoPrenderCelda(Juego_t* juego);

void juegoApagarCelda(Juego_t* juego);

//Intenta agregar la celda en el tablero como una celda inicial encendida
//Si la posicion se encuentra fuera de las dimensiones del tablero devuelve
//false, sino devuelve true
bool juegoAgregarCelda(Juego_t* juego, unsigned int i, unsigned int j);

//Lleva la matriz al siguiente estado tomando en cuenta los estados actuales
void juegoAvanzarEstado(Juego_t* juego);

//Devuelve una matriz de las dimensiones indicadas en el constructor con
//el estado actual del tablero
const unsigned char* juegoObtenerEstadoActual(Juego_t* juego);

#endif
