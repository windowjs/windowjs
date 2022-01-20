#include "js_api_file.h"

#include "console.h"
#include "fail.h"
#include "file.h"
#include "js_api_canvas.h"
#include "thread.h"

namespace {

void ReadText(const v8::FunctionCallbackInfo<v8::Value>& args) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(args.GetIsolate());
  if (args.Length() < 1 || !args[0]->IsString()) {
    api->js()->ThrowError("String argument is required.");
    return;
  }
  std::string path = api->js()->ToString(args[0]);

  args.GetReturnValue().Set(api->PostToBackgroundAndResolve(
      [p = std::move(path)]() -> JsApi::ResolveFunction {
        std::string content;
        std::string error;
        ReadFile(p, &content, &error);
        if (!error.empty()) {
          return JsApi::Reject(std::move(error));
        }
        return JsApi::Resolve(std::move(content));
      }));
}

void ReadJson(const v8::FunctionCallbackInfo<v8::Value>& args) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(args.GetIsolate());
  if (args.Length() < 1 || !args[0]->IsString()) {
    api->js()->ThrowError("String argument is required.");
    return;
  }
  std::string path = api->js()->ToString(args[0]);

  args.GetReturnValue().Set(api->PostToBackgroundAndResolve(
      [p = std::move(path)]() -> JsApi::ResolveFunction {
        std::string content;
        std::string error;
        ReadFile(p, &content, &error);
        if (!error.empty()) {
          return JsApi::Reject(std::move(error));
        }
        return [c = std::move(content)](JsApi* api, const JsScope& scope,
                                        v8::Promise::Resolver* resolver) {
          v8::Local<v8::String> s = api->js()->MakeString(std::move(c));
          // v8::JSON::Parse throws an exception on failures, which gets
          // propagated to the promise by PostToBackgroundAndResolve.
          v8::MaybeLocal<v8::Value> value = v8::JSON::Parse(scope.context, s);
          if (!value.IsEmpty()) {
            IGNORE_RESULT(
                resolver->Resolve(scope.context, value.ToLocalChecked()));
          };
        };
      }));
}

void ReadArrayBuffer(const v8::FunctionCallbackInfo<v8::Value>& args) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(args.GetIsolate());
  if (args.Length() < 1 || !args[0]->IsString()) {
    api->js()->ThrowError("String argument is required.");
    return;
  }
  std::string path = api->js()->ToString(args[0]);

  args.GetReturnValue().Set(api->PostToBackgroundAndResolve(
      [p = std::move(path)]() -> JsApi::ResolveFunction {
        std::string content;
        std::string error;
        ReadFile(p, &content, &error);
        if (!error.empty()) {
          return JsApi::Reject(std::move(error));
        }
        return [c = std::move(content)](JsApi* api, const JsScope& scope,
                                        v8::Promise::Resolver* resolver) {
          v8::Local<v8::ArrayBuffer> buffer =
              v8::ArrayBuffer::New(scope.isolate, c.size());
          std::memcpy(buffer->GetBackingStore()->Data(), c.data(), c.size());
          IGNORE_RESULT(resolver->Resolve(scope.context, buffer));
        };
      }));
}

void ReadImageData(const v8::FunctionCallbackInfo<v8::Value>& args) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(args.GetIsolate());
  if (args.Length() < 1 || !args[0]->IsString()) {
    api->js()->ThrowError("String argument is required.");
    return;
  }
  std::string path = api->js()->ToString(args[0]);

  args.GetReturnValue().Set(api->PostToBackgroundAndResolve(
      [p = std::move(path)]() -> JsApi::ResolveFunction {
        std::string error;
        sk_sp<SkData> data = ReadFile(p, &error);
        if (!error.empty()) {
          return JsApi::Reject(std::move(error));
        }
        ASSERT(data);
        sk_sp<SkImage> image = SkImage::MakeFromEncoded(data);
        if (!image) {
          return JsApi::Reject("Failed to decode image");
        }
        return [image](JsApi* api, const JsScope& scope,
                       v8::Promise::Resolver* resolver) {
          v8::Local<v8::Value> args[] = {
              v8::Number::New(api->isolate(), image->width()),
              v8::Number::New(api->isolate(), image->height()),
          };
          v8::Local<v8::Object> object =
              api->GetImageDataConstructor()
                  ->NewInstance(scope.context, 2, args)
                  .ToLocalChecked();
          ImageDataApi* image_data = api->GetImageDataApi(object);
          SkImageInfo image_info =
              SkImageInfo::Make(image->width(), image->height(),
                                kRGBA_8888_SkColorType, kUnpremul_SkAlphaType);
          image->readPixels(nullptr, image_info,
                            image_data->backing_store()->Data(),
                            image->width() * 4, 0, 0);
          IGNORE_RESULT(resolver->Resolve(scope.context, object));
        };
      }));
}

