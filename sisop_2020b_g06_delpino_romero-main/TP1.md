TP1: Memoria virtual en JOS
===========================

backtrace_func_names
--------------------

Salida del comando `backtrace`:

~~~
qemu-system-i386 -nographic -drive file=obj/kern/kernel.img,index=0,media=disk,format=raw -serial mon:stdio -gdb tcp:127.0.0.1:26000 -D qemu.log  -d guest_errors
6828 decimal is 15254 octal!
entering test_backtrace 5
entering test_backtrace 4
entering test_backtrace 3
entering test_backtrace 2
entering test_backtrace 1
entering test_backtrace 0
ebp f0110f18 eip f010006a args 00000000 00000000 00000000 0000001a f0100c6f
 kern/init.c:21: test_backtrace+42
ebp f0110f38 eip f010008f args 00000000 00000001 f0110f78 0000001a f0100c6f
 kern/init.c:19: test_backtrace+79
ebp f0110f58 eip f010008f args 00000001 00000002 f0110f98 0000001a f0100c6f
 kern/init.c:19: test_backtrace+79
ebp f0110f78 eip f010008f args 00000002 00000003 f0110fb8 0000001a f0100c6f
 kern/init.c:19: test_backtrace+79
ebp f0110f98 eip f010008f args 00000003 00000004 00000000 0000001d f0110fc8
 kern/init.c:19: test_backtrace+79
ebp f0110fb8 eip f010008f args 00000004 00000005 f0110fd8 f0100766 00010094
 kern/init.c:19: test_backtrace+79
ebp f0110fd8 eip f01000d4 args 00000005 00001aac 00000650 00000000 00000000
 kern/init.c:46: i386_init+64
ebp f0110ff8 eip f010003e args 00112021 00000000 00000000 00000000 00000000
 kern/entry.S:84: entry+50
leaving test_backtrace 0
leaving test_backtrace 1
leaving test_backtrace 2
leaving test_backtrace 3
leaving test_backtrace 4
leaving test_backtrace 5
Physical memory: 131072K available, base = 640K, extended = 130432K
>>>
>>> kernel panic at kern/pmap.c:131: mem_init: This function is not finished

>>>
Welcome to the JOS kernel monitor!
Type 'help' for a list of commands.
K>


K> backtrace

Type 'help' for a list of commands.
K> backtrace
ebp f0110f08 eip f0100a6e args 00000001 f0110f30 00000000 0000000a 00000009
 kern/monitor.c:141: runcmd+261
ebp f0110f88 eip f0100ab3 args f01020e4 f0110fcc f0110fb8 f0100106 00000000
 kern/monitor.c:159: monitor+62
ebp f0110f98 eip f0100106 args 00000000 f0110fcc 00000083 f0100b4a 00010094
 kern/init.c:85: _panic+27
ebp f0110fb8 eip f0100b76 args f010214c 00000083 f0102120 f0100b5f 00010094
 kern/pmap.c:241: page_init+0
ebp f0110fd8 eip f01000d9 args 00000005 00001aac 00000650 00000000 00000000
 kern/init.c:46: i386_init+69
ebp f0110ff8 eip f010003e args 00112021 00000000 00000000 00000000 00000000
 kern/entry.S:84: entry+50
K>
~~~

boot_alloc_pos
--------------
Utilizando el comando _`readelf -s obj/kern/kernel`_ podemos ver la tabla de s??mbolos generada por el linker de nuestro binario _kernel_.

Luego buscamos la entrada con identificador _end_ que apunta al final del BSS Segment.

  A continuaci??n podemos ver que tiene un valor `0xF0114950`.
~~~
Num:    Valor  Tam  Tipo    Uni??n  Vis      Nombre Ind
102: f0114950     0 NOTYPE  GLOBAL DEFAULT    6    end
~~~

Dentro de la funci??n _boot_alloc_ se realiza la siguiente operaci??n:
~~~
nextfree = ROUNDUP((char *) end, PGSIZE);  // Primer posici??n. Fin del BSS.
~~~
En donde `PGSIZE` esta definido como 4096, que en hexadecimal corresponder??a a 0x1000.
Por otro lado, `ROUNDUP`redondear??a end al m??ltiplo m??s cercano de `PGSIZE`.
Lo que significar??a el valor alineado a PGSIZE.


