# Final Source Layout

This note expands upon the repository restructure plan. The goal is to split the tree into clearer components so that kernels, device drivers and libraries can evolve independently.

## Directory layout

```
core/       - kernel sources and microkernel glue
drivers/    - hardware drivers and board support
libs/       - user space libraries used by servers and tests
servers/    - POSIX and capability-aware services
third_party/ - external dependencies kept in-tree
cmake/      - common build system logic
util/       - small command line utilities
examples/   - demonstration programs
Historical Archives/ - preserved snapshots and tarballs
```
The directory is now named `mach_kernel`. Kernel headers such as `spinlock.h` remain in `core/include/` while
public headers are provided under `include/`.

Other existing directories such as `tests/` and `docs/` remain at the top level.
The legacy release trees once stored at the repository root have been moved
into `Historical Archives/`.

## Header organisation

Public headers reside directly under `include/`. Kernel-only headers stay in `core/include` and driver specific headers remain next to their driver sources. Libraries may keep local headers, but those intended for broad use have been moved under `include/`.

Build scripts will be updated so that these include paths are automatically added when compiling each component.

## Build file updates

Top level `CMakeLists.txt` and `Makefile.new` will reference the new subdirectories. Each directory gains its own `CMakeLists.txt` describing local targets. Drivers and libraries can be built individually or as part of the whole tree.

## Testing

Once the restructure is complete, unit tests continue to build via

```sh
make -f Makefile.new test
```

or with CMake:

```sh
cmake --build build --target test
```

Running the above commands should build all tests under `tests/` against the new layout. Ensure the `core`, `drivers` and `libs` directories are listed in the include paths when invoking the compiler.

### Follow-up tasks

After relocating the files perform a clean rebuild to regenerate any cached
objects and configuration:

```sh
rm -rf build
cmake -B build
cmake --build build
```

Check that utilities in `util/` and the programs under `examples/` still compile
successfully.
