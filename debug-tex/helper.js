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

    return inner;
}

// Make sure to get https://vuejs.org/js/vue.js and load it first
Vue.component('buffer-table', {
    props: ['text', 'start', 'loc', 'limit'],
    render: function(createElement) {
        return createElement('tt', {
            domProps: {
                innerHTML: makeTableFromBuffer(this.text, this.start, this.loc, this.limit),
            },
        });
    }
});

var app = new Vue({
  el: '#full-table',
  data: {
  }
});
