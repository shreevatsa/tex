
include ../make/kpathsea.mk
include ../make/generic.mk

DEBUGFLAGS = -O2 -g
#DEBUGFLAGS = -ggdb -pg

CFLAGS = $(DEBUGFLAGS) -Wall $(XCFLAGS)
LDFLAGS =  $(DEBUGFLAGS)  $(XLDFLAGS)
CXX_LINK = $(CXX) -o $@ $(LDFLAGS)
#LIBS = -nodefaultlibs -Wl,-Bstatic -lstdc++ -Wl,-Bdynamic -lm -lgcc_eh -lgcc -lc -lglib-2.0
LIBS =  
LOADLIBES = $(LIBS) $(XLOADLIBES)
LIBDIR = ../libs

LIBXPDF = $(LIBDIR)/xpdf/xpdf/libxpdf.a $(LIBDIR)/xpdf/goo/libGoo.a
LIBPNG  = $(LIBDIR)/libpng/libpng.a
LIBZLIB = $(LIBDIR)/zlib/libz.a

sources = adjust.c align.c box.c buildpage.c calc.c call.c cmdchr.c control.c disc.c dvi.c dump.c\
          tex_error.c eqtb.c etex.c exten.c font.c glue.c help.c hash.c hyphenate.c if.c insert.c tex_io.c\
          kern.c lig.c linebreak.c postlinebreak.c mag.c main.c math.c mathbuild.c mark.c mem.c mltex.c nest.c \
          nodelist.c pack.c par.c pdflowlevel.c pdfpag.c pdfbasic.c pdffont.c pdfout.c pdfproc.c pdfxref.c\
          penalty.c prefix.c print.c rule.c sa.c save.c scan.c show.c snap.c string.c tex.c \
	      tfm.c thread.c tokenlist.c tokens.c trie.c vf.c vsplit.c xet.c xordchr.c lib.c

objects = $(sources:.c=.o)

cpdfetex: $(headers) $(objects)
	cd pdftex           && $(MAKE)
	cd $(LIBDIR)/xpdf   && $(MAKE)
	cd $(LIBDIR)/zlib   && $(MAKE)
	cd $(LIBDIR)/libpng && $(MAKE)
	$(CXX_LINK) $(objects) pdftex/libpdf.a  $(kpathsea) $(LIBXPDF) $(LIBPNG) $(LIBZLIB) $(LOADLIBES)

$(objects): %.o: %.c
	$(CC) -c $(CFLAGS) -I$(kpathsea_parent)  $< -o $@

depend:
	rm -f depend
	$(CC) -I$(kpathsea_parent) -MM >> depend $(sources)

include depend

clean:
	cd pdftex           && $(MAKE) clean
	cd $(LIBDIR)/xpdf   && $(MAKE) clean
	cd $(LIBDIR)/zlib   && $(MAKE) clean
	cd $(LIBDIR)/libpng && $(MAKE) clean
	rm -f depend *.o cpdfetex cpdfetex.exe *~