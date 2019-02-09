/**
* @fileoverview Utilities for parsing and displaying the internal memory of TANGLE.
*/

interface HTMLElement {
    scrollIntoViewIfNeeded(): void;
 }
interface String {
    byteAt(n : number) : Byte;
 }
 type Byte = 0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|27|28|29|30|31|32|33|34|35|36|37|38|39|40|41|42|43|44|45|46|47|48|49|50|51|52|53|54|55|56|57|58|59|60|61|62|63|64|65|66|67|68|69|70|71|72|73|74|75|76|77|78|79|80|81|82|83|84|85|86|87|88|89|90|91|92|93|94|95|96|97|98|99|100|101|102|103|104|105|106|107|108|109|110|111|112|113|114|115|116|117|118|119|120|121|122|123|124|125|126|127|128|129|130|131|132|133|134|135|136|137|138|139|140|141|142|143|144|145|146|147|148|149|150|151|152|153|154|155|156|157|158|159|160|161|162|163|164|165|166|167|168|169|170|171|172|173|174|175|176|177|178|179|180|181|182|183|184|185|186|187|188|189|190|191|192|193|194|195|196|197|198|199|200|201|202|203|204|205|206|207|208|209|210|211|212|213|214|215|216|217|218|219|220|221|222|223|224|225|226|227|228|229|230|231|232|233|234|235|236|237|238|239|240|241|242|243|244|245|246|247|248|249|250|251|252|253|254|255;

String.prototype.byteAt = function(n: number): Byte {
    return this.charCodeAt(n);
}

/**
* Byte memory (names: double-quoted strings, and names of modules, macros, Pascal identifiers and keywords)
* @param byte_mem itself contains |ww|=3 byte arrays
* @param byte_start contains integer indices into the arrays in |byte_mem|
* @returns The list of names that the arrays |byte_mem| and |byte_start| represent.
*/
function listNames(byte_mem: Array<string>, byte_start: Array<number>): Array<string> {
    // Note for future: This implementation creates new memory holding the strings. If it turns out
    // to be expensive for some reason, can change this to something that returns
    // a function that given i, returns name i.
    const ww = byte_mem.length;
    let ret = [];
    // Name i occupies positions byte_start[i] to byte_start[i + ww], in byte_mem[w][].
    for (let i = 0; i < byte_start.length - ww; ++i) {
        let w = i % ww;
        ret.push(byte_mem[w].slice(byte_start[i], byte_start[i + ww]));
    }
    for (let i = 0; i < ret.length; ++i) {
        let h = 0;
        for (let j = 0; j < ret[i].length; ++j) {
            h = (h * 2 + ret[i].byteAt(j)) % 353;
        }
        // console.log(ret[i], '\t -> \t', h);
    }
    return ret;
}

interface byteCell {
    index: (number|string);    // index in the array
    rawValue: string; // like "d0" -- 2 hex chars
    show: string;     // The actual character to display
    id: string;       // An id for the element in the HTML DOM
}
type byteCells = Array<byteCell>;
// Representation of a single array of |byte_mem|, as a row of "cells".
function byteArrayDiv(id_prefix: string, s: string) {
    let array: byteCells = [];
    for (let i = 0; i < s.length; ++i) {
        let c = s.byteAt(i);
        if (c < 32 || c >= 127) throw "Decide what to do with unprintable characters like: " + c;
        array.push({
                index: i,
                rawValue: two_hex(s.byteAt(i)),
                show: s[i],
                id: id_prefix + "_" + i,
            });
        }
    array.push({index: s.length, rawValue: 'NO', show: 'NOPE', id: id_prefix + "_" + s.length});
    // Now that we have the [index, rawValue, show, id] for every element, create the row of cells.
    let d = document.createElement('div');
    d.classList.add('memArray'); d.classList.add('hbox');
    for (let cell of array) {
        let dd = document.createElement('div'); dd.classList.add('memCell'); dd.id = cell.id;
        dd.innerHTML = (
            `<div class="cellIndex">${cell.index}</div>` +
            `<div class="cellShow">${escapeForHtml(cell.show)}</div>` +
            `<div class="cellRaw">${cell.rawValue != undefined ? cell.rawValue : ' '}</div>`
        );
        d.appendChild(dd);
    }
    return d;
}

// Returns a HTML node.
function memArrayDiv(id_prefix: string, s: string) {
    let array: byteCells = [];
    for (let i = 0; i < s.length;) {
        let [show, len]: [string, number] = token_show(s, i);
        array.push({
            index: `${i}` + (len == 2 ? `,${i+1}`: ``),
            rawValue: two_hex(s.byteAt(i)) + (len == 2 ? ` ${two_hex(s.byteAt(i+1))}` : ``),
            show: show,
            id: id_prefix + "_" + i,
        });
        i += len;
    }
    array.push({index: s.length, rawValue: 'NO', show: 'NOPE', id: id_prefix + "_" + s.length});
    let d = document.createElement('div');
    d.classList.add('memArray'); d.classList.add('hbox');
    for (let cell of array) {
        let dd = document.createElement('div'); dd.classList.add('memCell'); dd.id = cell.id;
        dd.innerHTML = (
            `<div class="cellIndex">${cell.index}</div>` +
            `<div class="cellShow">${escapeForHtml(cell.show)}</div>` +
            `<div class="cellRaw">${cell.rawValue}</div>`
        );
        d.appendChild(dd);
    }
    return d;
}

