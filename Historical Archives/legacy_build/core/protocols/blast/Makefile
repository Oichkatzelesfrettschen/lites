# 
# $RCSfile: Makefile,v $
#
# x-kernel v3.2
#
# Copyright (c) 1993,1991,1990  Arizona Board of Regents
#
# $Revision: 1.11 $
# $Date: 1993/11/11 15:35:13 $
#

include Files

#
# Configure BLAST to send nacks when last fragment doesn't complete
# the datagram on all platforms except Mach
#
ifneq ($(XPLATFORM), mach3)

TMP_CPPFLAGS := -DBLAST_LAST_FRAG_NACKS

endif

include $(MAKE_INCLUDE)/Makefile.common
