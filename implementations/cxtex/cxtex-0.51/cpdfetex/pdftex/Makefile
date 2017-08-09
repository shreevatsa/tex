
include ../../make/kpathsea.mk
include ../../make/generic.mk

LIBXPDFDIR =../../libs/xpdf
LIBZLIBDIR =../../libs/zlib
LIBPNGDIR = ../../libs/libpng 

CFLAGS = -O2
# CFLAGS = -ggdb -pg

DEFS = -DHAVE_CONFIG_H -DHAVE_PROTOTYPES

INCLUDES =  -I. -I../$(kpathsea_parent) -I$(LIBZLIBDIR) -I$(LIBPNGDIR)
CXXINCLUDES = $(INCLUDES) -I$(LIBXPDFDIR) -I$(LIBXPDFDIR)/xpdf -I$(LIBXPDFDIR)/goo

objects = epdf.o mapfile.o papersiz.o utils.o config.o vfpacket.o pkin.o \
writefont.o writet1.o writet3.o writezip.o writeenc.o writettf.o \
writejpg.o writepng.o writeimg.o md5.o

sources =  $(objects:.o=.c)
 
OBJS = $(objects) pdftoepdf.o

.SUFFIXES: .cc .c .o
.c.o: 
	$(CC)  $(CFLAGS) $(DEFS) -DpdfTeX $(INCLUDES) -c $< -o $@
.cc.o:
	$(CXX) $(CFLAGS) $(DEFS) $(CXXINCLUDES) -c $<  -o $@

libpdf.a: $(OBJS) depend
	rm -f $@
	$(AR) $(ARFLAGS) $@  $(OBJS)
	$(RANLIB) $@

depend:
	rm -f depend
	$(CC) $(DEFS) -DpdfTeX $(CXXINCLUDES) -MM >> depend $(sources)
	$(CC) $(DEFS) $(CXXINCLUDES) -MM >> depend pdftoepdf.cc
 

include depend

clean:
	rm -f depend *.o libpdf.a *~

