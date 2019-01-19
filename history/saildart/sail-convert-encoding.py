# -*- coding: utf-8 -*-
"""
On SAIL, the bytes 0 to 127 had their own meanings, which only partially
agree with the meanings given to those bytes by ASCII.

Noteworthy differences:

- bytes 1 to 31 were not all control characters: many were printable
  characters like ↓ α β ∧ ¬ ε π λ ∞ ∂ ⊂ ⊃ ∩ ∪ ∀ ∃ ⊗ ↔ _ → ~ ≠ ≤ ≥ ≡ ∨
  -- only bytes 0, 9, 10, 11, 12, 13 were non-printable characters (and
  9='\t', 10='\n', 13='\r' also had the obvious meanings; and end-of-line
  was '\r\n').

- there were no straight-quote characters, at least for single quotes: while
                ASCII 34=042 " and
                ASCII 39=047 '
                ASCII 96=140 ` grave accent
  instead there were opening and closing quotes:
                 SAIL 34=042 “ opening (Is this true? Double check.)
                 SAIL 39=047 ’ closing
                 SAIL 96=140 ‘ opening
  (TODO: Double-check the situation for double quotes. TEXDR.AFT uses both
  ``this''=‘‘this’’ and "this" style of double-quoting, but only `this'=‘this’
  style of single-quoting.)
  We can still see its legacy in TeX treating every " and ' as closing ones.
  (We can also see ``this style'' of quoting in some of the Emacs and other
   GNU documentation.)

- Byte 94=0136 meant ^ in ASCII, and ↑ an up arrow in SAIL.

- Byte 95=0137 meant _ in ASCII, and ← a left arrow in SAIL.

- Byte 125=0175 meant } in ASCII, and ALT (whatever that is) in SAIL.

- Byte 126=0176 meant ~ in ASCII, and } in SAIL.

Summarizing these differences:

7-bit SAIL ASCII to Unicode and UTF-8 table
https://en.wikipedia.org/wiki/Stanford_Extended_ASCII
(See also Page 369 of The TeXbook.)

        0   1  2  3  4   5  6 7
   000 null ↓  α  β  ∧   ¬  ε π
   010  λ   \t \n \v \f \r  ∞ ∂
   020  ⊂   ⊃  ∩  ∪  ∀   ∃  ⊗ ↔
   030  _   →  ~  ≠  ≤   ≥  ≡ ∨
   040  ␣   !  “  #  $   %  & ’
   050  (   )  *  +  ,   -  . /
   060  0   1  2  3  4   5  6 7
   070  8   9  :  ;  <   =  > ?
   100  @   A  B  C  D   E  F G
   110  H   I  J  K  L   M  N O
   120  P   Q  R  S  T   U  V W
   130  X   Y  Z  [  \   ]  ↑ ←
   140  ‘   a  b  c  d   e  f g
   150  h   i  j  k  l   m  n o
   160  p   q  r  s  t   u  v w
   170  x   y  z  {  |  ALT } BS

Differently, the discrepancies between ASCII and SAIL:

Byte     ASCII     SAIL
1-8      control   various
14-31    control   various
34       "          “ or " (double-check)
39       '          ’ (closing)
94       ^          ↑ an up arrow
95=0137  _          ← a left arrow
96       `          ‘ (opening)
125=0175 }          ALT (whatever that is)
126=0176 ~          } in SAIL

And what's same between ASCII and SAIL:
Byte    ASCII/SAIL
9-13    control chars (\t \n \v \f \r)
32      space
33      !
35-38   # $ % &
40-47   ()*+,-./
48-57   0 to 9
58-64   :;<=>?@
65-90   A to Z
91-93   [ \ ]
97-122  a to z
123     {
124     |
127     control char (DEL / BS)

Summarized yet again differently,

- the characters in SAIL but not in ASCII:

       ↓ α β ∧ ¬ ε π λ ∞ ∂ ⊂ ⊃ ∩ ∪ ∀ ∃ ⊗ ↔ → ≠ ≤ ≥ ≡ ∨

- the characters in ASCII but not in SAIL:
        ' (straight single quote)
        ^ (circumflex / caret / something else that DEK calls it)
        ` (grave accent)

- the characters at different positions in SAIL and in ASCII:

        _ ASCII 95=0137, SAIL 24=030
        ~ ASCII 126=0176, SAIL 26=032

--------------------------------------------------------------------------------

Apart from these character encoding differences, there was a difference in the
file format of "plain text" files.

Namely, in what we now think of as plain text files have, for each character of
each line the user enters, a byte for that character, and then one or two bytes
for the end of line. At SAIL, the text editor (TV Editor) had a notion of pages
-- this means that the user's position is (page, line, column), instead of just
(line, column). The bytes in the file are: one byte for each character that the
user enters, two bytes (13 and 10 i.e. CR and LF) for the end of each line, and
a stream of zero bytes at the end of each page. Moreover, each page starts with
byte 12 (i.e. FF) after the first one, and the first page is an index page that
is in the format (using Unicode here):

    COMMENT ⊗   VALID 00016 PAGES
    C REC  PAGE   DESCRIPTION
    C00001 00001
    C00004 00002	Preliminary preliminary description of 
    C00007 00003	In order to explain TEX more fully, I w
    C00013 00004	  1 %Example TEX input related to Semin
    C00028 00005	The first thing that must be emphasized
    C00034 00006	It is time to explain TEX's mechanism f
    C00043 00007	Now let's look at more of the example. 
    C00049 00008	Line 46 begins a "boxinsert", one of th
    C00054 00009	The ``big(){...~'' in lines 113, 143, e
    C00058 00010	The next step in understanding TEX is t
    C00066 00011	Some tokens are not defined as macros b
    C00070 00012	At the outermost level, TEX has implici
    C00078 00013	Now let's consider the page-building ro
    C00086 00014	The nextparagraph routine assembles lin
    C00094 00015	Besides using the permissible breaks, T
    C00102 00016	To conclude this memo, I should explain
    C00103 ENDMK
    C⊗;

It appears that the "C REC" number "n" means that the FF at the beginning
of the page is at byte 1+(n-1)*640 = 640*n-639, and the page's first char
at 640*n-638.

Strategy for converting such a text file to something readable today:

* Given the poor support for pages today, the program must return not a single
  text file but a sequence of pages. Or, it could write out each page to a
  separate file.

* First pass: look at the set of all bytes encountered. Leave out the printable
  ones that are the same in ASCII / SAIL (namely: see "same" below). Leave out
  the printable ones that mean something else in SAIL (see "modify" below).

  Remaining in case of TEXDR.AFT are:

    0 (end of page)
    9 = o11 (control char: \t = HT)
   10 = o12 (control char: \n = LF)
   12 = o13 (control char: \v = VT)
   13 = o15 (control char: \r = CR)
"""


