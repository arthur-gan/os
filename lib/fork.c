// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>
#include <inc/memlayout.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW		0x800

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;
	uint32_t err = utf->utf_err;

	// Check that the faulting access was (1) a write, and (2) to a
	// copy-on-write page.  If not, panic.
	// Hint:
	//   Use the read-only page table mappings at uvpt
	//   (see <inc/memlayout.h>).

	// LAB 4: Your code here.

	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.

	// LAB 4: Your code here.

	// panic("pgfault not implemented");

    if (!(err & FEC_WR))
        panic("Page fault not caused by write");

	addr = ROUNDDOWN(addr, PGSIZE);

	if (!(PTE_COW & uvpt[(uintptr_t) addr >> PGSHIFT]))
	    panic("Faulting page is not copy-on-write");

	if (sys_page_alloc(0, PFTEMP, PTE_P | PTE_U | PTE_W) < 0)
	    panic("Copy-on-write pgflt handler failed to alloc new page");

	memmove(PFTEMP, addr, PGSIZE);
	sys_page_map(0, PFTEMP, 0, addr, PTE_P | PTE_U | PTE_W);
	sys_page_unmap(0, PFTEMP);
}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
int
duppage(envid_t envid, unsigned pn)
{
	// LAB 4: Your code here.
	// panic("duppage not implemented");

	int result = NO_ERROR;
    pte_t pte_perm = uvpt[pn] & PTE_PERM;
    void* va = (void*) (pn * PGSIZE);

    if (pte_perm & PTE_U && pte_perm & PTE_P) {
        if (pte_perm & PTE_W || pte_perm & PTE_COW) {
            // Clear PTE_W and set PTE_COW
            // IMPORTANT: Leave other bits the same.
            pte_perm &= (~(int) PTE_W);
            pte_perm |= PTE_COW;

            result = sys_page_map(0, va, envid, va, pte_perm);
            if (NO_ERROR == result)
                sys_page_map(0, va, 0, va, pte_perm);
        } else {
            sys_page_map(0, va, envid, va, pte_perm);
        }
    }

	return result;
}

/*
 * Copy the content of page mapped at addr(va) in curenv
 * into a new physical page and map the new physical page
 * at dstenv's addr(va)
 *
 * Returns:
 * NO_ERROR success
 * -E_BADENV if curenv has no permission to touch dstenv
 * -E_NO_MEM if there is insufficient free physical mem
 *  in the system to complete the request
 * -E_INVAL if addr is not present in curenv
 */
int
copy_page(envid_t dstenv, void* addr) {
    int r;
    r = sys_page_alloc(dstenv, addr, PTE_P|PTE_U|PTE_W);
    if (r < 0)
        panic("page_alloc failed in copy_page");

    r = sys_page_map(dstenv, addr, 0, UTEMP, PTE_P|PTE_U|PTE_W);
    if (r < 0)
        panic("sys_page_map failed in copy_page");

    memmove(UTEMP, addr, PGSIZE);
    sys_page_unmap(0, UTEMP);

    return r;
}

//
// User-level fork with copy-on-write.
// Set up our page fault handler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use uvpd, uvpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
envid_t
fork(void)
{
	// LAB 4: Your code here.
	// panic("fork not implemented");

    int res = NO_ERROR;

    set_pgfault_handler(pgfault);
    if (res != NO_ERROR)
        panic("Fork parent failed to set pagefault handler");

    envid_t eid = sys_exofork();
    if (eid < 0)
        panic("sys_exofork failed");

    if (eid == 0) {
        thisenv = &envs[ENVX(sys_getenvid())];
    } else {
        // Copy all pages from UTEXT to UXSTACKTOP
        // UXSTACK page is not shared.
        // All other pages are shared.
        uint8_t * addr = (void*) UTEXT;
        for (;  (uintptr_t) addr < UTOP; addr += PGSIZE) {
            int mapped = PTE_P & uvpd[(uint32_t)addr >> PDXSHIFT];
            if (mapped)
                mapped = PTE_P & uvpt[(uint32_t)addr >> PTXSHIFT];

            if (mapped) {
                if ((uintptr_t) addr == UXSTACKTOP - PGSIZE)
                    // UXSTACK must be deep copied,
                    // not just alloc'd.
                    // Otherwise, calling fork() in page fault
                    // handler's will not work.
                    res = copy_page(eid, addr);
                else
                    res = duppage(eid, (uintptr_t) addr / PGSIZE);
                if(NO_ERROR != res)
                    panic("Fork parent failed to copy page to child");
            }
        }

        // Mark child as runnable
        sys_env_set_status(eid, ENV_RUNNABLE);
    }

    // Return eid not res.
    return eid;
}

// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}
