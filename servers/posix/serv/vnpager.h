/*
 * Configuration header for VNPAGER feature
 *
 * VNPAGER controls the vnode pager implementation
 * for memory-mapped file I/O.
 *
 * When enabled (1): Use vnode-based pager
 * When disabled (0): Traditional file I/O
 */

#ifndef _SERV_VNPAGER_H_
#define _SERV_VNPAGER_H_

/*
 * VNPAGER - Enable vnode pager
 *
 * Set to 0 for traditional file I/O (default)
 * Set to 1 to enable vnode pager
 */
#ifndef VNPAGER
#define VNPAGER	0
#endif

#endif /* _SERV_VNPAGER_H_ */
