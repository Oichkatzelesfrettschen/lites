/*
 * One file structure is allocated
 * for each open/creat/pipe call.
 * Main use is to hold the read/write
 * pointer associated with each open
 * file.
 */
#ifndef _FILE_H_
#define _FILE_H_

struct file {
    char f_flag;
    char f_count;      /* reference count */
    int f_inode;       /* pointer to inode structure */
    char *f_offset[2]; /* read/write character pointer */
} file[NFILE];

/* flags */
#define FREAD 01
#define FWRITE 02
#define FPIPE 04
#endif /* _FILE_H_ */
