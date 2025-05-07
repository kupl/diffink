cmake_minimum_required(VERSION 3.20.0)
project(diffink)

# Compiler
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

# Compiler config
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build config
option(LANG_C "Enable C language support" ON)
option(LANG_CSHARP "Enable C# language support" OFF)
option(LANG_CPP "Enable C++ language support" ON)
option(LANG_CSS "Enable CSS language support" OFF)
option(LANG_GO "Enable Go language support" OFF)
option(LANG_HASKELL "Enable Haskell language support" OFF)
option(LANG_HTML "Enable HTML language support" OFF)
option(LANG_JAVA "Enable Java language support" ON)
option(LANG_JAVASCRIPT "Enable JavaScript language support" OFF)
option(LANG_JSON "Enable JSON language support" OFF)
option(LANG_JULIA "Enable Julia language support" OFF)
option(LANG_OCAML "Enable OCaml language support" OFF)
option(LANG_PHP "Enable PHP language support" OFF)
option(LANG_PYTHON "Enable Python language support" ON)
option(LANG_RUBY "Enable Ruby language support" OFF)
option(LANG_RUST "Enable Rust language support" OFF)
option(LANG_SCALA "Enable Scala language support" OFF)
option(LANG_TYPESCRIPT "Enable TypeScript language support" OFF)

# Source
file(GLOB_RECURSE SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

# External
file(GLOB EXTERNAL_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter/lib/src/lib.c
  ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-*/src/*.c
  ${CMAKE_CURRENT_SOURCE_DIR}/externals/xxHash/*.c
)

# Fronted
file(GLOB FRONTEND_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/frontend/src/*.cpp
)

add_executable(${CMAKE_PROJECT_NAME}
  ${SOURCES}
  ${EXTERNAL_SRC}
  ${FRONTEND_SRC}
)

# Compiler flags
if(LANG_C)
  message(STATUS "DiffInk: C language support is enabled.")
  target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE DIFFINK_LANGUAGE_SUPPORT_C)
endif()
if(LANG_CSHARP)
  message(STATUS "DiffInk: C# language support is enabled.")
  target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE DIFFINK_LANGUAGE_SUPPORT_CSHARP)
endif()
if(LANG_CPP)
  message(STATUS "DiffInk: C++ language support is enabled.")
  target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE DIFFINK_LANGUAGE_SUPPORT_CPP)
endif()
if(LANG_CSS)
  message(STATUS "DiffInk: CSS language support is enabled.")
  target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE DIFFINK_LANGUAGE_SUPPORT_CSS)
endif()
if(LANG_GO)
  message(STATUS "DiffInk: Go language support is enabled.")
  target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE DIFFINK_LANGUAGE_SUPPORT_GO)
endif()
if(LANG_HASKELL)
  message(STATUS "DiffInk: Haskell language support is enabled.")
  target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE DIFFINK_LANGUAGE_SUPPORT_HASKELL)
endif()
if(LANG_HTML)
  message(STATUS "DiffInk: HTML language support is enabled.")
  target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE DIFFINK_LANGUAGE_SUPPORT_HTML)
endif()
if(LANG_JAVA)
  message(STATUS "DiffInk: Java language support is enabled.")
  target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE DIFFINK_LANGUAGE_SUPPORT_JAVA)
endif()
if(LANG_JAVASCRIPT)
  message(STATUS "DiffInk: JavaScript language support is enabled.")
  target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE DIFFINK_LANGUAGE_SUPPORT_JAVASCRIPT)
endif()
if(LANG_JSON)
  message(STATUS "DiffInk: JSON language support is enabled.")
  target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE DIFFINK_LANGUAGE_SUPPORT_JSON)
endif()
if(LANG_JULIA)
  message(STATUS "DiffInk: Julia language support is enabled.")
  target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE DIFFINK_LANGUAGE_SUPPORT_JULIA)
endif()
if(LANG_OCAML)
  message(STATUS "DiffInk: OCaml language support is enabled.")
  target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE DIFFINK_LANGUAGE_SUPPORT_OCAML)
endif()
if(LANG_PHP)
  message(STATUS "DiffInk: PHP language support is enabled.")
  target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE DIFFINK_LANGUAGE_SUPPORT_PHP)
endif()
if(LANG_PYTHON)
  message(STATUS "DiffInk: Python language support is enabled.")
  target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE DIFFINK_LANGUAGE_SUPPORT_PYTHON)
endif()
if(LANG_RUBY)
  message(STATUS "DiffInk: Ruby language support is enabled.")
  target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE DIFFINK_LANGUAGE_SUPPORT_RUBY)
endif()
if(LANG_RUST)
  message(STATUS "DiffInk: Rust language support is enabled.")
  target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE DIFFINK_LANGUAGE_SUPPORT_RUST)
endif()
if(LANG_SCALA)
  message(STATUS "DiffInk: Scala language support is enabled.")
  target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE DIFFINK_LANGUAGE_SUPPORT_SCALA)
endif()
if(LANG_TYPESCRIPT)
  message(STATUS "DiffInk: TypeScript language support is enabled.")
  target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE DIFFINK_LANGUAGE_SUPPORT_TYPESCRIPT)
endif()

# Binary path
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# External: tree-sitter
file(GLOB TREE_SITTER_BINDINGS_C
  ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-*/bindings/c
)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/externals
  ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter/lib/include
  ${TREE_SITTER_BINDINGS_C}
  ${CMAKE_CURRENT_SOURCE_DIR}/frontend/include
)

