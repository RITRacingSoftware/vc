import SCons

import os
from pathlib import Path

"""
Instructions for building everything.
"""

# some useful constants
ARM_CC = 'arm-none-eabi-gcc'
ARM_OBJCOPY = 'arm-none-eabi-objcopy'
ARM_AS = 'arm-none-eabi-as'
ARM_LD = 'arm-none-eabi-ld'
PYTHON = 'python3.8'


"""
Common directory definition
"""

REPO_ROOT_DIR = Dir('.')
SRC_DIR = REPO_ROOT_DIR.Dir('src')
APP_DIR = SRC_DIR.Dir('app')
DRIVER_DIR = SRC_DIR.Dir('driver')
COMMON_DIR = SRC_DIR.Dir('common')
SIL_DIR = SRC_DIR.Dir('sil')
SIL_TESTS_DIR = SIL_DIR.Dir('tests')
BUILD_DIR = REPO_ROOT_DIR.Dir('build', create=True)
LIBS_DIR = REPO_ROOT_DIR.Dir('libs')
STM32_CUBE_DIR = LIBS_DIR.Dir('STM32CubeG4')
STM32_LIB_DIR = STM32_CUBE_DIR.Dir('Drivers/STM32G4xx_HAL_Driver')
STM32_CMSIS_DIR = STM32_CUBE_DIR.Dir('Drivers/CMSIS')
STM32_CMSIS_DEV_DIR = STM32_CMSIS_DIR.Dir('Device/ST/STM32G4xx')
FREERTOS_DIR = LIBS_DIR.Dir("FreeRTOS-Kernel")

LINKER_FILE = REPO_ROOT_DIR.File('STM32G473RETx_FLASH.ld')

"""
Find the modules, accumulate a list of include paths while we're at it.
"""
include_paths = [
    APP_DIR,
    SRC_DIR,
    COMMON_DIR,
    DRIVER_DIR,
    SIL_DIR,
    LIBS_DIR.Dir('cmock/src'),
    LIBS_DIR.Dir('cmock/vendor/unity/src'),
    LIBS_DIR.Dir('vector_blf/src/'),
    STM32_LIB_DIR.Dir('Inc'),
    STM32_CMSIS_DIR.Dir('Core/Include'),
    STM32_CMSIS_DEV_DIR.Dir('Include'),
    FREERTOS_DIR.Dir('include'),
    FREERTOS_DIR.Dir('portable/GCC/ARM_CM4F'),
    BUILD_DIR.Dir('common/formula-main-dbc'), # We want to link against formula_main_dbc.h
]

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
    CPPDEFINES=['VC_SIL'],
    LIBS=['m'],
    SHELL='bash'
)

linux_cpp_env = Environment(
    CC='g++',
    CPPPATH=include_paths,
    CPPFLAGS=["-ggdb", "-fPIC"],
    LIBS=['m', 'Vector_BLF'],
    CPPDEFINES=['VC_SIL']
)

# arm hex creation tool
def TOOL_ARM_ELF_HEX(env):
    """
    Uses arm toolchain to generate elf or hex files from compiled code.
    """

    """
    Description of command below:
    -mcpu=cortex-m4: the cortex-m4 is our microprocessor. This tells the compiler to use its instruction set
    -mfpu=fpv4-sp-d16: our microprocessor has a floating point unit, use it
    -mfloat-abi=hard: ^
    --specs=nosys.specs: this removes a default spec that tries to compile a wrapper layer of sorts for linux debugging
    (will get error looking for _exit function if removed)
    SOURCE must be a list of strings
    """
    arm_elf_builder = SCons.Builder.Builder(action=[
        ARM_CC + ' -lm -T' + LINKER_FILE.abspath + ' -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -Wl,-Map=${TARGET.dir.abspath}/map.map,-lm --specs=nosys.specs -mthumb ${SOURCES[:].abspath} -o ${TARGET.abspath} -lm'
    ])

    arm_hex_builder = SCons.Builder.Builder(action=[
        ARM_OBJCOPY + ' -O binary ${SOURCE.abspath} ${TARGET.abspath}'
    ])

    env.Append(BUILDERS = {
        'BuildElf' : arm_elf_builder,
        'BuildHex' : arm_hex_builder
    })

