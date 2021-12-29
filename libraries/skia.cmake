add_library(skia INTERFACE)

# This enables our code to #include <skia/include/core/SkSurface.h>
target_include_directories(skia INTERFACE "${PROJECT_SOURCE_DIR}/libraries")
# This is expected by Skia's code, which includes "include/core/SkSurface.h"
target_include_directories(skia INTERFACE "${PROJECT_SOURCE_DIR}/libraries/skia")

target_compile_definitions(skia INTERFACE
  _CRT_SECURE_NO_WARNINGS
  _HAS_EXCEPTIONS=0
  WIN32_LEAN_AND_MEAN
  NOMINMAX
  NDEBUG
  SK_GL
  SK_HAS_JPEG_LIBRARY
  SK_USE_LIBGIFCODEC
  SK_HAS_PNG_LIBRARY
  SK_HAS_WEBP_LIBRARY
  SK_XML
)

set(SKIA_OUT_DIR "${PROJECT_SOURCE_DIR}/libraries/skia/out/Release")

if (WIN32)
  target_link_libraries(skia INTERFACE
    ${SKIA_OUT_DIR}/skia.lib
    FontSub.dll
    Gdi32.dll
    Ole32.dll
    OleAut32.dll
    OpenGL32.dll
    User32.dll
    Usp10.dll
  )
elseif (APPLE)
  target_link_libraries(skia INTERFACE
    ${SKIA_OUT_DIR}/libskia.a
    "-framework Cocoa"
    "-framework IOKit"
    "-framework CoreFoundation"
  )
else()
  target_link_libraries(skia INTERFACE
    ${SKIA_OUT_DIR}/libskia.a
    z
    jpeg
    png16
    webp
    webpmux
    webpdemux
    fontconfig
    freetype
    GL
  )
endif()
