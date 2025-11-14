#ifndef BSD_COMPAT_H
#define BSD_COMPAT_H

/* Common BSD version constants used across the tree */
#define BSD     199306          /* System version (year & month) */
#define BSD4_3  1
#define BSD4_4  1

/* Fast file system format identifiers */
#define FS_42INODEFMT       -1  /* 4.2BSD inode format */
#define FS_44INODEFMT       2   /* 4.4BSD inode format */
#define FS_42POSTBLFMT      -1  /* 4.2BSD rotational table format */
#define FS_DYNAMICPOSTBLFMT 1   /* dynamic rotational table format */

#endif /* BSD_COMPAT_H */
