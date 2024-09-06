# Bull Shit Build System
Minimal Build System implemented in C.

``` makefile
let Log = Verbose
let CC = gcc
let CFLAGS = -Wall -Wextra
let BUILD_NAME = hello

label build in
    run $CC main.c -o $BUILD_NAME $CFLAGS
end

label hello in
    run $CC hello.c -o hello $CFLAGS
    run ./hello
    run rm -r hello
end
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
