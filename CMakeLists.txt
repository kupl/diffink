cmake_minimum_required(VERSION 3.10.0)
project(diffink)

# Compiler
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

# Flag
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Source
file(GLOB_RECURSE SOURCES
  src/*.cpp
  )
message(src)

# External: tree-sitter
file(GLOB TREE_SITTER_SRC
  externals/tree-sitter/lib/src/lib.c
  externals/tree-sitter-*/src/*.c
  )

add_executable(${CMAKE_PROJECT_NAME}
  ${SOURCES}
  ${TREE_SITTER_SRC}
  )

# Include
include_directories(
  include
  )

# External: tree-sitter family
file(GLOB TREE_SITTER_BINDINGS_C
  externals/tree-sitter-*/bindings/c
  )

# Externals
include_directories(
  externals
  externals/tree-sitter/lib/include
  ${TREE_SITTER_BINDINGS_C}
  )

