
# texutilk notes:
#
# You will need libkpathsea.a and the kpathsea C includes. 
#
# The easiest way to acquire these is to install either 
# the teTeX or TeXlive Web2C sources, then compile TeX 
# from these sources. That wil automatically compile
# kpathsea.a for you.
#
# Next find and then fix the two paths below. 
# Both paths will usually have the same prefix.
#
# Then run "make texutilk". 

include ../make/kpathsea.mk
include ../make/generic.mk

#KFLAGS = 
#LIBFLAGS = 
KFLAGS = -D USE_KPSE=1 -I$(kpathsea_parent)
LIBFLAGS = $(kpathsea)

CFLAGS = -Wall -W -g

sources = texutil.c perlemu.c funclib.c

objects = $(sources:%.c=%.o)

texutil: texutil_main.o texutil.a Makefile
	$(CC) $(CFLAGS) -o texutil texutil_main.o texutil.a $(LIBFLAGS)

texexec: texexec_main.o texexec.o texutil.a Makefile
	$(CC) $(CFLAGS)  -o texexec texexec_main.o texexec.o texutil.a $(LIBFLAGS) 

texutil.a:  $(objects) Makefile
	rm -f *~
	$(AR) rc texutil.a $(objects)
	$(RANLIB) texutil.a

texutil.exe: texutil_main.o texutil_32.a Makefile
	$(CC) $(CFLAGS) -o texutil texutil_main.o texutil_32.a glob/libglob.a  $(LIBFLAGS)

texexec.exe: texexec_main.o texexec.o texutil_32.a Makefile
	$(CC) $(CFLAGS)  -o texexec texexec_main.o texexec.o texutil_32.a glob/libglob.a  $(LIBFLAGS) 

texutil_32.a:  $(objects) Makefile glob/libglob.a
	rm -f *~
	cd glob && make
	$(AR) rc texutil_32.a $(objects)
	$(RANLIB) texutil_32.a


$(objects) texutil_main.o texexec.o texexec_main.o: %.o:%.c
	$(CC) $(CFLAGS) $(KFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f *.o texutil.a texutil_32.a texexec texutil texexec.exe texutil.exe 
