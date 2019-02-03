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


let b = pooltypeMem.b;
let bs = pooltypeMem.bs;
let names = pooltypeMem.names;
let texts = poolTypeMem.texts;

// For a cell (string index / char), output its [index, value, show, id]
function pretty_cell(id_prefix, s, n) {
  let ret = {};
  ret["index"] = n;
  let c = s.charCodeAt(n);
  ret["value"] = two_hex(c);
  if (c < 32 || c >= 127) throw "Decide what to do with unprintable characters like: " + c;
  ret["show"] = s[n];
  ret["id"] = id_prefix + "_" + n;
  return ret;
}
// The above for each cell in an array
function pretty_array(id_prefix, s) {
  let ret = [];
  for (let i = 0; i < s.length; ++i) ret.push(pretty_cell(id_prefix, s, i));
  ret.push({index: s.length, value: 'NO', show: 'NOPE', id: id_prefix + "_" + s.length});
  return ret;
}
// For each cell in the start array, output its [index, array_num, start_index, id, start_id]
function pretty_start(start, m) {
  let ret = [];
  for (let i = 0; i < start.length; ++i) {
    let w = i % m;
    let tmp = [i, w, start[i], "b" + w + "_" + start[i], "b" + w + "_" + start[i+m]];
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
  d.innerHTML = `<div class="cellIndex">${cell.index}</div><div class="cellShow">${cell.show}</div><div class="cellRaw">${cell.value}</div>`;
  d.id = `${cell.id}`;
  d.classList.add('memCell');
  return d;
}
function memArrayDiv(array) {
  let d = document.createElement('div');
  d.classList.add('memArray'); d.classList.add('hbox');
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
  d.classList.add('startArray'); d.classList.add('hbox');
  for (let cell of array) { d.appendChild(startCellDiv(cell)); }
  return d;
}


function addDivs(elt) {
  for (let i = 0; i < b.length; ++i) {
    elt.appendChild(memArrayDiv(pretty_array("b" + i, b[i])));
  }
  elt.appendChild(startArrayDiv(pretty_start(bs, 3)));
}
addDivs(document.getElementById('whereToAddDivs'));

function addDivsBetter(elt) {
  // Directly compute all the strings and write them out.
  for (let i = 0; i < names.length; ++i) {
    let s = names[i].join('');
    let d = document.createElement('div');
    d.innerHTML = " " + i + ": " + escapeForHtml(s);
    elt.appendChild(d);
  }
}
addDivsBetter(document.getElementById('whereToAddDivsBetter'));