stm32_c_env = Environment(
    tools=[TOOL_ARM_ELF_HEX, 'gcc', 'as'],
    CC=ARM_CC,
    AS=ARM_AS,
    LD=ARM_LD,
    CPPPATH=include_paths,
    CPPDEFINES=['STM32G473', 'STM32G473xx'],
    CCFLAGS=['-ggdb','-mcpu=cortex-m4', '-mfpu=fpv4-sp-d16', '-mfloat-abi=hard', '-mthumb', '-lm'],
    ASFLAGS=['-mthumb'],
    LDFLAGS=['-T{}'.format(LINKER_FILE.abspath), '-mcpu=cortex-m4', '-mfpu=fpv4-sp-d16', '-mfloat-abi=hard', '-mthumb', '-Wall', '--specs=nosys.specs', '-lm']
)

# janky
linux_cpp_env['STATIC_AND_SHARED_OBJECTS_ARE_THE_SAME']=1

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
        'cd ${TARGET.dir.abspath} && cantools generate_c_source ${SOURCE.abspath}'
    ])

    env.Append(BUILDERS = {
        'GenerateDbcSource' : can_src_builder
    })

dbc_env = Environment(
    tools=[TOOL_DBC_CODE_GENERATE]
)

DBC_GEN_DIR = BUILD_DIR.Dir("common/formula-main-dbc");

dbc_src = dbc_env.GenerateDbcSource(
    source=LIBS_DIR.Dir('formula-main-dbc').File('formula_main_dbc.dbc'),
    target=[DBC_GEN_DIR.File('formula_main_dbc.c'), DBC_GEN_DIR.File('formula_main_dbc.h')]
)

Alias('dbc-gen', dbc_src)

"""
The next few sections are unit test building/running.
Unit tests are just C scripts that get compiled into binary executables.
The cmock library is used to "Mock" out modules that arent under test.
Each unit Module Under Test (mut) executable requires the following objects:
-mut's object, compiled for linux
-mut's test script object
-mut's test runner object (c file for this generated by cmock)
-each dependency's mock object (c files for these generated by cmock)
-cmock and unity static libraries

The instructions for generating the relevant c code and compiling these things are below:
"""


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
        'ruby ' + CMOCK_SCRIPT_DIR.abspath + '/cmock.rb ' + '-o' + CMOCK_CONFIG_FILE.abspath + ' ${SOURCE.abspath}'
        #' && mv mocks ${TARGET.dir.abspath}/..'
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

# Instructions for generating and compiling cmocked modules
# list of each module's 'mocks' dir (where the mocks are stored)
mock_modules = []
cmock_generated_headers = []
cmock_objs = {}
for module_name, module_dir in (app_modules + driver_modules):
    build_dir = BUILD_DIR.Dir(SRC_DIR.rel_path(module_dir))
    mocks_dir = module_dir.Dir('mocks')

    # later source in this file will need these directories
    mock_modules.append(mocks_dir)
    linux_c_env['CPPPATH'] += [mocks_dir]
    cmock_c = mocks_dir.File('Mock{}.c'.format(module_name))
    cmock_header = mocks_dir.File('Mock{}.h'.format(module_name))
    cmock_src = cmock_env.GenerateMocks(
        # only input is the module's header file
        source=module_dir.File(module_name + '.h'),
        target=[cmock_c, cmock_header]
    )
    cmock_objs[module_name] = linux_c_env.Object(
        source=cmock_c,
        target=build_dir.File(f'Mock{module_name}.o')
        )
    cmock_generated_headers += [cmock_header]
    Clean(cmock_header, mocks_dir) # tell scons to clean these up when --clean is specified

Alias('cmock-headers', cmock_generated_headers)
Alias('cmock-objs', cmock_objs.values())

# Instructions for generating unit test runner source and compiling it
cmock_testrunner_src = {}
for module_name, module_dir in app_modules:
    build_dir = BUILD_DIR.Dir(SRC_DIR.rel_path(module_dir))
    mocks_dir = build_dir.Dir('mocks')
    testrunner_c = mocks_dir.File(f'testrunner_{module_name}.c')
    testrunner = cmock_env.GenerateTestRunner(
        # input is just the file containing the unit tests
        source=module_dir.File('test_{}.c'.format(module_name)),
        target=testrunner_c
    )
    cmock_testrunner_src[module_name] = testrunner

