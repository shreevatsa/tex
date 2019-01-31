let t = [
  "\320\005\200\023=0 255;\320\n\200 i\030\060\200!\f37\200\"\200\034[i]\030' ';\200 i\030\f177\200!\f377\200\"\200\034[i]\030' ';\200\r\200&(\000);\200'9999;\200\017\320\022\200\003:\200\071\200:\200\033\200\024;\200;:\200<;",
  "\320\002\200\001\200\002(\200\003,\200\004);\200\005\071\071\071\071;\200\006\250\a\200\b\250\t\200\n\200\v;\200\b\250\f\200\r\250\016\200\017;\200\025\320\v\200 i\030\200\026\200!\200\027\200\"\200\031[\200#(i)]\030\200\037;\200 i\030\f200\200!\f377\200\"\200\031[\200\034[i]]\030i;\200 i\030\060\200!\f176\200\"\200\031[\200\034[i]]\030i;\320\017\200\r\200\v;\250(;s\030\062\065\066;\250);\200&('(',\200$:1,' characters in all.)');9999:\200\017.\320\023\200=(\200\003);\200;\030\200>;\200+\200?(\200\003)\200,\200%('! I can''t read the POOL file.');\200@\250A;\200B\200;;\200+\200C\200?(\200\003)\200,\200%('! There''s junk after the check sum')",
  "\000\030\000+1\320\006i:\200\030;\320\fk,l:0 255;m,n:\200\024;s:\200\030;l\030\000;\200+l<10\200,l\030l+\200-\200.l\030l-10+\200/\320\024\200+\200?(\200\003)\200,\200%('! POOL file contained no check sum');\200D(\200\003,m,n);\200+m\032'*'\200,\200\r\200+(\200\031[m]<\200-)\200\067(\200\031[m]>\200E)\200\067(\200\031[n]<\200-)\200\067(\200\031[n]>\200E)\200,\200%('! POOL line doesn''t begin with two digits');l\030\200\031[m]*10+\200\031[n]-\200-*11;\200\060(s:3,': \"');\200$\030\200$+l;\200 k\030\061\200!l\200\"\200\r\200+\200F(\200\003)\200,\200\r\200&('\"');\200%('! That POOL line was too short');\200\017;\200D(\200\003,m);\200\060(\200\034[\200\031[m]]);\200+\200\031[m]=\200\065\200,\200\060(\200\034[\200\065]);\200\017;\200&('\"');\200\020(s);\200\017\200.\200;\030\200G;\200H(\200\003)",
  "\000\030\000-1\320\a\200\031:\200\032[\200\024]\200\033\200\023;\200\034:\200\032[\200\023]\200\033\200\024;\320\r\200$:\200\030;\320\020\200 k\030\060\200!255\200\"\200\r\200\060(k:3,': \"');l\030k;\200+(\250\061)\200,\200\r\200\060(\200\034[\200\062],\200\034[\200\062]);\200+k<\f100\200,l\030k+\f100\200.\200+k<\f200\200,l\030k-\f100\200.\200\r\200*(k\200\063\061\066);\200\060(\200\034[l]);\200*(k\200\064\061\066);\200\020(\200$);\200\017;\200$\030\200$+2;\200\017;\200+l=\200\065\200,\200\060(\200\034[l],\200\034[l])\200.\200\060(\200\034[l]);\200\020(\200$);\200&('\"');\200\017",
  "\320\b\200\034[\f40]\030' ';\200\034[\f41]\030'!';\200\034[\f42]\030'\"';\200\034[\f43]\030'#';\200\034[\f44]\030'$';\200\034[\f45]\030'%';\200\034[\f46]\030'&';\200\034[\f47]\030'''';\200\034[\f50]\030'(';\200\034[\f51]\030')';\200\034[\f52]\030'*';\200\034[\f53]\030'+';\200\034[\f54]\030',';\200\034[\f55]\030'-';\200\034[\f56]\030'.';\200\034[\f57]\030'/';\200\034[\f60]\030'0';\200\034[\f61]\030'1';\200\034[\f62]\030'2';\200\034[\f63]\030'3';\200\034[\f64]\030'4';\200\034[\f65]\030'5';\200\034[\f66]\030'6';\200\034[\f67]\030'7';\200\034[\f70]\030'8';\200\034[\f71]\030'9';\200\034[\f72]\030':';\200\034[\f73]\030';';\200\034[\f74]\030'<';\200\034[\f75]\030'=';\200\034[\f76]\030'>';\200\034[\f77]\030'?';\200\034[\f100]\030'@';\200\034[\f101]\030'A';\200\034[\f102]\030'B';\200\034[\f103]\030'C';\200\034[\f104]\030'D';\200\034[\f105]\030'E';\200\034[\f106]\030'F';\200\034[\f107]\030'G';\200\034[\f110]\030'H';\200\034[\f111]\030'I';\200\034[\f112]\030'J';\200\034[\f113]\030'K';\200\034[\f114]\030'L';\200\034[\f115]\030'M';\200\034[\f116]\030'N';\200\034[\f117]\030'O';\200\034[\f120]\030'P';\200\034[\f121]\030'Q';\200\034[\f122]\030'R';\200\034[\f123]\030'S';\200\034[\f124]\030'T';\200\034[\f125]\030'U';\200\034[\f126]\030'V';\200\034[\f127]\030'W';\200\034[\f130]\030'X';\200\034[\f131]\030'Y';\200\034[\f132]\030'Z';\200\034[\f133]\030'[';\200\034[\f134]\030'\\';\200\034[\f135]\030']';\200\034[\f136]\030'^';\200\034[\f137]\030'_';\200\034[\f140]\030'`';\200\034[\f141]\030'a';\200\034[\f142]\030'b';\200\034[\f143]\030'c';\200\034[\f144]\030'd';\200\034[\f145]\030'e';\200\034[\f146]\030'f';\200\034[\f147]\030'g';\200\034[\f150]\030'h';\200\034[\f151]\030'i';\200\034[\f152]\030'j';\200\034[\f153]\030'k';\200\034[\f154]\030'l';\200\034[\f155]\030'm';\200\034[\f156]\030'n';\200\034[\f157]\030'o';\200\034[\f160]\030'p';\200\034[\f161]\030'q';\200\034[\f162]\030'r';\200\034[\f163]\030's';\200\034[\f164]\030't';\200\034[\f165]\030'u';\200\034[\f166]\030'v';\200\034[\f167]\030'w';\200\034[\f170]\030'x';\200\034[\f171]\030'y';\200\034[\f172]\030'z';\200\034[\f173]\030'{';\200\034[\f174]\030'|';\200\034[\f175]\030'}';\200\034[\f176]\030'~';\320\016\200$\030\060;\320\021(k<\200\066)\200\067(k>\200\070)",
];
let ts =[0, 0, 0, 0, 0, 0, 45, 5, 5, 0, 11, 47, 12, 35, 1206, 61, 127, 36, 43, 1213, 78, 190, 65, 250, 1229, 98, 315, 435];

