# Bull Shit Build System
Minimal Build System implemented in C.

``` asm
CC = gcc
CFLAGS = -Wall -Wextra
Log = Verbose

label build in
    $CC hello.c -o hello $CFLAGS
    ./hello
    rm -r hello
end
```

## Quick Start
``` bash
chmod +x build.sh
./build.sh
./bsbs
```

``` console
1    CC = gcc
2    CFLAGS = -Wall -Wextra
3    Log = Verbose
5    label build in
6        $CC hello.c -o hello $CFLAGS
7        ./hello
8        rm -r hello
9    end
+ gcc hello.c -o hello -Wall -Wextra
+ ./hello
Hello moto
+ rm -r hello

```
