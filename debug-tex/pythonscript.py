"""
Script for controlling a gdb process that is running tex, and dumping data about tex's workings.

See `do.sh` for how this is called:

    gdb --eval-command 'source pythonscript.py' --args /home/shreevatsa/build-tex/texlive/full/Master/bin/x86_64-linux/tex expandafter.tex
    # The `--args` makes the filename (`expandafter.tex`) be passed to the program (`tex`) instead of to gdb.

Guiding principle: Generate data, not formatting.
- Do nothing related to presenting the information in a useful way on the terminal.
- Optimize for usable consumption of data by another program.
- If the interpretation of data can be done by another program, then leave it to that program.
- If the interpretation of data *requires* further data that is available only via gdb and not in *TeX: The Program* (or even web2c sources/output), then dump that too.

"""

from __future__ import division
from __future__ import print_function
import json

# # Old way, when the commandline only had `gdb --eval-command 'source pythonscript.py'
# TEX_BINARY = '/home/shreevatsa/build-tex/texlive/full/Master/bin/x86_64-linux/tex'
# TEX_INPUT = '/home/shreevatsa/debug-tex/expandafter.tex'
# COMMAND = '%s %s' % (TEX_BINARY, TEX_INPUT)
# gdb.execute(COMMAND)

out = open('py-gdb-output.txt', 'w')
out.write('dumped_from_gdb = ')  # A hack! Printed before the `json.dump`.
to_be_written_out = []           # The value whose JSON representation is finally written out.

# Used in showTokenList
NULL = -268435455  # TeX in §115 defines it as min_halfword which in §110 is defined as 0, but in web2c it's -0xFFFFFFF = -2^28 + 1.

# Used in printInStateRecord, to decide how to call showTokenList
MACRO = 5          # Token type, defined in §307

# Used in showToken to decide whether something is a "simple" token or needs printCs
CS_TOKEN_FLAG = 4095 # 07777 = 0xfff = 2^12-1, defined in §289

# The command codes are defined in §207–§210

# Some state for tracking how many times the `expand` procedure has been (recursively) called -- don't know if there's an easier way in gdb
expand_call_stack = []

def is_json_serializable(x):
    try:
        return x == json.loads(json.dumps(x))
    except:
        print('Could not serialize ', x)
        return False

def ret_json(func):
    def real(*args, **kwargs):
        ret = func(*args, **kwargs)
        try:
            assert ret == json.loads(json.dumps(ret)), func
            return ret
        except:
            print('Could not serialize ', ret, ' when returning ', func)
            return ''
    return real

@ret_json
def gettexstring(n):
    """Turns a 'string number' in tex into an actual string of characters."""
    # return gdb.parse_and_eval('gettexstring(%s)' % n)
    # ^ Calling the program's function would be more accurate, as a different function is compiled in the case of Aleph, XeTeX etc.
    # But that function malloc-s, so I'm hesitant to call it here, as it may alter program behaviour.
    start = int(gdb.parse_and_eval('strstart[%d]' % n))
    end = int(gdb.parse_and_eval('strstart[%d]' % (n + 1)))
    return [int(gdb.parse_and_eval('strpool[%d]' % i)) for i in range(start, end)]

def link(node):
    # Replacement for TeX define in §118: link(#) ≡ mem[#].hh.rh
    return gdb.parse_and_eval('mem[%s].hh.v.RH' % node)

def info(node):
    # Replacement for TeX define in §118: info(#) ≡ mem[#].hh.lh
    return gdb.parse_and_eval('mem[%s].hh.v.LH' % node)

def text(node):
    # Replacement for TeX define in §256: text(#) ≡ hash[#].rh { string number for control sequence name }
    return gdb.parse_and_eval('hash[%s].v.RH' % node)

