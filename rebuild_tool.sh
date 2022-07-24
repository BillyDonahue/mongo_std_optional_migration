#!/bin/bash

proj_root="/Users/billy/prog/mongodb/boost_optional_migration"

Clang_DIR="/usr/local/opt/llvm"
MONGO_CLANG_TIDY_DIR="${proj_root}/tool"

build="${proj_root}/tool-build"

set -e

function build() {
    pushd "$build"
    cmake -G Ninja -DMCT_Clang_INSTALL_DIR="$Clang_DIR" "${MONGO_CLANG_TIDY_DIR}"
    ninja
    popd
}

function test() {
    pushd "$build"
    cmake ../tool-test -B tool-test-build -G Ninja
    ninja -C tool-test-build
    cmd=(
        "${Clang_DIR}/bin/clang-tidy"
        --load "${build}/lib/libClangTidyMongoStdOptionalMigrationModule.dylib"
        -checks='-*,mongo-std-optional-migration-*'
        "../tool-test/input.cpp"
        -p tool-test-build
    )
    echo "Running: ${cmd[@]}"
    "${cmd[@]}"
    popd
}

mkdir -p "$build"
build
test
