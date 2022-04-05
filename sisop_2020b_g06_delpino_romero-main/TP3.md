Parte 1 - Tarea: env_return:
============================

1- Al terminar un proceso su función umain() ¿dónde retoma la ejecución el kernel? Describir la secuencia de llamadas desde que termina umain() hasta que el kernel dispone del proceso.

R: Se llama a la función exit() en el archivo lib/libmain.c, luego la función exit()
(definida en inc/exit.c) llama la syscall sys_env_destroy(0) tomando el control el Kernel
y llamando a la función env_destroy(), esta obviamente es solamente accesible para el
kernel.

2- ¿en qué cambia la función env_destroy() en este TP, respecto al TP anterior?

R:Si el proceso actual se está ejecutando en otra CPU, se lo marca como zombie que
será liberado luego, cuando sea "trapeado" por el kernel.

Parte 1 - Tarea: sys_yield:
===========================

Se lanzan los 3 procesos pero no se ejecutan uno tras otro debido a la llamada
sys_yield, entre cada iteración. Se alterna entre los 3 procesos, es decir los 3
se ejecutan "en paralelo", esto se logra alternando entre procesos.
Si comentáramos la linea sys_yield en el código de usuario, los 3 procesos de
ejecutarían completamente uno tras otro.

------Salida-----
~~~
SMP: CPU 0 found 1 CPU(s)
enabled interrupts: 1 2
[00000000] new env 00001000
[00000000] new env 00001001
[00000000] new env 00001002
Hello, I am environment 00001000.
Hello, I am environment 00001001.
Hello, I am environment 00001002.
Back in environment 00001000, iteration 0.
Back in environment 00001001, iteration 0.
Back in environment 00001002, iteration 0.
Back in environment 00001000, iteration 1.
Back in environment 00001001, iteration 1.
Back in environment 00001002, iteration 1.
Back in environment 00001000, iteration 2.
Back in environment 00001001, iteration 2.
Back in environment 00001002, iteration 2.
Back in environment 00001000, iteration 3.
Back in environment 00001001, iteration 3.
Back in environment 00001002, iteration 3.
Back in environment 00001000, iteration 4.
All done in environment 00001000.
[00001000] exiting gracefully
[00001000] free env 00001000
Back in environment 00001001, iteration 4.
All done in environment 00001001.
[00001001] exiting gracefully
[00001001] free env 00001001
Back in environment 00001002, iteration 4.
All done in environment 00001002.
[00001002] exiting gracefully
[00001002] free env 00001002
No runnable environments in the system!
~~~

Parte 2 - Tarea: envid2env
==========================

La función envid2env carga en el parámetro "env_store" el proceso actual cuando
se le pasa el valor 0 como parámetro para el "envid". Entonces "env_destroy"
libera el proceso actual.

Parte 2 - Tarea: dumbfork
==========================

1- Si, antes de llamar a dumbfork(), el proceso se reserva a sí mismo una página con
sys_page_alloc() ¿se propagará una copia al proceso hijo? ¿Por qué?

R: Sí porque vive en el espacio de direcciones del padre.

2- ¿Se preserva el estado de solo-lectura en las páginas copiadas? Mostrar, con código
en espacio de usuario, cómo saber si una dirección de memoria es modificable por el
proceso, o no. (Ayuda: usar las variables globales uvpd y/o uvpt.)

R: No se preserva, se les asigna permisos de escritura a todos.

~~~
if (uvpd[PDX(addr)] & PTE_P)
    if ((pte = uvpt[PGNUM(addr)]) &  PTE_P)
        if(!(pte & PTE_W))
            //Solo lectura
~~~

3- Describir el funcionamiento de la función duppage():

R: Consta de 3 pasos en general. Primero se reserva una página para el proceso
hijo. Luego, se mapean(copian) en la nueva pagina alocada. Dado que no puedo copiar
a un espacio de direcciones diferente(memmove o memcpy), se mapea temporalmente en
UTEMP y luego con memmove copiamos los datos.

4- Supongamos que se añade a duppage() un argumento booleano que indica si la página
debe quedar como solo-lectura en el proceso hijo. Indicar qué llamada adicional se
debería hacer si el booleano es true. Describir un algoritmo alternativo que no aumente
el número de llamadas al sistema, que debe quedar en 3 (1 × alloc, 1 × map, 1 × unmap).