function point(pointed: string, next: string) {
    let p = document.getElementById(pointed);
    let n = document.getElementById(next);
    if (n) n.scrollIntoViewIfNeeded();
    if (p) p.scrollIntoViewIfNeeded();
    if (p) p.classList.add('pointed');
    if (n) n.classList.add('pointNext');
}
function unpoint(pointed: string, next: string) {
    let p = document.getElementById(pointed);
    let n = document.getElementById(next);
    if (p) p.classList.remove('pointed');
    if (n) n.classList.remove('pointNext');
}
function startArrayDiv(start: Array<number>, ww: number, idPrefix: string) {
    let d = document.createElement('div');
    d.classList.add('startArray'); d.classList.add('hbox');
    for (let i = 0; i < start.length; ++i) {
        let w: number = i % ww;  // Read as z = i % zz, for token start array.
        // The IDs of the corresponding (pointed-to) cells in the memory arrays.
        let [pointed, next] = [idPrefix + w + "_" + start[i], idPrefix + w + "_" + start[i+ww]];
        let dd = document.createElement('div');
        dd.classList.add('startCell');
        dd.innerHTML = `<div class="cellIndex">${i}</div><div class="cellShow">${start[i]}</div><div class="cellRaw">${w}</div>`;
        dd.addEventListener('mouseover', () => { point(pointed, next); });
        dd.addEventListener('mouseout', () => { unpoint(pointed, next) });
        d.appendChild(dd);
    }
    return d;
}

//=========================================================================================================================================================================

// Token memory
// ============
type TokenBytes = Array<Byte>;
type TokenBytesText = Array<TokenBytes>;
type Index = number;
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
                s.push([t[z].byteAt(j)]);
                j += 1;
            } else {
                s.push([t[z].byteAt(j), t[z].byteAt(j+1)]);
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

// A short human-readable representation of the token, to show inside a small div.
function token_show(s: string, n: number): [string, number] {
    let a: Byte = s.byteAt(n);
    let tokenBytes: TokenBytes = (a >= 128) ? [s.byteAt(n), s.byteAt(n + 1)] : [s.byteAt(n)];
    let token: Token = parseToken(tokenBytes);
    if (token.type == 'Name@') return [`<N@${token.value}>`, 2];
    if (token.type == 'Module@') return [`Mod@${token.value}`, 2];
    if (token.type == 'Module#') return [`Mod#${token.value}`, 2];
    if (token.type == 'param' || token.type == 'begin_comment' || token.type == 'end_comment') {
        return [`<${token.value}>`, 1];
    }
    if (token.type == 'char' || token.type == 'left_arrow' || token.type == 'not_equal' || token.type == 'double_dot') return [`${token.value}`, 1];
    return [`<${token.type}>`, 1];
}





//=========================================================================================================================================================================


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

    equiv: [0, 0, 0, 0, 0, 0, 0, 5, 0, 8, 0, 0, 7, 0, 9, 0, 2, 3, 4, 0, 6, 0, 1073741824, 1073742079, 0, 0, 0, 0, 0, 1073741824, 1073741837, 1073741951, 0, 0, 0, 0, 0, 15,         0, 0, 18, 21, 17, 0, 0, 1073741872, 0, 1073741921, 0, 19, 1073741918, 0, 0, 1073741858, 1073741856, 0, 1073741950, 0, 0, 0, 0, 0, 0, 51, 0, 22, 0, 0, 0,         1073741881, 0],

    text_link: [1, 16, 0, 0, 0, 10000, 0, 10000, 12, 10, 11, 14, 13, 20, 10000, 0, 10000, 0, 10000, 10000, 10000, 10000, 10000, 0],

};
pooltypeMem['names'] = listNames(pooltypeMem.b, pooltypeMem.bs);
// console.log('The first name is: ', pooltypeMem['names'][1]);
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

function one_hex(n: number) { if (n < 0 || n >= 16) throw "Not a hex digit: " + n; return n.toString(16); }
function two_hex(n: Byte) { if (n < 0 || n >= 256) throw "Not a byte: " + n; return one_hex((n - n % 16) / 16) + one_hex(n % 16); }

// This block copied from https://gist.github.com/hsablonniere/2581101
if (!('scrollIntoViewIfNeeded' in Element.prototype)) {
    Element.prototype['scrollIntoViewIfNeeded'] = function (centerIfNeeded) {
        centerIfNeeded = arguments.length === 0 ? true : !!centerIfNeeded;

        var parent = this.parentNode,
        parentComputedStyle = window.getComputedStyle(parent, null),
        parentBorderTopWidth = parseInt(parentComputedStyle.getPropertyValue('border-top-width')),
        parentBorderLeftWidth = parseInt(parentComputedStyle.getPropertyValue('border-left-width')),
        overTop = this.offsetTop - parent.offsetTop < parent.scrollTop,
        overBottom = (this.offsetTop - parent.offsetTop + this.clientHeight - parentBorderTopWidth) > (parent.scrollTop + parent.clientHeight),
        overLeft = this.offsetLeft - parent.offsetLeft < parent.scrollLeft,
        overRight = (this.offsetLeft - parent.offsetLeft + this.clientWidth - parentBorderLeftWidth) > (parent.scrollLeft + parent.clientWidth),
        alignWithTop = overTop && !overBottom;

        if ((overTop || overBottom) && centerIfNeeded) {
            parent.scrollTop = this.offsetTop - parent.offsetTop - parent.clientHeight / 2 - parentBorderTopWidth + this.clientHeight / 2;
        }

        if ((overLeft || overRight) && centerIfNeeded) {
            parent.scrollLeft = this.offsetLeft - parent.offsetLeft - parent.clientWidth / 2 - parentBorderLeftWidth + this.clientWidth / 2;
        }

        if ((overTop || overBottom || overLeft || overRight) && !centerIfNeeded) {
            this.scrollIntoView(alignWithTop);
        }
    };
}