function escapeForHtml(text) {
  var map = {
    '&': '&amp;',
    '<': '&lt;',
    '>': '&gt;',
    '"': '&quot;',
    "'": '&#039;'
  };
  return text.replace(/[&<>"']/g, function(m) { return map[m]; });
}

function one_hex(n) { if (n < 0 || n >= 16) throw "Not a hex digit: " + n; return n.toString(16); }
function two_hex(n) { if (n < 0 || n >= 256) throw "Not a byte: " + n; return one_hex((n - n % 16) / 16) + one_hex(n % 16); }
function token_show_single(c) {
  if (c == 0) return "<#>";
  if (c == 2) return "<verbatim>";
  if (c == 3) return "<force_line>";
  if (c == 9) return "<{>";
  if (c == 10) return "<}>";
  if (c == 12) return "<'>";
  if (c == 13) return "<''>";
  if (c == 24) return ":=";
  if (c == 26) return "!=";
  if (c == 32) return "<..>";
  if (c == 125) return "<check_sum>";
  if (c == 127) return "<@&>"; // join
  if (c < 32 || c >= 127) throw "Decide what to do with unprintable characters like: " + c;
  return String.fromCharCode(c);
}
function token_show(s, n) {
  let a = s.charCodeAt(n);
  if (a >= 128) {
    // Two bytes
    if (a < 168) {
      // Identifier or string
      a = (a - 128) * 256 + s.charCodeAt(n + 1);
      return [`<N@${a}>`, 2]; /* Name at */
    }
    if ( a < 208) {
      // Module name
      a = (a - 168) * 256 + s.charCodeAt(n + 1);
      return [`<Mod@${a}>`, 2];
    }
    a = (a - 208) * 256 + s.charCodeAt(n + 1);
    return [`<Mod#${a}>`, 2];
  }
  return [token_show_single(a), 1];
}

// For a cell (string index / char), output its [index, value, show, id]
function pretty_cell(id_prefix, s, n) {
  let ret = {};
  let [show, len] = token_show(s, n);
  ret["show"] = show;
  ret["index"] = `${n}` + (len == 2 ? `,${n+1}` : ``);
  ret["value"] = two_hex(s.charCodeAt(n)) + (len == 2 ? ` ${two_hex(s.charCodeAt(n+1))}` : ``);
  ret["id"] = id_prefix + "_" + n;
  return [ret, len];
}
// The above for each cell in an array
function pretty_array(id_prefix, s) {
  let ret = [];
  for (let i = 0; i < s.length;) {
    let [cell, len] = pretty_cell(id_prefix, s, i);
    ret.push(cell);
    i += len;
  }
  ret.push({index: s.length, value: 'NO', show: 'NOPE', id: id_prefix + "_" + s.length});
  return ret;
}
// For each cell in the start array, output its [index, array_num, start_index, id, start_id]
function pretty_start(start, m) {
  let ret = [];
  for (let i = 0; i < start.length; ++i) {
    let w = i % m;
    let tmp = [i, w, start[i], "t" + w + "_" + start[i], "t" + w + "_" + start[i+m]];
    ret.push(tmp);
  }
  return ret;
}

// n = bs.length;
// console.log([pretty_start(bs, n , 3)]);
// console.log([pretty_array("b0", b0), pretty_array("b1", b1), pretty_array("b2", b2)]);
// console.log(b0.length, b1.length, b2.length, n);


// What we want:
// One div for each array -- a div for each cell, each with an id.
// One div for the "start" array -- a div for each cell, with onclick / onhover events.

// This block copied from https://gist.github.com/hsablonniere/2581101
if (!Element.prototype.scrollIntoViewIfNeeded) {
  Element.prototype.scrollIntoViewIfNeeded = function (centerIfNeeded) {
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

// Returns a HTML node.
function memCellDiv(cell) {
  let d = document.createElement('div');
  d.innerHTML = `<div class="cellIndex">${cell.index}</div><div class="cellShow">${escapeForHtml(cell.show)}</div><div class="cellRaw">${cell.value}</div>`;
  d.id = `${cell.id}`;
  d.classList.add('memCell');
  return d;
}
function memArrayDiv(array) {
  let d = document.createElement('div');
  d.classList.add('memArray');
  for (let cell of array) { d.appendChild(memCellDiv(cell)); }
  return d;
}
function point(pointed, next) {
  let p = document.getElementById(pointed);
  let n = document.getElementById(next);
  if (n) n.scrollIntoViewIfNeeded();
  if (p) p.scrollIntoViewIfNeeded();
  if (p) p.classList.add('pointed');
  if (n) n.classList.add('pointNext');
}
function unpoint(pointed, next) {
  let p = document.getElementById(pointed);
  let n = document.getElementById(next);
  if (p) p.classList.remove('pointed');
  if (n) n.classList.remove('pointNext');
}
function startCellDiv(cell) {
  let d = document.createElement('div');
  d.classList.add('startCell');
  let [i, w, start, pointed, next] = cell;
  d.innerHTML = `<div class="cellIndex">${i}</div><div class="cellShow">${start}</div><div class="cellRaw">${w}</div>`;
  d.addEventListener('mouseover', () => { point(pointed, next); });
  d.addEventListener('mouseout', () => { unpoint(pointed, next) });
  return d;
}
function startArrayDiv(array) {
  let d = document.createElement('div');
  d.classList.add('startArray');
  for (let cell of array) { d.appendChild(startCellDiv(cell)); }
  return d;
}

function addDivs(elt) {
  for (let i = 0; i < 5; ++i) {
    elt.appendChild(memArrayDiv(pretty_array("t" + i, t[i])));
  }
  elt.appendChild(startArrayDiv(pretty_start(ts, 5)));
}
addDivs(document.getElementById('whereToAddDivs'));
