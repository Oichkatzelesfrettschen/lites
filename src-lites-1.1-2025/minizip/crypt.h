#ifndef _MINIZIP_CRYPT_H
#define _MINIZIP_CRYPT_H

#ifdef __cplusplus
extern "C" {
#endif

const unsigned long *get_crc_table(void);
void init_keys(const char *passwd, unsigned long *pkeys, const unsigned long *pcrc_32_tab);
int zdecode(unsigned long *pkeys, const unsigned long *pcrc_32_tab, int c);

#ifdef __cplusplus
}
#endif

#endif /* _MINIZIP_CRYPT_H */
