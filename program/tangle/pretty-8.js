const t = pooltypeMem.t;
const ts = pooltypeMem.ts;
const names = pooltypeMem.names;
const texts = pooltypeMem.texts;

function tokMemArrays(elt) {
    elt.classList.add('vbox');
    for (let i = 0; i < 5; ++i)
        elt.appendChild(memArrayDiv("t" + i, t[i]));
    elt.appendChild(startArrayDiv(ts, t.length, "t"));
}
tokMemArrays(document.getElementById('tokMemArrays'));

tokMemListTexts(document.getElementById('tokMemListTexts'));

tokMemListTextsResolved(document.getElementById('tokMemListTextsResolved'));
