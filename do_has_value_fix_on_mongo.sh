#!/bin/bash

files=(
    $(git grep "is_initialized" src/mongo/ |
        cut -d: -f1 |
        uniq)
)
for f in "${files[@]}" ; do
    /usr/local/opt/llvm/bin/clang-tidy \
        --load ../tool-build/lib/libClangTidyMongoStdOptionalMigrationModule.dylib \
        -checks='-*,mongo-std-optional-migration-*' \
        "$f"
done
