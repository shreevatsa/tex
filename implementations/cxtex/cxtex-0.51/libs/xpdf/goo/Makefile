# Generated automatically from Makefile.in by configure.
#========================================================================
#
# Goo library Makefile
#
# Copyright 1996 Derek B. Noonburg
#
#========================================================================

srcdir = .

kpathsea_include_dir = -I$(srcdir)/../../../texk -I../..
ALLCFLAGS = $(CFLAGS) -DHAVE_CONFIG_H -I.. -I$(srcdir) -I. $(kpathsea_include_dir)
ALLCXXFLAGS = $(CXXFLAGS) -DHAVE_CONFIG_H -I.. -I$(srcdir) -I. $(kpathsea_include_dir)
CXXFLAGS = -g -O2
CC = gcc
CXX = c++
CFLAGS = -g -O2

# library.mk -- stuff only useful for libraries.
AR = ar
ARFLAGS = rc
RANLIB = ranlib
# End of library.mk.


#------------------------------------------------------------------------

.SUFFIXES:
.SUFFIXES: .cc .c .o

.cc.o:
	$(CXX) $(ALLCXXFLAGS) -c $<
.c.o:
	$(CC) $(ALLCFLAGS) -c $<

#------------------------------------------------------------------------

CXX_SRC = \
	$(srcdir)/GHash.cc \
	$(srcdir)/GList.cc \
	$(srcdir)/GString.cc \
	$(srcdir)/gmempp.cc \
	$(srcdir)/gfile.cc

C_SRC = \
	$(srcdir)/gmem.c \
	$(srcdir)/parseargs.c

#------------------------------------------------------------------------

GOO_CXX_OBJS = GHash.o GList.o GString.o gmempp.o gfile.o
GOO_C_OBJS = gmem.o parseargs.o
GOO_OBJS = $(GOO_CXX_OBJS) $(GOO_C_OBJS)

libGoo.a: $(GOO_OBJS)
	rm -f $@
	$(AR) $(ARFLAGS) $@ $(GOO_OBJS)
	$(RANLIB) $@

#------------------------------------------------------------------------

mostlyclean clean:
	rm -f $(GOO_OBJS) libGoo.a
	rm -f Makefile.bak

distclean maintainer-clean extraclean: clean
	rm -f Makefile

#------------------------------------------------------------------------

depend:
	$(CXX) $(CXXFLAGS) -MM $(CXX_SRC) >Makefile.dep
	$(CC) $(CFLAGS) -MM $(C_SRC) >>Makefile.dep
	cat Makefile.dep | sed 's|$(srcdir)|$$(srcdir)|' > $(srcdir)/depend.mk
	cd ..; config.status