tvedit = {
    0: 'o0 End of each page',
    9: 'o10 \t = HT = horizontal tab',
    10: 'o12 \n = LF = line feed = end of line',
    12: 'o14 \f = FF = form feed = ^L',
    13: 'o15 \r = CR = carriage return',
}

# The control chracters are 0, 9, 10, 12, 13 (see "tvedit" above).
# Assert the following:
# All 0 bytes are followed either by EOF or 12=^L (and precisely as many as needed to hit 640*n)
# The 9 bytes... we can ignore. They can appear anywhere.
# All 10=CR bytes are followed by 13=LF bytes, and vice-versa. These only occur in pairs.
# All 12=^L bytes occur at multiples of 640 (and in fact the multiple given by the index page).
#
#
# A way to do this: split the input into chunks of 640 bytes each.
# Check that we have an exact multiple.
# Check that 0s occur only at end (i.e. if there's a 0 then everything to the end is 0).
# Check that that CR => next is LF, and LF => prev is CR.
# Check that ^L occurs only at position 1.
# Maybe: Check that the index page matches, but why...

def convert_pages(filename):
  f = open(filename, 'rb').read()
  # Split into groups of 640 and check we have an exact multiple
  assert len(f) % 640 == 0
  chars = []
  i = 0
  while i < len(f):
    assert i % 640 == 0, i
    assert i + 640 <= len(f), (i, len(f))
    cur = [f[j] for j in range(i, i + 640)]
    chars.append(cur)
    i += 640
  # Check that 0s are only at the end (and followed by EOF or FF)
  for (chunk, cur) in enumerate(chars):
    for i in range(len(cur)):
      if cur[i] == chr(0):
        assert all(cur[j] == chr(0) for j in range(i, len(cur))), cur
        assert chunk == len(chars) - 1 or chars[chunk + 1][0] in (chr(12), chr(0)), (chunk, len(chars), chars[chunk + 1][0])
  # Check that CR<=>LF
  for (chunk, cur) in enumerate(chars):
    for i in range(len(cur)):
      if cur[i] == chr(13):
        try:
          if i + 1 < len(cur):
            assert cur[i + 1] == chr(10)
          else:
            assert chunk + 1 < len(chars) and chars[chunk + 1][0] == chr(10)
        except Exception as e:
          print('=====================Full line==================')
          print(''.join(cur))
          print('=====================Part line==================')
          print(''.join(cur[:i]))
          print('%d of %d' % (i, len(cur)))
          print(ord(cur[i + 1]))
          raise e
      if cur[i] == chr(10):
        assert (i > 0 and cur[i - 1] == chr(13) or
                chunk > 0 and chars[chunk - 1][-1] == chr(13)), (cur[:i], i, ord(cur[i - 1]))
  # Check that FF=>1 (and put them into pages)
  pages = []
  current_page = []
  for (chunk, cur) in enumerate(chars):
    for i in range(len(cur)):
      if cur[i] == chr(12):
        assert i == 0
    if cur[0] == chr(12):
      assert current_page
      pages.append(current_page)
      current_page = [cur]
    else:
      current_page.append(cur)

  # print('Pages: %d' % len(pages))
  # for page in pages: print len(page)

  # Now, finally replace things as you go.


  same = ([32, 33] + range(35, 38+1) + range(40, 47+1) +  # space ! # $ % & ( ) * + , - . /
          range(48, 57+1) +  # digits 0 to 9
          range(58,64+1) +   # :;<=>?@
          range(65,90+1) +   # A to Z
          range(91,93+1) +   # [ \ ]
          range(97,122+1) +  # a to z
          range(123, 124+1)) # { and |

  modify = {
      1: '↓',
      2: 'α',
      3: 'β',
      # 4: '∧',
      5: '¬',
      6: 'ε',
      7: 'π',
      8: 'λ',

      14: '∞',
      # 15: '∂',
      # 16: '⊂',
      # 17: '⊃',
      # 18: '∩',
      # 19: '∪',
      # 20: '∀',
      # 21: '∃',
      22: '⊗',
      # 23: '↔',
      24: '_',
      25: '→',
      # 26: '~',
      27: '≠',
      28: '≤',
      29: '≥',
      30: '≡',
      # 31: '∨',

      34: '"',
      39: '’',
      94: '↑',
      95: '←',
      96: '‘',
      126: '}',
  }

  fixed_pages = []
  for page in pages:
    cur_page = []
    for chunk in page:
      for c in chunk:
        if ord(c) in same or c == '\n' or c == '\t':
          cur_page.append(c)
        elif ord(c) in modify:
          cur_page.append(modify[ord(c)])
        elif ord(c) == 0 or c == '\r' or c == '\f':
          pass
        else:
          assert False, (c, ord(c), ''.join(chunk))
    fixed_pages.append(cur_page)
  return fixed_pages

if __name__ == '__main__':
  import sys
  # filename = 'file-282172.txt'
  filename = sys.argv[1]
  fixed_pages = convert_pages(filename)
  for (i, page) in enumerate(fixed_pages):
    with open(filename + '-page-%02d' % (i + 1), 'w') as f:
      f.write(''.join(page))
