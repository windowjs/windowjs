#ifndef WINDOWJS_JS_SCOPE_H
#define WINDOWJS_JS_SCOPE_H

#include "js.h"

struct JsScope {
  explicit JsScope(Js* js)
      : js(js),
        isolate(js->isolate()),
        isolate_scope(isolate),
        handle_scope(isolate),
        context(js->context()),
        context_scope(context),
        strings(js->strings()) {}

  Js* js;
  v8::Isolate* isolate;
  v8::Isolate::Scope isolate_scope;
  v8::HandleScope handle_scope;
  v8::Local<v8::Context> context;
  v8::Context::Scope context_scope;
  JsStrings* strings;

  v8::Local<v8::String> GetConstantString(StringId id) const {
    return strings->GetConstantString(id, isolate);
  }

  v8::Local<v8::String> MakeString(std::string_view s) const {
    return js->MakeString(s);
  }

  void SetValue(v8::Local<v8::Object> object, const char* key,
                v8::Local<v8::Value> value) const {
    v8::Local<v8::String> k =
        v8::String::NewFromUtf8(isolate, key).ToLocalChecked();
    ASSERT(object->Set(context, k, value).FromJust());
  }

  void SetValue(v8::Local<v8::Object> object, StringId id,
                v8::Local<v8::Value> value) const {
    ASSERT(object->Set(context, GetConstantString(id), value).FromJust());
  }

  void Set(v8::Local<v8::Object> object, const char* key,
           v8::Local<v8::Value> value) const {
    SetValue(object, key, value);
  }

  void Set(v8::Local<v8::Object> object, StringId id,
           v8::Local<v8::Value> value) const {
    SetValue(object, id, value);
  }

  void Set(v8::Local<v8::Object> object, const char* key,
           v8::Local<v8::Object> value) const {
    SetValue(object, key, value);
  }

  void Set(v8::Local<v8::Object> object, StringId id,
           v8::Local<v8::Object> value) const {
    SetValue(object, id, value);
  }

  void Set(v8::Local<v8::Object> object, const char* key, bool value) const {
    SetValue(object, key, value ? v8::True(isolate) : v8::False(isolate));
  }

  void Set(v8::Local<v8::Object> object, StringId id, bool value) const {
    SetValue(object, id, value ? v8::True(isolate) : v8::False(isolate));
  }

  void Set(v8::Local<v8::Object> object, const char* key, int value) const {
    SetValue(object, key, v8::Number::New(isolate, value));
  }

  void Set(v8::Local<v8::Object> object, StringId id, int value) const {
    SetValue(object, id, v8::Number::New(isolate, value));
  }

  void Set(v8::Local<v8::Object> object, const char* key, double value) const {
    SetValue(object, key, v8::Number::New(isolate, value));
  }

  void Set(v8::Local<v8::Object> object, StringId id, double value) const {
    SetValue(object, id, v8::Number::New(isolate, value));
  }

  void Set(v8::Local<v8::Object> object, const char* key,
           StringId value) const {
    SetValue(object, key, GetConstantString(value));
  }

  void Set(v8::Local<v8::Object> object, StringId id, StringId value) const {
    SetValue(object, id, GetConstantString(value));
  }

  void Set(v8::Local<v8::Object> object, const char* key,
           v8::MaybeLocal<v8::Function> value) const {
    SetValue(object, key, value.ToLocalChecked());
  }

  void Set(v8::Local<v8::Object> object, StringId id,
           v8::MaybeLocal<v8::Function> value) const {
    SetValue(object, id, value.ToLocalChecked());
  }

  void Set(v8::Local<v8::Object> object, const char* key,
           v8::Local<v8::Function> value) const {
    SetValue(object, key, value);
  }

  void Set(v8::Local<v8::Object> object, StringId id,
           v8::Local<v8::Function> value) const {
    SetValue(object, id, value);
  }

  void Set(v8::Local<v8::Object> object, const char* key,
           v8::FunctionCallback function,
           v8::Local<v8::Value> data = {}) const {
    SetValue(object, key,
             v8::Function::New(context, function, data).ToLocalChecked());
  }

  void Set(v8::Local<v8::Object> object, StringId id,
           v8::FunctionCallback function,
           v8::Local<v8::Value> data = {}) const {
    SetValue(object, id,
             v8::Function::New(context, function, data).ToLocalChecked());
  }

  void Set(v8::Local<v8::Object> object, const char* key,
           v8::AccessorNameGetterCallback get,
           v8::Local<v8::External> data = {}) const {
    v8::Local<v8::String> k =
        v8::String::NewFromUtf8(isolate, key).ToLocalChecked();
    ASSERT(object->SetAccessor(context, k, get, nullptr, data).FromJust());
  }

  void Set(v8::Local<v8::Object> object, const char* key,
           v8::AccessorNameGetterCallback get,
           v8::AccessorNameSetterCallback set,
           v8::Local<v8::External> data = {}) const {
    v8::Local<v8::String> k =
        v8::String::NewFromUtf8(isolate, key).ToLocalChecked();
    ASSERT(object->SetAccessor(context, k, get, set, data).FromJust());
  }

  void Set(v8::Local<v8::Object> object, StringId id,
           v8::AccessorNameGetterCallback get,
           v8::Local<v8::External> data = {}) const {
    ASSERT(
        object->SetAccessor(context, GetConstantString(id), get, nullptr, data)
            .FromJust());
  }

  void Set(v8::Local<v8::Object> object, StringId id,
           v8::AccessorNameGetterCallback get,
           v8::AccessorNameSetterCallback set,
           v8::Local<v8::External> data = {}) const {
    ASSERT(object->SetAccessor(context, GetConstantString(id), get, set, data)
               .FromJust());
  }

  void Set(v8::Local<v8::ObjectTemplate> prototype, StringId id,
           v8::AccessorGetterCallback getter,
           v8::AccessorSetterCallback setter = nullptr) const {
    prototype->SetNativeDataProperty(GetConstantString(id), getter, setter);
  }

  void Set(v8::Local<v8::ObjectTemplate> prototype, StringId id,
           v8::FunctionCallback function) const {
    prototype->Set(GetConstantString(id),
                   v8::FunctionTemplate::New(isolate, function));
  }

  void Set(v8::Local<v8::FunctionTemplate> templ, StringId id,
           v8::AccessorGetterCallback getter,
           v8::AccessorSetterCallback setter = nullptr) const {
    templ->SetNativeDataProperty(GetConstantString(id), getter, setter);
  }

  void Set(v8::Local<v8::FunctionTemplate> templ, StringId id,
           v8::FunctionCallback function) const {
    templ->Set(GetConstantString(id),
               v8::FunctionTemplate::New(isolate, function));
  }
};

#endif  // WINDOWJS_JS_SCOPE_H
