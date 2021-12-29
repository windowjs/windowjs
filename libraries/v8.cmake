add_library(v8 INTERFACE)

target_include_directories(v8 INTERFACE
    "${PROJECT_SOURCE_DIR}/libraries/v8/include"
    "${PROJECT_SOURCE_DIR}/libraries/v8"
    "${PROJECT_SOURCE_DIR}/libraries"
)

target_compile_definitions(v8 INTERFACE
  V8_COMPRESS_POINTERS
)

set(V8_OUT_DIR "${PROJECT_SOURCE_DIR}/libraries/v8/out/Release")

if (WIN32)
  target_link_libraries(v8 INTERFACE
    ${V8_OUT_DIR}/obj/v8_monolith.lib
    DbgHelp.dll
    WinMM.dll
  )
else()
  target_link_libraries(v8 INTERFACE
    ${V8_OUT_DIR}/obj/libv8_monolith.a
    pthread
    dl
  )
endif()
