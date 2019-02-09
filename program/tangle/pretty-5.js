let elt = document.getElementById('whereToAddDivs');
for (let i = 0; i < pooltypeMem.b.length; ++i)
    elt.appendChild(byteArrayDiv("b" + i, pooltypeMem.b[i]));
elt.appendChild(startArrayDiv(pooltypeMem.bs, 3, "b"));

const names = pooltypeMem.names;
for (let i = 0; i < names.length; ++i) {
    let d = document.createElement('div');
    d.innerHTML = `${i}: <code>${escapeForHtml(names[i])}</code>`;
    document.getElementById('whereToAddDivsBetter').appendChild(d);
}