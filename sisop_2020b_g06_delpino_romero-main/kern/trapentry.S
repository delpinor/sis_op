/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>

#include <kern/picirq.h>


###################################################################
# exceptions/interrupts
###################################################################

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
#define TRAPHANDLER(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	name:			/* function starts here */		\
	pushl $(num);							\
	jmp _alltraps

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps

.text

/*
 * Lab 3: Your code here for generating entry points for the different traps.
 */
TRAPHANDLER_NOEC(trap_0, T_DIVIDE)	/* divide error */
TRAPHANDLER_NOEC(trap_1, T_DEBUG) 	/* debug exception */
TRAPHANDLER_NOEC(trap_2, T_NMI) 		/* non-maskable interrupt */
TRAPHANDLER_NOEC(trap_3, T_BRKPT) 	/* breakpoint */
TRAPHANDLER_NOEC(trap_4, T_OFLOW) 	/* overflow */
TRAPHANDLER_NOEC(trap_5, T_BOUND) 	/* bounds check */
TRAPHANDLER_NOEC(trap_6, T_ILLOP) 	/* illegal opcode */
TRAPHANDLER_NOEC(trap_7, T_DEVICE) 	/* device not available */

TRAPHANDLER(trap_8, T_DBLFLT) 			/* double fault */
TRAPHANDLER(trap_10, T_TSS) 				/* invalid task switch segment */
TRAPHANDLER(trap_11, T_SEGNP) 			/* segment not present */
TRAPHANDLER(trap_12, T_STACK) 			/* stack exception */
TRAPHANDLER(trap_13, T_GPFLT) 			/* general protection fault */
TRAPHANDLER(trap_14, T_PGFLT) 			/* page fault */

TRAPHANDLER_NOEC(trap_16, T_FPERR) 	/* floating point error */

TRAPHANDLER(trap_17, T_ALIGN) 			/* aligment check */

TRAPHANDLER_NOEC(trap_18, T_MCHK) 	/* machine check */
TRAPHANDLER_NOEC(trap_19, T_SIMDERR)/* SIMD floating point error */

TRAPHANDLER_NOEC(irq_0, IRQ_OFFSET + IRQ_TIMER);

TRAPHANDLER_NOEC(trap_48, T_SYSCALL)/* system call */

/* LAB 5: kbd_interrupt */
TRAPHANDLER_NOEC(irq_1, IRQ_OFFSET + IRQ_KBD)
TRAPHANDLER_NOEC(irq_4, IRQ_OFFSET + IRQ_SERIAL)

/*
 * Lab 3: Your code here for _alltraps
 */
 _alltraps:
 		/* Se pushea el DS, el ES y todos los registros */
    pushl %ds
    pushl %es
    pushal
		/* Se carga GD_KD en %ds y %es mediante %ax
		 * (registro intermedio de 16 bits)
		 */
    mov $GD_KD, %ax
    mov %ax, %ds
    mov %ax, %es
		/* Argumento para trap */
    pushl %esp
		/* Se llama a trap */
    call trap
