s%@prefix@%/usr/local%g
s%@exec_prefix@%/usr/local%g
s%@bindir@%/usr/local/bin%g
s%@scriptdir@%/usr/local/bin%g
s%@libdir@%/usr/local/lib%g
s%@datadir@%/usr/local/share%g
s%@infodir@%/usr/local/info%g
s%@includedir@%/usr/local/include%g
s%@mandir@%/usr/local/man%g
s%@manext@%1%g
s%@man1dir@%/usr/local/man/man1%g
s%@texmf@%NONE/share/texmf%g
s%@web2cdir@%NONE/share/texmf/web2c%g
s%@vartexfonts@%/var/tmp/texfonts%g
s%@texinputdir@%NONE/share/texmf/tex%g
s%@mfinputdir@%NONE/share/texmf/metafont%g
s%@mpinputdir@%NONE/share/texmf/metapost%g
s%@fontdir@%NONE/share/texmf/fonts%g
s%@fmtdir@%NONE/share/texmf/web2c%g
s%@basedir@%NONE/share/texmf/web2c%g
s%@memdir@%NONE/share/texmf/web2c%g
s%@texpooldir@%NONE/share/texmf/web2c%g
s%@mfpooldir@%NONE/share/texmf/web2c%g
s%@mppooldir@%NONE/share/texmf/web2c%g
s%@dvips_plain_macrodir@%NONE/share/texmf/tex/plain/dvips%g
s%@dvilj_latex2e_macrodir@%NONE/share/texmf/tex/latex/dvilj%g
s%@dvipsdir@%NONE/share/texmf/dvips%g
s%@psheaderdir@%NONE/share/texmf/dvips%g
s%@default_texsizes@%300:600%g
s%NONE/share/texmf%\$TEXMF%g
/^ *TEXMFMAIN[ =]/s%\$TEXMF%NONE/share/texmf%
/^[% ]*TEXMFLOCAL[ =]/s%\$TEXMF%NONE/share/texmf%
/^[% ]*VARTEXMF[ =]/s%\$TEXMF%NONE/share/texmf%
/^[% ]*TEXMFCNF[ =]/s%\$TEXMF%NONE/share/texmf%
/^ *TEXMFCNF[ =]/s%@web2c@%NONE/share/texmf/web2c%
