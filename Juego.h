#ifndef _JUEGO_H_
#define  _JUEGO_H_

#include <stdbool.h>
#include <stdio.h>
#include "CharsCelulas.h"

typedef struct {
  unsigned char* tablero;
  int tam_i;
  int tam_j;
} Juego_t;

/* Inicializa el objeto Juego */
int juegoCrear(Juego_t* juego, FILE* posiciones_iniciales, int tam_i, int tam_j);

/* Libera los recursos del objeto Juego */
void juegoDestruir(Juego_t* juego);

/* Prende la celula que se encuentra en la posicion del cursor */
void juegoPrenderCelda(Juego_t* juego);

/* Apaga la celula que se encuentra en la posicion del cursor */
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
