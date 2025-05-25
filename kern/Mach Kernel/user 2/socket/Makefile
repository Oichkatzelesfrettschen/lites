#
# $RCSfile: Makefile,v $
#
# Copyright (c) 1993,1991,1990  Arizona Board of Regents
#
# $Revision: 1.12 $
# $Date: 1993/11/16 20:41:06 $


XSIDIR	= ../../api/socket
TMP_INCLUDES	= -I$(XSIDIR)/gen -I$(XSIDIR)
override LIB	= libxksocket_$(HOW).a


TMP_CPPFLAGS = -D_PATH_SERVICES='"etc/services"'


FILES	= accept bind close connect listen socket \
	  getpeername getsockname getsockopt setsockopt select \
	  ioctl fcntl dup dup2 fork vfork read readv \
	  write writev recv recvfrom recvmsg send \
	  sendto sendmsg xsi_main getservbyname \
	  getservbyport getservent


all: 
	cd syscalls ; make	
	make $(LIB)
	cd tst ; make

% :: RCS/%,v

./$(HOW)/xsi_user.o: $(XSIDIR)/gen/xsi_user.c
	$(CC) $(CFLAGS) -o $@ -c $^



.PHONY:	TFM


include $(MAKE_INCLUDE)/Makefile.common

OBJ += ./$(HOW)/xsi_user.o

INCLUDES := $(TMP_INCLUDES)
WARNINGS :=

$(LIB): $(OBJ) syscalls/sc_*.o
	ar ruv $@ $(OBJ) syscalls/sc_*.o
	ranlib $@




