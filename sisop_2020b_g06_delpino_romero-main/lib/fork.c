// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW 0x800

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;
	uint32_t err = utf->utf_err;
	int r;

	// Check that the faulting access was (1) a write, and (2) to a
	// copy-on-write page.  If not, panic.
	// Hint:
	//   Use the read-only page table mappings at uvpt
	//   (see <inc/memlayout.h>).

	// LAB 4: Your code here.

	// el error ocurrió en una dirección no mapeada -> panic
	if (!(err & FEC_PR))
		panic("pgfault: dirección no mapeada");

	// el error ocurrió por una lectura y no una escritura -> paic
	if (!(err & FEC_WR))
		panic("pgfault: error por una lectura");

	// Se recupera la PTE asociada
	pte_t pte = uvpt[PGNUM(addr)];

	// la página afectada no está marcada como copy-on-write -> paic
	if (!(pte & PTE_COW))
		panic("pgfault: no está marcada como copy-on-write");

	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.

	// LAB 4: Your code here.

	// se reserva una nueva página en una dirección temporal
	r = sys_page_alloc(0, PFTEMP, PTE_P | PTE_U | PTE_W);
	if (r < 0)
		panic("pgfault: error al reservar pagina en dirección "
		      "temporal");

	// se escribe en ella los contenidos apropiados, pero primero se alinea
	addr = (void *) ROUNDDOWN(addr, PGSIZE);
	memcpy(PFTEMP, addr, PGSIZE);

	// se mapea en la dirección destino
	r = sys_page_map(0, PFTEMP, 0, addr, PTE_P | PTE_U | PTE_W);
	if (r < 0)
		panic("pgfault: error al mapear en destino");

	// se elimina el mapeo usado en la dirección temporal
	r = sys_page_unmap(0, PFTEMP);
	if (r < 0)
		panic("pgfault: error al eliminar mapeo temporal");

	// panic("pgfault not implemented");
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
static int
duppage(envid_t envid, unsigned pn)
{
	int r;

	// LAB 4: Your code here.
	// panic("duppage not implemented");

	void *address = (void *) (pn * PGSIZE);

	// Se recupera la PTE asociada
	pte_t pte = uvpt[pn];

	// Los permisos en el hijo son los de la página original menos PTE_W
	// Copiamos los permisos, y sacamos el PTE_W
	int perm = pte & PTE_SYSCALL & ~PTE_W;

	// Si es una pagina que debe ser compartida, son los mismos que el padre
	if (pte & PTE_SHARE) {
		perm = (perm | PTE_W);
	} else if (pte & PTE_W) {
		// Si y solo si se sacó PTE_W, se añade el bit especial PTE_COW
		perm = perm | PTE_COW;
	}

	// Se mapea en el hijo la página física correspondiente en
	// la misma página virtual
	r = sys_page_map(0, address, envid, address, perm);

	if (r < 0)
		panic("duppage: error al mapear en el hijo");

	// Si los permisos resultantes en el hijo incluyen PTE_COW,
	// se remapea la página en el padre con estos permisos
	if (perm & PTE_COW) {
		r = sys_page_map(0, address, 0, address, perm);

		if (r < 0)
			panic("duppage: error al remapear en el padre");
	}
	return 0;
}
// primer versión de duppage
static void
dup_or_share(envid_t dstenv, void *va, int perm)
{
	// Si es solo lectura solo mapeamos. No creamos una copia.
	if (!(perm & PTE_W)) {
		if (sys_page_map(0, va, dstenv, va, perm) < 0)  // Solo lectura
			panic("dup_or_share: sys_page_map failed");
	} else {
		if (sys_page_alloc(dstenv, va, PTE_P | PTE_U | PTE_W) < 0)
			panic("dup_or_share: sys_page_alloc failed");
		// Temporalmente mapeamos la "va" del padre en UTEMP.
		if (sys_page_map(dstenv, va, 0, UTEMP, PTE_P | PTE_U | PTE_W) < 0)
			panic("dup_or_share: sys_page_map failed");
		// Copiamos desde "va" hasta la "va" del proceso hijo.
		memmove(UTEMP, va, PGSIZE);
		// Se desmampea ya que su uso fue temporal.
		if (sys_page_unmap(0, UTEMP) < 0)
			panic("dup_or_share: sys_page_unmap failed");
	}
}
// Primer versión de fork()
envid_t
fork_v0(void)
{
	envid_t envid;
	pte_t pte;
	if ((envid = sys_exofork()) < 0)
		panic("sys_exofork: %e", envid);
	// Proceso hijo
	if (envid == 0) {
		thisenv = &envs[ENVX(sys_getenvid())];
		return 0;
	}
	for (uint32_t addr = 0; addr < UTOP; addr += PGSIZE) {
		if (uvpd[PDX(addr)] & PTE_P)  // Entrada en la Page Directory
			if ((pte = uvpt[PGNUM(addr)]) &
			    PTE_P)  // Entrada en la Page Table
				dup_or_share(envid,
				             (void *) addr,
				             pte & PTE_SYSCALL);
	}
	if (sys_env_set_status(envid, ENV_RUNNABLE) < 0)
		panic("fork_v0: sys_env_set_status failed");
	return envid;
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
	// return fork_v0();

	// Se instalada, en el padre, la función pgfault como manejador de page
	// faults. Esto también reservará memoria para su pila de excepciones.
	set_pgfault_handler(&pgfault);

	envid_t envid = sys_exofork();

	// Si sys_exofork retorna error se entra en panic
	if (envid < 0)
		panic("fork: error en sys_exofork");

	// Si envid es 0, entoces es el hijo. Se corrige thisenv y retorna 0
	if (envid == 0) {
		thisenv = &envs[ENVX(sys_getenvid())];
		return 0;
	}

	// Si pasa el if anterior, es el padre
	int result = 0;

	size_t pdx_ant = 0;
	bool es_primer_pdx = true;

	// Se itera sobre el espacio de memoria del padre (desde 0 hasta UTOP)
	for (uint32_t addr = 0; addr < UTOP; addr += PGSIZE) {
		size_t pdx = PDX(addr);

		if ((pdx == pdx_ant) && (!es_primer_pdx))
			continue;

		pdx_ant = pdx;
		es_primer_pdx = false;

		// Se obtiene la PDE
		pde_t pde = uvpd[pdx];

		// Si no esta presente seguimos con la proxima
		if (!(pde & PTE_P))
			continue;

		// Se recorren todas las NPTENTRIES
		for (size_t ptx = 0; ptx < NPTENTRIES; ptx++) {
			// Construimos la direccion virtual
			// Usamos 0 para el offset
			uintptr_t pgaddr = (uintptr_t) PGADDR(pdx, ptx, 0);

			// Se obtiene la PTE
			pte_t pte = uvpt[PGNUM(pgaddr)];

			// No se debe mapear la región correspondiente a la
			// pila de excepciones (one page of memory with its top at UXSTACKTOP)
			// Se llama a page_alloc y continua con la siguiente NPTENTRIES
			if (pgaddr == (UXSTACKTOP - PGSIZE)) {
				result = sys_page_alloc(envid,
				                        (void *) pgaddr,
				                        PTE_P | PTE_U | PTE_W);
				if (result)
					panic("fork: error en sys_page_alloc");

				continue;
			}

			// Si no esta presente seguimos con la proxima NPTENTRIES
			if (!(pte & PTE_P))
				continue;

			// Si está todo ok, se llama a duppage
			duppage(envid, PGNUM(pgaddr));
		}
	}

	// pgfault como el controlador del hijo
	// En caso de error, se entra en panic
	result = sys_env_set_pgfault_upcall(envid, thisenv->env_pgfault_upcall);

	if (result)
		panic("fork: error en sys_env_set_pgfault_upcall");

	// Se marca al proceso hijo como ENV_RUNNABLE
	// En caso de error, se entra en panic
	result = sys_env_set_status(envid, ENV_RUNNABLE);

	if (result)
		panic("fork: error en sys_env_set_status");

	return envid;
}
// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}