Esta cuenta se puede hacer r??pidamente por el valor de PGSIZE mencionado anteriormente, por lo que el m??ltiplo m??s cercano ser??a `0xF0115000`.
~~~
0xF0114950 % 0x1000 = 0x950
0xF0115000 % 0x1000 = 0x000
~~~

En conclusi??n, la primera direcci??n de memoria que devolver?? ser?? `0xF0115000`.

---
Sesi??n de GDB

Al tener problemas con el comando _finish_ se resolvi?? de dos maneras distintas.
La primera poniendo un breakpoint en la ??ltima l??nea de la funci??n y luego inspeccionamos el valor de _result_

~~~
(gdb) b pmap.c:122
Punto de interrupci??n 1 at 0xf0100b9f: file kern/pmap.c, line 122.
(gdb) c
Continuando.
Se asume que la arquitectura objetivo es i386
=> 0xf0100b9f <boot_alloc+58>:	repz ret

Breakpoint 1, boot_alloc (n=4160749568) at kern/pmap.c:122
122	}
(gdb) p result
$1 = 0xf0115000 ""
(gdb) p/x result
$2 = 0xf0115000
(gdb)
~~~

Podemos ver que, efectivamente, el valor devuelto en la primera llamada de la funci??n es `0xF0115000`.

La segunda metodolog??a fue ir inspeccionando los valores en las primeras llamadas de la funci??n `boot_alloc`.

~~~
(gdb) b boot_alloc
Punto de interrupci??n 1 at 0xf0100b65: file kern/pmap.c, line 91.
(gdb) c
Continuando.
Se asume que la arquitectura objetivo es i386
=> 0xf0100b65 <boot_alloc>:	push   %ebp

Breakpoint 1, boot_alloc (n=981) at kern/pmap.c:91
91	{
(gdb) p nextfree
$1 = 0x0
(gdb) p &end
$2 = (<data variable, no debug info> *) 0xf0114950
(gdb) c
Continuando.
=> 0xf0100b65 <boot_alloc>:	push   %ebp

Breakpoint 1, boot_alloc (n=158) at kern/pmap.c:91
91	{
(gdb) p nextfree
$3 = 0xf0116000 ""
(gdb)
~~~

En la sesi??n anterior podemos verificar el valor de `end` obtenido en el punto anterior.

Por otro lado, el ??ltimo valor de `nextfree` corresponde a la siguiente p??gina libre, por lo que ser??a el valor devuelto en la primera llamada a `boot_alloc` m??s el tama??o de p??gina.

~~~
0xF0115000 + 0x1000 = 0xF0116000
~~~

page_alloc
----------
`page2pa()`: Devuelve la direcci??n f??sica de la p??gina cargada en el argumento.

`page2kva()`: Devuelve la direcci??n virtual de la p??gina (al valor devuelto por la funci??n anterior le suma KERNBASE).


map_region_large
----------------
Con el uso de **Large Pages**, no es necesario la utilizaci??n de una _page table_ intermedia, ya que con el `PDE` se pueden referenciar los `4 MiB`.

Como cada page table es de 1024 entradas (de 4 bytes), podemos decir, que por cada large page, se ahorran `4 KiB` (1024 x 4 bytes).

Por otro lado, se realizan 3 llamadas a la funci??n `boot_map_region`, y solo la ??ltima es la que hace uso de dichas _large pages_.
Esto se da debido a que se quieren mapear `256MiB` (los primeros 256MiB de memoria f??sica en KERNBASE), en el cual, tanto la direcci??n f??sica (`0x00000000`) como la direcci??n virtual (`0xF0000000`), est??n alineadas a `4MiB`.

Y como se necesitan 64 p??ginas, el total de memoria ahorrada es de `256KiB`.

~~~
64 x 4KiB = 256KiB
~~~

En la primera llamada a `boot_map_region` no se utilizan _large pages_ porque la _physical address_ no est?? alineada a 4MiB. Mientras que en la segunda, el par??metro `size` es inferior a 4MiB.

Esto es una cantidad fija ya que el tama??o que recibe la funci??n mencionda, es siempre constante dentro del contexto de _JOS_.
Es decir, que el ahorro de memoria no depende de la memoria f??sica de la computadora.
