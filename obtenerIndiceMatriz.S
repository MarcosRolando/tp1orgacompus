#include<sys/regdef.h>

#define SS 8

#define O_FP 4
#define O_GP 0

#define O_A0 (SS)
#define O_A1 (O_A0 + 4)
#define O_A2 (O_A1 + 4)

.text
.align 2

.globl obtenerIndiceMatriz
.ent obtenerIndiceMatriz

obtenerIndiceMatriz:
        .set noreorder
        .cpload t9
        .set reorder

        subu sp, sp, SS /* seteo stack de tamaño SS */

        /* SRA */
        sw fp, O_FP(sp)
        sw gp, O_GP(sp)
        move fp, sp

        /* Pongo los argumentos en el stack del caller */
        sw a0, O_A0(fp)
        sw a1, O_A1(fp)
        sw a2, O_A2(fp)

        /* seteo valor de retorno */
        lw v0, O_A2(fp) /* v0 = j */
        mul t0, a0, a1  /* t0 = i * tam_j */
        add v0, v0, t0  /* v0 = j + i * tam_j */

        /* stack unwindig */
        lw gp, O_GP(sp)
        lw fp, O_FP(sp)
        addiu sp, sp, SS

        jr ra

        .end obtenerIndiceMatriz
