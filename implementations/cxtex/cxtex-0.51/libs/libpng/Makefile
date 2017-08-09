# Generated automatically from Makefile.in by configure.
# Makefile for libpng - te@dbs.uni-hannover.de

# This file (together with libpng.ac withenable.ac) is
# copyright Thomas Esser, 2002 and hereby put into the
# public domain.

# For conditions of distribution and use of the other files in this
# directory, see copyright notice in png.h.

# common.mk -- used by all Makefiles.
SHELL = /bin/sh

top_srcdir = ..
srcdir = .

CC = gcc
CFLAGS = -g -O2 $(XCFLAGS)
CPPFLAGS =  $(XCPPFLAGS)
DEFS =  -DHAVE_ALLOCA_H=1 -DHAVE_ALLOCA=1  $(XDEFS)
LDFLAGS =  $(XLDFLAGS)

# Kpathsea needs this for compiling, programs need it for linking.
LIBTOOL = $(SHELL) $(kpathsea_parent)/libtool

# You can change [X]CPPFLAGS, [X]CFLAGS, or [X]DEFS, but
# please don't change ALL_CPPFLAGS or ALL_CFLAGS.
# prog_cflags is set by subdirectories of web2c.
ALL_CPPFLAGS = $(DEFS) -I. -I$(srcdir) -I$(kpathsea_parent) \
  -I$(kpathsea_srcdir_parent) $(prog_cflags) $(CPPFLAGS)
ALL_CFLAGS = $(ALL_CPPFLAGS) $(CFLAGS)
compile = $(CC) $(ALL_CFLAGS)

.SUFFIXES:
.SUFFIXES: .c .o .lo # in case the suffix list has been cleared, e.g., by web2c
.c.o:
	$(compile) -c $< -o $@

# Installation.
INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_SCRIPT = ${INSTALL_PROGRAM}
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_LIBTOOL_LIBS = $(LIBTOOL) --mode=install $(INSTALL_DATA)
INSTALL_LIBTOOL_PROG = $(LIBTOOL) --mode=install $(INSTALL_PROGRAM)

# Creating (symbolic) links.
LN = ln -s

# We use these for many things.
kpathsea_parent = ..
kpathsea_dir = $(kpathsea_parent)/kpathsea
kpathsea_srcdir_parent = $(top_srcdir)/..
kpathsea_srcdir = $(kpathsea_srcdir_parent)/kpathsea
kpathsea = $(kpathsea_dir)/libkpathsea.la

#M#ifeq ($(CC), gcc)
#M#XDEFS = -Wpointer-arith $(warn_more)
#M#CFLAGS = -g $(XCFLAGS)
#M#endif
# End of common.mk.
# programs.mk -- used by Makefiles for executables only.

# Don't include $(CFLAGS), since ld -g under Linux forces
# static libraries, e.g., libc.a and libX*.a.
LDFLAGS =  $(XLDFLAGS)

# proglib is for web2c; 
# XLOADLIBES is for the installer.
LIBS = 
LOADLIBES = $(proglib) $(kpathsea) $(LIBS) $(XLOADLIBES)

# May as well separate linking from compiling, just in case.
CCLD = $(CC)
link_command = $(CCLD) -o $@ $(LDFLAGS) 

# When we link with Kpathsea, have to take account that it might be a
# shared library, etc.
kpathsea_link = $(LIBTOOL) --mode=link $(link_command)
# End of programs.mk.
# library.mk -- stuff only useful for libraries.
AR = ar
ARFLAGS = rc
RANLIB = ranlib
# End of library.mk.

ZLIBDIR=../zlib
ZLIBSRCDIR=$(srcdir)/$(ZLIBDIR)
XDEFS=-I$(ZLIBDIR) -I$(ZLIBSRCDIR)

OBJS = png.o pngset.o pngget.o pngrutil.o pngtrans.o pngwutil.o \
        pngread.o pngrio.o pngwio.o pngwrite.o pngrtran.o \
        pngwtran.o pngmem.o pngerror.o pngpread.o

all: libpng.a

libpng.a: $(OBJS)
	rm -f $@
	$(AR) $(ARFLAGS) $@ $(OBJS)
	$(RANLIB) $@

mostlyclean clean:
	rm -f *.o libpng.a pngtest pngout.png

distclean realclean extraclean maintainer-clean: clean
	rm -f config.status config.log Makefile

# DO NOT DELETE THIS LINE -- make depend depends on it.

example.o: example.c
png.o: png.c png.h pngconf.h
pngerror.o: pngerror.c png.h pngconf.h
pnggccrd.o: pnggccrd.c png.h pngconf.h
pngget.o: pngget.c png.h pngconf.h
pngmem.o: pngmem.c png.h pngconf.h
pngpread.o: pngpread.c png.h pngconf.h
pngread.o: pngread.c png.h pngconf.h
pngrio.o: pngrio.c png.h pngconf.h
pngrtran.o: pngrtran.c png.h pngconf.h
pngrutil.o: pngrutil.c png.h pngconf.h
pngset.o: pngset.c png.h pngconf.h
pngtest.o: pngtest.c png.h pngconf.h
pngtrans.o: pngtrans.c png.h pngconf.h
pngvcrd.o: pngvcrd.c png.h pngconf.h
pngwio.o: pngwio.c png.h pngconf.h
pngwrite.o: pngwrite.c png.h pngconf.h
pngwtran.o: pngwtran.c png.h pngconf.h
pngwutil.o: pngwutil.c png.h pngconf.h
