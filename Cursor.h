#ifndef _CURSOR_H_
#define  _CURSOR_H_

#include <stdbool.h>

typedef struct Cursor {
  int posicionCursor_i;
  int posicionCursor_j;
} Cursor_t;


/* Inicializa el objeto Cursor */
void cursorCrear(Cursor_t* cursor);

/* Retorna true si el cursor esta en la posicion fila i y columna j,
* false en caso contrario */
bool cursorEstaEnPosicion(Cursor_t* juego, int i, int j);

/*Desplaza el cursor a la izquierda*/
void cursorMoverIzquierda(Cursor_t* cursor);

/*Desplaza el cursor a la derecha*/
void cursorMoverDerecha(Cursor_t* cursor, int tam_j);

/*Desplaza el cursor hacia arriba*/
void cursorMoverArriba(Cursor_t* juego);

/*Desplaza el cursor hacia abajo*/
void cursorMoverAbajo(Cursor_t* cursor, int tam_i);

/* Libera los recursos del objeto Cursor */
void cursorDestruir(Cursor_t* cursor);


#endif