@ret_json
def printCs(p):
    """print_cs, module §262 = zprintcs in tex0.c { prints a purported control sequence }

    Currently returns one of:
      {'active_character': [char]}
      {'control_sequence': [char]}
      {'control_sequence': []}
      {'control_sequence': [char, char, ...]}
"""
    # gdb.execute('p zprintcs(%s)' % p)
    HASH_SIZE = 2100  # Defined in §12, probably changes in e-TeX!!
    ACTIVE_BASE = 1                                     # §222
    SINGLE_BASE = 257                                   # §222
    NULL_CS = 513                                       # §222
    HASH_BASE = 514                                     # §222
    FROZEN_CONTROL_SEQUENCE = HASH_BASE + HASH_SIZE     # §222
    FROZEN_NULL_FONT = FROZEN_CONTROL_SEQUENCE + 10     # §222
    UNDEFINED_CONTROL_SEQUENCE = FROZEN_NULL_FONT + 257 # §222
    if p < HASH_BASE:  # Single character
        if p >= SINGLE_BASE:
            # A single-character control sequence, like \a
            if p == NULL_CS:
                return {'control_sequence': []}
            else:
                ret = gettexstring(p - SINGLE_BASE)
                return {'control_sequence': ret}
        else:
            # A single-character active character, like ~
            assert p >= ACTIVE_BASE, 'p >= ACTIVE_BASE'
            ret = gettexstring(p - ACTIVE_BASE)
            return {'active_character': ret}
    else:  # Not a single character
        # assert p <= UNDEFINED_CONTROL_SEQUENCE, ('p <= UNDEFINED_CONTROL_SEQUENCE', p, UNDEFINED_CONTROL_SEQUENCE)
        # ^ Changed in one of the change files, probably tex-final.ch :-(
        assert text(p) >= 0 and text(p) <= gdb.parse_and_eval('strptr'), 'text(p) >= 0 etc.'
        return {'control_sequence': gettexstring(text(p))}

