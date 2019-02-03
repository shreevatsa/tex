/*
Utilities for parsing and displaying the internal memory of TANGLE.
*/

// Byte memory
// ===========

// Given arrays |b| (itself containing |ww|=3 byte arrays) and |bs| (containing integer indices into the arrays in |b|),
// transform into a single map from index to string.
function listStrings(b: Array<string>, bs: Array<number>): Array<string> {
    // This implementation creates new memory holding the strings. If it turns out
    // to be expensive for some reason, can change this to something that returns
    // a function that given i, returns string i.
    const ww = b.length;
    let ret = [];
    for (let i = 0; i < bs.length; ++i) {
        let w = i % ww;
        // String i occupies positions bs[i] to bs[i + ww], in b[w].
        if (i + ww >= bs.length) continue;
        if (i != ret.length) throw "Skipped over some element?" + i + " " + ret.length;
        let s = [];
        for (let j = bs[i]; j < bs[i + ww]; ++j) s.push(b[w][j]);
        ret.push(s);
    }
    return ret;
}

type TokenBytes = Array<number>;
type TokenBytesText = Array<TokenBytes>;
type Index = number;

// Token memory
// ============
// Given arrays |t| (itself containing |zz|=5 byte arrays) and |ts| (containing integer indices into the arrays in |t|),
// transform into a single map from index to list of (one-byte and two-byte) tokens.
// NOTE: This does not do any "understanding" of the tokens, just puts each one out as a list of 1 or 2 bytes (integers).
function listTexts(t: Array<string>, ts: Array<number>): Array<TokenBytesText> {
    // This implementation creates new memory holding the tokens. If it turns out
    // to be expensive for some reason, can change this to something that returns
    // a function that given i, returns text i.
    const zz = t.length;
    let ret: Array<TokenBytesText> = [];
    for (let i = 0; i < ts.length; ++i) {
        let z = i % zz;
        // Text i occupies positions ts[i] to ts[i + zz] - 1, in t[z].
        if (i + zz >= ts.length) continue;
        let s: Array<TokenBytes> = [];
        let j: Index = ts[i];
        while (j < ts[i + zz]) {
            if (t[z].charCodeAt(j) < 128) {
                s.push([t[z].charCodeAt(j)]);
                j += 1;
            } else {
                s.push([t[z].charCodeAt(j), t[z].charCodeAt(j+1)]);
                j += 2;
            }
            // console.log('Added token', s[s.length - 1]);
        }
        ret.push(s);
    }
    return ret;
}

interface Token {
    type: string,
    value: string | number,
};

// Given a token t which is a list of either 1 or 2 bytes, say what it is.
// Returns in the format {type: "...", value: "..."} where value is optional.
// NOTE: Incomplete; the real function needs some context on whether within a string, whether 32 is space or .., and whether 125 is check_sum or closing }.
function parseToken(t: TokenBytes): Token {
    if (t.length == 2) {
        // See section 75 of TANGLE
        let a = t[0];
        if (a < 0x80) throw "Expected first byte to be at least 128 = 0o200 = 0x80";
        if (0x80 <= a && a < 0xa8) {
            // Identifier or string (name)
            return {type: 'Name@', value: (a - 0x80) * 0x100 + t[1]};
        }
        if (0xa8 <= a && a < 0xd0) {
            // Module name
            return {type: "Module@", value: (a - 0xa8) * 0x100 + t[1]};
        }
        if (0xd0 <= a && a < 0x100) {
            // Module number
            return {type: "Module#", value: (a - 0xd0) * 0x100 + t[1]};
        }
        throw "Expected a byte, not " + a;
    }
    // A single byte.
    let c = t[0];
    // See sections 72, 76, 15 of TANGLE
    if (c == 0) return {type: 'param', value: '#'};
    if (c == 2) return {type: 'verbatim', value: ''};
    if (c == 3) return {type: 'force_line', value: '\\n'};
    if (c == 9) return {type: 'begin_comment', value: '{'};
    if (c == 10) return {type: 'end_comment', value: '}'};
    if (c == 12) return {type: 'octal', value: "'"};
    if (c == 13) return {type: 'hex', value: "''"};
    if (c == 24) return {type: 'left_arrow', value:':='};
    if (c == 26) return {type: 'not_equal', value:'!='};
    if (c == 32) return {type: 'double_dot', value:'..'};
    if (c == 125) return {type: 'check_sum', value: '@$'};
    if (c == 127) return {type: 'join', value: '@&'};
    if (c < 32 || c >= 127) throw 'Decide what to do with unprintable characters like: ' + c;
    let s = String.fromCharCode(c);
    return {type: 'char', value: s};
}

