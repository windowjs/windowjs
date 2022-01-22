#ifndef WINDOWJS_JS_API_CODEC_H
#define WINDOWJS_JS_API_CODEC_H

#include <v8/include/v8.h>

#include "js_api.h"
#include "js_scope.h"

v8::Local<v8::Object> MakeCodecApi(JsApi* api, const JsScope& scope);

#endif  // WINDOWJS_JS_API_CODEC_H
