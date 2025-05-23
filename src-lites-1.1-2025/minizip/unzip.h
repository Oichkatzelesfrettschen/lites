#ifndef _UNZIP_H
#define _UNZIP_H

#include "ioapi.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tm_unz_s {
    uInt tm_sec;  /* seconds after the minute - [0,59] */
    uInt tm_min;  /* minutes after the hour - [0,59] */
    uInt tm_hour; /* hours since midnight - [0,23] */
    uInt tm_mday; /* day of the month - [1,31] */
    uInt tm_mon;  /* months since January - [0,11] */
    uInt tm_year; /* years - 1980 */
} tm_unz;

typedef struct unz_global_info_s {
    uLong number_entry; /* total number of entries in the central dir*/
    uLong size_comment; /* size of the global comment of the zipfile */
} unz_global_info;

typedef struct unz_file_info_s {
    uLong version;            /* version made by                 2 bytes */
    uLong version_needed;     /* version needed to extract       2 bytes */
    uLong flag;               /* general purpose bit flag        2 bytes */
    uLong compression_method; /* compression method              2 bytes */
    uLong dosDate;            /* last mod file date in Dos fmt   4 bytes */
    uLong crc;                /* crc-32                          4 bytes */
    uLong compressed_size;    /* compressed size                 4 bytes */
    uLong uncompressed_size;  /* uncompressed size               4 bytes */
    uLong size_filename;      /* filename length                 2 bytes */
    uLong size_file_extra;    /* extra field length              2 bytes */
    uLong size_file_comment;  /* file comment length             2 bytes */
    uLong disk_num_start;     /* disk number start               2 bytes */
    uLong internal_fa;        /* internal file attributes        2 bytes */
    uLong external_fa;        /* external file attributes        4 bytes */
    tm_unz tmu_date;
} unz_file_info;

typedef struct unz_file_pos_s {
    uLong pos_in_zip_directory; /* offset in zip directory   */
    uLong num_of_file;          /* # of the file             */
} unz_file_pos;

typedef void *unzFile;

int unzStringFileNameCompare(const char *fileName1, const char *fileName2, int iCaseSensitivity);

unzFile unzOpen(const char *path);
unzFile unzOpen2(const char *path, zlib_filefunc_def *pzlib_filefunc_def);
int unzClose(unzFile file);
int unzGetGlobalInfo(unzFile file, unz_global_info *pglobal_info);
int unzGetCurrentFileInfo(unzFile file, unz_file_info *pfile_info, char *szFileName,
                          uLong fileNameBufferSize, void *extraField, uLong extraFieldBufferSize,
                          char *szComment, uLong commentBufferSize);
int unzGoToFirstFile(unzFile file);
int unzGoToNextFile(unzFile file);
int unzLocateFile(unzFile file, const char *szFileName, int iCaseSensitivity);
int unzGetFilePos(unzFile file, unz_file_pos *file_pos);
int unzGoToFilePos(unzFile file, unz_file_pos *file_pos);
int unzOpenCurrentFile(unzFile file);
int unzOpenCurrentFilePassword(unzFile file, const char *password);
int unzOpenCurrentFile2(unzFile file, int *method, int *level, int raw);
int unzOpenCurrentFile3(unzFile file, int *method, int *level, int raw, const char *password);
int unzReadCurrentFile(unzFile file, void *buf, unsigned len);
z_off_t unztell(unzFile file);
int unzeof(unzFile file);
int unzGetLocalExtrafield(unzFile file, void *buf, unsigned len);
int unzCloseCurrentFile(unzFile file);
int unzGetGlobalComment(unzFile file, char *szComment, uLong uSizeBuf);
uLong unzGetOffset(unzFile file);
int unzSetOffset(unzFile file, uLong pos);

#ifdef __cplusplus
}
#endif

#endif /* _UNZIP_H */
