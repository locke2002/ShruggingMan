# ShruggingMan
Shrugging Man Enterprises

This MUD is a hobby project.

Systems: Ubuntu versions 14.04 - 15.04
GCC: 4.9.2

make afkmud

  Compiling o/imc.o....
g++ -c  -Wall -Wformat-security -Wshadow -Wpointer-arith -Wcast-align -Wredundant-decls  -g2 -rdynamic -pipe -Os   -export-dynamic -DIMC imc.cpp -o o/imc.o
In file included from imc.cpp:60:0:
descriptor.h:32:18: fatal error: zlib.h: No such file or directory
 #include <zlib.h>
                  ^
compilation terminated.
Makefile:169: recipe for target 'o/imc.o' failed
make: *** [o/imc.o] Error 1

