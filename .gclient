vars = {
  'glad_git': 'https://github.com/Dav1dde/glad.git',
  'glad_commit': '7ece538856bf124d798ab323c8e1e64ebb83cb50',

  'glfw_git': 'https://github.com/glfw/glfw.git',
  # Tag 3.3.5. Using commit hash to support gclient --no-history.
  'glfw_commit': 'd3b73abba0cab8cbb2a638151477f54d8502a07e',

  # Update skia_deps below when the Skia version is updated.
  'skia_git': 'https://skia.googlesource.com/skia.git',
  'skia_commit': '0f253afb724ee9f2c4424463893727c8b3196973',

  # Update v8_deps below when the v8 version is updated.
  'v8_git': 'https://chromium.googlesource.com/v8/v8.git',
  # Tag 9.8.146. Using commit hash to support gclient --no-history.
  'v8_commit': '8337eb615b587f52a501b2a815fdcb39c4fa0584',

  'libuv_git': 'https://github.com/libuv/libuv.git',
  # Tag v1.42.0. Using commit hash to support gclient --no-history.
  'libuv_commit': '6ce14710da7079eb248868171f6343bc409ea3a4',
}

# Skia dependencies. Copied from libraries/skia/DEPS.
skia_deps = {
  #"buildtools"                            : "https://chromium.googlesource.com/chromium/src/buildtools.git@b138e6ce86ae843c42a1a08f37903207bebcca75",
  #"third_party/externals/angle2"          : "https://chromium.googlesource.com/angle/angle.git@fe42ac4df49fcd8b1e371276972b201f78a9a00d",
  #"third_party/externals/brotli"          : "https://skia.googlesource.com/external/github.com/google/brotli.git@e61745a6b7add50d380cfd7d3883dd6c62fc2c71",
  #"third_party/externals/d3d12allocator"  : "https://skia.googlesource.com/external/github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator.git@169895d529dfce00390a20e69c2f516066fe7a3b",
  # Dawn requires jinja2 and markupsafe for the code generator, tint for SPIRV compilation, and abseil for string formatting.
  # When the Dawn revision is updated these should be updated from the Dawn DEPS as well.
  #"third_party/externals/dawn"            : "https://dawn.googlesource.com/dawn.git@b6c1a51775ae80695eb6c474691d02366d4eb7ad",
  #"third_party/externals/jinja2"          : "https://chromium.googlesource.com/chromium/src/third_party/jinja2@ee69aa00ee8536f61db6a451f3858745cf587de6",
  #"third_party/externals/markupsafe"      : "https://chromium.googlesource.com/chromium/src/third_party/markupsafe@0944e71f4b2cb9a871bcbe353f95e889b64a611a",
  #"third_party/externals/tint"            : "https://dawn.googlesource.com/tint@f71784fc2b2a4b0ae020a1bfdcc6da95ead7ad79",
  #"third_party/externals/abseil-cpp"      : "https://chromium.googlesource.com/chromium/src/third_party/abseil-cpp@789af048b388657987c59d4da406859034fe310f",
  #"third_party/externals/dng_sdk"         : "https://android.googlesource.com/platform/external/dng_sdk.git@c8d0c9b1d16bfda56f15165d39e0ffa360a11123",
  #"third_party/externals/egl-registry"    : "https://skia.googlesource.com/external/github.com/KhronosGroup/EGL-Registry@a0bca08de07c7d7651047bedc0b653cfaaa4f2ae",
  "third_party/externals/expat"           : "https://chromium.googlesource.com/external/github.com/libexpat/libexpat.git@a28238bdeebc087071777001245df1876a11f5ee",
  #"third_party/externals/freetype"        : "https://chromium.googlesource.com/chromium/src/third_party/freetype2.git@cff026d41599945498044d2f4dcc0e610ffb6929",
  #"third_party/externals/harfbuzz"        : "https://chromium.googlesource.com/external/github.com/harfbuzz/harfbuzz.git@3160789701463bcd822c26010da4de33c92c85d9",
  #"third_party/externals/icu"             : "https://chromium.googlesource.com/chromium/deps/icu.git@a0718d4f121727e30b8d52c7a189ebf5ab52421f",
  #"third_party/externals/imgui"           : "https://skia.googlesource.com/external/github.com/ocornut/imgui.git@9418dcb69355558f70de260483424412c5ca2fce",
  #"third_party/externals/libgifcodec"     : "https://skia.googlesource.com/libgifcodec@fd59fa92a0c86788dcdd84d091e1ce81eda06a77",
  "third_party/externals/libjpeg-turbo"   : "https://chromium.googlesource.com/chromium/deps/libjpeg_turbo.git@24e310554f07c0fdb8ee52e3e708e4f3e9eb6e20",
  "third_party/externals/libpng"          : "https://skia.googlesource.com/third_party/libpng.git@386707c6d19b974ca2e3db7f5c61873813c6fe44",
  "third_party/externals/libwebp"         : "https://chromium.googlesource.com/webm/libwebp.git@9ce5843dbabcfd3f7c39ec7ceba9cbeb213cbfdf",
  #"third_party/externals/microhttpd"      : "https://android.googlesource.com/platform/external/libmicrohttpd@748945ec6f1c67b7efc934ab0808e1d32f2fb98d",
  #"third_party/externals/oboe"            : "https://chromium.googlesource.com/external/github.com/google/oboe.git@b02a12d1dd821118763debec6b83d00a8a0ee419",
  #"third_party/externals/opengl-registry" : "https://skia.googlesource.com/external/github.com/KhronosGroup/OpenGL-Registry@14b80ebeab022b2c78f84a573f01028c96075553",
  #"third_party/externals/piex"            : "https://android.googlesource.com/platform/external/piex.git@bb217acdca1cc0c16b704669dd6f91a1b509c406",
  #"third_party/externals/sfntly"          : "https://chromium.googlesource.com/external/github.com/googlei18n/sfntly.git@b55ff303ea2f9e26702b514cf6a3196a2e3e2974",
  #"third_party/externals/spirv-cross"     : "https://chromium.googlesource.com/external/github.com/KhronosGroup/SPIRV-Cross@0e2880ab990e79ce6cc8c79c219feda42d98b1e8",
  #"third_party/externals/spirv-headers"   : "https://skia.googlesource.com/external/github.com/KhronosGroup/SPIRV-Headers.git@814e728b30ddd0f4509233099a3ad96fd4318c07",
  #"third_party/externals/spirv-tools"     : "https://skia.googlesource.com/external/github.com/KhronosGroup/SPIRV-Tools.git@4b092d2ab81854e61632bdd1e658907f0071c37e",
  #"third_party/externals/swiftshader"     : "https://swiftshader.googlesource.com/SwiftShader@a465cca8ec8dffa0a308731e3aec9a26c9215f6c",
  #"third_party/externals/v8"              : "https://chromium.googlesource.com/v8/v8.git@5f1ae66d5634e43563b2d25ea652dfb94c31a3b4",
  #"third_party/externals/wuffs"           : "https://skia.googlesource.com/external/github.com/google/wuffs-mirror-release-c.git@600cd96cf47788ee3a74b40a6028b035c9fd6a61",
  "third_party/externals/zlib"            : "https://chromium.googlesource.com/chromium/src/third_party/zlib@c876c8f87101c5a75f6014b0f832499afeb65b73",
}

