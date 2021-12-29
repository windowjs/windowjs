vars = {

  'glad_git': 'https://github.com/Dav1dde/glad.git',
  'glad_commit': '7ece538856bf124d798ab323c8e1e64ebb83cb50',

  'glfw_git': 'https://github.com/glfw/glfw.git',
  'glfw_commit': '3.3.5',

  # Update skia_deps below when the Skia version is updated.
  'skia_git': 'https://skia.googlesource.com/skia.git',
  'skia_commit': '0f253afb724ee9f2c4424463893727c8b3196973',

  'v8_git': 'https://chromium.googlesource.com/v8/v8.git',
  'v8_commit': '9.8.146',

  'libuv_git': 'https://github.com/libuv/libuv.git',
  'libuv_commit': 'v1.42.0',
}

# Skia dependencies. Copied from libraries/skia/DEPS because it sets
# gclient_gn_args_from, which breaks gclient.
skia_deps = {
  "buildtools"                            : "https://chromium.googlesource.com/chromium/src/buildtools.git@b138e6ce86ae843c42a1a08f37903207bebcca75",
  "third_party/externals/angle2"          : "https://chromium.googlesource.com/angle/angle.git@fe42ac4df49fcd8b1e371276972b201f78a9a00d",
  "third_party/externals/brotli"          : "https://skia.googlesource.com/external/github.com/google/brotli.git@e61745a6b7add50d380cfd7d3883dd6c62fc2c71",
  "third_party/externals/d3d12allocator"  : "https://skia.googlesource.com/external/github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator.git@169895d529dfce00390a20e69c2f516066fe7a3b",
  # Dawn requires jinja2 and markupsafe for the code generator, tint for SPIRV compilation, and abseil for string formatting.
  # When the Dawn revision is updated these should be updated from the Dawn DEPS as well.
  "third_party/externals/dawn"            : "https://dawn.googlesource.com/dawn.git@b6c1a51775ae80695eb6c474691d02366d4eb7ad",
  "third_party/externals/jinja2"          : "https://chromium.googlesource.com/chromium/src/third_party/jinja2@ee69aa00ee8536f61db6a451f3858745cf587de6",
  "third_party/externals/markupsafe"      : "https://chromium.googlesource.com/chromium/src/third_party/markupsafe@0944e71f4b2cb9a871bcbe353f95e889b64a611a",
  "third_party/externals/tint"            : "https://dawn.googlesource.com/tint@f71784fc2b2a4b0ae020a1bfdcc6da95ead7ad79",
  "third_party/externals/abseil-cpp"      : "https://chromium.googlesource.com/chromium/src/third_party/abseil-cpp@789af048b388657987c59d4da406859034fe310f",
  "third_party/externals/dng_sdk"         : "https://android.googlesource.com/platform/external/dng_sdk.git@c8d0c9b1d16bfda56f15165d39e0ffa360a11123",
  "third_party/externals/egl-registry"    : "https://skia.googlesource.com/external/github.com/KhronosGroup/EGL-Registry@a0bca08de07c7d7651047bedc0b653cfaaa4f2ae",
  "third_party/externals/expat"           : "https://chromium.googlesource.com/external/github.com/libexpat/libexpat.git@a28238bdeebc087071777001245df1876a11f5ee",
  "third_party/externals/freetype"        : "https://chromium.googlesource.com/chromium/src/third_party/freetype2.git@cff026d41599945498044d2f4dcc0e610ffb6929",
  "third_party/externals/harfbuzz"        : "https://chromium.googlesource.com/external/github.com/harfbuzz/harfbuzz.git@3160789701463bcd822c26010da4de33c92c85d9",
  "third_party/externals/icu"             : "https://chromium.googlesource.com/chromium/deps/icu.git@a0718d4f121727e30b8d52c7a189ebf5ab52421f",
  "third_party/externals/imgui"           : "https://skia.googlesource.com/external/github.com/ocornut/imgui.git@9418dcb69355558f70de260483424412c5ca2fce",
  "third_party/externals/libgifcodec"     : "https://skia.googlesource.com/libgifcodec@fd59fa92a0c86788dcdd84d091e1ce81eda06a77",
  "third_party/externals/libjpeg-turbo"   : "https://chromium.googlesource.com/chromium/deps/libjpeg_turbo.git@24e310554f07c0fdb8ee52e3e708e4f3e9eb6e20",
  "third_party/externals/libpng"          : "https://skia.googlesource.com/third_party/libpng.git@386707c6d19b974ca2e3db7f5c61873813c6fe44",
  "third_party/externals/libwebp"         : "https://chromium.googlesource.com/webm/libwebp.git@9ce5843dbabcfd3f7c39ec7ceba9cbeb213cbfdf",
  "third_party/externals/microhttpd"      : "https://android.googlesource.com/platform/external/libmicrohttpd@748945ec6f1c67b7efc934ab0808e1d32f2fb98d",
  "third_party/externals/oboe"            : "https://chromium.googlesource.com/external/github.com/google/oboe.git@b02a12d1dd821118763debec6b83d00a8a0ee419",
  "third_party/externals/opengl-registry" : "https://skia.googlesource.com/external/github.com/KhronosGroup/OpenGL-Registry@14b80ebeab022b2c78f84a573f01028c96075553",
  "third_party/externals/piex"            : "https://android.googlesource.com/platform/external/piex.git@bb217acdca1cc0c16b704669dd6f91a1b509c406",
  "third_party/externals/sfntly"          : "https://chromium.googlesource.com/external/github.com/googlei18n/sfntly.git@b55ff303ea2f9e26702b514cf6a3196a2e3e2974",
  "third_party/externals/spirv-cross"     : "https://chromium.googlesource.com/external/github.com/KhronosGroup/SPIRV-Cross@0e2880ab990e79ce6cc8c79c219feda42d98b1e8",
  "third_party/externals/spirv-headers"   : "https://skia.googlesource.com/external/github.com/KhronosGroup/SPIRV-Headers.git@814e728b30ddd0f4509233099a3ad96fd4318c07",
  "third_party/externals/spirv-tools"     : "https://skia.googlesource.com/external/github.com/KhronosGroup/SPIRV-Tools.git@4b092d2ab81854e61632bdd1e658907f0071c37e",
  "third_party/externals/swiftshader"     : "https://swiftshader.googlesource.com/SwiftShader@a465cca8ec8dffa0a308731e3aec9a26c9215f6c",
  #"third_party/externals/v8"              : "https://chromium.googlesource.com/v8/v8.git@5f1ae66d5634e43563b2d25ea652dfb94c31a3b4",
  "third_party/externals/wuffs"           : "https://skia.googlesource.com/external/github.com/google/wuffs-mirror-release-c.git@600cd96cf47788ee3a74b40a6028b035c9fd6a61",
  "third_party/externals/zlib"            : "https://chromium.googlesource.com/chromium/src/third_party/zlib@c876c8f87101c5a75f6014b0f832499afeb65b73",
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
      "libraries/libuv": vars['libuv_git'] + '@' + vars['libuv_commit'],
    },
  },
  {
    "name": "libraries/v8",
    "url": vars['v8_git'] + '@' + vars['v8_commit'],
    "managed": True,
    "custom_deps": {
      "third_party/icu": None,
      "third_party\\icu": None,
      "third_party/android_ndk": None,
      "third_party\\android_ndk": None,
      "third_party/android_platform": None,
      "third_party\\android_platform": None,
      "third_party/android_sdk/public": None,
      "third_party\\android_sdk\\public": None,
      "third_party/fuchsia-sdk": None,
      "third_party\\fuchsia-sdk": None,
      "third_party/google_benchmark/src": None,
      "third_party\\google_benchmark\\src": None,
      "tools/swarming_client": None,
      "tools\\swarming_client": None,
      "test/benchmarks/data": None,
      "test\\benchmarks\\data": None,
      "test/mozilla/data": None,
      "test\\mozilla\\data": None,
      "test/test262/data": None,
      "test\\test262\\data": None,
      "test/test262/harness": None,
      "test\\test262\\harness": None,
      "third_party/qemu-linux-x64": None,
      "third_party\\qemu-linux-x64": None,
      "third_party/qemu-mac-x64": None,
      "third_party\\qemu-mac-x64": None,
      "third_party/aemu-linux-x64": None,
      "third_party\\aemu-linux-x64": None,
      "third_party/aemu-mac-x64": None,
      "third_party\\aemu-mac-x64": None,
      "tools/clang/dsymutil": None,
      "tools\\clang\\dsymutil": None,
    },
  },
]

for (k, v) in skia_deps.items():
  solutions[0]['custom_deps']['libraries/skia/' + k] = v

hooks = [
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
