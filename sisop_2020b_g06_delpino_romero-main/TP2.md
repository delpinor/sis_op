TP2: Procesos de usuario
========================

env_alloc
---------
**Respuesta a pregunta 1:**

El valor calculado para 'generation' es 4096. Haciendo un OR Binario entre este
valor y la posición del vector env[] y mostrando en hexadecimal, valor de env_id
para la posición i es:

~~~
i = 0 ---> env_id = 0x1000
i = 1 ---> env_id = 0x1001
i = 2 ---> env_id = 0x1002
i = 3 ---> env_id = 0x1003
i = 4 ---> env_id = 0x1004
~~~

**Respuesta a pregunta 2:**

Una vez que finaliza una proceso, la estructura es reutilizable pero el env_id
es único. Estos adquieran un nuevo identificador por cada nuevo proceso.
Cuando finaliza el primer proceso, el valor de env_id es 4726, en hexadecimal 0x1276.
Con este valor hacemos los desplazamientos para obtener un nuevo valor para 'generation'
que es 8192 y finalmente haciendo un OR Binario con el valor 630, obtenemos
8892 que en hexadecimal es 0x2276. El proceso se repite tomando el cuenta el
valor anterior de env_id como punto de inicio.
Sean las i ejecuciones:

~~~
i = 0 ---> env_id = 0x2276
i = 1 ---> env_id = 0x3276
i = 2 ---> env_id = 0x4276
i = 3 ---> env_id = 0x5276
i = 4 ---> env_id = 0x6276
~~~

Se puede apreciar que el valor de 'Uniqueifier' cambia pero el valor de
'environment index' se mantiene como menciona en la documentación.

env_init_percpu
---------------
**Respuesta a pregunta 1:**

Se escriben 6 bytes(48 bits) en el registro GDTR.

**Respuesta a pregunta 2:**

Representa el rango de la GDT, siendo la dirección BASE de la GDT los bits 16 a 47, y los
bits de 0 a 15 el límite en bytes desde la BASE.

env_pop_tf
----------
**Respuesta a pregunta 1:**

+ **El tope de la pila justo antes `popal`**:

Antes de dicha instrucción el tope de la pila va a estar apuntando al primer elemento del _struct Trapframe_, que recibe como parámetro la función `env_pop_tf`.
Como el _struct Trapframe_ tiene como primer elemento un _struct PushRegs_, el tope del stack va a estar apuntando al primer elemento de _PushRegs_, es decir, a `uint32_t reg_edi`.

Esto se puede ver gracias a que la única función anterior a `popal` es `movl %0,%%esp`, que lo que hace es mover el puntero de tf (_Trapframe_) al `esp`.


+ **El tope de la pila justo antes `iret`**:

Luego de la primera instrucción explicada en el punto anterior, lo que se ejecuta es `popal`, que hace pop del stack de los 8 registros de propósito general (`EAX - ECX - EDX - EBX - OESP - EBP - ESI - EDI`).
Por lo que en este punto el tope de pila se encuentra en `tf_es`.

Después le siguen `popl %%es` y `popl %%ds`, por lo que hace pop del stack los registros `tf_es` y `tf_ds` respectivamente, con sus propios paddings.
En este punto el tope de la pila estaría apuntando a `tf_trapno`.

Por último se ejecuta `addl $0x8,%%esp`, es decir, que le suma 8 al valor de esp.
Esto sucede para saltearse los valores de `tf_trapno` y de `tf_err` ya que estos valores no interesan para el estado de un env en esta etapa.

En conclusión, antes de `iret`, el tope de la pila estará apuntando a `uintptr_t tf_eip`.

+ **El tercer elemento de la pila justo antes de `iret`**:

Como se dijo anteriormente, antes de `iret`, el tope de la pila apunta a `uintptr_t tf_eip`, por lo que el tercer elemento de la pila será `uintptr_t tf_eflags`.


**Respuesta a pregunta 2:**

La CPU (en x86) guarda el nivel de privilegio actual en el registro CS, utilizando sus dos bits menos significativos.

Entonces, un cambio de ring (nivel de privilegio) se determina comparando el `CPL` (_Current Privilege Level_) con el `DPL` (_Descriptor Privilege Level_).


gdb_hello
---------
1- Poner un breakpoint en `env_pop_tf()` y continuar la ejecución hasta allí.
~~~
(gdb) b env_pop_tf
Punto de interrupción 1 at 0xf0102ff6: file kern/env.c, line 493.
(gdb) c
Continuando.
Se asume que la arquitectura objetivo es i386
=> 0xf0102ff6 <env_pop_tf>:	push   %ebp

