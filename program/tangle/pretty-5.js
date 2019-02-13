const elt = document.getElementById('byteMemArrays');
elt.classList.add('vbox');
for (let i = 0; i < pooltypeMem.b.length; ++i)
    elt.appendChild(byteArrayDiv("b" + i, pooltypeMem.b[i]));
elt.appendChild(startArrayDiv(pooltypeMem.bs, 3, "b"));

byteMemListNames(document.getElementById('byteMemListNames'));