~~~
void
duppage(envid_t dstenv, void *addr, int readonly)
{
	int r;
    int default_perms = PTE_P | PTE_U;
    if(!readonly)
        default_perms |= PTE_W;  // Si no es solo lectura.
	// This is NOT what you should do in your fork.
	if ((r = sys_page_alloc(dstenv, addr, default_perms)) < 0)
		panic("sys_page_alloc: %e", r);
	if ((r = sys_page_map(dstenv, addr, 0, UTEMP, default_perms)) < 0)
		panic("sys_page_map: %e", r);
	memmove(UTEMP, addr, PGSIZE);
	if ((r = sys_page_unmap(0, UTEMP)) < 0)
		panic("sys_page_unmap: %e", r);
}
~~~

5- ¿Por qué se usa ROUNDDOWN(&addr) para copiar el stack? ¿Qué es addr y por qué, si el
stack crece hacia abajo, se usa ROUNDDOWN y no ROUNDUP?

R: Es una varible que vive en el stack del padre. Se redondea hacia abajo porque
debo copiar todo el "User Normal Stack", es decir desde la dirección virtual
0xeebfd000 hasta USTACKTOP.

Parte 3 - Tarea: multicore_init:
================================

1- ¿Qué código copia, y a dónde, la siguiente línea de la función boot_aps()?

memmove(code, mpentry_start, mpentry_end - mpentry_start);

R: Copia desde mpentry_start, en la dirección 0x7000, el código de inicio
para cada AP.

2- ¿Para qué se usa la variable global mpentry_kstack? ¿Qué ocurriría si el espacio
para este stack se reservara en el archivo kern/mpentry.S, de manera similar a bootstack
en el archivo kern/entry.S?

R: Lo usamos para configurar el kernel stack de las APs, para separar el stack de
las APs. Se usa cuando se ejecuta el código en mpentry.S. En caso de usar de manera
similar a bootstack, estaríamos usando el mismo kernel stack lo que podría causar
conflictos.

3- Cuando QEMU corre con múltiples CPUs, éstas se muestran en GDB como hilos de ejecución
separados. Mostrar una sesión de GDB en la que se muestre cómo va cambiando el valor de
la variable global mpentry_kstack:

Nota: Cuando hacemos "p cpunum()", gdb nos tira un error similar a otro TP. Como
respuesta a esa salida, debería imprimir el número de la CPU, es decir debería
el número del thread menos uno.

~~~
(base) delpinor@delpinor-hp:~/tps$ make gdb
gdb -q -s obj/kern/kernel -ex 'target remote 127.0.0.1:26000' -n -x .gdbinit
Leyendo símbolos desde obj/kern/kernel...hecho.
Remote debugging using 127.0.0.1:26000
aviso: No executable has been specified and target does not support
determining executable automatically.  Try using the "file" command.
0x0000fff0 in ?? ()
(gdb) watch mpentry_kstack
Hardware watchpoint 1: mpentry_kstack
(gdb) continue
Continuando.
Se asume que la arquitectura objetivo es i386
=> 0xf0100186 <boot_aps+127>:	mov    %esi,%ecx

Thread 1 hit Hardware watchpoint 1: mpentry_kstack

Old value = (void *) 0x0
New value = (void *) 0xf0254000 <percpu_kstacks+65536>
boot_aps () at kern/init.c:109
109			lapic_startap(c->cpu_id, PADDR(code));
(gdb) bt
#0  boot_aps () at kern/init.c:109
#1  0xf010020f in i386_init () at kern/init.c:55
#2  0xf0100047 in relocated () at kern/entry.S:88
(gdb) info threads
  Id   Target Id         Frame
