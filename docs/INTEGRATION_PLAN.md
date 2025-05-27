# Integration of Eigen, MLP and Cap'n Proto

This document proposes incorporating the Eigen linear algebra library, a small MLP module and the Cap'n Proto serialization framework into the modernised Lites tree.

## Motivation

The ongoing modernisation envisions a capability-aware POSIX layer that can host advanced services. Integrating these libraries enables new features without rewriting common building blocks.

- **Eigen** provides a header-only C++ template library for dense and sparse linear algebra. Its inclusion allows mathematical routines in user space, kernel helpers or test suites without depending on external packages at runtime.
- **MLP module** offers a simple multi-layer perceptron infrastructure to experiment with heuristic-based scheduling, memory management or security checks.
- **Cap'n Proto** delivers efficient schema-based RPC and message serialisation. Using it within the IPC helpers simplifies structured communication across user space components.

## Planned work

1. **Embed Eigen**
   - Add `third_party/eigen` to track a minimal subset of Eigen (e.g. `Eigen/Core` and `Eigen/Dense`).
   - Update the build system to include the Eigen headers when compiling optional modules. The tree remains self-contained, requiring no external install.
   - Document basic usage with an example test under `tests/eigen_demo/`.

2. **Introduce a toy MLP library**
   - Create `libmlp/` with small C++ utilities supporting fully connected layers and ReLU/softmax activations.
   - Expose a C-friendly interface in `include/mlp.h` so kernel-adjacent code can call into it.
   - Provide sample heuristics in `kern/sched_mlp.c` demonstrating how the scheduler could consult a tiny model.

3. **Cap'n Proto for structured IPC**
   - Vendor the Cap'n Proto C++ sources in `third_party/capnp`. Only the core serialization and RPC code is required.
   - Extend `ipc.c` with optional Cap'n Proto encoders/decoders for mailbox messages.
   - Document the message schema for the POSIX wrappers.

4. **Update documentation**
   - Describe the integration steps in this document and link it from `docs/INDEX.md` and `docs/MODERNIZATION.md`.
   - Outline new build flags (`USE_EIGEN`, `USE_CAPNP`, etc.) in `README.md`.

## Open tasks

- [ ] Add third-party sources for Eigen and Cap'n Proto.
- [ ] Implement `libmlp` and example scheduler hook.
- [ ] Provide Cap'n Proto bindings in `ipc.c` and new tests.
- [ ] Update build scripts and CI checks for the optional components.


