# Notes on using GDB and Python

(I keep forgetting, so better write down something so I don't have to keep re-reading the GDB manual.)

## Without Python

Firstly, some amount of scripting is possible in GDB itself, without Python. 
In fact it's enough for doing a fair bit, because it includes the basic "survival kit" 
(term from [here](https://www.adacore.com/gems/gem-119-gdb-scripting-part-1)) for a language:

- set variables with `set`, e.g. `set $myfoo = 42` (called "convenience variables", presumably to distinguish from variables in the program itself).

- conditionals with `if ... [else ..] end`

- loops with `while ... ... end`

- print with `print`, `output`, `printf`

- define functions with `define ... end`

- put them into a file and run with `-x`, e.g. `gdb -x ~/.gdbinit-tangle`

## With Python

The way to use Python and GDB together is to run Python code after starting GDB.

More specfically, within GDB, you can type `python-interactive` and get a Python shell.
This is similar to the one you get by running `python`, except that 
a few modules like `sys`, `os`, `re` and `itertools` are pre-imported, 
the main one being `gdb`.

### Main stuff

This `gdb` module makes some things available from the Python code:

 - `gdb.parse_and_eval(expression)`.
 
    Here, the "expression" is a C expression if a C program is being debugged. The return is a `gdb.Value` object. call `str` or `int` on it.
    
 - `gdb.breakpoints()` and `gdb.rbreak(regex)`
   
    List of breakpoints that have been set (possibly restricted to those matching `regex`).
    Each is a `gdb.Breakpoint` object; more on this later.
    
### Working with `Value`s

 - For simple scalars, just use them. Use `str(v)` or `int(v)` etc to get its value.
 
 - For structs / class instances, if `my_obj` is held in `myObjVal`, get the value of `my_obj.member` with `myObjVal['member']`.
 
 - The values have types. Use `myVal.type` to get the `gdb.Type` type of `myVal`. (But really, don't you know the type already?)
 
 - When examining a type (a `gdb.Type` instance) `myType`, some useful information may 
   be in `mytype.name()` and (if not a simple type) in `myType.fields()`.

 - If you want to print better, you can write a pretty printer whose `to_string` will get called on `str`. The main thing this gives,
   compared to just running your printing code directly on the value, is that
       (1) when printing to the interactive gdb terminal, gdb can do better if it's told things like whether
           the value is an array-like one (plus the default "print" will work),
       (2) you can use other printers (e.g. those of children) easily (just `str` on the children), without having to look them up.
       
### Working with the being-debugged ("inferior") program itself

- Use `gdb.selected_inferior()` to get the program, of type `gdb.Inferior`.

- If you have an `inferior` of type `gdb.Inferior`, on it call `inferior.read_memory(address, length)` to get 
  a buffer holding that memory. There are also `write_memory` and `search_memory`.

- Can register event handlers, e.g. `gdb.events.exited.connect(my_handler)` where `my_handler` is a function taking `event`.
  These are the event "registries" in which it's worth registering:
  
  - `events.cont` includes `gdb.ContinueEvent`.

  - `events.stop` includes `gdb.BreakpointEvent`.
  
  - `events.inferior_call` includes `gdb.InferiorCallPreEvent` (a function is about to be called) and `gdb.InferiorCallPostEvent` (just called).
  
  - There's something called "recording". Looks very powerful if you can get it to work.

    - `gdb.start_recording("full")` or `gdb.start_recording("btrace", "pt")` (for example)

    - `gdb.current_recording()` returns a `gdb.Record` object, which has members like `function_call_history`.
       To me trying to get these members of this record object either returns a NotImplementedError (if full) or 
       causes gdb to crash with a segmentation fault.
       
### Working with stack frames

 - `gdb.selected_frame()`, `gdb.newest_frame()` return a `gdb.Frame` object. 
   This has methods like `.name()`, `.type()` (e.g. inlined), `.block()` (the code block),
   `.function()` (the name of the function), `.older()`, `.newer()`, and `.find_sal()` (symtab and line).
   
 - **Block**: Once having found the block from the frame, can look at its attributes like `.function` (the name, or None),
   `.superblock` (parent / enclosing block), and `.global_block` or `.is_global`.
   
 - **Symbols**: Can get up symbols with `gdb.lookup_global_symbol(name)`. Returns a `gdb.Symbol` object, which
   has attributes like `.type`, `.symtab` (in which symbol table does it appear), `.line`, `.name`, `.is_argument`, `.is_constant`,
   `.is_function`, `.is_variable`, and most usefully, `.value()` or `.value(frame)` depending on whether it's global or local.
   
 - **Symtab and line**, line tables: (Read up on this later.)
 
### Working with **breakpoints**

 - Can create one with `gdb.Breakpoint(breakpoint_spec,
                                       breakpoint_type=gdb.BP_BREAKPOINT or gdb.BP_WATCHPOINT,
                                       watchpoint_class=gdb.WP_READ or gdb.WP_WRITE or gdb.WP_ACCESS,
                                       is_internal=False or True,
                                       is_temporary=False or True,
                                       spec_is_qualified=False or True)` or source, function, label, line.
 
 - Can redefine `.stop(self)` on the breakpoint, to change what happens when the breakpoint is hit. 
   (If it returns False, the program won't be stopped unless of course there's another breakpoint at the same place which makes it stop.)

 - Can access `.is_valid()` and `.delete()` and various other (writable!) variables like `.commands`, `.condition`, `.silent`, `.enabled`
 
 - A `gdb.FinishBreakpoint` is one where you'll be after a `finish` command. It has `.return_value` which is a `gdb.Value` value.
 
### Simulating typing stuff into the GDB terminal

 - `gdb.execute(command, from_tty=False, to_string=False)`

    Runs the command `command` as if it was typed into the interactive GDB session. 
    So in principle you could just use `gdb.execute` from Python and never type anything directly into GDB :-)
    
 - `gdb.parameter(name)` and `gdb.convenience_variable(name)`.
 
    Gets the value of an internal GDB setting. E.g. after `gdb.execute('set print elements 500')` is run, 
    `gdb.parameter('print elements')` returns 200. And after `gdb.execute('save $myfoo = 42')` or equivalently 
    `gdb.set_convenience_variable('myfoo', 42)` is run, `gdb.convenience_variable('myfoo')` returns 42. 
    But obviously you could just save a Python variable with that value.
    
 - There's a way to change what type GDB shows a value as; look up "Type Printer".
 
 - There's a way to change what frames GDB prints when it runs `backtrace` (`bt`) and similar commands.

 - Can define new commands for use from the GDB CLI -- just subclass from the `gdb.Command` class and your `invoke` will be called.
   Similarly new parameters (`gdb.Parameter`) and new functions (`gdb.Function`).
 