Alias('cmock-testrunner-src', cmock_testrunner_src.values())
Alias('cmock-testrunners')

"""
Instructions for Unit test compilation.
For each module, creates a linux binary from the test_<Module Name>.c file
found in the module directory.
"""
# compiles the cmock library
# also compiles unity (unit testing framework, submodule of cmock lib)
CMOCK_ROOT_DIR = REPO_ROOT_DIR.Dir('libs/cmock/')
CMOCK_BUILD_DIR = BUILD_DIR.Dir("cmock", create=True)
cmock_libs = [CMOCK_BUILD_DIR.File('src/libcmock.a.p/cmock.c.o'), CMOCK_BUILD_DIR.File('vendor/unity/src/libunity.a.p/unity.c.o')]
cmock_libs = Command(
    cmock_libs,
    [],
    f'cd {CMOCK_ROOT_DIR.abspath} && meson setup {CMOCK_BUILD_DIR} && cd {CMOCK_BUILD_DIR} && meson compile'
)
Clean(cmock_libs, CMOCK_ROOT_DIR.Dir('build'))

"""
Application module linux target compilation instructions.
"""
modules_to_compile = app_modules
linux_objs = {}
for (name, path) in modules_to_compile:
    linux_objs[name] = linux_c_env.Object(
        source=path.File(f'{name}.c'),
        target=BUILD_DIR.File(f'{SRC_DIR.rel_path(path)}/{name}.o')
    )

Alias('linux_objs', linux_objs.values())

"""
Common module linux target compilation instructions.
"""
modules_to_compile = common_modules
common_objs = {}
for (name, path) in modules_to_compile:
    common_objs[name] = linux_c_env.Object(
        source=path.File(f'{name}.c'),
        target=BUILD_DIR.File(f'{SRC_DIR.rel_path(path)}/{name}.o')
    )

Alias('common_objs', common_objs.values())


"""
Unit test script compilation instructions.
"""
test_script_objs = {}
for (name, path) in app_modules:
    test_script_objs[name] = linux_c_env.Object(
        source=path.File(f'test_{name}.c'),
        target=BUILD_DIR.File(f'test_{SRC_DIR.rel_path(path)}/{name}.o')
    )

Alias ('test_script_objs', test_script_objs.values())

"""
Now we finally have instructions for everything needed for unit test executable compilationg.
Gather these things for each executable and compile them.
"""
unit_test_execs = []
for name, obj in cmock_testrunner_src.items():
    obj = obj[0]
    # get all mocks besides module under test
    objs = []
    for n, o in cmock_objs.items():
        if n != name:
            objs += [o]

    unit_test_execs += [
        linux_c_env.Program(
            source=[obj, linux_objs[name], test_script_objs[name]] + objs + cmock_libs + common_objs.values(),
            target=BUILD_DIR.Dir(SRC_DIR.rel_path(obj.dir)).File(f'{name}_tests')
            )
    ]

"""
Now run those unit tests.
Also provide an option to run Valgrind, a runtime memory checker, on each unit test.
This checks for invalid memory accesses, stack overflows, memory leaks, etc.
"""

def TOOL_VALGRIND(env):
    """
    Runs a valgrind memory check on a linux program.
    """

    """
    SOURCE = binary node
    TARGET = text results file
    """
    env['SHELL'] = 'bash'


    valgrind_memcheck_builder = SCons.Builder.Builder(action=[
        'valgrind --tool=memcheck --leak-check=yes --track-origins=yes ${SOURCE.abspath} 2>&1'
    ])

    env.Append(BUILDERS = {
        'MemCheck' : valgrind_memcheck_builder
    })

valgrind_env = Environment(
    tools=[TOOL_VALGRIND]
)

valgrind_env = Environment(
    tools=[TOOL_VALGRIND]
)

