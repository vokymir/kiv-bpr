#!/bin/bash

mkdir -p build bin

cmake -S . -B build
cmake --build build
