
VERSION=0.51

.PHONY: all clean

all:
	cd texk/kpathsea && $(MAKE)
	cd cpdfetex && $(MAKE)         && cp cpdfetex.exe cpdfetex ../built 
	cd texutil  && $(MAKE) texutil && cp texutil.exe  texutil  ../built
	cd texutil  && $(MAKE) texexec && cp texexec.exe  texexec  ../built

clean:
	cd texk/kpathsea && $(MAKE) clean
	cd cpdfetex      && $(MAKE) clean
	cd texutil       && $(MAKE) clean

dist: clean
	ln -s ../current cxtex-$(VERSION)
	cd built && strip *.exe && zip -m ../cxtex-win32-bin-$(VERSION).zip *.exe
	tar cvzf cxtex-source-$(VERSION).tar.gz --exclude RCS cxtex-$(VERSION)/built cxtex-$(VERSION)/cpdfetex cxtex-$(VERSION)/libs cxtex-$(VERSION)/make cxtex-$(VERSION)/texk cxtex-$(VERSION)/texutil cxtex-$(VERSION)/Makefile
	rm cxtex-$(VERSION)
