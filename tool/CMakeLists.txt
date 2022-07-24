cmake_minimum_required(VERSION 3.13.4)
project(mongo-clang-tidy)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# SET CLANG INSTALLATION DIR
if(DEFINED Clang_ROOT)
  set(MCT_CLANG_PACKAGE_DIR "${Clang_ROOT}/../../..")
elseif(DEFINED Clang_DIR)
  set(MCT_CLANG_PACKAGE_DIR "${Clang_DIR}/../../..")
endif()
mark_as_advanced(MCT_CLANG_PACKAGE_DIR)

# Set this to a valid Clang installation directory.
set(MCT_Clang_INSTALL_DIR "${MCT_CLANG_PACKAGE_DIR}" CACHE PATH "Clang installation directory")

# VERIFY CLANG INSTALLATION DIR
set(MCT_LLVM_INCLUDE_DIR "${MCT_Clang_INSTALL_DIR}/include/llvm")
if(NOT EXISTS "${MCT_LLVM_INCLUDE_DIR}")
  message(FATAL_ERROR " MCT_Clang_INSTALL_DIR (${MCT_LLVM_INCLUDE_DIR}) is invalid.")
endif()

set(MCT_LLVM_CMAKE_FILE "${MCT_Clang_INSTALL_DIR}/lib/cmake/clang/ClangConfig.cmake")
if(NOT EXISTS "${MCT_LLVM_CMAKE_FILE}")
  message(FATAL_ERROR " MCT_LLVM_CMAKE_FILE (${MCT_LLVM_CMAKE_FILE}) is invalid.")
endif()

# LOAD CLANG CONFIGURATION
#    Extracted from:
#    http://llvm.org/docs/CMake.html#embedding-llvm-in-your-project
list(APPEND CMAKE_PREFIX_PATH "${MCT_Clang_INSTALL_DIR}/lib/cmake/clang/")

find_package(Clang REQUIRED CONFIG)

# Sanity check. As Clang does not expose e.g. `CLANG_VERSION_MAJOR` through
# AddClang.cmake, we have to use LLVM_VERSION_MAJOR instead.
# TODO: Revisit when next version is released.
if(NOT "${LLVM_VERSION_MAJOR}" VERSION_EQUAL "14")
  message(FATAL_ERROR "Found LLVM ${LLVM_VERSION_MAJOR}, but need LLVM 14")
endif()

message(STATUS "Found Clang ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using ClangConfig.cmake in: ${MCT_Clang_INSTALL_DIR}")

message("CLANG STATUS:
  Includes (clang)    ${CLANG_INCLUDE_DIRS}
  Includes (llvm)     ${LLVM_INCLUDE_DIRS}")

# Set the LLVM and Clang header and library paths
include_directories(SYSTEM "${LLVM_INCLUDE_DIRS};${CLANG_INCLUDE_DIRS}")

#===============================================================================
# 3. CLANG-TUTOR BUILD CONFIGURATION
#===============================================================================
# Use the same C++ standard as LLVM does
set(CMAKE_CXX_STANDARD 14 CACHE STRING "")

# Build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type (default Debug):" FORCE)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fdiagnostics-color=always")

# LLVM/Clang is normally built without RTTI. Be consistent with that.
if(NOT LLVM_ENABLE_RTTI)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

# -fvisibility-inlines-hidden is set when building LLVM and on Darwin warnings
# are triggered if llvm-tutor is built without this flag (though otherwise it
# builds fine). For consistency, add it here too.
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-fvisibility-inlines-hidden"
  SUPPORTS_FVISIBILITY_INLINES_HIDDEN_FLAG)
if(${SUPPORTS_FVISIBILITY_INLINES_HIDDEN_FLAG} EQUAL "1")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
endif()

# Set the build directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

add_subdirectory(lib)
