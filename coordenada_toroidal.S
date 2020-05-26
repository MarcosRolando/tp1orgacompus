#include <sys/regdef.h>

#define INDICE_INICIAL 0


#define O_A3 (O_A2 + 4)
#define O_A2 (O_A1 + 4)
#define O_A1 (O_A0 + 4)
#define O_A0 SS

#define O_GP 0
#define O_FP (O_GP + 4)

#define SS (O_FP + 4)

.text
.align 2

.globl obtenerCoordenadaToroidal
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

/*VER SI SE SACA EL BRANCH AL FINAL DE i_menor, TENGO ENTENDIDO QUE LOS BRANCHS SON CAROS*/

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
