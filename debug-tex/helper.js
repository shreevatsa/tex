function makeTableFromBuffer(text, start, loc, limit) {
    let inner = '<table border=1><tr>';
    for (let c of text) {
	inner += '<td>' + c + '</td>';
    }
    inner += '</tr>';
    inner += '<tr><td colspan=';
    inner += loc - start;
    inner += '>';
    inner += '<td>↑</td>';
    inner += '</tr>';
    inner += '</table>';

    const elt = document.createElement('tt');
    // elt.innerHTML = '<table><tr><td>e</td><td>x</td><td>p</td><td>a</td><td>n</td><td>d</td><td>a</td><td>f</td><td>t</td><td>e</td><td>r</td><td>.</td><td>t</td><td>e</td><td>x</td></tr><tr><td>↑</td><td colspan=15></td><td>↑</td></tr></table>';
    elt.innerHTML = inner;
    return elt;
}

function update() {
    // document.getElementById('replace1').appendChild(makeTableFromBuffer('expandafter.tex', 1, 17, 15));
    document.getElementById('replace2').appendChild(makeTableFromBuffer('\\expandafter\\uppercase\\expandafter{a}%', 17, 57, 58));
}

update();

var app = new Vue({
  el: '#replace1',
  data: {
      replace1Html: makeTableFromBuffer('expandafter.tex', 1, 17, 15).outerHTML,
  }
});
