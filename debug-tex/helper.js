function strFromArray(arr) {
    let s = '';
    for (let c of arr) {
        s += String.fromCharCode(c);
    };
    return s;
}


// Make sure to get https://vuejs.org/js/vue.js and load it first
Vue.component('buffer-table-row-one', {
    props: ['textc'],
    template: '<tr><td v-for="c of textc"><span :title="c">{{c == 32 ? "&nbsp;" : String.fromCharCode(c)}}</span></td></tr>',
});
Vue.component('buffer-table-row-two', {
    props: ['start', 'loc', 'limit'],
    computed: {
        colspant: function() {
            return this.loc - this.start;
        },
    },
    template: '<tr><td :colspan="colspant" v-if="colspant > 0"></td><td align="center">↑</td></tr>',
});
Vue.component('buffer-table', {
    props: ['text', 'start', 'loc', 'limit'],
    template: '<tt><table border=1><buffer-table-row-one :textc="text"/><buffer-table-row-two :start="start" :loc="loc" :limit="limit"/></table></tt>',
});


Vue.component('single-token', {
    props: ['token'],
    methods: {
        strFromArray: strFromArray,
    },
    template: `
      <span v-if="'noncsToken' in token" :title="token.noncsToken[0]">{{String.fromCharCode(token.noncsToken[1])}}<sub>{{token.noncsToken[0]}}</sub></span>
      <span v-else-if="'active_character' in token" title="active character">{{strFromArray(token.active_character)}}</span>
      <span v-else-if="'control_sequence' in token" title="control sequence">\\{{strFromArray(token.control_sequence)}}</span>
      <span v-else :title="token">Unknown token type {{token}} with keys {{token.keys()}} like {{token.keys()[0]}}</span>
    `,
});

Vue.component('token-table-row-one', {
    props: ['tokens'],
    // For good version: reproduce print_cmd_chr, starting section 298
    template: '<tr><td v-for="c of tokens"><single-token :token="c"/></td></tr>',
});
Vue.component('token-table-row-two', {
    props: ['start', 'loc', 'tokens'],
    computed: {
        colspan: function() {
            return this.loc == 'NULL' ? this.tokens.length : this.loc - this.start;
        },
    },
    template: '<tr><td :colspan="colspan" v-if="colspan > 0"></td><td align="center">↑</td></tr>',
});
Vue.component('token-table', {
    props: ['tokens', 'start', 'loc'],
    template: '<tt><table border=1><token-table-row-one :tokens="tokens"/><token-table-row-two :start="start" :loc="loc" :tokens="tokens"/></table></tt>',
});

Vue.component('input-state-row', {
    props: ['statefield', 'indexfield', 'startfield', 'locfield', 'limitfield', 'namefield', 'rownumber', 'text', 'tokens', 'filename'],
    methods: {
        strFromArray: strFromArray,
    },
    template: `
 <tbody>
  <tr class="meanings" v-if="statefield == 0">
    <td rowspan=2>{{rownumber}}</td>

    <td class="statefield2">token list</td>

    <td class="indexfield2" v-if="indexfield == 3">token list type: 3 (= backed up)</td>
    <td class="indexfield2" v-else-if="indexfield == 4">token list type: 4 (= inserted)</td>
    <td class="indexfield2" v-else>unknown token list type {{indexfield}}</td>

    <td class="startfield2" colspan=2><token-table :tokens=tokens :start=startfield :loc="locfield"/></td>

    <td class="limitfield2" v-if="indexfield == 5">macro, parameters start at {{limitfield}}</td>
    <td class="limitfield2" v-else>(ignore, not a macro)</td>

    <td class= "namefield2" v-if="indexfield == 5">macro, eqtb address is {{namefield}}</td>
    <td class= "namefield2" v-else>(ignore, not a macro)</td>
  </tr>
  <tr class="meanings" v-else>
    <td rowspan=2>{{rownumber}}</td>

    <td class="statefield1" v-if="statefield == 1">scanner state: MID_LINE</td>
    <td class="statefield1" v-else-if="statefield == 17">scanner state: SKIP_BLANKS</td>
    <td class="statefield1" v-else-if="statefield == 33">scanner state: NEW_LINE</td>
    <td class="statefield1" v-else>Unknown scanner state {{statefield}}</td>

    <td class="indexfield1" v-if="indexfield == 0">open-files depth: (reading from terminal)</td>
    <td class="indexfield1" v-else>open-files depth: {{indexfield}}</td>

    <td class="startfield1" colspan=3><buffer-table :text=text :start=startfield :loc=locfield :limit="limitfield"/></td>

    <td class= "namefield1" v-if="namefield == 0">filename: (reading from terminal)</td>
    <td class= "namefield1" v-else-if="namefield <= 16">filename: (reading from input stream {{namefield - 1}})</td>
    <td class= "namefield1" v-else-if="namefield == 17">filename: (reading from terminal with read_toks)</td>
    <td class= "namefield1" v-else>filename: {{namefield}} (= <tt>{{strFromArray(filename)}}</tt>)</td>
  </tr>
  <tr class="values">
    <td class="statefield1">{{statefield}}</td>
    <td class="indexfield1">{{indexfield}}</td>
    <td class="startfield1">{{startfield}}</td>
    <td class=  "locfield1">{{locfield}}</td>
    <td class="limitfield1">{{limitfield}}</td>
    <td class= "namefield1">{{namefield}}</td>
  </tr>
 </tbody>`,
});

Vue.component('input-state-record', {
    props: ['instaterecord', 'rownumber'],
    template: `
      <input-state-row v-if="instaterecord.statefield == 0"
          :rownumber="rownumber"
          :statefield="instaterecord.statefield"
          :indexfield="instaterecord.indexfield"
          :startfield="instaterecord.startfield"
          :locfield="instaterecord.locfield"
          :limitfield="instaterecord.limitfield"
          :namefield="instaterecord.namefield"
          :tokens="instaterecord.tokens"
      />
      <input-state-row v-else
          :rownumber="rownumber"
          :statefield="instaterecord.statefield"
          :indexfield="instaterecord.indexfield"
          :startfield="instaterecord.startfield"
          :locfield="instaterecord.locfield"
          :limitfield="instaterecord.limitfield"
          :namefield="instaterecord.namefield"
          :text="instaterecord.buffertext"
          :filename="instaterecord.filename"
      />
    `,
});

let inputStateComponent = Vue.component('input-state', {
    props: ['instaterecords'],
    template: `
<table border=1 id="full-table">
 <input-state-record v-for="(instaterecord, index) in instaterecords" :rownumber="index" :instaterecord="instaterecord"/>
</table>
    `,
});

var vm = new inputStateComponent({
    el: '#input-state-table',
    data: {
        'gdbDumpIndex': 0,
        'instaterecords': dumped_from_gdb[0],
    },
    methods: {
        setGdbDumpIndex: function(n) {
            this.gdbDumpIndex = n;
            this.instaterecords = dumped_from_gdb[n];
        },
    },
});

// Vue.config.productionTip = false;
Vue.config.debug = true;

document.getElementById('timeSlider').max = dumped_from_gdb.length;
document.getElementById('totalTime').textContent = dumped_from_gdb.length;
document.getElementById('timeSlider').onchange = event => {
    vm.setGdbDumpIndex(event.target.value);
    document.getElementById('currentTime').textContent = event.target.value;
}
