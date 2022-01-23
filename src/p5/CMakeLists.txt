# Merge the p5.js loader and the p5.js library.
add_custom_command(
    OUTPUT merged_p5.js
    COMMAND merge-p5 ${CMAKE_CURRENT_BINARY_DIR}/merged_p5.js ./src/p5/p5.js ./libraries/p5.min.js
    DEPENDS merge-p5 p5.js ../../libraries/p5.min.js
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

# Generate the embedded p5 source.
add_custom_command(
    OUTPUT generated_p5.cc
    COMMAND embed ${CMAKE_CURRENT_BINARY_DIR}/generated_p5.cc kEmbeddedDefaultSource ${CMAKE_CURRENT_BINARY_DIR}/merged_p5.js
    DEPENDS embed merged_p5.js
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

add_executable(windowjs-p5 EXCLUDE_FROM_ALL
    config_windowjs_p5.cc
    generated_p5.cc
)

target_link_libraries(windowjs-p5 PRIVATE windowjs-library)

if(MSVC)
  if(CMAKE_BUILD_TYPE STREQUAL Release)
    # Configure the target to be a windows instead of a console application.
    set_target_properties(windowjs-p5 PROPERTIES LINK_FLAGS "/entry:mainCRTStartup /subsystem:windows")
  endif()
endif()