Breakpoint 1, env_pop_tf (tf=0xf01c0000) at kern/env.c:493
493	{
(gdb)
~~~

2- En QEMU, entrar en modo monitor, y mostrar las cinco primeras líneas del comando info registers.
~~~
(qemu) info registers
EAX=003bc000 EBX=00010094 ECX=f03bc000 EDX=00000228
ESI=00010094 EDI=00000000 EBP=f0118fd8 ESP=f0118fbc
EIP=f0102ff6 EFL=00000092 [--S-A--] CPL=0 II=0 A20=1 SMM=0 HLT=0
ES =0010 00000000 ffffffff 00cf9300 DPL=0 DS   [-WA]
CS =0008 00000000 ffffffff 00cf9a00 DPL=0 CS32 [-R-]
~~~

3- De vuelta a GDB, imprimir el valor del argumento _tf_
~~~
(gdb) p tf
$1 = (struct Trapframe *) 0xf01c0000
(gdb)
~~~

4- Imprimir, con `x/Nx tf` tantos enteros como haya en el _struct Trapframe_ donde `N = sizeof(Trapframe) / sizeof(int)`
~~~
(gdb) print sizeof(struct Trapframe) / sizeof(int)
$3 = 17
(gdb) x/17x tf
0xf01c0000:	0x00000000	0x00000000	0x00000000	0x00000000
0xf01c0010:	0x00000000	0x00000000	0x00000000	0x00000000
0xf01c0020:	0x00000023	0x00000023	0x00000000	0x00000000
0xf01c0030:	0x00800020	0x0000001b	0x00000000	0xeebfe000
0xf01c0040:	0x00000023
(gdb)
~~~

El valor obtenido en `print sizeof(struct Trapframe) / sizeof(int)` es el esperado (`17`) ya que el _struct Trapframe_ tiene un elemento del tipo _struct PushRegs_, 8 del tipo `uint16_t` (`2 Bytes c/u`), 3 son `uint32_t` (`4 Bytes c/u`), y 2 `uintptr_t` (`4 Bytes c/u`). A su vez, dentro de PushRegs se encuentran 8 registros del tipo `uint32_t` (`4 Bytes c/u`).

Pasando en limpio:
~~~
8  del tipo uint16_t
11 del tipo uint32_t
2  del tipo uintptr_t
~~~

Por lo que
~~~
(8 * 2Bytes + 11 * 4Bytes + 2 * 4Bytes) / 4Bytes = (17 * 4Bytes) / 4bytes = 17
~~~

5- Avanzar hasta justo después del `movl ...,%esp`, usando `si M` para ejecutar tantas instrucciones como sea necesario en un solo paso:
~~~
(gdb) disas
Dump of assembler code for function env_pop_tf:
=> 0xf0102ff6 <+0>:	push   %ebp
   0xf0102ff7 <+1>:	mov    %esp,%ebp
   0xf0102ff9 <+3>:	sub    $0xc,%esp
   0xf0102ffc <+6>:	mov    0x8(%ebp),%esp
   0xf0102fff <+9>:	popa   
   0xf0103000 <+10>:	pop    %es
   0xf0103001 <+11>:	pop    %ds
   0xf0103002 <+12>:	add    $0x8,%esp
   0xf0103005 <+15>:	iret   
   0xf0103006 <+16>:	push   $0xf010555f
   0xf010300b <+21>:	push   $0x1f7
   0xf0103010 <+26>:	push   $0xf010551a
   0xf0103015 <+31>:	call   0xf01000a9 <_panic>
End of assembler dump.
(gdb) si 4
=> 0xf0102fff <env_pop_tf+9>:	popa   
0xf0102fff	494		asm volatile("\tmovl %0,%%esp\n"
(gdb) disas
Dump of assembler code for function env_pop_tf:
   0xf0102ff6 <+0>:	push   %ebp
   0xf0102ff7 <+1>:	mov    %esp,%ebp
   0xf0102ff9 <+3>:	sub    $0xc,%esp
   0xf0102ffc <+6>:	mov    0x8(%ebp),%esp
=> 0xf0102fff <+9>:	popa   
   0xf0103000 <+10>:	pop    %es
   0xf0103001 <+11>:	pop    %ds
   0xf0103002 <+12>:	add    $0x8,%esp
   0xf0103005 <+15>:	iret   
   0xf0103006 <+16>:	push   $0xf010555f
   0xf010300b <+21>:	push   $0x1f7
   0xf0103010 <+26>:	push   $0xf010551a
   0xf0103015 <+31>:	call   0xf01000a9 <_panic>
End of assembler dump.
(gdb)
~~~

6- Comprobar, con `x/Nx $sp` que los contenidos son los mismos que _tf_ (donde `N` es el tamaño de _tf_).
~~~
(gdb) x/17x $sp
0xf01c0000:	0x00000000	0x00000000	0x00000000	0x00000000
0xf01c0010:	0x00000000	0x00000000	0x00000000	0x00000000
0xf01c0020:	0x00000023	0x00000023	0x00000000	0x00000000
0xf01c0030:	0x00800020	0x0000001b	0x00000000	0xeebfe000
0xf01c0040:	0x00000023
(gdb)
~~~

En este punto podemos ver que el resultado obtenido es igual a lo obtenido en el punto 4, donde imprimimos el _Trapframe_.

7- Describir cada uno de los valores. Para los valores no nulos, se debe indicar dónde se configuró inicialmente el valor, y qué representa.

En este punto de la ejecución, como ya vimos en el punto anterior, el stack pointer tiene la misma estructura que el _Trapframe_.  
Por lo que primero debemos recordar como es la estructura de dicho struct.

~~~
struct PushRegs {
	/* registers as pushed by pusha */
	uint32_t reg_edi;
	uint32_t reg_esi;
	uint32_t reg_ebp;
	uint32_t reg_oesp;		/* Useless */
	uint32_t reg_ebx;
	uint32_t reg_edx;
	uint32_t reg_ecx;
	uint32_t reg_eax;
} __attribute__((packed));

struct Trapframe {
	struct PushRegs tf_regs;
	uint16_t tf_es;
	uint16_t tf_padding1;
	uint16_t tf_ds;
	uint16_t tf_padding2;
	uint32_t tf_trapno;
	/* below here defined by x86 hardware */
	uint32_t tf_err;
	uintptr_t tf_eip;
	uint16_t tf_cs;
	uint16_t tf_padding3;
	uint32_t tf_eflags;
	/* below here only when crossing rings, such as from user to kernel */
	uintptr_t tf_esp;
	uint16_t tf_ss;
	uint16_t tf_padding4;
} __attribute__((packed));
~~~

Ahora bien, vamos a analizar cada línea obtenida al inspeccionar el contenido del stack pointer.


Para comenzar vemos que el primer elemento del _Trapframe_ es `tf_regs`, que es un `struct PushRegs`.
Esta estructura contiene 8 elementos del tipo `uint32_t`, por lo que cada uno ocupa 4 bytes.
Entonces siguiendo el orden de `PushRegs`, podemos determinar que:

~~~
0xf01c0000: 0x00000000   0x00000000   0x00000000   0x00000000
             reg_edi      reg_esi      reg_ebp       reg_oesp
~~~

Siguiendo con la misma lógica podemos decir que la segunda línea obtenida corresponde a:

~~~
0xf01c0010: 0x00000000   0x00000000   0x00000000   0x00000000
             reg_ebx      reg_edx      reg_ecx       reg_eax
~~~

Ya terminamos con todos los campos que corresponden al _struct PushRegs_, por lo que los siguientes valores hacen referencia a los otros elementos del _struct Trapframe_.
El punto a considerar, es el tamaño de cada uno de estos elementos, ya que tendremos dos tipos distintos y que ocupan diferente espacio, que son `uint16_t` (`2 Bytes`) y `uint32_t` (`4 Bytes`)
~~~
0xf01c0020:  0x00000023            0x00000023         0x00000000      0x00000000
        tf_padding1 + tf_es   tf_padding2 + tf_ds      tf_trapno       tf_err
~~~

El primer valor lo podemos dividir en `tf_padding1 = 0x0000` y en `tf_es = 0x0023`, mientras que el segundo lo hacemos en `tf_padding2 = 0x0000` y en `tf_ds = 0x0023` (todos son del tipo `uint16_t`).
Tanto `tf_es` como `tf_ds` contienen el mismo valor (`0x0023`).
Esto se setea dentro de la función `env_alloc`, en donde se realiza el _or_ binario entre `GD_UD | 3`, en donde `GD_UD = 0x20` corresponde al _Global Descriptor Numbers User Data_ (definido en el archivo _memlayout.h_), y `3` hace refenrencia al ring 3 de privilegio.  

Siguiendo la estructura de una _Trapframe_, podemos decir que la próxima línea a que se obtuvo corresponde a:
~~~
0xf01c0030: 0x00800020      0x0000001b        0x00000000   0xeebfe000
             tf_eip     tf_cs + tf_padding3    tf_eflags    tf_esp
~~~

En el primer caso `tf_eip` (_Extended Instruction Pointer_) es del tipo `uintptr_t` por lo que ocupa `4 Bytes`.
Su valor (`0x00800020`) corresponde al address de la sección `.text` (_text segment_) dentro del ejecutable, es decir, la primera línea dentro de este.
Esto último lo podemos confirmar utilizando el comando `readelf` en los _Encabezados de Sección_:
~~~
Encabezados de Sección:
  [Nr] Nombre            Tipo            Direc    Desp   Tam    ES Opt En Inf Al
  [ 1] .text             PROGBITS        00800020 006020 000d19 00  AX  0   0 16
~~~

El segundo caso, lo podemos dividir en `tf_padding3 = 0x0000` y `tf_cs = 0x001b` ya que ambos ocupan 2 bytes (son del tipo `uint16_t`).
El valor de `tf_cs` se setea en la función `env_alloc` y surge del _or_ binario entre `GD_UT | 3`, en donde `GD_UT = 0x18` corresponde al _Global Descriptor Numbers User Text_ (definido en el archivo _memlayout.h_), y `3` hace refenrencia al ring 3 de privilegio.
Podemos verificarlo haciendo:
~~~
GD_UT | 3 = 0x18 | 0x03 = 00011000 | 00000011 = 00011011 = 0x1b
GD_UT | 3 = 0x0000001b
~~~

El último valor, `tf_esp`, también se setea en la función `env_alloc` igualándose a `USTACKTOP`, cuyo valor (`0xeebfe000`) se define en el archivo _memlayout.h_  que corresponde al tope del stack.


Finalizando, la última línea corresponde a los últimos dos campos del _Trapframe_ que son del tipo `uint16_t`:

~~~
0xf01c0040: 0x00000023
        tf_ss + tf_padding4
~~~

Que separándolos nos quedaría `tf_padding4 = 0x0000` y `tf_ss = 0x0023`.
El valor de `tf_ss` se setea en `env_alloc` y corresponde a `GD_UD | 3`, que fue explicado cuando hablamos de `tf_es` y `tf_ds` anteriormente.


8- Continuar hasta la instrucción `iret`, sin llegar a ejecutarla. Mostrar en este punto, de nuevo, las cinco primeras líneas de `info registers` en el monitor de QEMU. Explicar los cambios producidos.
~~~
(gdb) disas
Dump of assembler code for function env_pop_tf:
   0xf0102ff6 <+0>:	push   %ebp
   0xf0102ff7 <+1>:	mov    %esp,%ebp
   0xf0102ff9 <+3>:	sub    $0xc,%esp
   0xf0102ffc <+6>:	mov    0x8(%ebp),%esp
=> 0xf0102fff <+9>:	popa   
   0xf0103000 <+10>:	pop    %es
   0xf0103001 <+11>:	pop    %ds
   0xf0103002 <+12>:	add    $0x8,%esp
   0xf0103005 <+15>:	iret   
   0xf0103006 <+16>:	push   $0xf010555f
   0xf010300b <+21>:	push   $0x1f7
   0xf0103010 <+26>:	push   $0xf010551a
   0xf0103015 <+31>:	call   0xf01000a9 <_panic>
End of assembler dump.
(gdb) si 4
=> 0xf0103005 <env_pop_tf+15>:	iret   
0xf0103005	494		asm volatile("\tmovl %0,%%esp\n"
(gdb) disas
Dump of assembler code for function env_pop_tf:
   0xf0102ff6 <+0>:	push   %ebp
   0xf0102ff7 <+1>:	mov    %esp,%ebp
   0xf0102ff9 <+3>:	sub    $0xc,%esp
   0xf0102ffc <+6>:	mov    0x8(%ebp),%esp
   0xf0102fff <+9>:	popa   
   0xf0103000 <+10>:	pop    %es
   0xf0103001 <+11>:	pop    %ds
   0xf0103002 <+12>:	add    $0x8,%esp
=> 0xf0103005 <+15>:	iret   
   0xf0103006 <+16>:	push   $0xf010555f
   0xf010300b <+21>:	push   $0x1f7
   0xf0103010 <+26>:	push   $0xf010551a
   0xf0103015 <+31>:	call   0xf01000a9 <_panic>
End of assembler dump.
(gdb)
~~~

Al observar las primeras 5 líneas luego de `info registers`, obtuvimos lo siguiente:
~~~
(qemu) info registers
EAX=00000000 EBX=00000000 ECX=00000000 EDX=00000000
ESI=00000000 EDI=00000000 EBP=00000000 ESP=f01c0030
EIP=f0103005 EFL=00000096 [--S-AP-] CPL=0 II=0 A20=1 SMM=0 HLT=0
ES =0023 00000000 ffffffff 00cff300 DPL=3 DS   [-WA]
CS =0008 00000000 ffffffff 00cf9a00 DPL=0 CS32 [-R-]
~~~

Comparando con lo obtenido en el inciso 2, podemos ver que el `CS` (_Code Segment_) no se ve afectado.

Por otro lado, los registros de propósito general (`EAX - ECX - EDX - EBX - EBP - ESI - EDI`) si cambiaron de valor, esto se debe a la instrucción `popa` que fue mencionada anteriormente.
A su vez, el cambio producido en el registro `ES`, se debe a la instrucción `pop %es`.

Por último los cambios producidos dentro de `EIP` ocurren porque se avanzó en instrucciones, pero no depende del _Trapframe_.

9- Ejecutar la instrucción `iret`. En ese momento se ha realizado el cambio de contexto y los símbolos del kernel ya no son válidos.
~~~
(gdb) disas
Dump of assembler code for function env_pop_tf:
   0xf0102ff6 <+0>:	push   %ebp
   0xf0102ff7 <+1>:	mov    %esp,%ebp
   0xf0102ff9 <+3>:	sub    $0xc,%esp
   0xf0102ffc <+6>:	mov    0x8(%ebp),%esp
   0xf0102fff <+9>:	popa   
   0xf0103000 <+10>:	pop    %es
   0xf0103001 <+11>:	pop    %ds
   0xf0103002 <+12>:	add    $0x8,%esp
=> 0xf0103005 <+15>:	iret   
   0xf0103006 <+16>:	push   $0xf010555f
   0xf010300b <+21>:	push   $0x1f7
   0xf0103010 <+26>:	push   $0xf010551a
   0xf0103015 <+31>:	call   0xf01000a9 <_panic>
End of assembler dump.
(gdb) si 1
=> 0x800020:	cmp    $0xeebfe000,%esp
0x00800020 in ?? ()
~~~

- imprimir el valor del contador de programa con `p $pc` o `p $eip`.
~~~
(gdb) p $pc
$4 = (void (*)()) 0x800020
(gdb) p $eip
$5 = (void (*)()) 0x800020
(gdb)
~~~~

- cargar los símbolos de _hello_ con el comando `add-symbol-file`
~~~
(gdb) add-symbol-file obj/user/hello 0x800020
add symbol table from file "obj/user/hello" at
	.text_addr = 0x800020
(y or n) y
Leyendo símbolos desde obj/user/hello...hecho.
(gdb)
~~~

- volver a imprimir el valor del contador de programa
~~~
(gdb) p $pc
$2 = (void (*)()) 0x800020 <_start>
(gdb) p $eip
$3 = (void (*)()) 0x800020 <_start>
(gdb)
~~~

Mostrar una última vez la salida de `info registers` en QEMU, y explicar los cambios producidos.
~~~
(qemu) info registers
EAX=00000000 EBX=00000000 ECX=00000000 EDX=00000000
ESI=00000000 EDI=00000000 EBP=00000000 ESP=eebfe000
EIP=00800020 EFL=00000002 [-------] CPL=3 II=0 A20=1 SMM=0 HLT=0
ES =0023 00000000 ffffffff 00cff300 DPL=3 DS   [-WA]
CS =001b 00000000 ffffffff 00cffa00 DPL=3 CS32 [-R-]
~~~

En este punto, podemos ver que los registros `EAX - EBX - ECX - EDX - ESI - EDI - EBP` y el `ES` no sufrieron cambios.

Por otro lado, los campos `ESP`, `CS` y `EIP` se modificaron a los valores indicados por el _Trapframe_.

10- Poner un breakpoint temporal en la función `syscall()` y explicar qué ocurre justo tras ejecutar la instrucción `int $0x30`. Usar, de ser necesario, el monitor de QEMU.
~~~
(gdb) tbreak syscall
Punto de interrupción temporal 2 at 0x8009ed: syscall. (2 locations)
(gdb) c
Continuando.
=> 0x8009ed <syscall+17>:	mov    0x8(%ebp),%ecx

Temporary breakpoint 2, syscall (num=0, check=-289415544, a1=4005551752, a2=13, a3=0, a4=0, a5=0)
    at lib/syscall.c:23
23		asm volatile("int %1\n"
(gdb)
~~~

Lo que ocurre es que se genera una interrupción que es atrapada por el kernel, por lo que cambia el nivel de privilegio.

kern_idt
----------
**¿Cómo decidir si usar TRAPHANDLER o TRAPHANDLER_NOEC? ¿Qué pasaría si se usara solamente la primera?**

Para poder decidir cual de las dos usar hay que analizar el caso de cada interrupción, en particular, si dicha interrupción necesita devolver un código de error (`TRAPHANDLER`) o no (`TRAPHANDLER_NOEC`).

Si se utiliza solo la primera macro, cuando se invoque una interrupción que no pushea el código de error, se empezaría a tener un desfasaje de los registros del _Trapframe_.
En cambio, se utiliza la segundo función ya que ésta se encarga de pushear un 0 extra para completar el correspondiente código de error, solucionando así el problema mencionado.

**¿Qué cambia, en la invocación de handlers, el segundo parámetro (istrap) de la macro SETGATE? ¿Por qué se elegiría un comportamiento u otro durante un syscall?**
El parámetro `istrap` puede tomar dos valores distintos, 0 ó 1. En el caso que su valor sea cero, no se permiten otras interrumpciones cuando se está ejecutando.

Cuando no se permiten las interrumpciones anidadas mientras se ejecuta otra, el código es mucho más simple, pero con este comportam puede suceder que se atrasen las syscalls.

**Leer user/softint.c y ejecutarlo con make run-softint-nox. ¿Qué interrupción trata de generar? ¿Qué interrupción se genera? Si son diferentes a la que invoca el programa… ¿cuál es el mecanismo por el que ocurrió esto, y por qué motivos? ¿Qué modificarían en JOS para cambiar este comportamiento?**

~~~
[00000000] new env 00001000
Incoming TRAP frame at 0xefffffbc
TRAP frame at 0xf01c0000
  edi  0x00000000
  esi  0x00000000
  ebp  0xeebfdfd0
  oesp 0xefffffdc
  ebx  0x00000000
  edx  0x00000000
  ecx  0x00000000
  eax  0x00000000
  es   0x----0023
  ds   0x----0023
  trap 0x0000000d General Protection
  err  0x00000072
  eip  0x00800036
  cs   0x----001b
  flag 0x00000082
  esp  0xeebfdfd0
  ss   0x----0023
[00001000] free env 00001000
Destroyed the only environment - nothing more to do!
~~~

Podemos ver que el `trap` que ocurre corresponde a un _General Protection_ (trap número 13), aunque lo que se intenta generar es un _Page Fault_ (trap número 14).

Esto ocurre por el nivel de privilegio de la interrupción que se intenta generar, ya que fue declarada con un nivel de kernel, mientras que el archivo `softint.c` tiene privilegios a nivel de usuario.
Al ocurrir este error de privilegios ocurre justamente la excepción número 13.

Para modificarlo, debemos cambiar la configuración de _Page Fault_ que se encuentra en el archivo `trap.c`, reemplazando el último parámetro por un 3 (`ring_3`).

`SETGATE(idt[T_PGFLT], istrap, GD_KT, trap_14, ring_0);`


**Respuestas - Parte 5 - Protección de memoria:**
En el primer caso se intenta acceder(directamente) en una posición de memoria reservada para el kernel, es decir sin permisos de usuario. El espacio de direcciones es chequeado por la función "user_mem_assert". Si bien en ambos casos se intenta hacer lo mismo, en el segundo se intenta eliminar la referencia al puntero a través de una variable, se intentara copiar en first el contenido
de una dirección no mapeada en el espacio de direcciones del usuario(0xf010000c). Es atendido por otro mecanismo: La excepción Nro. 14 debido al page fault que se produce en modo usuario(ring 3). Es un problema si no contáramos con los mecanismos de Protección/Seguridad que se implementaron.

Las direcciones:
entry --> 0xEEBFDFC4
first --> 0xEEBFDFC3
