# Lites Modern Architecture, Design and Implementation

This document synthesizes the current project state with a roadmap toward
complete modernization. It summarises the code base structure, key design
goals and further development steps. It also serves as the definitive
architecture and implementation reference for ongoing work. Statistics have
been generated with `cloc`, `lizard` and `cscope`.

## Repository overview

The repository collects historical Lites releases and a consolidated modern
tree. The core components are:

- `core/` – kernel and low level services
- `drivers/` – device drivers and IOMMU helpers
- `libs/` – shared libraries such as the capability layer
- `servers/` – POSIX and capability aware services
- `include/` – public headers
- `docs/` – reference material and design notes

Legacy snapshots reside under `archives/`. Build files include
`Makefile.new` and CMake support. The project depends on an external Mach
kernel tree referenced by `LITES_MACH_DIR`.

## Code statistics

The current tree spans over **754k** lines of C code across more than three
thousand source files. Overall statistics from `cloc` (see `cloc.json`) are
reproduced below.

```
"C" :{
  "nFiles": 1058,
  "blank": 50906,
  "comment": 105064,
  "code": 336219}
```

The total number of code lines in all languages is **754,308** with
179,247 blank lines and 169,732 comment lines recorded in
`cloc.json`.

Cyclomatic complexity was measured using `lizard` across the major source
directories. The overall average is **4.8** with roughly **63k** logical lines
of code analysed spread across **1,973** functions. Detailed metrics are
available in `lizard.log`.

A `cscope.out` database covering the full tree is generated via

```sh
cscope -Rbq
```

which indexes **2827279** symbols for quick cross
reference queries.

### Extended analysis tools

Additional utilities help map out subsystem interactions. `doxygen` with
`graphviz` can generate call graphs from annotated sources. The `lizard`
report highlights functions with high cyclomatic complexity and should guide
refactoring efforts. Future work may integrate `cflow` and `radare2` for
deeper static and binary analysis when modernizing legacy modules.

## Current design highlights

- **Mach compatibility** – Lites builds on the Mach microkernel with BSD
  semantics preserved via a compatibility layer.
- **Capability APIs** – modern wrappers around POSIX resources track
  rights using capability objects. This enables fine grained revocation
  and delegation of privileges.
- **Mailbox IPC** – a lightweight message queue mechanism provides
  interprocess communication without Mach ports.
- **Security primitives** – keystore and enclave modules offer simple
  cryptographic services.
- **Architecture abstraction** – the build system supports multiple
  architectures such as x86_64, arm and riscv64.

## Roadmap and modernization plan

1. **C23 compliance** – adopt C23 features across the code base. Replace
   legacy constructs with `auto` for type deduction, use `constexpr`
   expressions where possible and leverage improved enumerations.
2. **Module decomposition** – incrementally break large source files into
   focused units. Introduce Doxygen blocks for each public interface.
3. **Enhanced build system** – migrate remaining Makefile fragments to
   modern CMake targets with interface libraries and imported Mach
   dependencies.
4. **POSIX layer expansion** – implement the POSIX roadmap tasks listed in
   `docs/POSIX_ROADMAP.md`, starting with process control and memory
   management.
5. **Refined capability model** – extend the capability libraries to
   support hierarchical namespaces and revocation trees.
6. **Testing framework** – restore historical unit tests using a
   cross‑architecture CI setup. Integrate sanitizers and static analysis
   (clang‑tidy, cppcheck).
7. **Documentation overhaul** – continue updating all documentation and
   examples. Ensure each subsystem is described with diagrams and sample
   code.

## Conclusion

Lites combines research operating system ideas with historical BSD code.
By gradually refactoring toward modern C23 idioms and improving modularity
we can evolve the project into a maintainable microkernel server. The
existing tools (`cloc`, `lizard`, `cscope`) aid in assessing progress and
spotting complex areas that need attention.

Third party components such as Intel's `asmlib` are kept in source form
under `third_party/`. Windows specific binaries like the provided `.dll`
archives are removed from the tree to keep the project self-contained for
UNIX-like targets.  Assembly routines may be rewritten in portable C23 as
development proceeds.
