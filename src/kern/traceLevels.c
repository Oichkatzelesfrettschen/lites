/*
 * trace variable initialization
 *
 * Warning: this file is generated from graph.comp and is overwritten
 * every time 'make compose' is run
 */

#include "xk_debug.h"
#include "compose.h"
#include "protocols.h"


void
initTraceLevels()
{
#ifdef XK_DEBUG

    traceipp = TR_GROSS_EVENTS;
    traceicmpp = TR_MORE_EVENTS;
    tracetcpp = TR_GROSS_EVENTS;
    traceudpp = TR_EVENTS;
    tracexksocketp = TR_GROSS_EVENTS;
    traceudptestp = TR_GROSS_EVENTS;

#endif XK_DEBUG
}
