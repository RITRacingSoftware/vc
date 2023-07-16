# RIT Formula SAE Vehicle Controller

## Requirements
- [Docker](https://www.docker.com/) is used to build anywhere
- [Git](https://git-scm.com/) is used to download dependencies

## Building Everything/Running all tests
Once docker is installed, use `setup.sh` to build the docker image. This only needs to be done once.

From now on, use `enter-docker.sh` to get a shell in the docker container and run `build.sh` to run the firmware build.

## Testing
Two types of tests exist for the codebase: Unit Testing, and Software in the Loop (SIL) testing.

Unit tests validate single modules, using CMock to mock out dependencies.

SIL tests build vc for linux and run it in a child process.
Python tests using a sockets interface feed the program input and validate its output.

## Building Specific Targets
Use `enter-docker.sh` to get a shell inside the f29bms docker container.

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
