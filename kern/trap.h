/* See COPYRIGHT for copyright information. */

#ifndef JOS_KERN_TRAP_H
#define JOS_KERN_TRAP_H
#ifndef JOS_KERNEL
# error "This is a JOS kernel header; user programs should not #include it"
#endif

#include <inc/trap.h>
#include <inc/mmu.h>

extern void DIVIDE();
extern void DEBUG();
extern void NMI();
extern void BRKPT();
extern void OFLOW();
extern void BOUND();
extern void ILLOP();
extern void DEVICE();
extern void DBLFLT();
extern void TSS();
extern void SEGNP();
extern void STACK();
extern void GPFLT();
extern void PGFLT();
extern void FPERR();
extern void ALIGN();
extern void MCHK();
extern void SIMDERR();
extern void SYSCALL();
extern void DEFAULT();

#define DECLARE_IRQ_STUB(irq_num) extern void IRQ_##irq_num##_STUB()
DECLARE_IRQ_STUB(0);
DECLARE_IRQ_STUB(1);
DECLARE_IRQ_STUB(2);
DECLARE_IRQ_STUB(3);
DECLARE_IRQ_STUB(4);
DECLARE_IRQ_STUB(5);
DECLARE_IRQ_STUB(6);
DECLARE_IRQ_STUB(7);
DECLARE_IRQ_STUB(8);
DECLARE_IRQ_STUB(9);
DECLARE_IRQ_STUB(10);
DECLARE_IRQ_STUB(11);
DECLARE_IRQ_STUB(12);
DECLARE_IRQ_STUB(13);
DECLARE_IRQ_STUB(14);
DECLARE_IRQ_STUB(15);


/* The kernel's interrupt descriptor table */
extern struct Gatedesc idt[];
extern struct Pseudodesc idt_pd;

void trap_init(void);
void trap_init_percpu(void);
void print_regs(struct PushRegs *regs);
void print_trapframe(struct Trapframe *tf);
void page_fault_handler(struct Trapframe *);
void backtrace(struct Trapframe *);

#endif /* JOS_KERN_TRAP_H */
