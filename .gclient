vars = {
  'glfw_git': 'https://github.com/glfw/glfw.git',
  # Tag 3.3.6. Using commit hash to support gclient --no-history.
  'glfw_commit': '7d5a16ce714f0b5f4efa3262de22e4d948851525',

  # Update skia_deps below when the Skia version is updated.
  'skia_git': 'https://skia.googlesource.com/skia.git',
  'skia_commit': '9f561bc846d00e1e674987cc0f74ddd808c438f1',

  # Update v8_deps below when the v8 version is updated.
  'v8_git': 'https://chromium.googlesource.com/v8/v8.git',
  # Tag 11.3.81. Using commit hash to support gclient --no-history.
  'v8_commit': 'a9f2a8e50aaf4e2f41ef38d3f3be3bf433c67973',

  'libuv_git': 'https://github.com/libuv/libuv.git',
  # Tag v1.43.0. Using commit hash to support gclient --no-history.
  'libuv_commit': '988f2bfc4defb9a85a536a3e645834c161143ee0',

  'angle_git': 'https://github.com/google/angle.git',
  'angle_commit': '51ddcabfd3abe64b392767828375ae688e5d4f3f',

  'chromium_git': 'https://chromium.googlesource.com',
}

# ANGLE dependencies. Copied from libraries/angle/DEPS.
angle_deps = {
  'build': 'https://chromium.googlesource.com/chromium/src/build.git@bdadc3003bd013080450432ed97e0afa30e50f2c',
  'testing': 'https://chromium.googlesource.com/chromium/src/testing@235da779448dadfb48ae6f503339066d56385dad',
  'tools/protoc_wrapper': 'https://chromium.googlesource.com/chromium/src/tools/protoc_wrapper@30392509cfa0f370f1df55dfb75bc37e3fad19ae',
  'third_party/abseil-cpp': 'https://chromium.googlesource.com/chromium/src/third_party/abseil-cpp@28c6deacabb12fb4c6823562b4693ab984c3f116',
  'third_party/catapult': 'https://chromium.googlesource.com/catapult.git@dabfa7bf23b8995e8b45f9c0f4c3ced4d1c04203',
  'third_party/jsoncpp': 'https://chromium.googlesource.com/chromium/src/third_party/jsoncpp@f62d44704b4da6014aa231cfc116e7fd29617d2a',
  'third_party/googletest': 'https://chromium.googlesource.com/chromium/src/third_party/googletest@17bbed2084d3127bd7bcd27283f18d7a5861bea8',
  'third_party/libjpeg_turbo': 'https://chromium.googlesource.com/chromium/deps/libjpeg_turbo.git@aa4075f116e4312537d0d3e9dbd5e31096539f94',
  'third_party/nasm': 'https://chromium.googlesource.com/chromium/deps/nasm.git@7fc833e889d1afda72c06220e5bed8fb43b2e5ce',
  'third_party/protobuf': 'https://chromium.googlesource.com/chromium/src/third_party/protobuf@57396ed0a2818b1ad68101832eecf53a38cd321f',
  'third_party/SwiftShader': 'https://swiftshader.googlesource.com/SwiftShader@0ba0b45490cd209448c5f976f41b34a746c9de5d',
  'third_party/vulkan_memory_allocator': 'https://chromium.googlesource.com/external/github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator@ebe84bec02c041d28f902da0214bf442743fc907',
  'third_party/zlib': 'https://chromium.googlesource.com/chromium/src/third_party/zlib@90e67ba3f8998a3532fc8e3db9539aada8060d43',
  'third_party/astc-encoder/src': 'https://chromium.googlesource.com/external/github.com/ARM-software/astc-encoder@573c475389bf51d16a5c3fc8348092e094e50e8f',
  'tools/clang': 'https://chromium.googlesource.com/chromium/src/tools/clang.git@64e9f9321c450d938fec79ef8e4431fd1a08f5ce',

  # Copied from libraries/angle/third_party/vulkan-deps/DEPS.
  'third_party/vulkan-deps': 'https://chromium.googlesource.com/vulkan-deps@5d7ca659e80483a7bd85a0bf2ca615cdb8902f05',
  'third_party/vulkan-deps/glslang/src': 'https://chromium.googlesource.com/external/github.com/KhronosGroup/glslang@9cdfc5a511e8fa9c0713a1c84503be74e7fae029',
  'third_party/vulkan-deps/spirv-cross/src': 'https://chromium.googlesource.com/external/github.com/KhronosGroup/SPIRV-Cross@7512345f61e5f9b543ebb87df678f3fe7735587b',
  'third_party/vulkan-deps/spirv-headers/src': 'https://chromium.googlesource.com/external/github.com/KhronosGroup/SPIRV-Headers@aa331ab0ffcb3a67021caa1a0c1c9017712f2f31',
  'third_party/vulkan-deps/spirv-tools/src': 'https://chromium.googlesource.com/external/github.com/KhronosGroup/SPIRV-Tools@9d71fb676491bc12be3398665236f407341b83c8',
  'third_party/vulkan-deps/vulkan-headers/src': 'https://chromium.googlesource.com/external/github.com/KhronosGroup/Vulkan-Headers@a3dd2655a3a68c2a67c55a0f8b77dcb8b166ada2',
  'third_party/vulkan-deps/vulkan-loader/src': 'https://chromium.googlesource.com/external/github.com/KhronosGroup/Vulkan-Loader@3db19f3e3d8b0c96dc1ffcb5888c06e306df478c',
  'third_party/vulkan-deps/vulkan-tools/src': 'https://chromium.googlesource.com/external/github.com/KhronosGroup/Vulkan-Tools@5bcfa1605e110150710170bfe199aa60e7f048fe',
  'third_party/vulkan-deps/vulkan-validation-layers/src': 'https://chromium.googlesource.com/external/github.com/KhronosGroup/Vulkan-ValidationLayers@b8eae8a7c87c837cbce9b49fdadc5ed3825e306d',
}

