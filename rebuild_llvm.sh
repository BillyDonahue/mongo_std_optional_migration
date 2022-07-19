#!/bin/bash

set -e

root="/Users/billy/prog/mongodb/boost_optional_migration"

src="$root/llvm-project"
build_dir="$root/llvm-build"
install="$root/llvm-install"

function llvm_configure() {
    pushd "$src"
    gen=Ninja
    opt=(
        -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra"
        -DCMAKE_BUILD_TYPE="Release"
        -DCMAKE_INSTALL_PREFIX="$install"
    )
    cmd=(
        cmake -S llvm
        -B "$build_dir"
        -G "$gen"
        "${opt[@]}" 
    )
    # echo "${cmd[@]}"
    "${cmd[@]}"
    popd
}

llvm_configure
cmake --build "$build_dir"
# cmake --build "$build_dir" -t check-all
cmake --build "$build_dir" -t install
