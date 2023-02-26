# libutil

## Dynamic Linking

Linking to `libutil`:

```bash
# 1) include and use libutil in your project
# 2) generate object file for your project
gcc -I ../path/to/libutil -c main.c -o main.o
# 3) generate shared object file
make
# 4) link your project to libutil
gcc -o main main.o -L../path/to/libutil -lutil
# you may need to add the lib location to your PATH
```