// Some parts of the internal memory of TANGLE, just after the reading phase (phase one) of processing pooltype.web
let pooltypeMem = {
    b: [
        "pool_filetypeGlobals in the outer blockLocal variables for initializationenddo_nothingcharintegerofcarriage_returntocountgotolc_hex\"0writediv\" packedbooleaneofuntil\"9read_ln",
        "programoutputTypes in the outer blockprocedurebeginincrASCII_codefirst_text_charxordxchrinvalid_codedoabortMake the first 256 stringsifelseCharacter |k| cannot be printedmodorfileresetrepeatnoteoln",
        "POOLtypelabelvarinitializeSet initial values of key variablesdecrtext_charlast_text_chararraynull_codeforchrwrite_lnRead the other strings from the \\.{POOL} file, or give an error message and abortthen\"a\"^\"\"\"\"~xsumfalseRead one string, but abort if there are problemsreadtrue",
    ],
    bs: [0, 0, 0, 0, 7, 8, 9, 13, 13, 13, 37, 16, 39, 46, 26, 73, 51, 61, 76, 55, 65, 86, 65, 74, 90, 80, 88, 97, 84, 93, 99, 88, 102, 114, 100, 105, 116, 102, 108, 121, 107, 116, 125, 133, 197, 131, 135, 201, 133, 139, 203, 138, 170, 205, 141, 173, 208, 143, 175, 210, 149, 179, 214, 156, 184, 219, 159, 190, 267, 164, 193, 271, 166, 197, 275, 173],

    t: [
        "\xd0\x05\x80\x13=0 255;\xd0\x0a\x80 i\x180\x80!\x0c37\x80\"\x80\x1c[i]\x18' ';\x80 i\x18\x0c177\x80!\x0c377\x80\"\x80\x1c[i]\x18' ';\x80\x0d\x80&(\x00);\x80'9999;\x80\x0f\xd0\x12\x80\x03:\x809\x80:\x80\x1b\x80\x14;\x80;:\x80<;",
        "\xd0\x02\x80\x01\x80\x02(\x80\x03,\x80\x04);\x80\x059999;\x80\x06\xa8\x07\x80\x08\xa8\x09\x80\x0a\x80\x0b;\x80\x08\xa8\x0c\x80\x0d\xa8\x0e\x80\x0f;\x80\x15\xd0\x0b\x80 i\x18\x80\x16\x80!\x80\x17\x80\"\x80\x19[\x80#(i)]\x18\x80\x1f;\x80 i\x18\x0c200\x80!\x0c377\x80\"\x80\x19[\x80\x1c[i]]\x18i;\x80 i\x180\x80!\x0c176\x80\"\x80\x19[\x80\x1c[i]]\x18i;\xd0\x0f\x80\x0d\x80\x0b;\xa8(;s\x18256;\xa8);\x80&('(',\x80$:1,' characters in all.)');9999:\x80\x0f.\xd0\x13\x80=(\x80\x03);\x80;\x18\x80>;\x80+\x80?(\x80\x03)\x80,\x80%('! I can''t read the POOL file.');\x80@\xa8A;\x80B\x80;;\x80+\x80C\x80?(\x80\x03)\x80,\x80%('! There''s junk after the check sum')",
        "\x00\x18\x00+1\xd0\x06i:\x80\x18;\xd0\x0ck,l:0 255;m,n:\x80\x14;s:\x80\x18;l\x18\x00;\x80+l<10\x80,l\x18l+\x80-\x80.l\x18l-10+\x80/\xd0\x14\x80+\x80?(\x80\x03)\x80,\x80%('! POOL file contained no check sum');\x80D(\x80\x03,m,n);\x80+m\x1a'*'\x80,\x80\x0d\x80+(\x80\x19[m]<\x80-)\x807(\x80\x19[m]>\x80E)\x807(\x80\x19[n]<\x80-)\x807(\x80\x19[n]>\x80E)\x80,\x80%('! POOL line doesn''t begin with two digits');l\x18\x80\x19[m]*10+\x80\x19[n]-\x80-*11;\x800(s:3,': \"');\x80$\x18\x80$+l;\x80 k\x181\x80!l\x80\"\x80\x0d\x80+\x80F(\x80\x03)\x80,\x80\x0d\x80&('\"');\x80%('! That POOL line was too short');\x80\x0f;\x80D(\x80\x03,m);\x800(\x80\x1c[\x80\x19[m]]);\x80+\x80\x19[m]=\x805\x80,\x800(\x80\x1c[\x805]);\x80\x0f;\x80&('\"');\x80\x10(s);\x80\x0f\x80.\x80;\x18\x80G;\x80H(\x80\x03)",
        "\x00\x18\x00-1\xd0\x07\x80\x19:\x80\x1a[\x80\x14]\x80\x1b\x80\x13;\x80\x1c:\x80\x1a[\x80\x13]\x80\x1b\x80\x14;\xd0\x0d\x80$:\x80\x18;\xd0\x10\x80 k\x180\x80!255\x80\"\x80\x0d\x800(k:3,': \"');l\x18k;\x80+(\xa81)\x80,\x80\x0d\x800(\x80\x1c[\x802],\x80\x1c[\x802]);\x80+k<\x0c100\x80,l\x18k+\x0c100\x80.\x80+k<\x0c200\x80,l\x18k-\x0c100\x80.\x80\x0d\x80*(k\x80316);\x800(\x80\x1c[l]);\x80*(k\x80416);\x80\x10(\x80$);\x80\x0f;\x80$\x18\x80$+2;\x80\x0f;\x80+l=\x805\x80,\x800(\x80\x1c[l],\x80\x1c[l])\x80.\x800(\x80\x1c[l]);\x80\x10(\x80$);\x80&('\"');\x80\x0f",
        "\xd0\x08\x80\x1c[\x0c40]\x18' ';\x80\x1c[\x0c41]\x18'!';\x80\x1c[\x0c42]\x18'\"';\x80\x1c[\x0c43]\x18'#';\x80\x1c[\x0c44]\x18'$';\x80\x1c[\x0c45]\x18'%';\x80\x1c[\x0c46]\x18'&';\x80\x1c[\x0c47]\x18'''';\x80\x1c[\x0c50]\x18'(';\x80\x1c[\x0c51]\x18')';\x80\x1c[\x0c52]\x18'*';\x80\x1c[\x0c53]\x18'+';\x80\x1c[\x0c54]\x18',';\x80\x1c[\x0c55]\x18'-';\x80\x1c[\x0c56]\x18'.';\x80\x1c[\x0c57]\x18'/';\x80\x1c[\x0c60]\x18'0';\x80\x1c[\x0c61]\x18'1';\x80\x1c[\x0c62]\x18'2';\x80\x1c[\x0c63]\x18'3';\x80\x1c[\x0c64]\x18'4';\x80\x1c[\x0c65]\x18'5';\x80\x1c[\x0c66]\x18'6';\x80\x1c[\x0c67]\x18'7';\x80\x1c[\x0c70]\x18'8';\x80\x1c[\x0c71]\x18'9';\x80\x1c[\x0c72]\x18':';\x80\x1c[\x0c73]\x18';';\x80\x1c[\x0c74]\x18'<';\x80\x1c[\x0c75]\x18'=';\x80\x1c[\x0c76]\x18'>';\x80\x1c[\x0c77]\x18'?';\x80\x1c[\x0c100]\x18'@';\x80\x1c[\x0c101]\x18'A';\x80\x1c[\x0c102]\x18'B';\x80\x1c[\x0c103]\x18'C';\x80\x1c[\x0c104]\x18'D';\x80\x1c[\x0c105]\x18'E';\x80\x1c[\x0c106]\x18'F';\x80\x1c[\x0c107]\x18'G';\x80\x1c[\x0c110]\x18'H';\x80\x1c[\x0c111]\x18'I';\x80\x1c[\x0c112]\x18'J';\x80\x1c[\x0c113]\x18'K';\x80\x1c[\x0c114]\x18'L';\x80\x1c[\x0c115]\x18'M';\x80\x1c[\x0c116]\x18'N';\x80\x1c[\x0c117]\x18'O';\x80\x1c[\x0c120]\x18'P';\x80\x1c[\x0c121]\x18'Q';\x80\x1c[\x0c122]\x18'R';\x80\x1c[\x0c123]\x18'S';\x80\x1c[\x0c124]\x18'T';\x80\x1c[\x0c125]\x18'U';\x80\x1c[\x0c126]\x18'V';\x80\x1c[\x0c127]\x18'W';\x80\x1c[\x0c130]\x18'X';\x80\x1c[\x0c131]\x18'Y';\x80\x1c[\x0c132]\x18'Z';\x80\x1c[\x0c133]\x18'[';\x80\x1c[\x0c134]\x18'\\';\x80\x1c[\x0c135]\x18']';\x80\x1c[\x0c136]\x18'^';\x80\x1c[\x0c137]\x18'_';\x80\x1c[\x0c140]\x18'`';\x80\x1c[\x0c141]\x18'a';\x80\x1c[\x0c142]\x18'b';\x80\x1c[\x0c143]\x18'c';\x80\x1c[\x0c144]\x18'd';\x80\x1c[\x0c145]\x18'e';\x80\x1c[\x0c146]\x18'f';\x80\x1c[\x0c147]\x18'g';\x80\x1c[\x0c150]\x18'h';\x80\x1c[\x0c151]\x18'i';\x80\x1c[\x0c152]\x18'j';\x80\x1c[\x0c153]\x18'k';\x80\x1c[\x0c154]\x18'l';\x80\x1c[\x0c155]\x18'm';\x80\x1c[\x0c156]\x18'n';\x80\x1c[\x0c157]\x18'o';\x80\x1c[\x0c160]\x18'p';\x80\x1c[\x0c161]\x18'q';\x80\x1c[\x0c162]\x18'r';\x80\x1c[\x0c163]\x18's';\x80\x1c[\x0c164]\x18't';\x80\x1c[\x0c165]\x18'u';\x80\x1c[\x0c166]\x18'v';\x80\x1c[\x0c167]\x18'w';\x80\x1c[\x0c170]\x18'x';\x80\x1c[\x0c171]\x18'y';\x80\x1c[\x0c172]\x18'z';\x80\x1c[\x0c173]\x18'{';\x80\x1c[\x0c174]\x18'|';\x80\x1c[\x0c175]\x18'}';\x80\x1c[\x0c176]\x18'~';\xd0\x0e\x80$\x180;\xd0\x11(k<\x806)\x807(k>\x808)",
    ],
    ts: [0, 0, 0, 0, 0, 0, 45, 5, 5, 0, 11, 47, 12, 35, 1206, 61, 127, 36, 43, 1213, 78, 190, 65, 250, 1229, 98, 315, 435],
};
pooltypeMem['names'] = listStrings(pooltypeMem.b, pooltypeMem.bs);
pooltypeMem['texts'] = listTexts(pooltypeMem.t, pooltypeMem.ts).map(text => text.map(token => parseToken(token)));

// Not even related to TANGLE
// ==========================

// Given text, escape for HTML. OWASP Rule#1: https://www.owasp.org/index.php?title=XSS_(Cross_Site_Scripting)_Prevention_Cheat_Sheet&oldid=244079#RULE_.231_-_HTML_Escape_Before_Inserting_Untrusted_Data_into_HTML_Element_Content
function escapeForHtml(text) {
    var map = {
        '&': '&amp;',
        '<': '&lt;',
        '>': '&gt;',
        '"': '&quot;',
        "'": '&#x27;',
        "/": '&#x2F',  // forward slash is included as it helps end an HTML entity
    };
    return text.replace(/[&<>"'/]/g, m => map[m]);
}

function one_hex(n) { if (n < 0 || n >= 16) throw "Not a hex digit: " + n; return n.toString(16); }
function two_hex(n) { if (n < 0 || n >= 256) throw "Not a byte: " + n; return one_hex((n - n % 16) / 16) + one_hex(n % 16); }
