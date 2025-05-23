#ifndef _ZLIBIOAPI_H
#define _ZLIBIOAPI_H

#include "zlib.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef voidpf (*open_file_func)(voidpf opaque, const char *filename, int mode);
typedef uLong (*read_file_func)(voidpf opaque, voidpf stream, void *buf, uLong size);
typedef uLong (*write_file_func)(voidpf opaque, voidpf stream, const void *buf, uLong size);
typedef long (*tell_file_func)(voidpf opaque, voidpf stream);
typedef long (*seek_file_func)(voidpf opaque, voidpf stream, uLong offset, int origin);
typedef int (*close_file_func)(voidpf opaque, voidpf stream);
typedef int (*error_file_func)(voidpf opaque, voidpf stream);

typedef struct zlib_filefunc_def_s {
    open_file_func zopen_file;
    read_file_func zread_file;
    write_file_func zwrite_file;
    tell_file_func ztell_file;
    seek_file_func zseek_file;
    close_file_func zclose_file;
    error_file_func zerror_file;
    voidpf opaque;
} zlib_filefunc_def;

void fill_fopen_filefunc(zlib_filefunc_def *pzlib_filefunc_def);

#define ZLIB_FILEFUNC_SEEK_SET 0
#define ZLIB_FILEFUNC_SEEK_CUR 1
#define ZLIB_FILEFUNC_SEEK_END 2

#define ZLIB_FILEFUNC_MODE_READ 1
#define ZLIB_FILEFUNC_MODE_WRITE 2
#define ZLIB_FILEFUNC_MODE_READWRITEFILTER (ZLIB_FILEFUNC_MODE_READ | ZLIB_FILEFUNC_MODE_WRITE)
#define ZLIB_FILEFUNC_MODE_EXISTING 4
#define ZLIB_FILEFUNC_MODE_CREATE 8

#define ZOPEN(func, filename, mode) ((func).zopen_file((func).opaque, filename, mode))
#define ZREAD(func, stream, buf, size) ((func).zread_file((func).opaque, stream, buf, size))
#define ZWRITE(func, stream, buf, size) ((func).zwrite_file((func).opaque, stream, buf, size))
#define ZTELL(func, stream) ((func).ztell_file((func).opaque, stream))
#define ZSEEK(func, stream, pos, mode) ((func).zseek_file((func).opaque, stream, pos, mode))
#define ZCLOSE(func, stream) ((func).zclose_file((func).opaque, stream))
#define ZERROR(func, stream) ((func).zerror_file((func).opaque, stream))

#ifdef __cplusplus
}
#endif

#endif /* _ZLIBIOAPI_H */