# Skia dependencies. Copied from libraries/skia/DEPS.
skia_deps = {
  "third_party/externals/expat"           : "https://chromium.googlesource.com/external/github.com/libexpat/libexpat.git@441f98d02deafd9b090aea568282b28f66a50e36",
  "third_party/externals/libjpeg-turbo"   : "https://chromium.googlesource.com/chromium/deps/libjpeg_turbo.git@ed683925e4897a84b3bffc5c1414c85b97a129a3",
  "third_party/externals/libpng"          : "https://skia.googlesource.com/third_party/libpng.git@386707c6d19b974ca2e3db7f5c61873813c6fe44",
  "third_party/externals/libwebp"         : "https://chromium.googlesource.com/webm/libwebp.git@fd7b5d48464475408d32d2611bdb6947d4246b97",
  "third_party/externals/wuffs"           : "https://skia.googlesource.com/external/github.com/google/wuffs-mirror-release-c.git@a0041ac0310b3156b963e2f2bea09245f25ec073",
  "third_party/externals/zlib"            : "https://chromium.googlesource.com/chromium/src/third_party/zlib@c876c8f87101c5a75f6014b0f832499afeb65b73",
}

# v8 dependencies. Copied from libraries/v8/DEPS.
v8_deps = {
  'base/trace_event/common'                : 'https://chromium.googlesource.com/chromium/src/base/trace_event/common.git@147f65333c38ddd1ebf554e89965c243c8ce50b3',
  'build'                                  : 'https://chromium.googlesource.com/chromium/src/build.git@ec8c8d82bdd61434e1e66c3f248200a900944ab0',
  'third_party/googletest/src'             : 'https://chromium.googlesource.com/external/github.com/google/googletest.git@af29db7ec28d6df1c7f0f745186884091e602e07',
  'third_party/jinja2'                     : 'https://chromium.googlesource.com/chromium/src/third_party/jinja2.git@264c07d7e64f2874434a3b8039e101ddf1b01e7e',
  'third_party/markupsafe'                 : 'https://chromium.googlesource.com/chromium/src/third_party/markupsafe.git@13f4e8c9e206567eeb13bf585406ddc574005748',
  'third_party/zlib'                       : 'https://chromium.googlesource.com/chromium/src/third_party/zlib.git@90e67ba3f8998a3532fc8e3db9539aada8060d43',
  'tools/clang'                            : 'https://chromium.googlesource.com/chromium/src/tools/clang.git@697c297330684462bbbccb2bead783b0d94f6d50',
}

solutions = [
  {
    "name": "libraries",
    "url": None,
    "managed": False,
    "custom_deps": {
      "libraries/angle": vars['angle_git'] + '@' + vars['angle_commit'],
      "libraries/glfw": vars['glfw_git'] + '@' + vars['glfw_commit'],
      "libraries/libuv": vars['libuv_git'] + '@' + vars['libuv_commit'],
      "libraries/skia": vars['skia_git'] + '@' + vars['skia_commit'],
      "libraries/v8": vars['v8_git'] + '@' + vars['v8_commit'],
    },
  },
]

for (k, v) in angle_deps.items():
  solutions[0]['custom_deps']['libraries/angle/' + k] = v

for (k, v) in skia_deps.items():
  solutions[0]['custom_deps']['libraries/skia/' + k] = v

for (k, v) in v8_deps.items():
  solutions[0]['custom_deps']['libraries/v8/' + k] = v

hooks = [
  # From libraries/angle/DEPS:
  {
    'cwd': 'libraries/angle',
    'name': 'win_toolchain',
    'pattern': '.',
    'condition': 'checkout_win',
    'action': ['python3', 'build/vs_toolchain.py', 'update', '--force'],
  },
  {
    'cwd': 'libraries/angle',
    'name': 'clang',
    'pattern': '.',
    'action': ['python3', 'tools/clang/scripts/update.py'],
  },
  {
    'cwd': 'libraries/angle',
    # Update LASTCHANGE.
    'name': 'lastchange',
    'pattern': '.',
    'action': ['python3', 'build/util/lastchange.py',
               '-o', 'build/util/LASTCHANGE'],
  },
  {
    'cwd': 'libraries/angle',
    'name': 'rc_win',
    'pattern': '.',
    'condition': 'checkout_win',
    'action': [ 'python3',
                '../depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-browser-clang/rc',
                '-s', 'build/toolchain/win/rc/win/rc.exe.sha1',
    ],
  },
  {
    'cwd': 'libraries/angle',
    'name': 'sysroot_x64',
    'pattern': '.',
    'condition': 'checkout_linux',
    'action': ['python3', 'build/linux/sysroot_scripts/install-sysroot.py',
               '--arch=x64'],
  },

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
  {
    "action": [
      "git",
      "apply",
      "--directory",
      "libraries/skia",
      "libraries/skia.patch",
    ],
  },
  {
    "action": [
      "git",
      "apply",
      "--directory",
      "libraries/angle",
      "libraries/angle.patch",
    ],
  },
  {
    "action": [
      "python",
      "libraries/angle_build.py",
    ],
  },
]
