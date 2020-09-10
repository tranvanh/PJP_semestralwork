#!/bin/bash
cd samples

i="$1"
j="${i%%.*}"
echo "$j"
echo "$("../cmake-build-debug/mila" $1)"
clang++ "./output.o" -o "$j"
echo "../cmake-build-debug/mila" "$1" -o "$j"
rm -f output.o