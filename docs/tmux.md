# tmux workflow

This guide explains how to run the Lites server under QEMU using tmux.
The `setup.sh` script installs tmux together with other development
tools, making the following workflow available out of the box.

## Starting QEMU in a tmux session

Invoke `scripts/tmux-qemu.sh` with an optional architecture argument:

```sh
scripts/tmux-qemu.sh            # defaults to x86_64
scripts/tmux-qemu.sh i686       # run the 32-bit build
```

The script creates a new tmux session with two panes. The left pane
executes `scripts/run-qemu.sh` while the right pane remains an
interactive shell from which you can issue additional commands or view
logs.

## Troubleshooting

### QEMU complains about an executable stack

Some toolchains mark binaries as requiring an executable stack. When
QEMU refuses to start the image, pass `-Wl,-z,noexecstack` in
`LDFLAGS` when linking `lites_server`. This clears the executable stack
flag and resolves the error.
