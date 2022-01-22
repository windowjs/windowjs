add_executable(embed
    embed.cc
    ../fail.cc
    ../fail.h
    ../generated_version.cc
    ../zip.cc
    ../zip.h
)

target_include_directories(embed PRIVATE ../../libraries/v8/third_party/zlib)
target_link_libraries(embed PRIVATE v8)

add_executable(merge-p5
    merge_p5.cc
)
