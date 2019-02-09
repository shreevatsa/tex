const t = pooltypeMem.t;
const ts = pooltypeMem.ts;
const names = pooltypeMem.names;
const texts = pooltypeMem.texts;

function addDivs(elt) {
    for (let i = 0; i < 5; ++i)
        elt.appendChild(memArrayDiv("t" + i, t[i]));
    elt.appendChild(startArrayDiv(ts, t.length, "t"));
}
addDivs(document.getElementById('whereToAddDivs'));

function addDivsBetter(elt) {
    elt.classList.add('vbox');
    // Directly compute all the texts and write them out.
    const zz = t.length;
    if (ts.length != texts.length + zz) throw "Internal error: Unexpected lengths: " + ts.length + " " + texts.length + " + " + zz;
    for (let i = 1; i < texts.length; ++i) {
        let text = texts[i];
        let cells = document.createElement('div'); cells.classList.add('hbox');
        for (let j = 0; j < text.length; ++j) {
            let token = text[j];
            let cell = document.createElement('div'); cell.classList.add('memCell');
            let cellType = document.createElement('div'); cellType.classList.add('cellType'); cellType.innerHTML = escapeForHtml(token.type);
            let cellValue = document.createElement('div'); cellValue.classList.add('cellValue'); cellValue.innerHTML = escapeForHtml('' + token.value);
            cell.appendChild(cellType);
            cell.appendChild(cellValue);
            cells.appendChild(cell);
        }
        let d = document.createElement('div');
        d.classList.add('tokensrow'); d.classList.add('hbox');
        d.innerHTML = " " + i + ": " + cells.innerHTML;
        elt.appendChild(d);
    }
}
addDivsBetter(document.getElementById('whereToAddDivsBetter'));

function addDivsEvenBetter(elt) {
    const zz = t.length;
    if (ts.length != texts.length + zz) throw "Internal error: Unexpected lengths: " + ts.length + " " + texts.length + " + " + zz;
    for (let i = 1; i < texts.length; ++i) {
        let text = texts[i];
        let cells = document.createElement('div'); cells.classList.add('hbox');
        for (let j = 0; j < text.length; ++j) {
            let token = text[j];
            let cell = document.createElement('div'); cell.classList.add('memCell');
            let cellType = document.createElement('div'); cellType.classList.add('cellType');
            let cellValue = document.createElement('div'); cellValue.classList.add('cellValue');
            cellType.innerHTML = escapeForHtml(token.type);
            let valueStr = escapeForHtml('' + token.value);
            if (token.type == 'Name@' || token.type == 'Module@') {
                cellType.innerHTML += valueStr;
                cellValue.innerHTML = names[token.value];
            } else {
                if (token.type == 'Module#') valueStr = '{' + valueStr + '}';
                cellValue.innerHTML = valueStr;
            }
            cell.appendChild(cellType);
            cell.appendChild(cellValue);
            cells.appendChild(cell);
        }
        let d = document.createElement('div'); d.classList.add('hbox');
        d.classList.add('tokensrow'); d.classList.add('hbox');
        d.innerHTML = " " + i + ": " + cells.innerHTML;
        elt.appendChild(d);
    }
}
addDivsEvenBetter(document.getElementById('whereToAddDivsEvenBetter'));
