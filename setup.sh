#!/usr/bin/env bash

git submodule update --init --recursive
docker build . -t vc
