/*
 * ptbldump.c
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.10 $
 * $Date: 1993/11/15 01:04:58 $
 */

/* 
 * Builds a protocol table with files listed on the command line and
 * then dumps C code which can rebuild the table.
 */

#include "x_stdio.h"
#include "xtype.h"
#include "idmap.h"
#include "prottbl.h"
#include "prottbl_i.h"
#include "romopt.h"

/* 
 * These are some bogus definitions to allow us to only link into one
 * of the x-kernel libraries
 */
void Kabort() {};
struct xobj *protl_tab[1];
void 	xFree( a ) char *a; {}
void 	xTraceLock() {}
void 	xTraceUnlock() {}
xkern_return_t findRomOpts(char *str, RomOpt *opt, int numOpts, VOID *arg)
{    
}

char *
char *xMalloc(unsigned n)
{
    return malloc(n);
}

char *
protocolTables[1] = {
    0
};


#define DECL 0
#define MAPS 1

char 	*names[2] = { "/tmp/xkDeclsXXXXXX", "/tmp/xkMapsXXXXXX" };

static FILE	*declFile, *mapFile;



static int
static int addMapPair(VOID *key, int value, VOID *arg)
{
    long	hlpId = *(long *)key;
    char	*hlpName;
    
    if ( (hlpName = protIdToStr(hlpId)) == 0 ) {
	fprintf(stderr, "ptbldump: protocol table inconsistency!  Exiting\n");
	exit(1);
    }
    fprintf(mapFile, "\t{ \"%s\", %d },\n", hlpName, value);
    return 1;
}


static int doEntry(VOID *key, int value, VOID *arg)
{
    PtblEntry	*e = (PtblEntry *)value;

    /* 
     * decl file entry
     */
    fprintf(declFile, "\t{ \"%s\", %d, ", e->name, e->id);
    if ( e->idMap ) {
	fprintf(declFile, "%sMap",  e->name);
    } else {
	fprintf(declFile, "0");
    }
    fprintf(declFile, " },\n");
    /* 
     * map file entry
     */
    if ( e->idMap ) {
	fprintf(mapFile, "static MapEntry %sMap[] = { \n", e->name);
	mapForEach(e->idMap, addMapPair, 0);
	fprintf(mapFile, "\t{ 0, 0 }\n");
	fprintf(mapFile, "};\n\n");
    }
    return 1;
}


static void
declPre()
{
    fprintf(declFile, "static Entry\tentries[] = {\n");
}

static void
declPost()
{
    fprintf(declFile, "\t{ 0, 0, 0 }\n");
    fprintf(declFile, "};\n");
}


static void
dumpTables()
{

    if ( mktemp(names[DECL]) == 0 ) {
	perror("ptbldump could not get tmp name");
	exit(1);
    }
    if ( mktemp(names[MAPS]) == 0 ) {
	perror("ptbldump could not get tmp name");
	exit(1);
    }
    if ( (declFile = fopen(names[DECL], "w")) == 0 ) {
	perror("ptbldump could not open temp file");
	exit(1);
    }
    if ( (mapFile = fopen(names[MAPS], "w")) == 0 ) {
	perror("ptbldump could not open temp file");
	exit(1);
    }
    declPre();
    mapForEach(ptblNameMap, doEntry, 0);
    declPost();
    fclose(mapFile);
    fclose(declFile);
    if ( fork() == 0 ) {
	execl("/bin/cat", "/bin/cat", names[MAPS], names[DECL], 0);
    }
    wait(0);
    unlink(names[MAPS]);
    unlink(names[DECL]);
}


int
int main(int argc, char **argv)
{
    int	i;
    
    prottbl_init();
    if ( argc > 1 ) {
	for ( i=1; i < argc; i++ ) {
	    if ( protTblBuild(argv[i])) {
		fprintf(stderr, "ptbldump -- error building protocol table\n");
		exit(1);
	    }
	}
	dumpTables();
    } else {
	fprintf(stderr, "usage: ptbldump ptbl1 [ ptbl2 ... ]\n");
    }
    return 0;
}
