cmake_minimum_required(VERSION 3.15)

if(WIN32)
  if(DEFINED LLVM_DIR)
    set(CMAKE_C_COMPILER "${LLVM_DIR}/bin/clang-cl.exe")
    set(CMAKE_CXX_COMPILER "${LLVM_DIR}/bin/clang-cl.exe")
    set(CMAKE_LINKER "${LLVM_DIR}/bin/lld-link.exe")
  endif()

  # Force static linking of runtime library for all targets.
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()

project(window.js
    VERSION 0.1
    DESCRIPTION "Window.js"
    LANGUAGES C CXX
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libraries/cmake")

add_subdirectory(libraries)
add_subdirectory(src)