void ReadImageBitmap(const v8::FunctionCallbackInfo<v8::Value>& args) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(args.GetIsolate());
  if (args.Length() < 1 || !args[0]->IsString()) {
    api->js()->ThrowError("String argument is required.");
    return;
  }
  std::string path = api->js()->ToString(args[0]);

  args.GetReturnValue().Set(api->PostToBackgroundAndResolve(
      [p = std::move(path)]() -> JsApi::ResolveFunction {
        std::string error;
        sk_sp<SkData> data = ReadFile(p, &error);
        if (!error.empty()) {
          return JsApi::Reject(std::move(error));
        }
        ASSERT(data);
        sk_sp<SkImage> image = SkImage::MakeFromEncoded(data);
        if (!image) {
          return JsApi::Reject("Failed to decode image");
        }
        return [image](JsApi* api, const JsScope& scope,
                       v8::Promise::Resolver* resolver) {
          RenderCanvasSharedContext* context = api->canvas_shared_context();
          sk_sp<SkImage> texture = image->makeTextureImage(
              context->skia_context(), GrMipMapped::kNo, SkBudgeted::kNo);
          ASSERT(texture);
          ASSERT(texture->isTextureBacked());
          v8::Local<v8::Value> args[] = {
              v8::External::New(api->isolate(), texture.release()),
          };
          v8::Local<v8::Object> object =
              api->GetImageBitmapConstructor()
                  ->NewInstance(scope.context, 1, args)
                  .ToLocalChecked();
          IGNORE_RESULT(resolver->Resolve(scope.context, object));
        };
      }));
}

void Write(const v8::FunctionCallbackInfo<v8::Value>& args) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(args.GetIsolate());
  if (args.Length() < 1 || !args[0]->IsString()) {
    api->js()->ThrowError("String argument is required.");
    return;
  }
  std::string path = api->js()->ToString(args[0]);

  if (args.Length() < 2) {
    api->js()->ThrowError(
        "String, Array or ArrayBufferView argument is required.");
    return;
  }

  std::string content;

  if (args[1]->IsString()) {
    content = api->js()->ToString(args[1]);
  } else if (args[1]->IsArrayBuffer()) {
    std::shared_ptr<v8::BackingStore> store =
        args[1].As<v8::ArrayBuffer>()->GetBackingStore();
    content.assign((const char*) store->Data(), store->ByteLength());
  } else if (args[1]->IsArrayBufferView()) {
    v8::Local<v8::ArrayBufferView> view = args[1].As<v8::ArrayBufferView>();
    content.resize(view->ByteLength());
    view->CopyContents(content.data(), view->ByteLength());
  } else {
    api->js()->ThrowError(
        "String, Array or ArrayBufferView argument is required.");
    return;
  }

  args.GetReturnValue().Set(api->PostToBackgroundAndResolve(
      [p = std::move(path),
       c = std::move(content)]() -> JsApi::ResolveFunction {
        std::string error;
        WriteFile(p, c, &error);
        if (!error.empty()) {
          return JsApi::Reject(std::move(error));
        }
        return JsApi::Resolve();
      }));
}

void PathFunction(const v8::FunctionCallbackInfo<v8::Value>& args,
                  std::function<void(const std::string&, std::string*)> f) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(args.GetIsolate());
  if (args.Length() < 1 || !args[0]->IsString()) {
    api->js()->ThrowError("String argument is required.");
    return;
  }
  std::string path = api->js()->ToString(args[0]);

  args.GetReturnValue().Set(api->PostToBackgroundAndResolve(
      [p = std::move(path), f = std::move(f)]() -> JsApi::ResolveFunction {
        std::string error;
        f(p, &error);
        if (!error.empty()) {
          return JsApi::Reject(std::move(error));
        }
        return JsApi::Resolve();
      }));
}