# contains scons nodes signifying unit test runs
unit_test_results = []
valgrind_test_results = []
for testrunner in unit_test_execs:
    # executable to run
    #testrunner = .File('testrunner_' + module_name)
    # results file to print to
    testrunner = testrunner[0]
    test_result_file = testrunner.dir.File('unit_test_results.txt')
    unit_test_results += linux_c_env.Command(
        source=testrunner,
        target=test_result_file,
        # TODO remove janky bash workaround and get Command() to use bash (sh test dont work)
        # Run the executable, tee (split) the output between stdout and the results file,
        # then check the status of the testrunner executable to fail the build if needed.
        # This is necessary if we want result files for unit tests (since the executables dont write to files)
        action='{} | tee {} && test $$PIPESTATUS -eq 0'.format(testrunner.abspath, test_result_file.abspath)
    )

    valgrind_test_results += valgrind_env.MemCheck(
        source=testrunner,
        target=testrunner.dir.File('memcheck_results.txt')
    )

Alias('memchecks', valgrind_test_results)
Alias('unit_tests', unit_test_results)
Default(unit_test_results)

Alias('testrunners', unit_test_execs)

"""
Simulation module compilation instructions.
"""

SIM_BUILD_DIR = BUILD_DIR.Dir("sim")

# compile all sil modules
sil_objs = []
for src_file in Glob(Path(SIL_DIR.abspath) / "*.cpp"):
    build_dir = SIM_BUILD_DIR.Dir('sil')

    obj = linux_cpp_env.Object(
        source=src_file,
        target=build_dir.File(f'{SIL_DIR.rel_path(src_file)}.o')
    )

    sil_objs.append(obj)

    # file_name = src_file.abspath.split('/')[-1]
    # print(file_name)
    # if file_name == 'BlfWriter.cpp':
    #     Depends(obj, blf_lib)


# compile all application modules using g++
cpp_app_objs = []
for module_name, module_dir in app_modules:
    build_dir = SIM_BUILD_DIR.Dir(SRC_DIR.rel_path(module_dir))
    cpp_app_objs.append(linux_cpp_env.Object(
        source=module_dir.File(f'{module_name}.c'),
        target=build_dir.File(f'{module_name}.o')
    ))


# compile all sil driver modules using g++
cpp_sil_driver_objs = []
for module_name, module_dir in driver_modules:
    build_dir = SIM_BUILD_DIR.Dir(SRC_DIR.rel_path(module_dir))
    cpp_sil_driver_objs.append(linux_cpp_env.Object(
        source=module_dir.File(f'SIL_{module_name}.c'),
        target=build_dir.File(f'SIL_{module_name}.o')
    ))

# compile all common modules using g++
cpp_common_objs = []
for module_name, module_dir in common_modules:
    build_dir = SIM_BUILD_DIR.Dir(SRC_DIR.rel_path(module_dir))
    cpp_sil_driver_objs.append(linux_cpp_env.Object(
        source=module_dir.File(f'{module_name}.c'),
        target=build_dir.File(f'{module_name}.o')
    ))

# compile ecusim
ecusim = SConscript('libs/ecu-sim/sconstruct.py')
ecusim_objs = ecusim['ecusim_objs']
# blf_lib = ecusim['blf_lib']
ecusim_src_dirs = ecusim['src_dirs']

linux_cpp_env['CPPPATH'] += ecusim_src_dirs

TEST_OUTPUT_DIR = SIM_BUILD_DIR.Dir('sil/tests/')

# compile smoke test object
smoke_test_obj = linux_cpp_env.Object(
    source=SIL_TESTS_DIR.File('smoke_test.cpp'),
    target=TEST_OUTPUT_DIR.File('smoke_test.o')
)

# Compile formula_main_dbc.c
sim_dbc_obj = linux_cpp_env.Object(source=DBC_GEN_DIR.File('formula_main_dbc.c'), target=SIM_BUILD_DIR.Dir('libs/formula-main-dbc').File('formula_main_dbc.o'))

# compile sil main program + link everything together
smoke_test = linux_cpp_env.Program(
    source=[smoke_test_obj] + sil_objs + ecusim_objs + cpp_sil_driver_objs + cpp_app_objs + sim_dbc_obj,
    target=TEST_OUTPUT_DIR.File('smoke_test')
)

smoke_test_results = Command(
    source=smoke_test,
    target=TEST_OUTPUT_DIR.File('smoke_test_results.txt'),
    action=f"{TEST_OUTPUT_DIR.File('smoke_test').abspath}"
)

Alias('smoke_test', smoke_test_results)




# # alternatively, can compile a shared object that can be opened as a python library

