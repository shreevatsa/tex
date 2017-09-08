function assertPrintable(c) {
    if (!(0 <= c && c <= 255)) {
        alert('Not even a byte in range 0-255: ' + c);
    }
    if (c <= 32 || c >= 127) {
        alert('Unexpected non-printable character ' + c);
    }
}

function charPrintable(c) {
    if (c == 13) { return '↵' };
    if (c == 32) { return ' '; }
    assertPrintable(c);
    return String.fromCharCode(c);
}

function strFromArray(arr) {
    let s = '';
    for (let c of arr) {
        assertPrintable(c);
        s += String.fromCharCode(c);
    };
    return s;
}

function reverseLines(text) {
    const lines = text.split('\n');
    lines.reverse();
    return lines.join('\n');
}

function reversedJoin(arr) {
    let s = '';
    for (let i = arr.length - 1; i >= 0; --i) {
        s += arr[i];
        if (i > 0) s += '\n';
    }
    return s;
}

const NULL = -268435455;

// Make sure to get https://vuejs.org/js/vue.js and load it first
Vue.component('buffer-table', {
    props: ['text', 'start', 'loc', 'limit'],
    methods: {
        charPrintable: charPrintable,
    },
    computed: {
        colspant: function() { return this.loc - this.start; },
    },
    template: `
 <tt>
  <table border=1>
    <tr><td v-for="c of text"><span :title="c">{{charPrintable(c)}}</span></td></tr>
    <tr><td :colspan="colspant" v-if="colspant > 0" align="right">↑</td><td align="center">↑</td></tr>
  </table>
 </tt>
    `,
});

Vue.component('single-token', {
    props: ['tokenAndLocation'],
    computed: {
        token: function() { return this.tokenAndLocation.token; },
    },
    methods: {
        strFromArray: strFromArray,
    },
    // For good version: reproduce print_cmd_chr, starting section 298
    template: `
      <span v-if="'noncsToken' in token" :title="token.noncsToken[0]">{{String.fromCharCode(token.noncsToken[1])}}<sub>{{token.noncsToken[0]}}</sub></span>
      <span v-else-if="'active_character' in token" title="active character">{{strFromArray(token.active_character)}}</span>
      <span v-else-if="'control_sequence' in token" title="control sequence">\\{{strFromArray(token.control_sequence)}}</span>
      <span v-else :title="token">Unknown token type {{token}} with keys {{token.keys()}} like {{token.keys()[0]}}</span>
    `,
});

Vue.component('token-table', {
    props: ['tokens', 'start', 'loc'],
    data: function() {
        return {NULL: NULL};
    },
    template: `
 <tt>
  <table border=1>
   <tr><td v-for="c of tokens"><single-token :tokenAndLocation="c"/></td></tr>
   <tr><td v-for="c of tokens" align="center">{{c.location == loc ? '↑' : ''}}</td><td align="center" v-if="loc == NULL">↑</td></tr>
  </table>
 </tt>
    `,
});

