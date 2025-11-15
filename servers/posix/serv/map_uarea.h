/*
 * Configuration header for MAP_UAREA feature
 *
 * MAP_UAREA controls whether user area (u-area) is memory-mapped
 * for faster access to process state.
 *
 * When enabled (1): Process u-area is mapped into shared memory
 * When disabled (0): Traditional kernel-only u-area access
 */

#ifndef _SERV_MAP_UAREA_H_
#define _SERV_MAP_UAREA_H_

/*
 * MAP_UAREA - Enable memory-mapped user area
 *
 * Set to 0 for traditional operation (default)
 * Set to 1 to enable memory-mapped u-area feature
 */
#ifndef MAP_UAREA
#define MAP_UAREA	0
#endif

#endif /* _SERV_MAP_UAREA_H_ */