# v8 dependencies. Copied from libraries/v8/DEPS.
v8_deps = {
  'base/trace_event/common'                : 'https://chromium.googlesource.com/chromium/src/base/trace_event/common.git@7f36dbc19d31e2aad895c60261ca8f726442bfbb',
  'build'                                  : 'https://chromium.googlesource.com/chromium/src/build.git@746224dc80909d8428c2d37ab26e4a4e48168b11',
  #'buildtools'                             : 'https://chromium.googlesource.com/chromium/src/buildtools.git@075dd7e22837a69189003e4fa84499acf63188cf',
  #'buildtools/clang_format/script'         : 'https://chromium.googlesource.com/external/github.com/llvm/llvm-project/clang/tools/clang-format.git@99876cacf78329e5f99c244dbe42ccd1654517a0',
  #'buildtools/third_party/libc++/trunk'    : 'https://chromium.googlesource.com/external/github.com/llvm/llvm-project/libcxx.git@79a2e924d96e2fc1e4b937c42efd08898fa472d7',
  #'buildtools/third_party/libc++abi/trunk' : 'https://chromium.googlesource.com/external/github.com/llvm/llvm-project/libcxxabi.git@89f2e82120461d34098edd216e57aa743f441107',
  #'buildtools/third_party/libunwind/trunk' : 'https://chromium.googlesource.com/external/github.com/llvm/llvm-project/libunwind.git@0f447a31381e0343f8b06c3f7049c076ddfbb1fc',
  #'test/benchmarks/data'                   : 'https://chromium.googlesource.com/v8/deps/third_party/benchmarks.git@05d7188267b4560491ff9155c5ee13e207ecd65f',
  #'test/mozilla/data'                      : 'https://chromium.googlesource.com/v8/deps/third_party/mozilla-tests.git@f6c578a10ea707b1a8ab0b88943fe5115ce2b9be',
  #'test/test262/data'                      : 'https://chromium.googlesource.com/external/github.com/tc39/test262.git@4b7f8b49cc07a89c093475f284ef112f26a8529f',
  #'test/test262/harness'                   : 'https://chromium.googlesource.com/external/github.com/test262-utils/test262-harness-py.git@278bcfaed0dcaa13936831fb1769d15e7c1e3b2b',
  #'third_party/depot_tools'                : 'https://chromium.googlesource.com/chromium/tools/depot_tools.git@3e68015b7f43914a60492618c3a010c5812662dc',
  #'third_party/google_benchmark/src'       : 'https://chromium.googlesource.com/external/github.com/google/benchmark.git@fd258bbd131552beb2483b960a5e4b69553fdacf',
  'third_party/googletest/src'             : 'https://chromium.googlesource.com/external/github.com/google/googletest.git@1b2606425c4040cacadaa22689423ec0a29f316d',
  #'third_party/icu'                        : 'https://chromium.googlesource.com/chromium/deps/icu.git@edf883ad2db9c723b058a6a17a146d68d6343143',
  #'third_party/instrumented_libraries'     : 'https://chromium.googlesource.com/chromium/src/third_party/instrumented_libraries.git@e09c4b66b6e87116eb190651421f1a6e2f3b9c52',
  'third_party/jinja2'                     : 'https://chromium.googlesource.com/chromium/src/third_party/jinja2.git@ee69aa00ee8536f61db6a451f3858745cf587de6',
  #'third_party/jsoncpp/source'             : 'https://chromium.googlesource.com/external/github.com/open-source-parsers/jsoncpp.git@9059f5cad030ba11d37818847443a53918c327b1',
  #'third_party/logdog/logdog'              : 'https://chromium.googlesource.com/infra/luci/luci-py/client/libs/logdog@17ec234f823f7bff6ada6584fdbbee9d54b8fc58',
  'third_party/markupsafe'                 : 'https://chromium.googlesource.com/chromium/src/third_party/markupsafe.git@1b882ef6372b58bfd55a3285f37ed801be9137cd',
  #'third_party/perfetto'                   : 'https://android.googlesource.com/platform/external/perfetto.git@aa4385bc5997ecad4c633885e1b331b1115012fb',
  #'third_party/protobuf'                   : 'https://chromium.googlesource.com/external/github.com/google/protobu@6a59a2ad1f61d9696092f79b6d74368b4d7970a3',
  'third_party/zlib'                       : 'https://chromium.googlesource.com/chromium/src/third_party/zlib.git@efd9399ae01364926be2a38946127fdf463480db',
  'tools/clang'                            : 'https://chromium.googlesource.com/chromium/src/tools/clang.git@16279ec9aa9650adc022985ea0831c9ca0c62fd9',
  #'tools/clang/dsymutil': {
  #  'packages': [
  #    {
  #      'package': 'chromium/llvm-build-tools/dsymutil',
  #      'version': 'M56jPzDv1620Rnm__jTMYS62Zi8rxHVq7yw0qeBFEgkC',
  #    }
  #  ],
  #  'condition': 'checkout_mac',
  #  'dep_type': 'cipd',
  #},
  #'tools/luci-go': {
  #    'packages': [
  #      {
  #        'package': 'infra/tools/luci/isolate/${{platform}}',
  #        'version': Var('luci_go'),
  #      },
  #      {
  #        'package': 'infra/tools/luci/swarming/${{platform}}',
  #        'version': Var('luci_go'),
  #      },
  #    ],
  #    'condition': 'host_cpu != "s390" and host_os != "aix"',
  #    'dep_type': 'cipd',
  #},
}

