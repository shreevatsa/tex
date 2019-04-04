all: svgs

mywebmac.tex:
	cp ../setup/mywebmac.tex .

weave.web:
	cp ~/dek/dist/web/weave.web .

weave.tex: weave.web mywebmac.tex
	weave weave

weave.dvi: weave.tex
	pdftex --output-format=dvi weave.tex
	dvisvgm --page=110 --font-format=woff2,autohint --bbox=10 -o weave-contents weave.dvi
	sed -i .bak -e "s/\input webmac/\input mywebmac/" weave.tex
	pdftex --output-format=dvi weave.tex

svgs: weave.dvi
	dvisvgm --page=1- --font-format=woff2,autohint --bbox=10 weave.dvi
	rm weave-267.svg

cleanup:
	rm -f weave.web weave.tex weave.tex.bak CONTENTS.tex weave.dvi weave.log weave.pdf 

clean: cleanup
	rm -f weave-???.svg weave-contents.svg
