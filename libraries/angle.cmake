add_library(angle INTERFACE)

target_include_directories(angle INTERFACE "${PROJECT_SOURCE_DIR}/libraries/angle/include")

target_compile_definitions(angle INTERFACE
  ANGLE_EXPORT=
  ANGLE_STATIC=1
  ANGLE_UTIL_EXPORT=
  EGLAPI=
  GL_APICALL=
  GL_API=
)

set(ANGLE_OUT_DIR "${PROJECT_SOURCE_DIR}/libraries/angle/out/Release")

if (WIN32)
  target_link_libraries(angle INTERFACE
    ${ANGLE_OUT_DIR}/obj/libEGL_static.lib
    dxguid.lib
    dxgi.lib
  )
elseif (APPLE)
  target_link_libraries(angle INTERFACE
    ${ANGLE_OUT_DIR}/obj/libEGL_static.a
    "-framework Foundation"
    "-framework IOKit"
    "-framework CoreFoundation"
    "-framework CoreGraphics"
    "-framework OpenGL"
    "-framework IOSurface"
    "-framework QuartzCore"
    "-framework Cocoa"
  )
else()
  target_link_libraries(angle INTERFACE
    ${ANGLE_OUT_DIR}/obj/libEGL_static.a
    atomic
    dl
    pthread
    rt
    X11
    Xext
    xcb
  )
endif()