void PathsFunction(
    const v8::FunctionCallbackInfo<v8::Value>& args,
    std::function<void(const std::string&, const std::string&, std::string*)>
        f) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(args.GetIsolate());
  if (args.Length() < 2 || !args[0]->IsString() || !args[1]->IsString()) {
    api->js()->ThrowError("Two String arguments are required.");
    return;
  }
  std::string from = api->js()->ToString(args[0]);
  std::string to = api->js()->ToString(args[1]);

  args.GetReturnValue().Set(api->PostToBackgroundAndResolve(
      [from = std::move(from), to = std::move(to),
       f = std::move(f)]() -> JsApi::ResolveFunction {
        std::string error;
        f(from, to, &error);
        if (!error.empty()) {
          return JsApi::Reject(std::move(error));
        }
        return JsApi::Resolve();
      }));
}

void BooleanPathFunction(
    const v8::FunctionCallbackInfo<v8::Value>& args,
    std::function<bool(const std::string&, std::string*)> f) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(args.GetIsolate());
  if (args.Length() < 1 || !args[0]->IsString()) {
    api->js()->ThrowError("String argument is required.");
    return;
  }
  std::string path = api->js()->ToString(args[0]);

  args.GetReturnValue().Set(api->PostToBackgroundAndResolve(
      [p = std::move(path), f = std::move(f)]() -> JsApi::ResolveFunction {
        std::string error;
        bool result = f(p, &error);
        if (!error.empty()) {
          return JsApi::Reject(std::move(error));
        }
        return JsApi::Resolve(result);
      }));
}

void ListFunction(const v8::FunctionCallbackInfo<v8::Value>& args,
                  std::function<std::vector<std::filesystem::path>(
                      const std::filesystem::path&, std::string*)>
                      f) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(args.GetIsolate());
  if (args.Length() < 1 || !args[0]->IsString()) {
    api->js()->ThrowError("String argument is required.");
    return;
  }
  std::string path = api->js()->ToString(args[0]);

  args.GetReturnValue().Set(api->PostToBackgroundAndResolve(
      [p = std::move(path), f = std::move(f)]() -> JsApi::ResolveFunction {
        std::string error;
        std::vector<std::filesystem::path> list = f(p, &error);
        if (!error.empty()) {
          return JsApi::Reject(std::move(error));
        }
        return [list = std::move(list)](JsApi* api, const JsScope& scope,
                                        v8::Promise::Resolver* resolver) {
          std::vector<v8::Local<v8::Value>> elements;
          elements.resize(list.size());
          for (unsigned i = 0; i < list.size(); i++) {
            elements[i] = api->js()->MakeString(list[i].u8string());
          }
          v8::Local<v8::Array> array =
              v8::Array::New(scope.isolate, &elements[0], elements.size());
          IGNORE_RESULT(resolver->Resolve(scope.context, array));
        };
      }));
}

void IsDir(const v8::FunctionCallbackInfo<v8::Value>& args) {
  BooleanPathFunction(args, ::IsDir);
}

void IsFile(const v8::FunctionCallbackInfo<v8::Value>& args) {
  BooleanPathFunction(args, ::IsFile);
}

void Size(const v8::FunctionCallbackInfo<v8::Value>& args) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(args.GetIsolate());
  if (args.Length() < 1 || !args[0]->IsString()) {
    api->js()->ThrowError("String argument is required.");
    return;
  }
  std::string path = api->js()->ToString(args[0]);

  args.GetReturnValue().Set(api->PostToBackgroundAndResolve(
      [p = std::move(path)]() -> JsApi::ResolveFunction {
        std::string error;
        size_t size = ::GetFileSize(p, &error);
        if (!error.empty()) {
          return JsApi::Reject(std::move(error));
        }
        return JsApi::Resolve((double) size);
      }));
}

void List(const v8::FunctionCallbackInfo<v8::Value>& args) {
  ListFunction(args, ::ListDir);
}

void ListTree(const v8::FunctionCallbackInfo<v8::Value>& args) {
  ListFunction(args, ::ListTree);
}

