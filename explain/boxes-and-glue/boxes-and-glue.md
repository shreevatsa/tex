# What you can do with boxes and glue

    ⟵⎯⎯⎯⎯⎯⎯⎯⎯⎯⎯⟶What↔you↔can↔do
    ⟵⎯⎯⎯⎯⎯⎯⟶with↔boxes↔and↔glue

(TODO: turn the above into an animation)

# TODO: this page is very incomplete

My idea with this page is to describe all the examples in terms of an abstract box-penalty-glue algebra rather than in specifically TeX syntax. Then any program that implements the box-glue-penalty model (e.g. the SILE typesetter, or bramstein/typeset) should produce identical results. And new typesetting programs/libraries will have a good set of testcases of things they should be able to do.

# Theory

## Boxes and glue

At the heart of TeX is a simple, elegant, and remarkably general and powerful abstraction: the notion of boxes and glue.

The simplest manifestation is as a natural way to justify a given line of text (a set of words separated by spaces): we want the leftmost word to begin at the left margin, and the rightmost word to end at the right margin. We can consider each word as an unbreakable "box", and insert uniform "glue" between the boxes, each calculated so that the total length comes out exactly as much as we want it.

[Show pictures / animation here]

When we consider refinements like allowing larger spaces between sentences or after certain punctuation, we are led to the next step: each "glue" actually has a natural width and a stretchability and a shrinkability, and the total width needed is distributed among these individual glues proportional to their stretch/shrink.

[Show pictures...]

## Boxes, glue, and penalties (or kerfs)

More properly we should probably call it the box-glue-penalty model, or even (the authors' later terminology, but not adopted in TeX itself) the box-glue-kerf model.

[Stuff about penalties, hyphenation, kerfs... the line-breaking algorithm]

And that is it!

# Examples

This beautifully unifies many distinct layout operations, and the full power of this model took several years to be apparent even to its authors.

## Simple examples

The model was designed for (left- and right-) justified lines, but it can do ragged-right lines, right-aligned lines, centered lines, etc. rather trivially:

To get ragged-right lines (no stretch for justification), simply add a glue of very large (or "infinite") stretchability at the end. It will absorb all the stretch, and all the spaces will be at their natural width.

[Add Picture]

To get right-aligned lines (ragged-left), add a glue of very large stretchability at the *beginning*: It will absorb all the stretch, and all the spaces will be at their natural width:

[Add picture]
[Mention that the image at the top of the page is an example of this]

To get centered lines, add equal glues of very large stretchability at both the beginning *and* the end: the two of them will absorb the desired stretch, leaving everything else at their natural width.

[Add picture]

Code from `plain.tex`:

    \def\line{\hbox to\hsize}
    \def\leftline#1{\line{#1\hss}}
    \def\rightline#1{\line{\hss#1}}
    \def\centerline#1{\line{\hss#1\hss}}

(`\hss` stands for "[infinite] horizontal stretch or shrink".)

## Less trivial examples

Author lines, indexes, automatically typesetting source code to different width (as "paragraphs"), ... the examples from the Knuth-Plass paper and from _The TeXbook_ exercices.

# Further reading

The book *A Beginner's Book of TeX* by Seroul and Levy is a fantastic introduction to (plain) TeX, and it is especially good at giving readers a solid understanding of boxes and glue, so that at the end of the book you feel like you can typeset nearly any layout.

Also see papers:

* Breaking Paragraphs into Lines: https://web.archive.org/web/20170318101040/http://www.eprg.org/G53DOC/pdfs/knuth-plass-breaking.pdf
* Choosing Better Line Breaks: [upload and add link here from my library scan]

# (Possible) Future work

Every single document that you've seen typeset with TeX basically uses this same simple method for laying out text: surprisingly complex effects are achieved by feeding it the right combination of boxes and glue.

I think it would be a great idea to take the monolithic TeX program, and split it into an input part that turns the user's file(s) into a list of boxes and glue, and another part that actually lays out this text. Then multiple programs could be written that target the same boxes-and-glue, or we could have different input formats that produce the same boxes-and-glue, or automated typesetting would be much easier if you could produce just the right boxes-and-glue and leave it to TeX, etc.

Another way of thinking about it: write something that given a (Xe/Lua)(La)TeX file, does all the work of TeX's "eyes and mouth" (reading/scanning and chewing/expanding), and produces a perfectly equivalent file that is suitable for input by "toothless" or "macroless" TeX -- we can still run TeX on it, but during this new run TeX won't do any expansion and there will be no macro tricks.
