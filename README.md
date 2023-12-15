# RIT Formula SAE Vehicle Controller

## Requirements
- [Git](https://git-scm.com/) is used to download dependencies
- [Docker](https://www.docker.com/) is used to build anywhere
  - [Docker Desktop](https://docs.docker.com/desktop) is recommended on Windows and Mac OS
- [Arm GNU Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
  - For Windows, select `Windows (mingw-w64-i686) hosted cross toolchains` -> `AArch32 bare-metal target (arm-none-eabi)`
- [VS Code](https://code.visualstudio.com) is recommended for editing code.
  - [C/C++ Extension](https://github.com/microsoft/vscode-cpptools)
  - [Native Debug Extension](https://marketplace.visualstudio.com/items?itemName=webfreak.debug)

## Setup
Once docker is installed and running, click `Terminal -> Run Task -> Setup` to build the docker image. If running from a linux terminal, execute `scripts/setup.sh` from the project's root directory. This only needs to be done once.

## Compilation
Click `Terminal -> Run Build Task` to compile the code.

## Testing
Two types of tests exist for the codebase: Unit Testing, and Software in the Loop (SIL) testing.

Unit tests validate single modules, using CMock to mock out dependencies. To run unit tests, click `Terminal -> Run Task -> Unit Tests`.

SIL tests build vc for linux and run it in a child process.
Python tests using a sockets interface feed the program input and validate its output. To run SIL tests, click `Terminal -> Run Task -> SIL Tests`.

## Building Specific Targets
Click `Terminal -> Run Task -> Enter Container` or execute `scripts/enter-docker.sh` to get a shell inside the docker container.

Specific targets can be built using SCons commands from the docker container shell:
- `scons vc.bin`
    -build stm32 binary for loading onto the VC
- `scons unit_tests`
    -build and run application module unit tests
- `scons sil`
    -build and run SIL tests
- `scons memchecks`
    -run valgrind memcheck tool on unit tests
- `scons src/app/<app module name>/unit_test_results.txt`
    -unit test a specific application module
- `scons src/app/<app module name>/memcheck_results.txt`
    -run valgrind memcheck on a specific application unit test runner

You can speed up builds with multithreading by adding `-j<number of cores>` to any scons command

## Custom SCons Build Options
- `--dbg`
    -adds `-g` to linux build commands, enabling debugging symbols (helps track valgrind error origins)

See [SCons](https://github.com/SCons/scons/wiki) and our `sconstruct.py` (top level) for more details on the build system.

## Source Code Structure
Each C module has its own folder where its header (.h), source (.c), and test source (test_\<module name>.c) files are located.

Application logic is implemented in hardware agnostic application modules under src/app. The test source for these modules contains unit tests that run automatically using Unity/CMock.

Driver code is implemented in hardware specific driver modules under src/driver. There are two source files for each driver module: SIM_HAL_\<module name>.c, drivers for interfacing with a simulation running on linux, and STM32_HAL_\<module name>.c, drivers for the stm32 microcontroller. The test source for driver modules is a program made for running on an stm32 evaluation board for testing the drivers.
