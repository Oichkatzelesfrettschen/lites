/*
 * port_mgr.h
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.10 $
 * $Date: 1993/03/26 19:23:08 $
 */

/* 
 * This file defines the prototypes for the port management functions.
 * A header file with these prototypes can be formed by defining the macro
 *	NAME -- token to prepend to routine names
 * and then including this file.
 */


#ifdef __STDC__
#define XPASTE(X,Y) X##Y
#define PASTE(X,Y) XPASTE(X,Y)

/* 
 * Initializes the port map.  Mallocs port state structure.
 *  Must be called before any other routines. 
 */
void	PASTE(NAME, PortMapInit) ( void * );

/* 
 * Closes the port map, frees the port state structure
 */
void	PASTE(NAME, PortMapClose) ( void * );

/* 
 * Attempts to get a free port >= FIRST_USER_PORT, placing the
 * new port in *port.  Returns 0 if successful, non-zero if not.
 */
int	PASTE(NAME, GetFreePort) ( void *, long * );

/* 
 * Increases the reference count of the port.  The port does not have
 * to have been previously acquired.
 */
int	PASTE(NAME, DuplicatePort) ( void *, long );

/* 
 * Decreases the reference count on a port previously acquired through
 * DuplicatePort() or GetFreePort().
 */
void	PASTE(NAME, ReleasePort) ( void *, long );

#endif
