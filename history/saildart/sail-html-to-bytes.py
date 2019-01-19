# -*- coding: utf-8 -*-

"""Program for converting a SailDart web blob (something
like http://www.saildart.org/TEXDR.AFT[1,DEK]_blob) into 
the actual binary file it represents.  Note: no encoding
conversion is done, e.g. the byte (decimal) 126 (= octal
176) now represents ~ in ASCII but represented } on SAIL
and the output of this program will contain byte 126 for
}, not 125 which is what } is on ASCII / Unicode.

So, call this program sail-html-to-bytes.py or something
like that, and sail-convert-encoding.py is to be written
separately."""

import sys

def bytes(word):
    """Each PDP-10 word is 36 bits long (12 octal digits),
    which we get in string form (12 ascii characters all '0' to '7').
    These 36 bits are 5 groups of 7-bit bytes, followed by an unused bit.
    We return the five bytes, as numeric values (in 0 to 127)."""
    assert len(word) == 12
    assert all('0' <= c <= '7' for c in word)
    # Convert word to binary, e.g. '416371546612' to '100001110011111001101100110110001010'
    s = ''.join(bin(ord(c) - ord('0'))[2:].rjust(3, '0') for c in word)
    assert len(s) == 36
    ret = [int(s[i*7 : (i+1)*7], 2) for i in range(5)]
    assert len(ret) == 5

    # The last bit is unused, but sometimes it's 1.
    # One thought was that it may be a parity bit. It isn't.
    # One observation is that in E (?) text files, the first word of each line (5 chars) in the comment / header block have it.
    # But in some other files, it occurs randomly.
    if s[-1] != '0':
        sys.stderr.write('Last bit 1 of word {word} (binary: {s}, total {total}, chars {ret} = {chars})\n'.format(
            word=word,
            s=s,
            total=sum(map(int, s)),
            ret=ret,
            chars=' '.join('#%s#' % c for c in map(chr, ret)),
        ))
    return ret


def to_bytes(words):
    out = [byte for word in words for byte in bytes(word)]
    return out

def print_bytes(out):
    for byte in out:
        sys.stdout.write(chr(byte))

def read_and_print(filename):
    with open(filename) as f:
        blob = f.read()
    import re
    matches = re.match('.*<p id="u8lump">(.*)</p>.*', blob, flags=re.DOTALL)
    words = matches.group(1).strip().split('\n')
    print_bytes(to_bytes(words))


if __name__ == '__main__':
    filename = sys.argv[1]
    read_and_print(filename)
