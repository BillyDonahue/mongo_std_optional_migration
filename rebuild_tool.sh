#!/bin/bash

proj_root="/Users/billy/prog/mongodb/boost_optional_migration"

Clang_DIR="/usr/local/opt/llvm"
MONGO_CLANG_TIDY_DIR="${proj_root}/tool"

build="${proj_root}/tool-build"

set -e

mkdir -p "$build"
pushd "$build"

function build() {
    cmake -G Ninja -DMCT_Clang_INSTALL_DIR="$Clang_DIR" "${MONGO_CLANG_TIDY_DIR}"
    ninja
}

function test() {
    cat <<EOF >input.cpp
#include <boost/optional.hpp>

struct X { void get() const; };

void foo(const boost::optional<int>& oi) {
  auto oip = &oi;
  X{}.get();  // unchanged
  oi.get();  // change to oi.value()
  oip->get();  // change to oip->get()
  oip->is_initialized();  // change to oip->has_value()
  oi.is_initialized();  // change to oi.has_value()
}
EOF
    cmd=(
        "${Clang_DIR}/bin/clang-tidy"
        --load "${build}/lib/libClangTidyMongoStdOptionalMigrationModule.dylib"
        -checks='mongo-std-optional-migration-*'
        "input.cpp"
        --
        -I "${proj_root}/mongo/src/third_party/boost"
    )
    echo "Running: ${cmd[@]}"
    "${cmd[@]}"
    popd
}

build
test
