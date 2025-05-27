# BSD Directory

This folder consolidates all BSD related sources found in the repository.  The
original directories are preserved in their original locations and linked here
via symbolic links for convenience.

The following symlinks are provided:

- `netbsd` -> `../netbsd`
- `i386_boot_bsd` -> `../i386/boot/bsd`
- `4_3BSD_Reno` -> `../43_BSD_Versions/4_3BSD_Reno`
- `4_3BSD_Tahoe` -> `../43_BSD_Versions/4_3BSD_Tahoe`
- `4_3UWiscBSD` -> `../43_BSD_Versions/4_3UWiscBSD`

To generate diffs between the versions run `generate_diffs.sh` in this
folder. This will produce patch files comparing each version of the BSD
sources found in the repository.
