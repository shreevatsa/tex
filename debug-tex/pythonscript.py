from __future__ import division

# TEX_BINARY = '/home/shreevatsa/build-tex/texlive/full/Master/bin/x86_64-linux/tex'
# TEX_INPUT = '/home/shreevatsa/debug-tex/expandafter.tex'
# COMMAND = '%s %s' % (TEX_BINARY, TEX_INPUT)
# gdb.execute(COMMAND)

# out = open('py-gdb-output.txt', 'w')

NULL = -268435455
MACRO = 5
CS_TOKEN_FLAG = 4095 # 07777

expand_call_stack = []

def safe_chr(x):
    ret = chr(x)
    assert ord(ret) in list(range(32, 127))
    return ret

def gettexstring(n):
    # Calling the program's function would be more accurate in face of Aleph, XeTeX etc., but it malloc-s, so I'm unsure...
    # return gdb.parse_and_eval('gettexstring(%s)' % n)
    if n == 0:
        return ''
    start = int(gdb.parse_and_eval('strstart[%d]' % n))
    end = int(gdb.parse_and_eval('strstart[%d]' % (n + 1)))
    return ''.join(chr(gdb.parse_and_eval('strpool[%d]' % i)) for i in range(start, end))

def link(node):
    # Replacement for TeX define: link (#) ≡ mem[#].hh.rh
    return gdb.parse_and_eval('mem[%s].hh.v.RH' % node)

def info(node):
    # Another TeX define: info (#) ≡ mem[#].hh.lh
    return gdb.parse_and_eval('mem[%s].hh.v.LH' % node)

def text(node):
    # Module 256: text (#) ≡ hash[#].rh { string number for control sequence name }
    return gdb.parse_and_eval('hash[%s].v.RH' % node)

def printCs(p):
    """print_cs, module 262. zprintcs in tex0.c"""
    # gdb.execute('p zprintcs(%s)' % p)
    ACTIVE_BASE = 1 # Module 222
    SINGLE_BASE = 257
    NULL_CS = 513
    HASH_BASE = 514
    HASH_SIZE = 2100 # Probably changes in e-TeX!!
    FROZEN_CONTROL_SEQUENCE = HASH_BASE + HASH_SIZE
    FROZEN_NULL_FONT = FROZEN_CONTROL_SEQUENCE + 10
    UNDEFINED_CONTROL_SEQUENCE = FROZEN_NULL_FONT + 257
    if p < HASH_BASE:
        # Single character
        if p >= SINGLE_BASE:
            if p == NULL_CS:
                return '<EMPTY CONTROL SEQUENCE>'
            else:
                ret = gettexstring(p - SINGLE_BASE)
                # if cat_code(p - SINGLE_BASE) == letter: ret += ' '
                return ret
        else:
            assert p >= ACTIVE_BASE, 'p >= ACTIVE_BASE'
            return gettexstring(p - ACTIVE_BASE)
    else:
        # Changed in one of the change files, probably tex-final.ch :-(
        # assert p <= UNDEFINED_CONTROL_SEQUENCE, ('p <= UNDEFINED_CONTROL_SEQUENCE', p, UNDEFINED_CONTROL_SEQUENCE)
        assert text(p) >= 0 and text(p) <= gdb.parse_and_eval('strptr'), 'text(p) >= 0 etc.'
        return gettexstring(text(p))


def showToken(t):
    # Module 293
    assert t == int(t)
    if t >= CS_TOKEN_FLAG:
        # print('Printing control sequence for %d which is greater than %d' % (t, CS_TOKEN_FLAG))
        return r'ControlSequence \%s' % printCs(t - CS_TOKEN_FLAG)
    m = t // 256 # 0400
    c = t % 256 # 0400
    return 'Token (%d,%d%s)' % (m, c, '=' + chr(c) if c > 0 else '')


