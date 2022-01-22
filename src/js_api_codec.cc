#include "js_api_codec.h"

#include <skia/include/utils/SkBase64.h>

namespace {

void Base64ToArrayBuffer(const v8::FunctionCallbackInfo<v8::Value>& args) {
  JsApi* api = JsApi::Get(args.GetIsolate());

  if (args.Length() < 1 || !args[0]->IsString()) {
    api->js()->ThrowInvalidArgument();
    return;
  }

  std::string source = api->js()->ToString(args[0]);
  size_t decoded_size = 0;
  if (SkBase64::Decode(source.data(), source.size(), nullptr, &decoded_size) !=
      SkBase64::kNoError) {
    api->js()->ThrowError("Invalid base64 data");
    return;
  }

  v8::Local<v8::ArrayBuffer> buffer =
      v8::ArrayBuffer::New(api->js()->isolate(), decoded_size);
  ASSERT(SkBase64::Decode(source.data(), source.size(),
                          buffer->GetBackingStore()->Data(),
                          &decoded_size) == SkBase64::kNoError);

  args.GetReturnValue().Set(buffer);
}

std::string ToBase64(const void* src, size_t size) {
  size_t encoded_size = SkBase64::Encode(src, size, nullptr);
  std::string encoded;
  encoded.resize(encoded_size);
  SkBase64::Encode(src, size, encoded.data());
  return encoded;
}

void ToBase64(const v8::FunctionCallbackInfo<v8::Value>& args) {
  JsApi* api = JsApi::Get(args.GetIsolate());

  if (args.Length() < 1) {
    api->js()->ThrowInvalidArgument();
    return;
  }

  std::string encoded;

  if (args[0]->IsString()) {
    std::string s = api->js()->ToString(args[0]);
    encoded = ToBase64(s.data(), s.size());
  } else if (args[0]->IsArrayBuffer()) {
    std::shared_ptr<v8::BackingStore> store =
        args[0].As<v8::ArrayBuffer>()->GetBackingStore();
    encoded = ToBase64(store->Data(), store->ByteLength());
  } else if (args[0]->IsUint8Array() || args[0]->IsUint8ClampedArray()) {
    v8::Local<v8::TypedArray> array = args[0].As<v8::TypedArray>();
    std::shared_ptr<v8::BackingStore> store =
        array->Buffer()->GetBackingStore();
    encoded =
        ToBase64(static_cast<const char*>(store->Data()) + array->ByteOffset(),
                 array->ByteLength());
  } else {
    api->js()->ThrowInvalidArgument();
    return;
  }

  args.GetReturnValue().Set(api->js()->MakeString(encoded));
}

}  // namespace

v8::Local<v8::Object> MakeCodecApi(JsApi* api, const JsScope& scope) {
  v8::Local<v8::Object> codec = v8::Object::New(scope.isolate);

  scope.Set(codec, StringId::base64ToArrayBuffer, Base64ToArrayBuffer);
  scope.Set(codec, StringId::toBase64, ToBase64);

  return codec;
}
