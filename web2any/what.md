# How I got here

## Wanting to show boxes and glue

- Encountered the ConTeXt manual ([2013](http://pmrb.free.fr/contextref.pdf), [2001](http://www.pragma-ade.com/general/manuals/cont-eni.pdf), [2000](http://www.ctex.org/documents/context/cont-enp.pdf)). Read its preface, saw Hans Hagen's strong recommendation of the book _A Beginner's Book of TeX_ by Seroul and Levy

- Read the book. Fell in love with the book, with TeX, and particularly with the boxes-and-glue (and penalties) model of typesetting.

- Also read the paper [Breaking Paragraphs into Lines](https://web.archive.org/web/20170318101040/http://www.eprg.org/G53DOC/pdfs/knuth-plass-breaking.pdf) by Knuth and Plass, which is also about the box-glue-penalty model. (Later looked for and read the shorter, later version _Choosing Better Line Breaks_ which calls it the box-glue-kerf model.)

- Wanted (and still want) to help more people understand and appreciate this elegant approach to typesetting: how complex layouts can be achieved with such simple primitives original designed for paragraphs.

- Watched [these videos](https://www.youtube.com/watch?v=jbrMBOF61e0&index=2&list=PL94E35692EB9D36F3) of Knuth (_TeX for Beginners_ (1981 February 23–27), _Advanced TeXarcana_ (1981 March 2–6), and _The Internal Details of TeX82_ (1982 July 28–30))

## Existing approaches

- I discovered the very cool `lua-visual-debug` by Patrick Gundlach aka topskip. ([Here](https://tex.stackexchange.com/questions/2083/how-can-i-visualize-boxes), see also [`layouts`](https://tex.stackexchange.com/questions/55618/visual-debugging-of-lengths-in-paragraphs-and-environments).) It's a great demonstration, but still hard to interpret and not very usable. (Also, try it with the plain-tex sample and you'll see extraneous output.)

- I left the following comments [there](https://tex.stackexchange.com/a/45388/48):

> This is a beautiful example of the power of LuaTeX. Thank you for this. I hope things like this can demystify TeX for a lot of people, and get them to appreciate its simple and powerful boxes-and-glue model of typesetting, hiding behind all the accumulated layers of LaTeX, package, and other macros. Perhaps the next step here would be something interactive: a log of all the boxes, glues, etc., where you can click or hover over an entry in the log to see its corresponding place in the output highlighted. (May require output to svg using dvisvgm, and different specials in place of pdfliteral.) – ShreevatsaR Jan 17 at 16:32

> After looking into this a bit more (especially at [nodetree](https://www.ctan.org/pkg/nodetree) and [dvisvgm](http://dvisvgm.bplaced.net/Manpage#specials)) I think it should be possible to implement: a html file would be generated, having on the left side an included svg for each page, and on the right side a nested list of everything (nodes, lists, boxes, glue), which is expandable/collapsable to control the amount of detail. As you hover on each element on the right, it will highlight a corresponding box on the left, using ids that were inserted into the svgs as specials. Would be wonderful. – ShreevatsaR Jan 19 at 23:45

The above is still something we could implement, I think: something to consider.

See https://www.ctan.org/pkg/nodetree?lang=en https://github.com/Josef-Friedrich/nodetree https://gist.github.com/pgundlach/556247

Also see https://www.overleaf.com/blog/512-pandoras-slash-hbox-using-luatex-to-lift-the-lid-of-tex-boxes  and https://www.overleaf.com/blog/513-how-tex-calculates-glue-settings-in-an-slash-hbox

## Wanting to show macro expansion

Meanwhile, a separate problem remains how to show the path from what the user typed into their `.tex` file, to the actual boxes and glue. Behind the layers of LaTeX (and other) macros, what's really going on?

- Bruno Le Floch [has written `unravel`](https://tex.stackexchange.com/questions/61010/a-latex-log-analyzer-application-visualizing-tex-expansion). It effectively implements much of TeX, within TeX macros, and not very perfectly. (One of the first things I tried is `\unravel` around an `\item` within an `enumerate` environment, and it didn't work fully.) Besides, a text-based UI isn't great, as you can't go back-and-forth, skip ahead to “visually selected” points, etc.

TeX's macro facility is IMO the least interesting (and most perverse) part of TeX. The entire implementation of LaTeX as TeX macros is IMO a mistake. The sooner we can get away from it the better. So I have interest in seeing input suitable for a “macroless TeX”, or “toothless TeX” (so I'll call it “pre-chewed TeX input”).

So the idea is to show:

user's .tex file (TeX/LaTeX, whatever) → macroless TeX → list of boxes and glue → rendered output (with dvisvgm)

And we can go back and forth ("reverse debugging"). (See also PythonTutor.com.)

At the end of the "Literate Programming" article, Knuth hints at this:

> there will be many ways to incorporate the WEB philosophy into a really effective programming environment. For example, it will be worthwhile to produce a unified system that does both tangling and compiling, instead of using separate programs as in Figure 1; and it will also be worthwhile to carry the unification one step further, so that run-time debugging as well as syntactic debugging can be done entirely in terms of the WEB source language. Furthermore, a WEB-like system could be designed to incorporate additional modularization, so that it would be easier to compile different parts of a program independently. The new generation of graphic workstations makes it desirable to display selected program sections on demand, [...] And so on [...]

## Wanting to make TeX less monolithic

One of the ways in which TeX is different from how a typical programmer of the modern era might write an equivalent system, is that it is entirely monolithic: a single program that works on an input file and produces a fully typeset DVI file (and in the original version even a file in the format of your specific printer, though DRF was able to persuade DEK to break it at least here (see his TUG interview)). (In fact all the programs of DEK are monolithic like this; it seems to be just the way he thinks.)

I think it would be useful (or at least extremely illuminating) to remodel TeX as a sequence of programs:

- The "eyes and mouth", which take the user's input files and turn it into "commands" for the "stomach",
- The "stomach" which takes instructions (paragraphs, font-changing commands etc., but no macros) and turns them into vertical and horizontal lists of boxes and glue (nested) -- the actual task of typesetting.
- The (um...) which takes this typesetting choices (a schema for the output) and produces the actual output, in DVI or PDF format

In practice these are all interleaved and talk to each other, but being able to see the actions of each of them (correlated with positions in the input file, tracked all the way to the output) would be educational if nothing else.

And if it works out well, different people could write different programs for individual parts, tweak them separately, etc.

## Wanting to read the TeX (and some other) programs more easily

As I read the programs out of a printout, and made annotations with a blue pen, flipping around pages by looking at the index etc., often feeling the absence of the mini-indices generated by TWILL / CTWILL in the printed books (Volume B), and of many other things besides, I have a strong feeling they would be easier to read if presented differently, on a web page where you can click around, expand and collapse sections in-place, hover over names to see their types and possibly definitions (for macros), see more context for every module (aka section) (what variables it may read or modify), see "this section continues in...", see "this procedure is used in...", and so on. Maybe even split the modules into "eyes-and-mouth", "stomach", etc.

So a `web2any` could read the WEB file and parse it into an AST, then display it differently on a web page. It could include user-submitted (by me basically) annotations, and whatnot. It could include diagrams, examples, ...?
