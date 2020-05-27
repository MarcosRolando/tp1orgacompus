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
