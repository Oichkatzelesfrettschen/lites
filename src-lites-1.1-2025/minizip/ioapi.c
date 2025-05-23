#include "ioapi.h"
#include <stdio.h>

static voidpf fopen_file_func(voidpf opaque, const char *filename, int mode) {
    FILE *file = NULL;
    const char *mode_fopen = "rb";
    (void)opaque;
    if ((mode & ZLIB_FILEFUNC_MODE_READWRITEFILTER) == ZLIB_FILEFUNC_MODE_READ)
        mode_fopen = "rb";
    else if (mode & ZLIB_FILEFUNC_MODE_EXISTING)
        mode_fopen = "r+b";
    else if (mode & ZLIB_FILEFUNC_MODE_CREATE)
        mode_fopen = "wb";
    file = fopen(filename, mode_fopen);
    return file;
}

static uLong fread_file_func(voidpf opaque, voidpf stream, void *buf, uLong size) {
    (void)opaque;
    return (uLong)fread(buf, 1, size, (FILE *)stream);
}

static uLong fwrite_file_func(voidpf opaque, voidpf stream, const void *buf, uLong size) {
    (void)opaque;
    return (uLong)fwrite(buf, 1, size, (FILE *)stream);
}

static long ftell_file_func(voidpf opaque, voidpf stream) {
    (void)opaque;
    return ftell((FILE *)stream);
}

static long fseek_file_func(voidpf opaque, voidpf stream, uLong offset, int origin) {
    (void)opaque;
    fseek((FILE *)stream, offset, origin);
    return ftell((FILE *)stream);
}

static int fclose_file_func(voidpf opaque, voidpf stream) {
    (void)opaque;
    return fclose((FILE *)stream);
}

static int ferror_file_func(voidpf opaque, voidpf stream) {
    (void)opaque;
    return ferror((FILE *)stream);
}

void fill_fopen_filefunc(zlib_filefunc_def *pz) {
    pz->zopen_file = fopen_file_func;
    pz->zread_file = fread_file_func;
    pz->zwrite_file = fwrite_file_func;
    pz->ztell_file = ftell_file_func;
    pz->zseek_file = fseek_file_func;
    pz->zclose_file = fclose_file_func;
    pz->zerror_file = ferror_file_func;
    pz->opaque = NULL;
}
