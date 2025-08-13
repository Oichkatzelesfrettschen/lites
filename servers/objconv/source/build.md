# Building objconv

This component compiles a standalone `objconv` utility from the C++ sources in this directory. Use a modern compiler and optimiser:

```bash
# GNU toolchain
g++ -O2 *.cpp -o objconv

# or using Clang
clang++ -O2 *.cpp -o objconv
```

The resulting binary performs object file conversion and inspection tasks used by the server build.