// Token list type: see §307 and §314
Vue.component('input-state-row-token-list', {
    props: {
        'rownumber': Number,
        'statefield': {
            type: Number,
            validator: function(value) { return value == 0; },
        },
        'indexfield': Number,
        'startfield': Number,
        'locfield': Number,
        'limitfield': Number,
        'namefield': Number,
        'tokens': Array,
    },
    data: function() {
        return {NULL: NULL};
    },
    methods: {
        strFromArray: strFromArray,
    },
    template: `
 <table border=1>
  <tr class="meanings">
    <!--<td rowspan=2>{{rownumber}}</td>-->

    <td class="startfield2" colspan=2><token-table :tokens=tokens :start=startfield :loc="locfield"/></td>

    <!--this is redundant info -->
    <!--<td class="statefield2">token list</td>-->

    <td class="indexfield2" v-if="indexfield == 3 && locfield==NULL">token list type: ⟨recently read⟩</td>
    <td class="indexfield2" v-else-if="indexfield == 3">token list type: ⟨to be read again⟩</td>
    <td class="indexfield2" v-else-if="indexfield == 4">token list type: ⟨inserted⟩</td>
    <td class="indexfield2" v-else-if="indexfield == 5">token list type: ⟨macro⟩</td>
    <td class="indexfield2" v-else-if="indexfield == 6">token list type: ⟨output text⟩</td>
    <td class="indexfield2" v-else>unknown token list type {{indexfield}}</td>

    <td class="limitfield2" v-if="indexfield == 5">macro, parameters start at {{limitfield}}</td>

    <td class= "namefield2" v-if="indexfield == 5">macro, eqtb address is {{namefield}}</td>
  </tr>
  <tr class="values">
    <td class="startfield1">{{startfield}}</td>
    <td class=  "locfield1">{{locfield == NULL ? 'NULL': locfield}}</td>
    <!--this is always going to be 0 -->
    <!--<td class="statefield1">{{statefield}}</td>-->
    <td class="indexfield1">{{indexfield}}</td>
    <td class="limitfield1" v-if="indexfield == 5">{{limitfield}}</td>
    <td class= "namefield1" v-if="indexfield == 5">{{namefield}}</td>
  </tr>
 </table>`,
});
Vue.component('input-state-row-non-token-list', {
    props: {
        'rownumber': Number,
        'statefield': {
            type: Number,
            validator: function(value) { return value != 0; },
        },
        'statefield': Number,
        'indexfield': Number,
        'startfield': Number,
        'locfield': Number,
        'limitfield': Number,
        'namefield': Number,
        'text': Array,
        'tokens': Array,
        'filename': Array,
    },
    methods: {
        strFromArray: strFromArray,
    },
    template: `
 <table border=1>
  <tr class="meanings">
    <!--<td rowspan=2>{{rownumber}}</td>-->

    <td class="startfield1" colspan=3><buffer-table :text=text :start=startfield :loc=locfield :limit="limitfield"/></td>

    <td class="statefield1" v-if="statefield == 1">scanner state: MID_LINE</td>
    <td class="statefield1" v-else-if="statefield == 17">scanner state: SKIP_BLANKS</td>
    <td class="statefield1" v-else-if="statefield == 33">scanner state: NEW_LINE</td>
    <td class="statefield1" v-else>Unknown scanner state {{statefield}}</td>

    <td class="indexfield1" v-if="indexfield == 0">open-files depth: (reading from terminal)</td>
    <td class="indexfield1" v-else>open-files depth: {{indexfield}}</td>


    <td class= "namefield1" v-if="namefield == 0">filename: (reading from terminal)</td>
    <td class= "namefield1" v-else-if="namefield <= 16">filename: (reading from input stream {{namefield - 1}})</td>
    <td class= "namefield1" v-else-if="namefield == 17">filename: (reading from terminal with read_toks)</td>
    <td class= "namefield1" v-else>filename: <tt>{{strFromArray(filename)}}</tt></td>
  </tr>
  <tr class="values">
    <td class="startfield1">{{startfield}}</td>
    <td class=  "locfield1">{{locfield}}</td>
    <td class="limitfield1">{{limitfield}}</td>
    <td class="statefield1">{{statefield}}</td>
    <td class="indexfield1">{{indexfield}}</td>
    <td class= "namefield1">{{namefield}}</td>
  </tr>
 </table>`,
});

Vue.component('input-state-record', {
    props: ['instaterecord', 'rownumber'],
    template: `
      <input-state-row-token-list v-if="instaterecord.statefield == 0"
          :rownumber="rownumber"
          :statefield="instaterecord.statefield"
          :indexfield="instaterecord.indexfield"
          :startfield="instaterecord.startfield"
          :locfield="instaterecord.locfield"
          :limitfield="instaterecord.limitfield"
          :namefield="instaterecord.namefield"
          :tokens="instaterecord.tokens"
      />
      <input-state-row-non-token-list v-else
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
    props: ['instaterecords', 'stacktrace'],
    template: `
<div style="display: flex; flex-direction: row;">
 <div style="flex: 1; overflow: scroll;">
  <pre>{{stacktrace}}</pre>
 </div>
 <div style="flex: 15; overflow: scroll;">
  <input-state-record v-for="(instaterecord, index) in instaterecords" :rownumber="index" :instaterecord="instaterecord"/>
 </div>
</div>
    `,
});

var vm = new inputStateComponent({
    el: '#input-state-table',
    data: {
        'gdbDumpIndex': 0,
        'instaterecords': dumped_from_gdb[0]['context'],
        'stacktrace': 'whatever',
    },
    methods: {
        setGdbDumpIndex: function(n) {
            this.gdbDumpIndex = n;
            this.instaterecords = dumped_from_gdb[n]['context'];
            this.stacktrace = reversedJoin(dumped_from_gdb[n]['bt']);
        },
    },
});

// Vue.config.productionTip = false;
Vue.config.debug = true;

function setTime(n) {
    document.getElementById('currentTime').textContent = n;
    // Note these are 0-indexed, so we need the -1
    vm.setGdbDumpIndex(n - 1);
}

document.getElementById('timeSlider').max = dumped_from_gdb.length;
document.getElementById('totalTime').textContent = dumped_from_gdb.length;
document.getElementById('timeSlider').oninput = event => {
    setTime(event.target.value);
}
setTime(1);
