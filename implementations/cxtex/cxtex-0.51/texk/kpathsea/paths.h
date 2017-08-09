/* paths.h: Generated from texmf.cnf. */
#ifndef DEFAULT_TEXMFMAIN
#define DEFAULT_TEXMFMAIN "NONE/share/texmf"
#endif
#ifndef DEFAULT_TEXMFLOCAL
#define DEFAULT_TEXMFLOCAL "NONE/share/texmf-local"
#endif
#ifndef DEFAULT_HOMETEXMF
#define DEFAULT_HOMETEXMF "$HOME/texmf"
#endif
#ifndef DEFAULT_VARTEXMF
#define DEFAULT_VARTEXMF "NONE/share/texmf-var"
#endif
#ifndef DEFAULT_TEXMF
#define DEFAULT_TEXMF "{$HOMETEXMF,!!$VARTEXMF,NONE/share/texmfLOCAL,!!NONE/share/texmf}"
#endif
#ifndef DEFAULT_SYSTEXMF
#define DEFAULT_SYSTEXMF "NONE/share/texmf"
#endif
#ifndef DEFAULT_TEMP
#define DEFAULT_TEMP "/var/tmp"
#endif
#ifndef DEFAULT_VARTEXFONTS
#define DEFAULT_VARTEXFONTS "/var/tmp/texfonts"
#endif
#ifndef DEFAULT_TEXMFDBS
#define DEFAULT_TEXMFDBS "NONE/share/texmf"
#endif
#ifndef DEFAULT_WEB2C
#define DEFAULT_WEB2C "NONE/share/texmf/web2c"
#endif
#ifndef DEFAULT_TEXINPUTS
#define DEFAULT_TEXINPUTS ".:NONE/share/texmf/tex/{generic,}//"
#endif
#ifndef DEFAULT_MFINPUTS
#define DEFAULT_MFINPUTS ".:NONE/share/texmf/metafont//:{NONE/share/texmf/fonts,/var/tmp/texfonts}/source//"
#endif
#ifndef DEFAULT_MPINPUTS
#define DEFAULT_MPINPUTS ".:NONE/share/texmf/metapost//"
#endif
#ifndef DEFAULT_MFTINPUTS
#define DEFAULT_MFTINPUTS ".:NONE/share/texmf/mft//"
#endif
#ifndef DEFAULT_WEBINPUTS
#define DEFAULT_WEBINPUTS ".:NONE/share/texmf/web//"
#endif
#ifndef DEFAULT_CWEBINPUTS
#define DEFAULT_CWEBINPUTS ".:NONE/share/texmf/cweb//"
#endif
#ifndef DEFAULT_TEXFORMATS
#define DEFAULT_TEXFORMATS ".:NONE/share/texmf/web2c"
#endif
#ifndef DEFAULT_MFBASES
#define DEFAULT_MFBASES "$TEXFORMATS"
#endif
#ifndef DEFAULT_MPMEMS
#define DEFAULT_MPMEMS "$TEXFORMATS"
#endif
#ifndef DEFAULT_TEXPOOL
#define DEFAULT_TEXPOOL "$TEXFORMATS"
#endif
#ifndef DEFAULT_MFPOOL
#define DEFAULT_MFPOOL "$TEXFORMATS"
#endif
#ifndef DEFAULT_MPPOOL
#define DEFAULT_MPPOOL "$TEXFORMATS"
#endif
#ifndef DEFAULT_VFFONTS
#define DEFAULT_VFFONTS ".:NONE/share/texmf/fonts/vf//"
#endif
#ifndef DEFAULT_TFMFONTS
#define DEFAULT_TFMFONTS ".:{NONE/share/texmf/fonts,/var/tmp/texfonts}/tfm//"
#endif
#ifndef DEFAULT_PKFONTS
#define DEFAULT_PKFONTS ".:{NONE/share/texmf/fonts,/var/tmp/texfonts}/pk/{$MAKETEX_MODE,modeless}//"
#endif
#ifndef DEFAULT_GFFONTS
#define DEFAULT_GFFONTS ".:NONE/share/texmf/fonts/gf/$MAKETEX_MODE//"
#endif
#ifndef DEFAULT_GLYPHFONTS
#define DEFAULT_GLYPHFONTS ".:NONE/share/texmf/fonts"
#endif
#ifndef DEFAULT_TEXFONTMAPS
#define DEFAULT_TEXFONTMAPS ".:NONE/share/texmf/fontname"
#endif
#ifndef DEFAULT_BIBINPUTS
#define DEFAULT_BIBINPUTS ".:NONE/share/texmf/bibtex/{bib,}//"
#endif
#ifndef DEFAULT_BSTINPUTS
#define DEFAULT_BSTINPUTS ".:NONE/share/texmf/bibtex/{bst,}//"
#endif
#ifndef DEFAULT_TEXPSHEADERS
#define DEFAULT_TEXPSHEADERS ".:NONE/share/texmf/{{dvips,fonts/{map,enc}/{dvips,}},pdftex,tex,fonts/type1}//"
#endif
#ifndef DEFAULT_T1FONTS
#define DEFAULT_T1FONTS ".:NONE/share/texmf/fonts/{type1,pfb}//:NONE/share/texmf/fonts/misc/hbf//"
#endif
#ifndef DEFAULT_AFMFONTS
#define DEFAULT_AFMFONTS ".:NONE/share/texmf/fonts/afm//"
#endif
#ifndef DEFAULT_TTFONTS
#define DEFAULT_TTFONTS ".:NONE/share/texmf/fonts/{truetype,ttf}//"
#endif
#ifndef DEFAULT_TTF2TFMINPUTS
#define DEFAULT_TTF2TFMINPUTS ".:NONE/share/texmf/ttf2pk//"
#endif
#ifndef DEFAULT_T42FONTS
#define DEFAULT_T42FONTS ".:NONE/share/texmf/fonts/type42//"
#endif
#ifndef DEFAULT_MISCFONTS
#define DEFAULT_MISCFONTS ".:NONE/share/texmf/fonts/misc//"
#endif
#ifndef DEFAULT_TEXCONFIG
#define DEFAULT_TEXCONFIG "NONE/share/texmf/{dvips,fonts/{map,enc}/{dvips,}}//"
#endif
#ifndef DEFAULT_INDEXSTYLE
#define DEFAULT_INDEXSTYLE ".:NONE/share/texmf/makeindex//:NONE/share/texmf/tex//"
#endif
#ifndef DEFAULT_TRFONTS
#define DEFAULT_TRFONTS "/usr/lib/font/devpost"
#endif
#ifndef DEFAULT_MPSUPPORT
#define DEFAULT_MPSUPPORT ".:NONE/share/texmf/metapost/support"
#endif
#ifndef DEFAULT_MIMELIBDIR
#define DEFAULT_MIMELIBDIR "/usr/local/etc"
#endif
#ifndef DEFAULT_MAILCAPLIBDIR
#define DEFAULT_MAILCAPLIBDIR "/usr/local/etc"
#endif
#ifndef DEFAULT_TEXDOCS
#define DEFAULT_TEXDOCS ".:NONE/share/texmf/doc//"
#endif
#ifndef DEFAULT_TEXSOURCES
#define DEFAULT_TEXSOURCES ".:NONE/share/texmf/source//"
#endif
#ifndef DEFAULT_TEXDOCSSUFFIX
#define DEFAULT_TEXDOCSSUFFIX ":.pdf:.ps:.dvi:.html:.txt:.tex"
#endif
#ifndef DEFAULT_TEXDOCSCOMPRESS
#define DEFAULT_TEXDOCSCOMPRESS ":.gz:.bz2:.zip:.Z"
#endif
#ifndef DEFAULT_TEXDOCEXT
#define DEFAULT_TEXDOCEXT "{$TEXDOCSSUFFIX}{$TEXDOCSCOMPRESS}"
#endif
#ifndef DEFAULT_OFMFONTS
#define DEFAULT_OFMFONTS ".:{NONE/share/texmf/fonts,/var/tmp/texfonts}/{ofm,tfm}//:$TFMFONTS"
#endif
#ifndef DEFAULT_OPLFONTS
#define DEFAULT_OPLFONTS ".:{NONE/share/texmf/fonts,/var/tmp/texfonts}/opl//"
#endif
#ifndef DEFAULT_OVFFONTS
#define DEFAULT_OVFFONTS ".:{NONE/share/texmf/fonts,/var/tmp/texfonts}/ovf//"
#endif
#ifndef DEFAULT_OVPFONTS
#define DEFAULT_OVPFONTS ".:{NONE/share/texmf/fonts,/var/tmp/texfonts}/ovp//"
#endif
#ifndef DEFAULT_OTPINPUTS
#define DEFAULT_OTPINPUTS ".:NONE/share/texmf/omega/otp//"
#endif
#ifndef DEFAULT_OCPINPUTS
#define DEFAULT_OCPINPUTS ".:NONE/share/texmf/omega/ocp//"
#endif
#ifndef DEFAULT_DVIPDFMINPUTS
#define DEFAULT_DVIPDFMINPUTS ".:NONE/share/texmf/{dvipdfm,{dvips,fonts/{map,enc}/{dvips,}}}//"
#endif
#ifndef DEFAULT_TEX4HTFONTSET
#define DEFAULT_TEX4HTFONTSET "alias,iso8859"
#endif
#ifndef DEFAULT_TEX4HTINPUTS
#define DEFAULT_TEX4HTINPUTS ".:NONE/share/texmf/tex4ht/base//:NONE/share/texmf/tex4ht/ht-fonts/{$TEX4HTFONTSET}//"
#endif
#ifndef DEFAULT_T4HTINPUTS
#define DEFAULT_T4HTINPUTS ".:NONE/share/texmf/tex4ht/base//"
#endif
#ifndef DEFAULT_XDVIINPUTS
#define DEFAULT_XDVIINPUTS ".:NONE/share/texmf/{xdvi,{dvips,fonts/{map,enc}/{dvips,}}}//"
#endif
#ifndef DEFAULT_KPSE_DOT
#define DEFAULT_KPSE_DOT "."
#endif
#ifndef DEFAULT_PERLINPUTS
#define DEFAULT_PERLINPUTS ".:NONE/share/texmf/{perl,context,}//"
#endif
#ifndef DEFAULT_PYTHONINPUTS
#define DEFAULT_PYTHONINPUTS ".:NONE/share/texmf/{python,}//"
#endif
#ifndef DEFAULT_RUBYINPUTS
#define DEFAULT_RUBYINPUTS ".:NONE/share/texmf/{ruby,context}//"
#endif
#ifndef DEFAULT_JAVAINPUTS
#define DEFAULT_JAVAINPUTS ".:NONE/share/texmf/{java,}//"
#endif
#ifndef DEFAULT_TEXMFCNF
#define DEFAULT_TEXMFCNF ".:$VARTEXMF/web2c:{$SELFAUTOLOC,$SELFAUTODIR,$SELFAUTOPARENT}{,{/share,}/texmf{.local,}/web2c}:NONE/share/texmf/web2c"
#endif
#ifndef DEFAULT_MISSFONT_LOG
#define DEFAULT_MISSFONT_LOG "missfont.log"
#endif
#ifndef DEFAULT_TEX_HUSH
#define DEFAULT_TEX_HUSH "none"
#endif
#ifndef DEFAULT_MPXCOMMAND
#define DEFAULT_MPXCOMMAND "makempx"
#endif
