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

#define TRAPHANDLER_IF(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps

#define _TRAPHANDLER(name) TRAPHANDLER(name, T_##name)
#define _TRAPHANDLER_NOEC(name) TRAPHANDLER_NOEC(name, T_##name)
#define _IRQHANDLER(irq_num) TRAPHANDLER_IF(IRQ_##irq_num##_STUB, IRQ_OFFSET + irq_num)

.text

/*
 * Lab 3: Your code here for generating entry points for the different traps.
 */

_TRAPHANDLER_NOEC(DIVIDE)
_TRAPHANDLER_NOEC(DEBUG)
_TRAPHANDLER_NOEC(NMI)
_TRAPHANDLER_NOEC(BRKPT)
_TRAPHANDLER_NOEC(OFLOW)
_TRAPHANDLER_NOEC(BOUND)
_TRAPHANDLER_NOEC(ILLOP)
_TRAPHANDLER_NOEC(DEVICE)
_TRAPHANDLER(DBLFLT)
_TRAPHANDLER(TSS)
_TRAPHANDLER(SEGNP)
_TRAPHANDLER(STACK)
_TRAPHANDLER(GPFLT)
_TRAPHANDLER(PGFLT)
_TRAPHANDLER_NOEC(FPERR)
_TRAPHANDLER_NOEC(ALIGN)
_TRAPHANDLER_NOEC(MCHK)
_TRAPHANDLER_NOEC(SIMDERR)
_TRAPHANDLER_NOEC(SYSCALL)
_TRAPHANDLER_NOEC(DEFAULT)
_IRQHANDLER(0)
_IRQHANDLER(1)
_IRQHANDLER(2)
_IRQHANDLER(3)
_IRQHANDLER(4)
_IRQHANDLER(5)
_IRQHANDLER(6)
_IRQHANDLER(7)
_IRQHANDLER(8)
_IRQHANDLER(9)
_IRQHANDLER(10)
_IRQHANDLER(11)
_IRQHANDLER(12)
_IRQHANDLER(13)
_IRQHANDLER(14)
_IRQHANDLER(15)

/*
 * Lab 3: Your code here for _alltraps
 */
    .globl _alltraps;
    .type _alltraps, @function;
    .align 2;
_alltraps:
    pushl %ds;  # pushl to pad %ds to 32bit
    pushl %es;  # pushl to pad %es to 32bit
    pushal;     # lay a copy of PushRegs on stack
    push %esp;  # %esp is Trapframe* tf argument to trap()
    call trap   # should *not* return

_alltrapsspin:
    jmp _alltrapsspin