* 1    Thread 1 (CPU#0 [running]) boot_aps () at kern/init.c:109
  2    Thread 2 (CPU#1 [halted ]) 0x000fd412 in ?? ()
  3    Thread 3 (CPU#2 [halted ]) 0x000fd412 in ?? ()
  4    Thread 4 (CPU#3 [halted ]) 0x000fd412 in ?? ()
(gdb) continue
Continuando.
=> 0xf0100186 <boot_aps+127>:	mov    %esi,%ecx

Thread 1 hit Hardware watchpoint 1: mpentry_kstack

Old value = (void *) 0xf0254000 <percpu_kstacks+65536>
New value = (void *) 0xf025c000 <percpu_kstacks+98304>
boot_aps () at kern/init.c:109
109			lapic_startap(c->cpu_id, PADDR(code));
(gdb) info threads
  Id   Target Id         Frame
* 1    Thread 1 (CPU#0 [running]) boot_aps () at kern/init.c:109
  2    Thread 2 (CPU#1 [running]) 0xf010029d in mp_main () at kern/init.c:127
  3    Thread 3 (CPU#2 [halted ]) 0x000fd412 in ?? ()
  4    Thread 4 (CPU#3 [halted ]) 0x000fd412 in ?? ()
(gdb) thread 2
[Switching to thread 2 (Thread 2)]
#0  0xf010029d in mp_main () at kern/init.c:127
127		xchg(&thiscpu->cpu_status, CPU_STARTED);  // tell boot_aps() we're up
(gdb) bt
#0  0xf010029d in mp_main () at kern/init.c:127
#1  0x00007060 in ?? ()
(gdb) p cpunum()
Could not fetch register "orig_eax"; remote failure reply 'E14'
(gdb) thread 1
[Switching to thread 1 (Thread 1)]
#0  boot_aps () at kern/init.c:109
109			lapic_startap(c->cpu_id, PADDR(code));
(gdb) p cpunum()
Could not fetch register "orig_eax"; remote failure reply 'E14'
(gdb) continue
Continuando.
=> 0xf0100186 <boot_aps+127>:	mov    %esi,%ecx

Thread 1 hit Hardware watchpoint 1: mpentry_kstack

Old value = (void *) 0xf025c000 <percpu_kstacks+98304>
New value = (void *) 0xf0264000 <percpu_kstacks+131072>
boot_aps () at kern/init.c:109
109			lapic_startap(c->cpu_id, PADDR(code));
~~~

4- ¿Qué valor tendrá el registro %eip cuando se ejecute esa línea?

R: La función lapic_startap empieza a ejecutar desde  0x7000 por lo el valor del
registro será 0x7000 + cantidad de instrucciones*4(como máximo), en este caso
contamos 12. Entonces el valor redondeado 0x703.

¿Se detiene en algún momento la ejecución si se pone un breakpoint en mpentry_start?

R: No se detiene porque cuando se establece el breakpoint mpentry_start es una dirección
alta y cuando el EIP pasa por la instrucción es una dirección baja.

Parte 4 - Tarea: ipc_recv:
================================
1- Un proceso podría intentar enviar el valor númerico `-E_INVAL` vía `ipc_send()`. ¿Cómo es posible distinguir si es un error, o no?

~~~
envid_t src = -1;
int r = ipc_recv(&src, 0, NULL);

if (r < 0)
  if (/* ??? */)
    puts("Hubo error.");
  else
    puts("Valor negativo correcto.")
~~~

R: Si observamos la definición de la función `int32_t ipc_recv(envid_t *from_env_store, void *pg, int *perm_store);` podemos ver que `envid_t src` corresponde al primer argumento (`envid_t *from_env_store`).

Dentro de dicha función, si ocurre algún error, y `from_env_store` no es nulo, se setea en `0` el valor de `from_env_store`, por lo que habría que verificar que si `src` es `0`, entonces hubo un error.

~~~
envid_t src = -1;
int r = ipc_recv(&src, 0, NULL);

if (r < 0)
  if (src == 0)
    puts("Hubo error.");
  else
    puts("Valor negativo correcto.")
~~~

Parte 4 - Tarea: sys_ipc_try_send:
================================
1- Se pide ahora explicar cómo se podría implementar una función `sys_ipc_send()` que sea bloqueante.

R: Viendo la implementación de `sys_ipc_recv`, que es bloqueante, llegamos a la conclusión de que podremos agregarle a `struct Env` un nuevo campo que funcione como flag cuyo nombre sería `bool env_ipc_sending`.

Ahora bien, si el proceso que va a recibir no está esperando, el proceso que envía se quedará esperando, hasta que se entre en `sys_ipc_recv`.
Cuando lo haga, se verificará que efectivamente se está tratando de enviar mediante el flag `env_ipc_sending`, por lo que se recibirá correctamente y el proceso que lo envió se despertaría.

Este caso funcionaría para múltiples llamadas y no hay un orden establecido en el cual se depertarán los procesos.

Parte 6 - Tarea: fork:
================================
1- ¿Puede hacerse con la función `set_pgfault_handler`()? De no poderse, ¿cómo llega al hijo el valor correcto de la variable global `_pgfault_handler`?

R: Debemos usar `sys_env_set_pgfault_upcall`. Cuando se llama a set_pgfault_handler se hace esta asignación:

~~~
//Punto de entrada definido lib/pfentry.S.
sys_env_set_pgfault_upcall(thisenv->env_id, _pgfault_upcall)
~~~

Luego dentro del fork setamos al proceso hijo, el valor de Env->env_pgfault_upcall del proceso padre.
