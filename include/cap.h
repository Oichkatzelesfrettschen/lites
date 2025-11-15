/*
 * Capability Security Header
 * Minimal stub for Lites 1.1.u3
 *
 * Provides capability-based security primitives
 */

#ifndef _CAP_H_
#define _CAP_H_

/* Capability structure forward declaration */
struct capability;
typedef struct capability *cap_t;

/* Basic capability operations (stubs for now) */
#define CAP_NONE    0x00
#define CAP_READ    0x01
#define CAP_WRITE   0x02
#define CAP_EXECUTE 0x04

#endif /* _CAP_H_ */
