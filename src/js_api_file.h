#ifndef WINDOWJS_JS_API_FILE_H
#define WINDOWJS_JS_API_FILE_H

#include <v8/include/v8.h>

#include "js_api.h"
#include "js_scope.h"

v8::Local<v8::Object> MakeFileApi(JsApi* api, const JsScope& scope);

#endif  // WINDOWJS_JS_API_FILE_H
