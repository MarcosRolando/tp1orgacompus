#include <sys/regdef.h>
#include "CharsCelulas.h"

#define SS 56

#define O_A0 SS
#define O_A1 (O_A0 + 4)
#define O_A2 (O_A1 + 4)
#define O_A3 (O_A2 + 4)
#define O_A4 (O_A3 + 4) /*parametro N, lo recibo por stack*/

/*definir los OFFSETS*/
#define O_RA 48
#define O_FP 44
#define O_GP 40
#define O_S1 36
#define O_S0 32

#define O_LTA_VE 28
#define O_LTA_CVI 24
#define O_LTA_CVJ 20
#define O_LTA_PV 16

#define O_ABA_A3 12
#define O_ABA_A2 8
#define O_ABA_A1 4
#define O_ABA_A0 0

.text
.align 2

.globl vecinos
.ent vecinos

vecinos:
  .frame fp, SS, ra /*Reservo el stack frame*/ /*SS es el stack size*/ /*24 bytes del SRA, 16 bytes del LTA, 16 bytes del ABA*/
  .set noreorder
  .cpload t9
  .set reorder

  /*Creo el stack frame*/ /*Saved Register Area (SRA)*/
  subu sp, sp, SS /*Mi pila ahora apunta al tope de mi stack*/
  .cprestore O_GP /*Equivalente a sw gp, O_GP(sp)*/
  sw ra, O_RA(sp)
  sw fp, O_FP(sp)
  sw s1, O_S1(sp)
  sw s0, O_S0(sp)
  move fp, sp /*El frame pointer ahora apunta al stack pointer y se quedara en esa posicion durante la funcion*/

  /*Guardo las variables que recibo en vecinos, excepto el 5to argumento N ya que ese lo guarda el caller*/
  sw a0, O_A0(fp) /* unsigned char *a */
  sw a1, O_A1(fp) /* unsigned int i*/
  sw a2, O_A2(fp) /*unsigned int j*/
  sw a3, O_A3(fp) /* unsigned int M*/

  /*Local and Temporary Area (LTA)*/ /*Aca pongo el codigo para guardar las variables locales*/
  li s0, 0 /*unsigned int vecinos_encendidos = 0*/
  sw s0, O_LTA_VE(fp) /*Guardo la variable*/
  li s0, 0 /*unsigned int coordenadaVecino i = 0*/
  sw s0, O_LTA_CVI(fp) /*Guardo la variable*/
  li s0, 0 /*unsigned int coordenadaVecino j = 0*/
  sw s0, O_LTA_CVJ(fp) /*Guardo la variable*/
  li s0, 0 /*int posVecino = 0*/
  sw s0, O_LTA_PV(fp) /*Guardo la varible*/

  /*algoritmo del doble for*/
forK: li s0, -2 /*for int k = -1*/ /*k es s0*/ /*en la siguiente instruccion lo paso a -1*/
forL: addi s0, 1 /*for int k = -1, lo hice asi para no meter ifs en la primera iteracion*/
      beq s0, 2, endloop /*k < 2*/
      li s1, -1 /*for int l = -1*/ /*l es s1*/


repeat: or t2, s1, s0 /*si s1 y s0 son 0 entonces t2 sera 0*/ /*algoritmo del if*/
        beqz t2, next /*if s1 = 0 y s0 = 0 no entro al if*/


        lw t3, O_A1(fp) /*cargo i en t3*/
        lw t4, O_A2(fp) /*cargo j en t4*/
        lw a3, O_A4(fp) /*cuarto argumento, N*/
        lw a2, O_A3(fp) /*tercer argumento, M*/
        add a0, t3, s0 /*i + k, primer argumento*/
        add a1, s1, t4 /*j + l, segundo argumento*/
	      jal obtenerCoordenadaToroidal /*Retorna en v0 la fila, en v1 la columna*/
	      sw v0, O_LTA_CVI(fp) /*guardo coordenada.i*/
	      sw v1, O_LTA_CVJ(fp) /*guardo coordenada.j*/

      	lw a0, O_A4(fp) /*cuarto argumento, N*/
        move a1, v0 /*cargo coordenadaVecino.i en a1*/
        move a2, v1 /*cargo coordenadaVecino.j en a2*/
        jal obtenerIndiceMatriz /*Retorna en v0 posVecino*/
      	sw v0, O_LTA_PV(fp) /*guardo la posicion del vecino*/

        lw t0, O_A0(fp) /*cargo el puntero a */
      	addu t0, t0, v0 /* a = a + posVecino, consigo el puntero a la celda*/
      	lb t0, 0(t0) /*cargo en s0 *a, es decir, el valor de la celda */
        beq t0, APAGADO, next /*si esta apagada la celda no entro al if*/
      	lw t0, O_LTA_VE(fp) /*cargo en t0 la cantidad de vecinos encendidos*/
      	addiu t0, 1 /*vecinos_encendidos++*/
      	sw t0, O_LTA_VE(fp) /*guardo la variable*/


