#!/bin/bash

# Build the plugin

proj_root="/Users/billy/prog/mongodb/boost_optional_migration"

Clang_DIR="$proj_root/llvm-install"
CLANG_TUTOR_DIR="$proj_root/clang-tutor"

build="clang-tutor-build"

mkdir "$build"
cd "$build"
cmake -DCT_Clang_INSTALL_DIR="$Clang_DIR" "$CLANG_TUTOR_DIR/HelloWorld/"
make
# Run the plugin
"$Clang_DIR/bin/clang" \
    -cc1 \
    -load ./libHelloWorld.{so,dylib} \
    -plugin hello-world \
    "$CLANG_TUTOR_DIR/test/HelloWorld-basic.cpp"
