set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

set(GLAD_API "gl=3.3")
set(GLAD_REPRODUCIBLE ON)

set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)

set(UV_LINT_WALL OFF)
set(UV_LINT_W4 OFF)

add_subdirectory(glad EXCLUDE_FROM_ALL)
add_subdirectory(glfw EXCLUDE_FROM_ALL)
add_subdirectory(libuv EXCLUDE_FROM_ALL)

include(skia.cmake)
include(v8.cmake)
