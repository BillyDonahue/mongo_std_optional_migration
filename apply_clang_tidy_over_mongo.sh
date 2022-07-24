#!/bin/bash

set -e

proj_root="/Users/billy/prog/mongodb/boost_optional_migration"
Clang_DIR="/usr/local/opt/llvm"
MONGO_CLANG_TIDY_DIR="${proj_root}/tool"
build="${proj_root}/tool-build"

CLANG_TIDY_FIX_MODE="${CLANG_TIDY_FIX_MODE:-fix}"
CLANG_TIDY="${CLANG_TIDY:-${Clang_DIR}/bin/clang-tidy}"

#    --fix
#    --fix-errors

CLANG_TIDY_EXTRA_ARGS=(
    --quiet
    --load "${build}/lib/libClangTidyMongoStdOptionalMigrationModule.dylib"
    --checks="-*,mongo-std-optional-migration-*"
    #--export-fixes="./apply_clang_tidy_over_mongo.yaml"
)

JQ_EXPR="
    .[] |
    .file |
    select(test(\"src/mongo\")) |
    select(test(\"parser_gen.cpp\") | not)
"

all_sources=($(jq -r "${JQ_EXPR}" ./compile_commands.json))

# test
# all_sources=(src/mongo/db/catalog/drop_indexes.cpp)

#cmd=(
#    "${CLANG_TIDY}"
#    "${CLANG_TIDY_EXTRA_ARGS[@]}"
#    -p ./compile_commands.json
#    "${all_sources[@]}"
#)
#echo "${cmd[@]}"

echo "${all_sources[@]}" |
    xargs \
        -t \
        -n 1 \
        -P 16 \
        -I % \
        "${CLANG_TIDY}" "${CLANG_TIDY_EXTRA_ARGS[@]}" -p ./compile_commands.json --export-fixes=%.fixes.yaml %