py_lib = linux_cpp_env.SharedLibrary(
    source=sil_objs + ecusim_objs + cpp_sil_driver_objs + cpp_app_objs + sim_dbc_obj,
    target=TEST_OUTPUT_DIR.File('libVcHandle.so')
)

Alias('vc_handle', py_lib)

# Now, instructions for running SIL tests
sil_tests = Glob(Path(SIL_DIR.Dir('tests').abspath) / "test_*.py")

SIL_BLF_DIR = SIM_BUILD_DIR.Dir('sil_test_blfs')

sil_test_results = []
for test in sil_tests:
    test_name = test.abspath.split('/')[-1].split('.')

    blf = SIL_BLF_DIR.File(f"{test_name}.blf")

    sil_test_result = Command(
        source=test,
        target=blf,
        action=f"python3 -m pytest -s {test.abspath}" # -s disables ouput capture so we can see test prints
    )

    sil_test_results += sil_test_result



Depends(sil_test_results, py_lib)

Alias('sil', sil_test_results)

"""
Now onto STM32 compilation stuff.

The following code compiles application, common, and drive modules for the stm32.
"""

STM32_BUILD_DIR = BUILD_DIR.Dir("stm32")

# compile all modules into stm32 objects

# compile our code
stm32_objs = []
for module_name, module_dir in app_modules + common_modules + driver_modules:
    if (module_name, module_dir) in driver_modules:
        source_file = module_dir.File(f'STM32_{module_name}.c')
    else:
        source_file = module_dir.File(f'{module_name}.c')

    build_dir = STM32_BUILD_DIR.Dir(SRC_DIR.rel_path(module_dir))
    stm32_objs.append(stm32_c_env.Object(
        source=[source_file],
        target=build_dir.File(f'{module_name}.o')
    ))

# compile stm32 hal drivers
for source in Glob(os.path.join(STM32_LIB_DIR.Dir('Src').abspath, '*.c')):
    file_name = source.abspath.split('/')[-1]
    stm32_objs += stm32_c_env.Object(source=source, target=STM32_BUILD_DIR.Dir('libs').Dir(LIBS_DIR.rel_path(source.dir)).File(f'{file_name}.o'))

# compile freertos sources
stm32_freertos_source = []
stm32_freertos_source += Glob(FREERTOS_DIR.abspath + '/*.c')
stm32_freertos_source.append(FREERTOS_DIR.File('portable/MemMang/heap_4.c'))
stm32_freertos_source.append(FREERTOS_DIR.File('portable/GCC/ARM_CM4F/port.c'))

for source_file in stm32_freertos_source:
    file_name = source_file.abspath.split('/')[-1]
    stm32_objs += stm32_c_env.Object(source=source_file, target=STM32_BUILD_DIR.Dir('libs').Dir(LIBS_DIR.rel_path(source_file.dir)).File(f'{file_name}.o'))

stm32_objs += stm32_c_env.Object(source=SRC_DIR.File('stm32_main.c'), target=STM32_BUILD_DIR.File('main.stm32.o'))

# Compile formula_main_dbc.c
stm32_objs += stm32_c_env.Object(source=DBC_GEN_DIR.File('formula_main_dbc.c'), target=STM32_BUILD_DIR.Dir('libs/formula-main-dbc').File('formula_main_dbc.o'))

# build the assembly files with the microcontroller startup routines in them
startup_src = [
    STM32_CMSIS_DEV_DIR.File('Source/Templates/gcc/startup_stm32g473xx.s'),
    STM32_CMSIS_DEV_DIR.File('Source/Templates/system_stm32g4xx.c')
]
for src in startup_src:
    file_name = src.abspath.split('/')[-1]
    stm32_objs += stm32_c_env.Object(
        source=src,
        target=STM32_BUILD_DIR.Dir('libs').Dir(LIBS_DIR.rel_path(src.dir)).File(f'{file_name}.o')
    )

Alias('stm32_objs', stm32_objs)

# stm32 elf generation
stm32_elf = stm32_c_env.BuildElf(
    source=stm32_objs,
    target=STM32_BUILD_DIR.File('vc.elf')
)

Clean(stm32_elf, BUILD_DIR.File('vc.map'))

# stm32 hex generation
vc_bin = stm32_c_env.BuildHex(
    source=stm32_elf,
    target=STM32_BUILD_DIR.File('vc.bin')
)

Alias('vc-bin', vc_bin)
