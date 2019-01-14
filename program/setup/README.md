To get SVGs of a WEB program with each section (module) in a separate one.

1. Run:

        weave foo.web
  
2. Replace the "\input webmac" at the top of `foo.tex` with "\input mywebmac".

3. Run

        pdftex --output-format=dvi foo.tex

   (see `mywebmac.tex` for changes needed to use `luatex` instead, if you prefer).

4. Loop with [dvisvgm](https://dvisvgm.de/Manpage/):

        dvisvgm --page=1- --font-format=woff2,autohint --bbox=10 foo.dvi
