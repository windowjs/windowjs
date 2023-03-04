vars = {
  'glfw_git': 'https://github.com/glfw/glfw.git',
  # Tag 3.3.6. Using commit hash to support gclient --no-history.
  'glfw_commit': '7d5a16ce714f0b5f4efa3262de22e4d948851525',

  # Update skia_deps below when the Skia version is updated.
  'skia_git': 'https://skia.googlesource.com/skia.git',
  'skia_commit': 'a63d89740ca8ab8572576d0ca07ab98201fe92d8',

  # Update v8_deps below when the v8 version is updated.
  'v8_git': 'https://chromium.googlesource.com/v8/v8.git',
  # Tag 11.3.81. Using commit hash to support gclient --no-history.
  'v8_commit': 'a9f2a8e50aaf4e2f41ef38d3f3be3bf433c67973',

  'libuv_git': 'https://github.com/libuv/libuv.git',
  # Tag v1.43.0. Using commit hash to support gclient --no-history.
  'libuv_commit': '988f2bfc4defb9a85a536a3e645834c161143ee0',

  'angle_git': 'https://github.com/google/angle.git',
  'angle_commit': '06edae72ba692eaf34aaca2d9a96168b5c13d2b2',

  'chromium_git': 'https://chromium.googlesource.com',
}

# ANGLE dependencies. Copied from libraries/angle/DEPS.
angle_deps = {
  'build': 'https://chromium.googlesource.com/chromium/src/build.git@3408ba573710d8e87858cc8297a07b9460b513c6',
  'testing': 'https://chromium.googlesource.com/chromium/src/testing@3ff9ded26319369788b1c025154f587271c15c89',
  'tools/protoc_wrapper': 'https://chromium.googlesource.com/chromium/src/tools/protoc_wrapper@c16b0dc8db35e95a04eaef88079237634c7f20c2',
  'third_party/abseil-cpp': 'https://chromium.googlesource.com/chromium/src/third_party/abseil-cpp@47dfacfcce5673b4bab6121b30c801550d78c577',
  'third_party/catapult': 'https://chromium.googlesource.com/catapult.git@7b5325d761f0819d2e774df3e21ebe6e340342ac',
  'third_party/jsoncpp': 'https://chromium.googlesource.com/chromium/src/third_party/jsoncpp@30a6ac108e24dabac7c2e0df4d33d55032af4ee7',
  'third_party/googletest': 'https://chromium.googlesource.com/chromium/src/third_party/googletest@17bbed2084d3127bd7bcd27283f18d7a5861bea8',
  'third_party/libjpeg_turbo': 'https://chromium.googlesource.com/chromium/deps/libjpeg_turbo.git@02959c3ee17abacfd1339ec22ea93301292ffd56',
  'third_party/nasm': 'https://chromium.googlesource.com/chromium/deps/nasm.git@9215e8e1d0fe474ffd3e16c1a07a0f97089e6224',
  'third_party/protobuf': 'https://chromium.googlesource.com/chromium/src/third_party/protobuf@d29d689c1d9446e0623af9693fe1945ecfab9c45',
  'third_party/SwiftShader': 'https://swiftshader.googlesource.com/SwiftShader@cbd31d271e4cedc709192d02b946aee9cdfe4653',
  'third_party/vulkan_memory_allocator': 'https://chromium.googlesource.com/external/github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator@5e49f57a6e71a026a54eb42e366de09a4142d24e',
  'third_party/zlib': 'https://chromium.googlesource.com/chromium/src/third_party/zlib@dd9a1330e5aeb2f194d711746a03e01391d6f17c',
  'tools/clang': 'https://chromium.googlesource.com/chromium/src/tools/clang.git@b9894ca3bc6596a03cb033ddbdd36990f11b8ce7',

  # Copied from libraries/angle/third_party/vulkan-deps/DEPS.
  'third_party/vulkan-deps': 'https://chromium.googlesource.com/vulkan-deps@3a6f074dbd38e318d7b36551276c0dca5870808b',
  'third_party/vulkan-deps/glslang/src': 'https://chromium.googlesource.com/external/github.com/KhronosGroup/glslang@16e3b403aabc7681af428792ddbbfa8ae556b5bb',
  'third_party/vulkan-deps/spirv-cross/src': 'https://chromium.googlesource.com/external/github.com/KhronosGroup/SPIRV-Cross@1a36968e5d92e08ba38335ad00701da21c627a13',
  'third_party/vulkan-deps/spirv-headers/src': 'https://chromium.googlesource.com/external/github.com/KhronosGroup/SPIRV-Headers@b42ba6d92faf6b4938e6f22ddd186dbdacc98d78',
  'third_party/vulkan-deps/spirv-tools/src': 'https://chromium.googlesource.com/external/github.com/KhronosGroup/SPIRV-Tools@7fa9e746ef815b27f7747c9bab2689c113fb33d9',
  'third_party/vulkan-deps/vulkan-headers/src': 'https://chromium.googlesource.com/external/github.com/KhronosGroup/Vulkan-Headers@1dace16d8044758d32736eb59802d171970e9448',
  'third_party/vulkan-deps/vulkan-loader/src': 'https://chromium.googlesource.com/external/github.com/KhronosGroup/Vulkan-Loader@cdebcc4ee08a8e18521cb0486db7201b8793bd4b',
  'third_party/vulkan-deps/vulkan-tools/src': 'https://chromium.googlesource.com/external/github.com/KhronosGroup/Vulkan-Tools@bb32aa13d4920261b5086219028ef329605d0126',
  'third_party/vulkan-deps/vulkan-validation-layers/src': 'https://chromium.googlesource.com/external/github.com/KhronosGroup/Vulkan-ValidationLayers@54c295b067de5531867a842b4b3579ee1d3c8545',
}

# Skia dependencies. Copied from libraries/skia/DEPS.
skia_deps = {
  "third_party/externals/expat"           : "https://chromium.googlesource.com/external/github.com/libexpat/libexpat.git@a28238bdeebc087071777001245df1876a11f5ee",
  "third_party/externals/libjpeg-turbo"   : "https://chromium.googlesource.com/chromium/deps/libjpeg_turbo.git@24e310554f07c0fdb8ee52e3e708e4f3e9eb6e20",
  "third_party/externals/libpng"          : "https://skia.googlesource.com/third_party/libpng.git@386707c6d19b974ca2e3db7f5c61873813c6fe44",
  "third_party/externals/libwebp"         : "https://chromium.googlesource.com/webm/libwebp.git@69c7f16111ec582bf1e7cb4d0d4f8d127e28a715",
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
      "git",
      "apply",
      "--directory",
      "libraries/angle/build",
      "libraries/angle_build.patch",
    ],
  },
  {
    "action": [
      "git",
      "apply",
      "--directory",
      "libraries/angle/third_party/vulkan-deps/glslang/src",
      "libraries/angle_glslang.patch",
    ],
  },
  {
    "action": [
      "python",
      "libraries/angle_build.py",
    ],
  },
]
