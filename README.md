# Bull Shit Build System
Minimal Build System implemented in C.

``` makefile
DEF Log = Verbose
DEF CC = gcc
DEF CFLAGS = -Wall -Wextra
DEF BUILD_NAME = hello

LABEL build {
    RUN $CC main.c -o $BUILD_NAME $CFLAGS
}

LABEL hello {
    RUN $CC hello.c -o hello $CFLAGS
    RUN ./hello
    RUN rm -r hello
}
```

## Quick Start
- Build the project
``` bash
gcc main.c -Wall -Wextra -o bsbs
```
- Build the example file 'hello'
``` bash
./bsbs hello
```
``` console
+ CC = gcc
+ CFLAGS = -Wall -Wextra
+ BUILD_NAME = hello
+ gcc hello.c -o hello -Wall -Wextra
+ ./hello
Hello moto
+ rm -r hello
```

- Build the project but using *bsbs*.
``` bash
./bsbs build
```
``` console
+ CC = gcc
+ CFLAGS = -Wall -Wextra
+ BUILD_NAME = hello
+ gcc main.c -o hello -Wall -Wextra
```
