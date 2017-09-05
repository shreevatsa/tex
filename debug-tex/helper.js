// Make sure to get https://vuejs.org/js/vue.js and load it first
Vue.component('buffer-table-row-one', {
    props: ['textc'],
    template: '<tr><td v-for="c of textc">{{c}}</td></tr>',
});
Vue.component('buffer-table-row-two', {
    props: ['start', 'loc', 'limit'],
    computed: {
        colspant: function() {
            return this.loc - this.start;
        },
    },
    template: '<tr><td :colspan="colspant"></td><td>↑</td></tr>',
});
Vue.component('buffer-table', {
    props: ['text', 'start', 'loc', 'limit'],
    template: '<tt><table border=1><buffer-table-row-one :textc="text"/><buffer-table-row-two :start="start" :loc="loc" :limit="limit"/></table></tt>',
});

Vue.component('input-state-raw-values', {
    props: ['statefield', 'indexfield', 'startfield', 'locfield', 'limitfield', 'namefield'],
    template: `<tr class="values">
    <td class="statefield1">{{statefield}}</td>
    <td class="indexfield1">{{indexfield}}</td>
    <td class="startfield1">{{startfield}}</td>
    <td class=  "locfield1">{{locfield}}</td>
    <td class="limitfield1">{{limitfield}}</td>
    <td class= "namefield1">{{namefield}}</td>
  </tr>`,
});

var app = new Vue({
  el: '#full-table',
  data: {
  }
});

// Vue.config.productionTip = false;
