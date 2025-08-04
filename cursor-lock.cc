#include "napi.h" // ✅ Correcto: comillas y aseguras include_path
#include <windows.h>

Napi::Value LockCursor(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() != 4 ||
      !info[0].IsNumber() || !info[1].IsNumber() ||
      !info[2].IsNumber() || !info[3].IsNumber()) {
    Napi::TypeError::New(env, "Expected four number arguments").ThrowAsJavaScriptException();
    return env.Null();
  }

  int x = info[0].As<Napi::Number>().Int32Value();
  int y = info[1].As<Napi::Number>().Int32Value();
  int width = info[2].As<Napi::Number>().Int32Value();
  int height = info[3].As<Napi::Number>().Int32Value();

  RECT rect = { x, y, x + width, y + height };
  ClipCursor(&rect);

  return env.Undefined();
}

Napi::Value UnlockCursor(const Napi::CallbackInfo& info) {
  ClipCursor(NULL);
  return info.Env().Undefined();
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set("lockCursor", Napi::Function::New(env, LockCursor));
  exports.Set("unlockCursor", Napi::Function::New(env, UnlockCursor));
  return exports;
}

NODE_API_MODULE(cursor_lock, Init)