@ret_json
def showTokenActual(t):
    assert t >= 0
    if t >= CS_TOKEN_FLAG:  # CS_TOKEN_FLAG is defined in §289
        return printCs(t - CS_TOKEN_FLAG)
    (m, c) = (t // 256, t % 256)
    return {'noncsToken': [m, c]}


@ret_json
def showToken(p):
    """Module §293 ⟨Display token p⟩

    Returns either {'noncsToken': [m, c]} or one of the possible return values of printCs."""
    if p < gdb.parse_and_eval('himemmin') or p > gdb.parse_and_eval('memend'):
        assert False, 'CLOBBERED: %s' % p
    t = int(info(p))
    return showTokenActual(t)

@ret_json
def showTokenList(start, loc):
    """Module §292 { prints a symbolic form of the token list that starts at a given node p }

    `start` is `p` in the TeX program, and `loc` is `q` and we're not currently using it."""
    p = start
    tokens = []
    while p != NULL:
        tokens.append({
            'location': int(p),
            'token': showToken(p)
            })
        p = link(p)
    return tokens

@ret_json
def printInStateRecord(x):
    """Prints an 'instaterecord' (input state record) -- the state of input at a given level of the input stack."""
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
        startNode = x['startfield']
        tokenList = showTokenList(startNode if x['indexfield'] < MACRO else link(startNode), x['locfield'])
        return {                                  # See §307 for an explanation of these
            'statefield': int(x['statefield']),   # is just going to be 0, indicating this is a token list
            'indexfield': int(x['indexfield']),   # tokenListType
            'startfield': int(x['startfield']),   # startNode
              'locfield': int(x['locfield']),     # currentNodeLoc
            'limitfield': int(x['limitfield']),   # where params start, if MACRO
             'namefield': int(x['namefield']),    # where in eqtb, if MACRO
                'tokens': tokenList,              # the actual tokens in the token list!
        }
    else:
        start = int(x['startfield'])
        limit = int(x['limitfield'])
        buffer_slice = [int(gdb.parse_and_eval('buffer[%d]' % i)) for i in range(start, limit + 1)]
        return {                                # See §303 for an explanation of these
            'statefield': int(x['statefield']), # Scanner state
            'indexfield': int(x['indexfield']), # Index (files open depth)
            'startfield': int(x['startfield']), # where current line starts in buffer
              'locfield': int(x['locfield']),   # next char to read in buffer (or > limit meaning buffer is read)
            'limitfield': int(x['limitfield']), # where current line ends in buffer
             'namefield': int(x['namefield']),  # file name
              'filename': gettexstring(x['namefield']),
            'buffertext': buffer_slice,
        }


def dump_context():
    context = []
    for i in range(int(gdb.parse_and_eval('inputptr'))):
        context.append(printInStateRecord(gdb.parse_and_eval('inputstack[%d]' % i)))
    context.append(printInStateRecord(gdb.parse_and_eval('curinput')))
    try:
        assert is_json_serializable(context)
        return context
    except Exception as e:
        print('Could not serialize context, got exception: ', e)
        for c in context:
            print(c)

'''
What does a frame contain?

Frame
- architecture(): (gdb.Architecture)
  - name(): 'i386:x86-64'
  - disassemble(start_pc [, end_pc [, count]]) -> Return a list of at most COUNT disassembled instructions from START_PC to END_PC.
- name(): 'expand'
- function():
  - name: 'expand'
- block() [iterable: gdb.Symbol]
  - sym.value(frame): (gdb.Value)

- find_sal()
  - symtab: (gdb.Symtab)
    - static_block()
- is_valid
- newer()
- older()
- pc
- read_register
- read_var
- select
- type(): 0
- unwind_stop_reason(): 0 (Return the reason why it is not possible to find frames older than this.)
'''

def frames():
    ret = []
    frame = gdb.selected_frame()
    while frame is not None and frame.is_valid():
        name = frame.name()
        local_vars = []
        # This is basically equivalent to `gdb.execute('info locals')`, but later we can probably be smarter using the types
        for sym in frame.block():
            if sym.type.name == '__CORE_ADDR':   # Bug in gdb probably worth reporting
                """A bug in gdb, causes:
/build/gdb-sBS5Fz/gdb-7.12.50.20170314/gdb/findvar.c:247: internal-error: store_typed_address: type is not a pointer or reference
A problem internal to GDB has been detected,
further debugging may prove unreliable.
Quit this debugging session? (y or n)
                """
                continue
            value = sym.value(frame)
            # Hack!
            if sym.name == 't' and name == 'expand':
                local_vars.append((sym.name, sym.type.name, str(value), showTokenActual(int(value))))
            else:
                local_vars.append((sym.name, sym.type.name, str(value)))
            pass
        ret.append({'name': name, 'locals': local_vars})
        frame = frame.older()
    return ret

class BpExpand(gdb.Breakpoint):
    """A Breakpoint to be set on the expand function."""
    def stop(self):
        # gdb.write('\n expand function called\n')
        expand_call_stack.append('(')
        return False

class BpExpandStartOrEnd(gdb.Breakpoint):
    """A Watchpoint to be set on the 'expanddepthcount' variable (tracking function expand() enter and exit)."""
    def stop(self):
        if expand_call_stack[-1] == '(':
            # gdb.write('\n expand function entering\n')
            expand_call_stack.append('|')
            enterOrExit = 'enter'
        else:
            assert expand_call_stack[-1] == '|'
            expand_call_stack.pop()
            assert expand_call_stack[-1] == '('
            expand_call_stack.pop()
            # gdb.write('\n expand function exiting\n')
            enterOrExit = 'exit'
        context = dump_context()
        bt = frames()
        to_be_written_out.append({
            'context': context,
            'bt': bt,
            'enterOrExit': enterOrExit,
        })
        return False

BpExpand('tex0.c:expand')
BpExpandStartOrEnd('expanddepthcount', type=gdb.BP_WATCHPOINT)
gdb.execute('set pagination off')
gdb.execute('run')
json.dump(to_be_written_out, out)
out.close()
assert not expand_call_stack, expand_call_stack
gdb.execute('quit')
