# x86_64_v1 Architecture Support

This directory contains the initial skeleton for a future `x86_64_v1`
port. It aims to replicate the existing `x86_64` implementation's
functionality without behavioural changes.

## Assumptions

- System V ABI is used throughout.
- No floating point context management exists yet.

## Notes

Basic TLB invalidation routines are currently provided only for the `x86_64`
architecture. Other targets, including `arm64` and `riscv64`, remain
unsupported and will need dedicated implementations.

Further machine specific code will be added over time.