void Copy(const v8::FunctionCallbackInfo<v8::Value>& args) {
  PathsFunction(args, ::Copy);
}

void CopyTree(const v8::FunctionCallbackInfo<v8::Value>& args) {
  PathsFunction(args, ::CopyTree);
}

void Remove(const v8::FunctionCallbackInfo<v8::Value>& args) {
  PathFunction(args, ::Remove);
}

void RemoveTree(const v8::FunctionCallbackInfo<v8::Value>& args) {
  PathFunction(args, ::RemoveTree);
}

void Rename(const v8::FunctionCallbackInfo<v8::Value>& args) {
  PathsFunction(args, ::Rename);
}

void GetCwd(v8::Local<v8::Name> property,
            const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->js()->MakeString(::GetCwd().u8string()));
}

void MkDirs(const v8::FunctionCallbackInfo<v8::Value>& args) {
  PathFunction(args, ::MkDirs);
}

void Basename(const v8::FunctionCallbackInfo<v8::Value>& args) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(args.GetIsolate());
  if (args.Length() < 1 || !args[0]->IsString()) {
    api->js()->ThrowError("String argument is required.");
    return;
  }
  std::string path = api->js()->ToString(args[0]);
  args.GetReturnValue().Set(api->js()->MakeString(::Basename(path).u8string()));
}

void Dirname(const v8::FunctionCallbackInfo<v8::Value>& args) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(args.GetIsolate());
  if (args.Length() < 1 || !args[0]->IsString()) {
    api->js()->ThrowError("String argument is required.");
    return;
  }
  std::string path = api->js()->ToString(args[0]);
  args.GetReturnValue().Set(api->js()->MakeString(::Dirname(path).u8string()));
}

void GetHome(v8::Local<v8::Name> property,
             const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  std::string error;
  std::string home = GetUserHomePath(&error);
  JsApi* api = JsApi::Get(info.GetIsolate());
  if (error.empty()) {
    info.GetReturnValue().Set(api->js()->MakeString(home));
  } else {
    api->js()->ThrowError(error);
  }
}

void GetTmp(v8::Local<v8::Name> property,
            const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  std::string error;
  std::string tmp = GetTmpDir(&error);
  JsApi* api = JsApi::Get(info.GetIsolate());
  if (error.empty()) {
    info.GetReturnValue().Set(api->js()->MakeString(tmp));
  } else {
    api->js()->ThrowError(error);
  }
}

void GetSep(v8::Local<v8::Name> property,
            const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  char sep[2] = {std::filesystem::path::preferred_separator, '\0'};
  info.GetReturnValue().Set(api->js()->MakeString(sep));
}

}  // namespace

v8::Local<v8::Object> MakeFileApi(JsApi* api, const JsScope& scope) {
  v8::Local<v8::Object> file = v8::Object::New(scope.isolate);

  scope.Set(file, StringId::readText, ReadText);
  scope.Set(file, StringId::readJSON, ReadJson);
  scope.Set(file, StringId::readArrayBuffer, ReadArrayBuffer);
  scope.Set(file, StringId::readImageBitmap, ReadImageBitmap);
  scope.Set(file, StringId::readImageData, ReadImageData);
  scope.Set(file, StringId::write, Write);

  scope.Set(file, StringId::isDir, IsDir);
  scope.Set(file, StringId::isFile, IsFile);
  scope.Set(file, StringId::size, Size);

  scope.Set(file, StringId::list, List);
  scope.Set(file, StringId::listTree, ListTree);
  scope.Set(file, StringId::copy, Copy);
  scope.Set(file, StringId::copyTree, CopyTree);
  scope.Set(file, StringId::remove, Remove);
  scope.Set(file, StringId::removeTree, RemoveTree);
  scope.Set(file, StringId::rename, Rename);

  scope.Set(file, StringId::mkdirs, MkDirs);

  scope.Set(file, StringId::cwd, GetCwd);
  scope.Set(file, StringId::home, GetHome);
  scope.Set(file, StringId::sep, GetSep);
  scope.Set(file, StringId::tmp, GetTmp);

  scope.Set(file, StringId::basename, Basename);
  scope.Set(file, StringId::dirname, Dirname);

  return file;
}
