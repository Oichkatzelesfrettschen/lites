# Lites History

This repository contains several snapshots of the Lites microkernel server.
The included `scripts/generate-diffs.sh` helper can be used to build patch
files that show how the tree evolved between the available versions.

## Available versions

- **lites-1.0** – earliest snapshot in this repository
- **lites-1.1** – successor to 1.0
- **lites-1.1-950808** – snapshot dated 1995-08-08
- **lites-1.1.u1** – first update
- **lites-1.1.u2** – second update
- **lites-1.1.u3** – third update
- **lites-1.1.1** – derived from 1.1 with the u1 snapshot integrated

Running the diff generator script will place compressed patch files
in a new `diffs` directory.  These patches allow quick inspection of the
changes that occurred between each successive version without relying on
merge commits.