def showTokenList(start, loc):
    """Module 292"""
    p = start
    token_strs = []
    while p != NULL:
        if p < gdb.parse_and_eval('himemmin') or p > gdb.parse_and_eval('memend'):
            assert False, 'CLOBBERED: %s' % p
        token_strs.append(showToken(int(info(p))))
        p = link(p)
    return ', '.join(token_strs)

def printInStateRecord(x):
    # print(x)
    assert type(x) == gdb.Value
    assert str(x.type) == 'instaterecord'
    assert x.type == gdb.lookup_type('instaterecord')
    t = x.type
    assert t.name == 'instaterecord'
    assert t.code == gdb.TYPE_CODE_TYPEDEF
    field_names = ['statefield', 'indexfield', 'startfield', 'locfield', 'limitfield', 'namefield']
    assert [field.name for field in t.fields()] == field_names
    # for field in t.fields():
    #     value = x[field.name]
    #     # print(field.name, value, field.type, int(value))
    #     print(field.name, int(value))
    TOKEN_LIST = 0
    MID_LINE = 1
    SKIP_BLANKS = 17
    NEW_LINE = 33
    state = int(x['statefield'])
    assert state in {TOKEN_LIST, MID_LINE, SKIP_BLANKS, NEW_LINE}, 'Unexpected state: %s' % state
    if state == TOKEN_LIST:
        tokenListType = int(x['indexfield'])
        startNode = x['startfield']
        currentNodeLoc = x['locfield']
        token_list_str = showTokenList(startNode, currentNodeLoc) if tokenListType < MACRO else showTokenList(link(startNode), currentNodeLoc)
        s = ('<TokenList.' +
             ' TokenListType: %s,' % tokenListType +
             ' StartNode: %s,' % startNode +
             ' CurrentNodeLoc: %s,' % currentNodeLoc +
             ' => [%s], ' % token_list_str +
             ' ParamStart: %s,' % x['limitfield'] +
             ' WhereInEqtb: %s>' % x['namefield'])
    else:
        start = int(x['startfield'])
        limit = int(x['limitfield'])
        loc = int(x['locfield'])
        # TODO: Deal with buffer[loc] being end_line_char (which is 13 = \r, quite annoying)
        if chr(gdb.parse_and_eval('buffer[%d]' % limit)) == '\r':
            limit -= 1
        already_read = ''.join(safe_chr(gdb.parse_and_eval('buffer[%d]' % i)) for i in range(start, min(loc, limit + 1)))
        to_read = ''.join(safe_chr(gdb.parse_and_eval('buffer[%d]' % i)) for i in range(loc, limit + 1))
        s = ('<ScannerState: %s,' % int(x['statefield']) +
             ' Index: %s,' % int(x['indexfield']) +
             ' BufferPositions: %s to %s,' % (start, limit) +
             ' Loc: %s,' % x['locfield'] +
             ' => %s | %s' % (already_read, to_read) +
             ' FileName: %s="%s">' % (x['namefield'], gettexstring(x['namefield'])))
    print(s)

def dump_context():
    for i in range(int(gdb.parse_and_eval('inputptr'))):
        printInStateRecord(gdb.parse_and_eval('inputstack[%d]' % i))
    printInStateRecord(gdb.parse_and_eval('curinput'))

class BpExpand(gdb.Breakpoint):
    def stop (self):
        # gdb.write('\n expand function called\n')
        expand_call_stack.append('(')
        return False

class BpExpandStartOrEnd(gdb.Breakpoint):
    def stop(self):
        if expand_call_stack[-1] == '(':
            expand_call_stack.append('|')
            gdb.write('\n expand function entering\n')
        else:
            assert expand_call_stack[-1] == '|'
            expand_call_stack.pop()
            assert expand_call_stack[-1] == '('
            expand_call_stack.pop()
            gdb.write('\n expand function exiting\n')
        dump_context()
        return False

BpExpand('tex0.c:expand')
BpExpandStartOrEnd('expanddepthcount', type=gdb.BP_WATCHPOINT)
gdb.execute('set pagination off')
gdb.execute('run')
# out.close()
assert not expand_call_stack, expand_call_stack
gdb.execute('quit')