next: addi s1, s1, 1 /*l++*/
      beq s1, 2, forL /*l < 2, brancheo*/
      b repeat /*brancheo a repeat para iterar en el for l*/


endloop:

  /*return vecinos_encendidos*/
  lw v0, O_LTA_VE(fp) /*Guardo en v0 el valor a retornar*/

  /*Destruyo el stack que cree*/
  lw s0, O_S0(sp)
  lw s1, O_S1(sp)
  lw gp, O_GP(sp) /*Cargo el gp anterior*/
  lw fp, O_FP(sp) /*Cargo el fp anterior*/
  lw ra, O_RA(sp) /*Cargo mi return address asi puedo volver al caller*/

  addiu sp, sp, SS

  jr ra /*Retornar*/
  .end vecinos


  #undef O_A0
  #undef O_A1
  #undef O_A2
  #undef O_A3
  #undef O_GP
  #undef O_FP
  #undef SS

  #define INDICE_INICIAL 0


  #define O_A3 (O_A2 + 4)
  #define O_A2 (O_A1 + 4)
  #define O_A1 (O_A0 + 4)
  #define O_A0 SS

  #define O_GP 0
  #define O_FP (O_GP + 4)

  #define SS (O_FP + 4)

  .ent obtenerCoordenadaToroidal

  obtenerCoordenadaToroidal:
    .frame fp, SS, ra
    .set noreorder
    .cpload t9
    .set reorder

    /*Se arma el SRA*//*No se guarda el ra porque no se llama a otra funcion*/
    subu sp, sp, SS
    .cprestore O_GP /*Equivalente a sw gp, O_GP(sp)*/
    sw fp, O_FP(sp) /*El frame pointer ahora apunta al stack pointer y se quedara en esa posicion durante la funcion*/

    /*Se cargan los argumentos recibidos en el ABA del stack del callee*/
    sw a0, O_A0(sp) /*a0 = posicion_i*/
    sw a1, O_A1(sp) /*a1 = posicion_j*/
    sw a2, O_A2(sp) /*a2 = tamanio_i*/
    sw a3, O_A3(sp) /*a3 = tamanio_j*/

    /*Modificacion de la posicion i*/
    bge a0, a2, i_mayor /*if (posicion_i >= tamanio_i)*/
    bltz a0, i_menor/*if (posicion_i < 0)*/
    move v0, a0 /*Si posicion_i es valida se devuelve posicion_i*/
    b modificacion_j /*Se pasa a operar con j*/

  /*Si posicion_i es mayor o igual a tamanio_i, la posicion adecuada es 0*/
  i_mayor: li v0, INDICE_INICIAL /*i es mayor a tamanio_i entonces se devuelve 0*/
           b modificacion_j /*Se pasa a operar con j*/

  /*Si posicion_i es menor a 0, la posicion adecuada es tamanio_i-1*/
  i_menor: move v0, a2 /*Se guarda tamanio_i en v0*/
           addi v0, -1 /*Se resta 1 al tamanio para obtener la posicion adecuada*/
           b modificacion_j /*Se pasa a operar con j*/

  /*Modificacion de la posicion j*/
  modificacion_j: bge a1, a3, j_mayor /*if (posicion_j >= tamanio_j)*/
                  bltz a1, j_menor /*if (posicion_j < 0)*/
                  move v1, a1 /*Si posicion_j es valida se devuelve posicion_j*/
                  b return /*Se pasa al stack unwinding*/

  /*Si posicion_j es mayor o igual a tamanio_j, la posicion adecuada es 0*/
  j_mayor: li v1, INDICE_INICIAL
           b return /*Se pasa al stack unwinding*/

  /*Si posicion_j es menor a 0, la posicion adecuada es tamanio_j-1*/
  j_menor: move v1, a3 /*Se guarda tamanio_j en v1*/
           addi v1, -1 /*Se resta 1 al tamanio para obtener la posicion adecuada*/


  /*Destruyo el stack que cree*/
  return: lw gp, O_GP(sp) /*Cargo el gp anterior*/
          lw fp, O_FP(sp) /*Cargo el fp anterior*/

          addu sp, sp, SS

          jr ra /*Retornar*/
          .end obtenerCoordenadaToroidal
