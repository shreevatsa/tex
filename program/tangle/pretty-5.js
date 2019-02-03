const names = pooltypeMem.names;

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
  for (let i = 0; i < pooltypeMem.b.length; ++i) {
    elt.appendChild(memArrayDiv(pretty_array("b" + i, pooltypeMem.b[i])));
  }
  elt.appendChild(startArrayDiv(pretty_start(pooltypeMem.bs, 3)));
}
addDivs(document.getElementById('whereToAddDivs'));

function addDivsBetter(elt) {
  // Directly compute all the strings and write them out.
  for (let i = 0; i < names.length; ++i) {
    let s = names[i];
    let d = document.createElement('div');
    d.innerHTML = " " + i + ": " + escapeForHtml(s);
    elt.appendChild(d);
  }
}
addDivsBetter(document.getElementById('whereToAddDivsBetter'));
