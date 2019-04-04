import sys
from enum import Enum

"""
Read and parse a .web file.
"""


filename = 'pooltype.web' if len(sys.argv) < 2 else sys.argv[1]

web_file_contents = open(filename).read()
# web_file_content_lines = web_file_contents.splitlines()

"""
The web file can be divided into: a limbo, followed by a list of modules.

Each module starts with "@*" or with "@ ", and ends when the next one begins.
"""

def this_line_is_a_new_module(line):
  return line.startswith('@ ') or line.startswith('@*')

class ModuleCategory(Enum):
  LIMBO = 0
  STARRED = 1
  REGULAR = 2

def split_modules(web_file_contents):
  lines = web_file_contents.splitlines()
  i = 0
  # First comes the limbo
  limbo = []
  while i < len(lines):
    if this_line_is_a_new_module(lines[i]):
      break
    limbo.append(lines[i])
    i += 1
  assert i < len(lines), "This WEB file has only limbo lines?"
  modules = [(limbo, ModuleCategory.LIMBO)]
  while i < len(lines):
    assert this_line_is_a_new_module(lines[i])
    module_category = (ModuleCategory.STARRED if lines[i][1] == '*' else ModuleCategory.REGULAR)
    lines[i] = lines[i][2:]
    cur_module = []
    while i < len(lines):
      cur_module.append(lines[i])
      i += 1
      if i >= len(lines) or this_line_is_a_new_module(lines[i]):
        break
    modules.append((cur_module, module_category))
  return modules

modules = split_modules(web_file_contents)

# for i in [0, 1, # 2, 3, len(modules) - 3, len(modules) - 2,
#           len(modules) - 1]:
#   print(i, modules[i][1], repr('\n'.join(modules[i][0])[:100]))

#================================================================================#
# So far, everything above takes the WEB file and splits it into modules.
# The type of |modules| is a pair (list_of_lines, ModuleCategory)
# Next we split into TeX part, definitions part, Pascal part

def split_module(lines):
  tex_part = []
  definition_part = []
  pascal_part = []
  i = 0

  # TeX part
  while i < len(lines):
    if lines[i].startswith('@d'): break
    if lines[i].startswith('@f'): break
    if lines[i].startswith('@p'): break
    if lines[i].startswith('@<'): break
    tex_part.append(lines[i])
    i += 1
  if i == len(lines):
    return (tex_part, definition_part, pascal_part)

  # Definition part, if it exists
  if lines[i].startswith('@d') or lines[i].startswith('@f'):
    while i < len(lines):
      if lines[i].startswith('@p'): break
      if lines[i].startswith('@<'): break
      # assert lines[i] == '' or lines[i].startswith('@d') or lines[i].startswith('@f'), ('#%s#' % lines[i], '\n'.join(lines))
      definition_part.append(lines[i])
      i += 1
  if i == len(lines):
    return (tex_part, definition_part, pascal_part)

  # Pascal part, if we're stil here.
  assert lines[i].startswith('@p') or lines[i].startswith('@<')
  while i < len(lines):
    pascal_part.append(lines[i])
    i += 1
  return (tex_part, definition_part, pascal_part)


split_modules = [split_module(module[0]) for module in modules]

for i in [0, 1, 2, 3, len(modules) - 3, len(modules) - 2, len(modules) - 1]:
  t, d, p = split_modules[i]
  t = repr('\n'.join(t) + ' '*30)[:30]
  d = repr('\n'.join(d) + ' '*30)[:30]
  p = repr('\n'.join(p) + ' '*30)[:30]
  print(i, modules[i][1], t, '\t', d, '\t', p)


#================================================================================#
