# Lites

Lites is a 4.4BSD based Unix server that runs on top of the Mach
microkernel.  This repository collects several historical releases and
patch sets.

The tree is preserved mainly for reference.  Should a directory named
`lites-1.1-2025` (or similarly named) appear, it represents an ongoing
modernization effort and is not part of the original snapshots.

Much of the original documentation and source code mirrors have long
vanished from the Internet.  The list below records a number of archival
links that are still reachable, mostly via the Internet Archive.

## Historical links

- http://web.archive.org/web/20031205031618/http://www.cs.hut.fi/~jvh/lites.html
- https://web.archive.org/web/19970716001117/http://www.cs.hut.fi/~jvh/lites-1.0-announcement.html
- http://ftp.funet.fi/pub/mach/lites/
- http://ftp.funet.fi/pub/mach/
- http://ftp.lip6.fr/pub/mach/mach4/mach/lites/
- http://ftp.lip6.fr/pub/mach/mach4/mach/
- https://www.cs.utah.edu/flux/lites/html/
- https://www.cs.utah.edu/flux/lites/html/lites-info.html
- https://www.cs.utah.edu/flux/oldsite.html
- https://www.cs.utah.edu/flux/oldproj.html
- https://www.cs.utah.edu/flux/mach4/html/Mach4-proj.html
- https://www.cs.utah.edu/flux/mach4-parisc/html/pamach.html
- http://www.cs.cmu.edu/afs/cs.cmu.edu/project/mach/public/www/mach.html
- http://web.archive.org/web/20020420174411/http://www-2.cs.cmu.edu/afs/cs.cmu.edu/project/art-6/www/rtmach.html
- http://web.archive.org/web/19990502223408/https://www.cs.utah.edu/projects/flux/mach4-i386/html/mach4-UK22.html
- http://web.archive.org/web/19990508181023/http://www.cs.utah.edu/projects/flux/
- http://web.archive.org/web/20031206081657/http://www-2.cs.cmu.edu/afs/cs/project/mach/public/www/sources/sources_top.html

For an overview of the available releases and a script that can generate diff patches between them, see [docs/HISTORY.md](docs/HISTORY.md).


The file `johannes_helander-unix_under_mach-the_lites_server.pdf` in this
repository contains a comprehensive thesis describing Lites' design in
detail.

## Building

Lites requires a Mach 3 or Mach 4 kernel and a 4.4BSD userland.  Each
release is provided as a `*.tar.gz` archive.  Unpack the desired
version and build it inside the resulting directory.  For example:

```sh
tar -xzf lites-1.1.u3.tar.gz
cd lites-1.1.u3
./conf/configure    # or use the ./configure symlink
make
```

The optional `setup.sh` script installs a wide range of cross-compilers
and emulators.  It can be used to reproduce historical build setups, but
it requires root privileges and network access.

Additional notes are kept in [`docs/`](docs/).

