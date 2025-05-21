#
# Utah makefile, more BSD like
#
# $Hdr: makefile 1.1 95/06/25$
#
DESTDIR=
MAKE=gmake
LITES_SRC=/usr/src/lites
LITES_OBJ=/usr/obj/lites
CONFIG=STD+WS+ACT

all .DEFAULT: $(LITES_OBJ)/config.status
	cd $(LITES_OBJ); $(MAKE) $@

#
# platform-specific configuration
#
$(LITES_OBJ)/config.status: $(LITES_OBJ)/.mkdirstamp
	cd $(LITES_OBJ); $(LITES_SRC)/conf/configure --srcdir=$(LITES_SRC) --host=$(MACHINE) --build=$(MACHINE) --with-config=$(CONFIG) --enable-mach4 --with-release=/usr/mach4

$(LITES_OBJ)/.mkdirstamp:
	-@mkdir $(LITES_OBJ)
	-@touch $(LITES_OBJ)/.mkdirstamp
