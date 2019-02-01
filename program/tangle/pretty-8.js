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

let t = pooltypeMem.t;
let ts = pooltypeMem.ts;

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

function addDivsBetter(elt) {
  // Directly compute all the texts and write them out.
  const zz = t.length;
  for (let i = 0; i < ts.length; ++i) {
    let z = i % zz;
    // Text i occupies positions ts[i] to ts[i + zz], in t[z].
    if (i + zz < ts.length) {
      let s = [];

      let j = ts[i];
      while (j < ts[i + zz]) {
	let [show, len] = token_show(t[z], j);
	s.push(show);// console.log('Pushed token: ', show, ' from position ', j, ' of ', z, 'for text', i);
	j += len;
      }
      s = s.join(' ');
      let d = document.createElement('div');
      d.classList.add('tokensrow');
      d.innerHTML = " " + i + ": " + escapeForHtml(s);
      elt.appendChild(d);
    }
  }
}
addDivsBetter(document.getElementById('whereToAddDivsBetter'));
