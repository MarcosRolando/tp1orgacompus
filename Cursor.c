#include "Cursor.h"

void cursorCrear(Cursor_t* cursor) {
  cursor->posicionCursor_i = 0;
  cursor->posicionCursor_j = 0;
}

bool cursorEstaEnPosicion(Cursor_t* cursor, int i, int j) {
    return ( (i == cursor->posicionCursor_i)  && (j == cursor->posicionCursor_j) );
}

void cursorMoverIzquierda(Cursor_t* cursor) {
    if (cursor->posicionCursor_j > 0) {
        cursor->posicionCursor_j--;
    }
}

void cursorMoverDerecha(Cursor_t* cursor, int tam_j) {
    if (cursor->posicionCursor_j < (tam_j - 1)) {
        cursor->posicionCursor_j++;
    }
}

void cursorMoverArriba(Cursor_t* juego) {
    if (juego->posicionCursor_i > 0) {
        juego->posicionCursor_i--;
    }
}

void cursorMoverAbajo(Cursor_t* cursor, int tam_i) {
    if (cursor->posicionCursor_i < (tam_i - 1)) {
        cursor->posicionCursor_i++;
    }
}

void cursorDestruir(Cursor_t* cursor) {
  //nada
}
