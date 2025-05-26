#ifndef _DDEKIT_ATTRIBS_H
#define _DDEKIT_ATTRIBS_H

#ifdef __ACK__


#else

#define DDEKIT_USED        [[gnu::used]]
#define DDEKIT_CONSTRUCTOR [[gnu::constructor]]


#define DDEKIT_PUBLIC PUBLIC
#define DDEKIT_PRIVATE static 
#endif
#endif
