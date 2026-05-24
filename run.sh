#!/bin/bash
set -e 

BUILD_DIR="build"
EXEC_NAME="intersection"

rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

cmake .. -DCMAKE_BUILD_TYPE=Release

cmake --build .

"./$EXEC_NAME" "$@"