solutions = [
  {
    "name": "libraries",
    "url": None,
    "managed": False,
    "custom_deps": {
      "libraries/glad": vars['glad_git'] + '@' + vars['glad_commit'],
      "libraries/glfw": vars['glfw_git'] + '@' + vars['glfw_commit'],
      "libraries/skia": vars['skia_git'] + '@' + vars['skia_commit'],
      "libraries/v8": vars['v8_git'] + '@' + vars['v8_commit'],
      "libraries/libuv": vars['libuv_git'] + '@' + vars['libuv_commit'],
    },
  },
]

for (k, v) in skia_deps.items():
  solutions[0]['custom_deps']['libraries/skia/' + k] = v

for (k, v) in v8_deps.items():
  solutions[0]['custom_deps']['libraries/v8/' + k] = v

hooks = [
  # From libraries/v8/DEPS:
  {
    # Update LASTCHANGE.
    'name': 'lastchange',
    'cwd': 'libraries/v8',
    'pattern': '.',
    'action': ['python3', 'build/util/lastchange.py',
               '-o', 'build/util/LASTCHANGE'],
  },
  {
    'name': 'sysroot_x64',
    'cwd': 'libraries/v8',
    'pattern': '.',
    'condition': 'checkout_linux and checkout_x64',
    'action': ['python3', 'build/linux/sysroot_scripts/install-sysroot.py',
               '--arch=x64'],
  },
  {
    # Update the Windows toolchain if necessary.
    'name': 'win_toolchain',
    'cwd': 'libraries/v8',
    'pattern': '.',
    'condition': 'checkout_win',
    'action': ['python3', 'build/vs_toolchain.py', 'update', '--force'],
  },
  {
    # Update the Mac toolchain if necessary.
    'name': 'mac_toolchain',
    'cwd': 'libraries/v8',
    'pattern': '.',
    'condition': 'checkout_mac',
    'action': ['python3', 'build/mac_toolchain.py'],
  },
  {
    # Note: On Win, this should run after win_toolchain, as it may use it.
    'name': 'clang',
    'cwd': 'libraries/v8',
    'pattern': '.',
    # clang not supported on aix
    'condition': 'host_os != "aix"',
    'action': ['python3', 'tools/clang/scripts/update.py'],
  },

  {
    "action": [
      "git",
      "apply",
      "--directory",
      "libraries/glfw",
      "libraries/glfw.patch",
    ],
  },
  {
    "action": [
      "git",
      "apply",
      "--directory",
      "libraries/glad",
      "libraries/glad.patch",
    ],
  },
  {
    "action": [
      "git",
      "apply",
      "--directory",
      "libraries/v8",
      "libraries/v8.patch",
    ],
  },
  {
    "action": [
      "git",
      "apply",
      "--directory",
      "libraries/v8/build",
      "libraries/v8_build.patch",
    ],
  },
  {
    "action": [
      "python",
      "libraries/v8_build.py",
    ],
  },
]
