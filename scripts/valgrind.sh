#!/usr/bin/env bash

valgrind --tool=memcheck --leak-check=yes --track-origins=yes $1
