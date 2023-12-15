#!/usr/bin/env bash

set -x

ELF=./build/stm32/vc.elf

openocd -f ./openocd.cfg -c "program ${ELF} verify reset"
