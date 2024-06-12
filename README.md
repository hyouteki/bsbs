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
