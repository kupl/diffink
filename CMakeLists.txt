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
option(LANG_C "Enable C language support" OFF)
option(LANG_CSHARP "Enable C# language support" OFF)
option(LANG_CPP "Enable C++ language support" OFF)
option(LANG_CSS "Enable CSS language support" OFF)
option(LANG_GO "Enable Go language support" OFF)
option(LANG_HASKELL "Enable Haskell language support" OFF)
option(LANG_HTML "Enable HTML language support" OFF)
option(LANG_JAVA "Enable Java language support" OFF)
option(LANG_JAVASCRIPT "Enable JavaScript language support" OFF)
option(LANG_JSON "Enable JSON language support" OFF)
option(LANG_JULIA "Enable Julia language support" OFF)
option(LANG_PYTHON "Enable Python language support" OFF)
option(LANG_RUBY "Enable Ruby language support" OFF)
option(LANG_RUST "Enable Rust language support" OFF)
option(LANG_SCALA "Enable Scala language support" OFF)

# Include
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/externals
  ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter/lib/include
  ${CMAKE_CURRENT_SOURCE_DIR}/frontend/include
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/include
)

# Source
file(GLOB_RECURSE SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/frontend/src/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/src/*.cpp
)

# External
file(GLOB EXTERNAL_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter/lib/src/lib.c
  ${CMAKE_CURRENT_SOURCE_DIR}/externals/xxHash/*.c
)

# Tree-sitter bindings
if(LANG_C)
  include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-c/bindings/c
  )
  file(GLOB C_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-c/src/*.c
  )
  list(APPEND EXTERNAL_SRC ${C_SRC})
endif()
if(LANG_CSHARP)
  include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-csharp/bindings/c
  )
  file(GLOB CSHARP_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-csharp/src/*.c
  )
  list(APPEND EXTERNAL_SRC ${CSHARP_SRC})
endif()
if(LANG_CPP)
  include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-cpp/bindings/c
  )
  file(GLOB CPP_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-cpp/src/*.c
  )
  list(APPEND EXTERNAL_SRC ${CPP_SRC})
endif()
if(LANG_CSS)
  include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-css/bindings/c
  )
  file(GLOB CSS_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-css/src/*.c
  )
  list(APPEND EXTERNAL_SRC ${CSS_SRC})
endif()
if(LANG_GO)
  include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-go/bindings/c
  )
  file(GLOB GO_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-go/src/*.c
  )
  list(APPEND EXTERNAL_SRC ${GO_SRC})
endif()
if(LANG_HASKELL)
  include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-haskell/bindings/c
  )
  file(GLOB HASKELL_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-haskell/src/*.c
  )
  list(APPEND EXTERNAL_SRC ${HASKELL_SRC})
endif()
if(LANG_HTML)
  include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-html/bindings/c
  )
  file(GLOB HTML_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-html/src/*.c
  )
  list(APPEND EXTERNAL_SRC ${HTML_SRC})
endif()
if(LANG_JAVA)
  include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-java/bindings/c
  )
  file(GLOB JAVA_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-java/src/*.c
  )
  list(APPEND EXTERNAL_SRC ${JAVA_SRC})
endif()
if(LANG_JAVASCRIPT)
  include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-javascript/bindings/c
  )
  file(GLOB JAVASCRIPT_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-javascript/src/*.c
  )
  list(APPEND EXTERNAL_SRC ${JAVASCRIPT_SRC})
endif()
if(LANG_JSON)
  include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-json/bindings/c
  )
  file(GLOB JSON_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-json/src/*.c
  )
  list(APPEND EXTERNAL_SRC ${JSON_SRC})
endif()
if(LANG_JULIA)
  include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-julia/bindings/c
  )
  file(GLOB JULIA_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-julia/src/*.c
  )
  list(APPEND EXTERNAL_SRC ${JULIA_SRC})
endif()
if(LANG_PYTHON)
  include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-python/bindings/c
  )
  file(GLOB PYTHON_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-python/src/*.c
  )
  list(APPEND EXTERNAL_SRC ${PYTHON_SRC})
endif()
if(LANG_RUBY)
  include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-ruby/bindings/c
  )
  file(GLOB RUBY_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-ruby/src/*.c
  )
  list(APPEND EXTERNAL_SRC ${RUBY_SRC})
endif()
if(LANG_RUST)
  include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-rust/bindings/c
  )
  file(GLOB RUST_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-rust/src/*.c
  )
  list(APPEND EXTERNAL_SRC ${RUST_SRC})
endif()
if(LANG_SCALA)
  include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-scala/bindings/c
  )
  file(GLOB SCALA_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/tree-sitter-scala/src/*.c
  )
  list(APPEND EXTERNAL_SRC ${SCALA_SRC})
endif()

# Executable
add_executable(${CMAKE_PROJECT_NAME}
  ${EXTERNAL_SRC}
  ${SOURCES}
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

# Binary path
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)