# Utility programs

This directory contains assorted helper tools built as part of Lites. Each file
can typically be compiled directly with the provided Makefile.

`MAKEDEV.lites` is a small shell script used to create device nodes when running
Lites on top of Mach. Invoke it from the desired `/dev` directory, e.g.:

```sh
sh MAKEDEV.lites all
```

This will create the time and X11 related special files expected by Lites.
