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
DRIVER_DIR = SRC_DIR.Dir('driver')
COMMON_DIR = SRC_DIR.Dir('common')
BUILD_DIR = REPO_ROOT_DIR.Dir('build', create=True)

"""
Find the modules, accumulate a list of include paths while we're at it.
"""
include_paths = [APP_DIR, SRC_DIR, COMMON_DIR, DRIVER_DIR]
app_modules = []
driver_modules = []
common_modules = []
all_modules = []

for dir_path in (d for d in Path(str(APP_DIR)).iterdir() if d.is_dir()):
    dir_path_str = str(dir_path)
    include_paths.append(dir_path_str)
    module_dir = REPO_ROOT_DIR.Dir(dir_path_str)
    module_name = dir_path_str.split('/')[-1]
    app_modules += [(module_name, module_dir)] 

for dir_path in (d for d in Path(str(DRIVER_DIR)).iterdir() if d.is_dir()):
    dir_path_str = str(dir_path)
    include_paths.append(dir_path_str)
    module_dir = REPO_ROOT_DIR.Dir(dir_path_str)
    module_name = dir_path_str.split('/')[-1]
    driver_modules += [(module_name, module_dir)] 

for dir_path in (d for d in Path(str(COMMON_DIR)).iterdir() if d.is_dir()):
    dir_path_str = str(dir_path)
    include_paths.append(dir_path_str)
    module_dir = REPO_ROOT_DIR.Dir(dir_path_str)
    module_name = dir_path_str.split('/')[-1]
    common_modules += [(module_name, module_dir)]

all_modules = app_modules + driver_modules + common_modules
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
cmock generation. Generates mocks for each module and unit test runner source for each application module.
"""
def TOOL_CMOCK(env):
    """
    Uses cmock.rb to generate cmock headers for a module.
    """
    # cmock ruby scripts are here
    CMOCK_SCRIPT_DIR = REPO_ROOT_DIR.Dir('libs/cmock/lib')
    
    CMOCK_CONFIG_FILE = SRC_DIR.File('cmock_config.yml')

    # build the command to run cmock from the SOURCE parameter
    cmock_header_builder = SCons.Builder.Builder(action=[
        'cd ' + '${SOURCE.dir.abspath}' + ' &&' +
        'ruby ' + CMOCK_SCRIPT_DIR.abspath + '/cmock.rb ' + '-o' + CMOCK_CONFIG_FILE.abspath + ' ${SOURCE.abspath}' +
        ' && mv mocks ${TARGET.dir.abspath}/..'
    ])

    # cmock test generation script is in unity repo for some reason
    CMOCK_TESTGEN_DIR = REPO_ROOT_DIR.Dir('libs/cmock/vendor/unity/auto/')
    
    # run ruby script from unity repo to generate unit test runner from unit test source file
    cmock_testrunner_builder = SCons.Builder.Builder(action=[
        'cd ${SOURCE.dir.abspath} && ruby ' + CMOCK_TESTGEN_DIR.abspath + '/generate_test_runner.rb ${SOURCE.abspath} ${TARGET.abspath}'
    ])

    # the string keys here are the functions called to use each builder
    env.Append(BUILDERS = {
        'GenerateMocks' : cmock_header_builder,
        'GenerateTestRunner' : cmock_testrunner_builder
    })

cmock_env = Environment(
    tools=[TOOL_CMOCK]
)

# Instructions for generating cmocked modules
# list of each module's 'mocks' dir (where the mocks are stored)
mock_modules = []
cmock_generated_headers = []
for module_name, module_dir in (app_modules + driver_modules):
    build_dir = BUILD_DIR.Dir(SRC_DIR.rel_path(module_dir))
    mocks_dir = build_dir.Dir('mocks')
    
    # later source in this file will need these directories
    mock_modules.append(mocks_dir)
    cmock_header = cmock_env.GenerateMocks(
           # only input is the module's header file
           source=module_dir.File(module_name + '.h'),
           target=[mocks_dir.File('Mock{}.c'.format(module_name)), mocks_dir.File('Mock{}.h'.format(module_name))]
    )
    cmock_generated_headers += cmock_header
    Clean(cmock_header, mocks_dir) # tell scons to clean these up when --clean is specified

Alias('cmock-headers', cmock_generated_headers)

# Instructions for generating unit test runner source
cmock_testrunner_src = []
for module_name, module_dir in app_modules:
    build_dir = BUILD_DIR.Dir(SRC_DIR.rel_path(module_dir))
    mocks_dir = build_dir.Dir('mocks')
    testrunner = cmock_env.GenerateTestRunner(
        # input is just the file containing the unit tests
        source=module_dir.File('test_{}.c'.format(module_name)),
        target=mocks_dir.File('testrunner_{}.c'.format(module_name))
    )
    cmock_testrunner_src += testrunner

Alias('cmock-testrunner-src', cmock_testrunner_src)

"""
Compile Cmock modules
"""
cmock_objs = []

"""
Module compilation instructions
"""
modules_to_compile = app_modules
linux_objs = []
for (name, path) in modules_to_compile:
    linux_objs += linux_c_env.Object(
        source=path.File(f'{name}.c'),
        target=BUILD_DIR.File(f'{SRC_DIR.rel_path(path)}/{name}.o')
    )

Alias('linux_objs', linux_objs)
