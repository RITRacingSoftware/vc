import SCons

from pathlib import Path

"""
Instructions for building everything.
"""


"""
Common directory definition
"""

REPO_ROOT_DIR = Dir('.')
SRC_DIR = REPO_ROOT_DIR.Dir('src')
APP_DIR = SRC_DIR.Dir('app')
COMMON_DIR = SRC_DIR.Dir('common')
BUILD_DIR = REPO_ROOT_DIR.Dir('build', create=True)

"""
Find the modules, accumulate a list of include paths while we're at it.
"""
include_paths = [APP_DIR, SRC_DIR, COMMON_DIR]
app_modules = []
for dir_path in (d for d in Path(str(APP_DIR)).iterdir() if d.is_dir()):
    dir_path_str = str(dir_path)
    include_paths.append(dir_path_str)
    module_dir = REPO_ROOT_DIR.Dir(dir_path_str)
    module_name = dir_path_str.split('/')[-1]
    app_modules += [(module_name, module_dir)] 

for dir_path in (d for d in Path(str(COMMON_DIR)).iterdir() if d.is_dir()):
    dir_path_str = str(dir_path)
    include_paths.append(dir_path_str)
    module_dir = REPO_ROOT_DIR.Dir(dir_path_str)
    module_name = dir_path_str.split('/')[-1]
    app_modules += [(module_name, module_dir)]
"""
Environment Definitions
"""

# environment for compiling c modules with a linux target
linux_c_env = Environment(
    CC='gcc',
    CPPPATH=include_paths,
    CCFLAGS=[],
    LIBS=['m']
)

"""
CAN unpacking/packing code generation (from DBC).
"""
def TOOL_DBC_CODE_GENERATE(env):
    """
    Uses the dbcc tool to generate C code for setting/getting CAN data
    from a dbc file.
    """

    """
    SOURCE - dbc file node
    TARGET - c file node that will be generated. The directory of this file will be used in the command.
    """
    can_src_builder = SCons.Builder.Builder(action=[
        'cd ${SOURCE.dir.abspath} && cantools generate_c_source ${SOURCE.abspath}' 
    ])

    env.Append(BUILDERS = {
        'GenerateDbcSource' : can_src_builder
    })

dbc_env = Environment(
    tools=[TOOL_DBC_CODE_GENERATE]
)

dbc_src = dbc_env.GenerateDbcSource(
    source=APP_DIR.File('main_bus/main_bus.dbc'),
    target=[APP_DIR.File('main_bus/main_bus.c'), APP_DIR.File('main_bus/main_bus.h')]
)

Alias('dbc-gen', dbc_src)

"""
Module compilation instructions
"""
modules_to_compile = app_modules
linux_objs = []
for (name, path) in modules_to_compile:
    linux_objs += linux_c_env.Object(
        source=path.File(f'{name}.c'),
        target=BUILD_DIR.File(f'{path.abspath}/{name}.o')
    )

Alias('linux_objs', linux_objs)
