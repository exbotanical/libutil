# libutil

## Dynamic Linking

Linking to `c-util`:

```bash
# 1) include and use c-util in your project
# 2) generate object file for your project
gcc -I ../path/to/c-util -c main.c -o main.o
# 3) generate shared object file
make
# 4) link your project to c-util
gcc -o main main.o -L../path/to/c-util -lc-util
# you may need to add the lib location to your PATH
```
