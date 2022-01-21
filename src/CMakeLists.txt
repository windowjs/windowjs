set(CMAKE_CXX_STANDARD 17)

if(MSVC)
  # Enable all warnings.
  add_compile_options(-W3)
  # Warnings are errors.
  add_compile_options(-WX)

  # Disable RTTI.
  add_compile_options(/GR-)

  if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Disable numeric conversion warnings.
    add_compile_options(-wd4267)
    add_compile_options(-wd4244)
    add_compile_options(-wd4305)
    # Disable 4068: unknown pragma (for clang pragmas)
    add_compile_options(-wd4068)
    # Disable warnings in external headers.
    add_compile_options(-external:W0)
    add_compile_options(-external:anglebrackets)
  endif()

  add_compile_options(-D_CRT_SECURE_NO_WARNINGS)

  if(CMAKE_BUILD_TYPE STREQUAL Release)
    add_compile_options(-O2)
  else()
    add_compile_options(-Zi)
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
      # Uncomment to detect leaks:
      # add_compile_options(-fsanitize=address)
    endif()
  endif()
else()
  add_compile_options(-Wall)
  add_compile_options(-Werror)
  add_compile_options(-fno-exceptions)
  add_compile_options(-fno-rtti)

  if(CMAKE_BUILD_TYPE STREQUAL Release)
    add_compile_options(-O3)
  else()
    add_compile_options(-g)

    # Uncomment to detect leaks:
    # add_compile_options(-fsanitize=address -fno-omit-frame-pointer -O1)
    # add_link_options(-fsanitize=address)
  endif()
endif()

if(CMAKE_BUILD_TYPE STREQUAL Release)
  add_compile_options(-DWINDOWJS_RELEASE_BUILD)
endif()

add_subdirectory(tools)

# Generate embedded blobs.
add_custom_command(
    OUTPUT generated_blobs.cc
    COMMAND embed ${CMAKE_CURRENT_BINARY_DIR}/generated_blobs.cc kEmbeddedConsoleSource ./src/console.js kEmbeddedWelcomeSource ./src/welcome.js
    DEPENDS embed console.js welcome.js
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Generate git revision info.
include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_HASH)
git_get_exact_tag(GIT_TAG)
if("${GIT_TAG}" MATCHES "NOTFOUND")
  set(GIT_TAG "")
endif()
configure_file(version.cc.in generated_version.cc)

add_executable(windowjs
    args.cc
    args.h
    console.cc
    console.h
    css.cc
    css.h
    fail.cc
    fail.h
    file.cc
    file.h
    generated_blobs.cc
    generated_version.cc
    js.cc
    js.h
    js_api.cc
    js_api.h
    js_api_canvas.cc
    js_api_canvas.h
    js_api_file.cc
    js_api_file.h
    js_api_process.cc
    js_api_process.h
    js_events.cc
    js_events.h
    js_scope.h
    js_strings.cc
    js_strings.h
    json.cc
    json.h
    main.cc
    main.h
    platform.h
    render_canvas.cc
    render_canvas.h
    signal.h
    stats.cc
    stats.h
    subprocess.cc
    subprocess.h
    task_queue.cc
    task_queue.h
    thread.cc
    thread.h
    util.h
    version.h
    weak.cc
    weak.h
    window.cc
    window.h
    zip.cc
    zip.h
)

target_include_directories(windowjs PRIVATE ../libraries/v8/third_party/zlib)
target_link_libraries(windowjs PRIVATE glad glfw skia uv_a v8)

if(MSVC)
  if(CMAKE_BUILD_TYPE STREQUAL Release)
    # Configure the target to be a windows instead of a console application.
    set_target_properties(windowjs PROPERTIES LINK_FLAGS "/entry:mainCRTStartup /subsystem:windows")
  endif()
endif()
