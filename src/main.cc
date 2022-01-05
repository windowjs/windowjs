#include "main.h"

#include <filesystem>
#include <iostream>
#include <memory>

#include "args.h"
#include "fail.h"
#include "file.h"
#include "js_api_process.h"
#include "json.h"
#include "thread.h"
#include "version.h"

int main(int argc, char* argv[]) {
  InitFail();
  InitArgs(argc, argv);
  InitLog();
  InitMainThread();
  uv_setup_args(argc, argv);
  uv_disable_stdio_inheritance();

  if (Args().version) {
    std::cout << GetVersionString() << std::endl;
    exit(0);
  }

  if (Args().profile_startup) {
    $(DEV) << "[profile-startup] main() enter: " << glfwGetTime();
  }

  if (Args().initial_module.empty()) {
    std::cerr << "Must pass an initial module.\n";
    return 1;
  }

  Js::Init(argv[0]);
  Window::Init();

  std::unique_ptr<Main> main = std::make_unique<Main>();
  main->RunUntilClosed();
  main.reset();

  uv_library_shutdown();
  Window::Shutdown();
  Js::Shutdown();
  ShutdownArgs();
  ShutdownLog();

  std::cerr << "All done, clean exit.\n";

  return 0;
}

Main::Main()
    : background_queue_(4),
      window_(this, 800, 600),
      gc_quit_(false),
      main_module_loaded_(false),
      reload_requested_(false),
      first_load_(true) {
  ASSERT(IsMainThread());
  SetLogHandler(this);
  window_.SetDelegate(this);
  window_.SetTitle(Args().initial_module);
  Reload();
}

Main::~Main() {
  ASSERT(IsMainThread());
  SetLogHandler(nullptr);
  events_.RemoveAll();
  gc_quit_ = true;
  gc_signal_.SetAndNotify();
  gc_thread_.join();
}

void Main::Reload() {
  ASSERT(IsMainThread());

  window_.OnLoadingStart();

  // Shutdown objects that will be recreated.
  if (!first_load_) {
    glfwSetTime(0.0);
    window_.stats()->Reset();
    messages_to_console_.clear();
    gc_quit_ = true;
    gc_signal_.SetAndNotify();
    gc_thread_.join();
    events_.RemoveAll();
    api_.reset();
    js_.reset();
    window_.SetWindowCanvas(nullptr);
    window_.console_overlay()->Clear();
    window_.console_overlay()->SetEnabled(false);
    window_.console_overlay()->SetEnableOnErrors(true);
    pending_events_.clear();
    background_queue_.ResetDropAllTasks();
    task_queue_.ResetDropAllTasks();
  }

  // Recreate those objects now.
  js_ =
      std::make_unique<Js>(this, std::filesystem::current_path(), &task_queue_);
  js_->isolate()->IsolateInForegroundNotification();
  js_->isolate()->DisableMemorySavingsMode();
  js_->isolate()->MemoryPressureNotification(v8::MemoryPressureLevel::kNone);
  js_->isolate()->SetIdle(false);

  api_ = std::make_unique<JsApi>(&window_, js_.get(), &events_, &task_queue_,
                                 &background_queue_);

  window_.stats()->SetJs(js_.get(), api_.get());

  gc_quit_ = false;
  gc_thread_ = std::thread([this] {
    GcThread();
  });

  // Load the initial module again.
  if (first_load_ && Args().profile_startup) {
    $(DEV) << "[profile-startup] load initial module start: " << glfwGetTime();
  }

  main_module_loaded_ = false;
  js_->LoadMainModule(Args().initial_module);

  if (first_load_ && Args().profile_startup) {
    $(DEV) << "[profile-startup] load initial module end: " << glfwGetTime();
  }
}

void Main::OnMainModuleLoaded() {
  main_module_loaded_ = true;
  window_.OnLoadingFinished();
  glfwPostEmptyEvent();
}

void Main::RunUntilClosed() {
  while (!glfwWindowShouldClose(window_.window())) {
    // === Loop part 1 ===
    //
    // This part of the loop runs inside Javascript, so this blocks waiting
    // for any long-running code.
    //
    // The first part of the loop dispatches any input events to Javascript
    // listeners, and runs any pending tasks.
    {
      v8::Locker locker(js_->isolate());
      window_.stats()->OnGcFinished();

      JsScope scope(js_.get());
      v8::TryCatch try_catch(scope.isolate);

      // Dispatch events.
      std::vector<PendingEvent> events;
      events.swap(pending_events_);
      for (const PendingEvent& event : events) {
        if (events_.HasListeners(event.type)) {
          events_.Dispatch(event.type, event.f(scope), scope);
          if (try_catch.HasCaught()) {
            js_->ReportException(try_catch.Message());
            try_catch.Reset();
          }
        }
      }
      ASSERT(!try_catch.HasCaught());

      // Each task is responsible for try/catching uncaught exceptions.
      task_queue_.RunTasks();
      ASSERT(!try_catch.HasCaught());
      window_.stats()->OnJsFinished();

      // CallAnimationFrameCallbacks handles exceptions internally too.
      api_->CallAnimationFrameCallbacks(scope);
      ASSERT(!try_catch.HasCaught());
      window_.stats()->OnRafFinished();

      if (first_load_ && Args().profile_startup) {
        $(DEV) << "[profile-startup] first requestAnimationFrame: "
               << glfwGetTime();
      }

      // Log any Promise failures that didn't have a handler.
      js_->HandleUncaughtExceptionsInPromises();
    }

    // Let the background thread do a GC pass now while we wait for vsync.
    gc_signal_.SetAndNotify();

    // === Loop part 2 ===
    //
    // This part of the loop does not run inside Javascript anymore.
    //
    // Any drawing from Javascript for the next frame is done by now. This
    // part of the loop blocks on swapping buffers and waiting for vsync,
    // while v8 does a GC pass.
    //
    // This part of the loop is skipped while the main module is still loading.
    // That can happen if it does a top-level await. Note that this also
    // prevents showing the main window until the module is finished.
    if (main_module_loaded_) {
      window_.RenderAndSwapBuffers();
      window_.stats()->OnSwapFinished();
    }

    if (reload_requested_) {
      reload_requested_ = false;
      Reload();
    }

    // === Loop part 3 ===
    //
    // Wait for the next event after painting.
    //
    // There are 3 ways to stop waiting and loop into the next frame:
    // 1. There are requestAnimationFrame listeners
    // 2. An input event is received by GLFW
    // 3. A scheduled task will run in T seconds.
    //
    // Case (1) just polls for events now and immediately goes into the next
    // frame. Case (2) waits "forever" until an event arrives. Case (3) waits
    // for T seconds or until an input event is received.
    double timeout = GetTimeoutUntilNextFrame();
    if (timeout < 0) {
      glfwWaitEvents();
    } else if (timeout == 0) {
      glfwPollEvents();
    } else {
      glfwWaitEventsTimeout(timeout);
    }

    window_.stats()->OnWaitFinished();

    first_load_ = false;
  }
}

double Main::GetTimeoutUntilNextFrame() const {
  double timeout = task_queue_.GetSecondsToNextTask();

  // Draw the next frame as soon as possible if there is a callback to
  // requestAnimationFrame.
  if (api_->has_animation_frame_callbacks() || window_.wants_frames()) {
    timeout = 0;
  }

  // If the window is minimized then don't render on every vsync until it's
  // restored again.
  if (timeout == 0 && glfwGetWindowAttrib(window_.window(), GLFW_ICONIFIED)) {
    timeout = -1;
  }

  return timeout;
}

void Main::GcThread() {
  for (;;) {
    gc_signal_.WaitAndClear();
    if (gc_quit_) {
      return;
    }
    v8::Locker locker(js_->isolate());
    js_->isolate()->IdleNotificationDeadline(Js::MonotonicallyIncreasingTime() +
                                             0.01);
  }
}

void Main::ShowConsole() {
  if (!console_) {
    std::string error;
    std::string exe = GetExePath(&error);
    ASSERT(error.empty());
    std::vector<std::string> args{Basename(exe).string(), "--child",
                                  "--console"};
    if (Args().child_log) {
      args.emplace_back("--log");
    }
    console_ = Pipe::Spawn(
        std::move(exe), std::move(args),
        [this](uint32_t type, std::string message) {
          // Called on the Pipe's background thread.
          task_queue_.Post([this, message = std::move(message)] {
            HandleMessageFromConsoleProcess(std::move(message));
          });
          glfwPostEmptyEvent();
        },
        [this](int64_t status, std::string error) {
          // Called on the Pipe's background thread.
          task_queue_.Post([this, error = std::move(error)] {
            HandleConsoleProcessExit(std::move(error));
          });
          glfwPostEmptyEvent();
        });
    std::stringstream json;
    json << "{\"type\":\"init\"";
    json << ",\"x\":" << window_.x();
    json << ",\"y\":" << window_.y();
    json << ",\"width\":" << window_.width();
    json << ",\"height\":" << window_.height();
    json << ",\"frameLeft\":" << window_.frame_left();
    json << ",\"frameRight\":" << window_.frame_right();
    json << ",\"frameTop\":" << window_.frame_top();
    json << ",\"frameBottom\":" << window_.frame_bottom();
    json << ",\"title\":" << Json::EscapeString(window_.title());
    json << "}";
    console_->SendMessage(0, json.str());
  }

  for (std::string& message : messages_to_console_) {
    console_->SendMessage(0, std::move(message));
  }
  messages_to_console_.clear();

  console_->SendMessage(0, "{\"type\":\"show\"}");
}

void Main::SaveScreenshot() {
  sk_sp<SkImage> image = window_.canvas()->MakeImageSnapshot();
  ASSERT(image);
  // Move the image from GPU to CPU memory, to do the encoding in a background
  // thread.
  image = image->makeNonTextureImage();
  ASSERT(image);
  background_queue_.Post([image]() {
    sk_sp<SkData> data = image->encodeToData(SkEncodedImageFormat::kPNG, 100);
    ASSERT(data);
    for (int i = 1; i < 1000; i++) {
      std::string name;
      if (i == 1) {
        name = "Screenshot.png";
      } else {
        name = "Screenshot-" + std::to_string(i) + ".png";
      }
      std::filesystem::path path = GetCwd() / name;
      std::string error;
      if (IsFile(path, &error) || !error.empty()) {
        continue;
      }
      if (WriteFile(path, data->data(), data->size(), &error)) {
        $(DEV) << "Saved screenshot to " << name << ".";
      } else {
        $(DEV) << "Screenshot couldn't be saved to " << name << ": " << error;
      }
      return;
    }
    $(DEV) << "Screenshot couldn't be saved: too many screenshots!";
  });
}

void Main::HandleMessageFromConsoleProcess(std::string message) {
  ASSERT(IsMainThread());
  std::string error;
  std::unique_ptr<Json> json = Json::Parse(message, &error);
  // Validated at the sender.
  ASSERT(json);
  ASSERT(json->IsDictionary());
  const Json& type = (*json)["type"];
  ASSERT(type.IsString());
  if (type.String() == "close") {
    OnClose();
  } else if (type.String() == "reload") {
    reload_requested_ = true;
  } else if (type.String() == "screenshot") {
    SaveScreenshot();
  } else if (type.String() == "focus") {
    window_.Focus();
  } else if (type.String() == "fps") {
    window_.stats()->SetEnabled(!window_.stats()->is_enabled());
  } else if (type.String() == "profile-frames") {
    window_.stats()->SetPrintFrameTimes(
        !window_.stats()->is_print_frame_times());
  } else if (type.String() == "overlay-console") {
    window_.console_overlay()->SetEnabled(
        !window_.console_overlay()->is_enabled());
  } else if (type.String() == "always-on-top") {
    window_.SetAlwaysOnTop(!window_.always_on_top());
  } else if (type.String() == "clear-logs") {
    OnClearLogs();
  } else if (type.String() == "eval") {
    const Json& source = (*json)["source"];
    ASSERT(source.IsString());
    std::unique_ptr<std::string> result = js_->ExecuteScript(source.String());
    if (result) {
      std::string json = "{\"type\":\"evalResponse\", \"result\":" +
                         Json::EscapeString(*result) + "}";
      console_->SendMessage(0, std::move(json));
    }
  }
}

void Main::HandleConsoleProcessExit(std::string error) {
  ASSERT(IsMainThread());
  if (!error.empty()) {
    std::cerr << "Console process quit unexpectedly: " << error << "\n";
  }
  console_.reset();
}

void Main::PostMessageToConsole(std::string json) {
  ASSERT(IsMainThread());
  if (console_) {
    console_->SendMessage(0, std::move(json));
  } else {
    while (messages_to_console_.size() > 1000) {
      messages_to_console_.pop_front();
    }
    messages_to_console_.emplace_back(std::move(json));
  }
}

void Main::OnLog(std::string message, ConsoleLogLevel level) {
  // Can be called on any thread. This is synchronized via SetLogHandler.
  std::string json =
      "{\"type\": \"log\", \"message\":" + Json::EscapeString(message) +
      ", \"level\":\"" + ConsoleLogLevelToString(level) + "\"}";
  task_queue_.Post(
      [this, json = std::move(json), message = std::move(message), level] {
        if (Args().is_child_process) {
          api_->parent_process()->SendMessage(ProcessApi::LOG, std::move(json));
        } else {
          PostMessageToConsole(std::move(json));
          window_.console_overlay()->OnLog(std::move(message), level);
        }
      });
}

void Main::OnClearLogs() {
  // Can be called on any thread. This is synchronized via SetLogHandler.
  if (!Args().is_child_process) {
    task_queue_.Post([this] {
      if (console_) {
        std::string json = "{\"type\": \"clearLogs\"}";
        PostMessageToConsole(std::move(json));
      } else {
        messages_to_console_.clear();
      }
      window_.console_overlay()->Clear();
      window_.console_overlay()->SetEnabled(false);
    });
  }
}

void Main::OnJavascriptException(std::string message,
                                 std::vector<std::string> stack_trace) {
  ASSERT(IsMainThread());
  std::string json =
      "{\"type\": \"exception\", \"message\":" + Json::EscapeString(message) +
      ", \"stacktrace\":[";
  std::string merged = message;
  bool first = true;
  for (const std::string& frame : stack_trace) {
    if (!first) {
      json.append(1, ',');
    }
    first = false;
    json.append(Json::EscapeString(frame));
    merged.append("\n  ").append(frame);
  }
  json.append("]}");
  if (Args().is_child_process) {
    api_->parent_process()->SendMessage(ProcessApi::EXCEPTION, std::move(json));
  } else {
    if (Args().log) {
      std::cerr << "Exception: " << message << "\n";
      for (const std::string& frame : stack_trace) {
        std::cerr << frame << "\n";
      }
    }
    PostMessageToConsole(std::move(json));
    window_.console_overlay()->OnLog(std::move(merged),
                                     ConsoleLogLevel::CONSOLE_ERROR);
  }
}

void Main::OnKey(int key, int scancode, int action, int mods) {
  if (!Args().disable_dev_keys) {
    if (!Args().is_child_process) {
      if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        OnClose();
      } else if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        ShowConsole();
      } else if (key == GLFW_KEY_F2 && action == GLFW_PRESS) {
        window_.stats()->SetEnabled(!window_.stats()->is_enabled());
      } else if (key == GLFW_KEY_F3 && action == GLFW_PRESS) {
        window_.stats()->SetPrintFrameTimes(
            !window_.stats()->is_print_frame_times());
      } else if (key == GLFW_KEY_F4 && action == GLFW_PRESS) {
        window_.console_overlay()->SetEnabled(
            !window_.console_overlay()->is_enabled());
      } else if (key == GLFW_KEY_F8 && action == GLFW_PRESS) {
        SaveScreenshot();
      }
    }
    if (key == GLFW_KEY_F5 && action == GLFW_PRESS) {
      if (!Args().is_child_process || (mods & GLFW_MOD_CONTROL) != 0) {
        reload_requested_ = true;
        return;
      }
    }
    if (key == GLFW_KEY_F6 && action == GLFW_PRESS) {
      if (!Args().is_child_process || (mods & GLFW_MOD_CONTROL) != 0) {
        window_.SetAlwaysOnTop(!window_.always_on_top());
        return;
      }
    }
    if (Args().enable_crash_keys) {
      if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
        *((volatile int*) 0) = 0;
      } else if (key == GLFW_KEY_F12 && action == GLFW_PRESS) {
        Fail("Crash key pressed");
      }
    }
  }
  auto f = [=](const JsScope& scope) {
    return MakeKeyEvent(key, scancode, action, mods, scope);
  };
  JsEventType type =
      action == GLFW_RELEASE ? JsEventType::KEYUP : JsEventType::KEYDOWN;
  pending_events_.push_back({type, f});
}

void Main::OnCharacter(unsigned int codepoint) {
  auto f = [=](const JsScope& scope) {
    return MakeKeyPressEvent(codepoint, scope);
  };
  pending_events_.push_back({JsEventType::KEYPRESS, f});
}

void Main::OnMouseMove(double x, double y) {
  auto f = [=](const JsScope& scope) {
    return MakeMouseMoveEvent(x, y, scope);
  };
  pending_events_.push_back({JsEventType::MOUSEMOVE, f});
}

void Main::OnMouseButton(int button, bool pressed, double x, double y) {
  JsEventType type = pressed ? JsEventType::MOUSEDOWN : JsEventType::MOUSEUP;
  auto f = [=](const JsScope& scope) {
    return MakeMouseButtonEvent(type, button, x, y, scope);
  };
  pending_events_.push_back({type, f});
  if (!pressed) {
    auto f = [=](const JsScope& scope) {
      return MakeMouseButtonEvent(JsEventType::CLICK, button, x, y, scope);
    };
    pending_events_.push_back({JsEventType::CLICK, f});
  }
}

void Main::OnMouseWheel(double x, double y) {
  auto f = [=](const JsScope& scope) {
    return MakeMouseWheelEvent(x, y, scope);
  };
  pending_events_.push_back({JsEventType::WHEEL, f});
}

void Main::OnMouseEnter(bool entered) {
  auto f = [=](const JsScope& scope) {
    return MakeEvent(entered ? StringId::mouseenter : StringId::mouseleave,
                     scope);
  };
  JsEventType type =
      entered ? JsEventType::MOUSEENTER : JsEventType::MOUSELEAVE;
  pending_events_.push_back({type, f});
}

void Main::OnFocus(bool focused) {
  auto f = [=](const JsScope& scope) {
    return MakeEvent(focused ? StringId::focus : StringId::blur, scope);
  };
  JsEventType type = focused ? JsEventType::FOCUS : JsEventType::BLUR;
  pending_events_.push_back({type, f});
}

void Main::OnResize(int width, int height) {
  // Windows doesn't pump the main message loop during resize gestures.
  // This means that glfwWaitEvents() won't return until the user finished the
  // resize gesture (i.e. releases the mouse button). The contents of the window
  // don't get refreshed while this happens.
  //
  // As a workaround, we enter Javascript from here and dispatch the 'resize'
  // event immediately, and swap the updated buffer to the screen.
  //
  // This makes resizes smoother (i.e. black borders aren't visible).
  bool has_resize_callbacks = events_.HasListeners(JsEventType::RESIZE);
  bool has_raf_callbacks =
      api_->has_animation_frame_callbacks() && main_module_loaded_;

  if (has_resize_callbacks || has_raf_callbacks) {
    v8::Locker locker(js_->isolate());
    JsScope scope(js_.get());
    v8::TryCatch try_catch(scope.isolate);

    // Dispatch any pending events now too. This makes sure that "minimize",
    // "maximize" and "restore" events are received before their corresponding
    // "resize" events.
    std::vector<PendingEvent> events;
    events.swap(pending_events_);
    for (const PendingEvent& event : events) {
      if (events_.HasListeners(event.type)) {
        events_.Dispatch(event.type, event.f(scope), scope);
        if (try_catch.HasCaught()) {
          js_->ReportException(try_catch.Message());
          try_catch.Reset();
        }
      }
    }

    if (has_resize_callbacks) {
      v8::Local<v8::Value> event = MakeEvent(StringId::resize, scope);
      events_.Dispatch(JsEventType::RESIZE, event, scope);
      if (try_catch.HasCaught()) {
        js_->ReportException(try_catch.Message());
        try_catch.Reset();
      }
    }

    if (has_raf_callbacks) {
      // This is only called after main_module_loaded_ has finished.
      api_->CallAnimationFrameCallbacks(scope);
      ASSERT(!try_catch.HasCaught());
    }
  }

  if (main_module_loaded_) {
    // main_module_loaded_ prevents us from rendering a frame from here
    // if the initial code does window.width = 1024, for example; we should
    // only show the first frame after the entire initial loading has finished.
    window_.RenderAndSwapBuffers();
  }
}

void Main::OnClose() {
  bool has_close_listeners = events_.HasListeners(JsEventType::CLOSE);
  bool should_close = true;

  if (has_close_listeners) {
    v8::Locker locker(js_->isolate());
    JsScope scope(js_.get());
    v8::TryCatch try_catch(scope.isolate);
    v8::Local<v8::Value> event = MakeEvent(StringId::close, scope);
    bool any_handled = events_.Dispatch(JsEventType::CLOSE, event, scope);
    if (try_catch.HasCaught()) {
      js_->ReportException(try_catch.Message());
      try_catch.Reset();
    } else {
      should_close = !any_handled;
    }
  }

  glfwSetWindowShouldClose(window_.window(),
                           should_close ? GLFW_TRUE : GLFW_FALSE);
  glfwPostEmptyEvent();
}

void Main::OnDrop(std::vector<std::string> paths) {
  auto f = [=, paths = std::move(paths)](const JsScope& scope) {
    return MakeDropEvent(std::move(paths), scope);
  };
  pending_events_.push_back({JsEventType::DROP, f});
}

void Main::OnMaximize() {
  auto f = [](const JsScope& scope) {
    return MakeEvent(StringId::maximize, scope);
  };
  pending_events_.push_back({JsEventType::MAXIMIZE, f});
}

void Main::OnMinimize() {
  auto f = [](const JsScope& scope) {
    return MakeEvent(StringId::minimize, scope);
  };
  pending_events_.push_back({JsEventType::MINIMIZE, f});
}

void Main::OnRestore() {
  auto f = [](const JsScope& scope) {
    return MakeEvent(StringId::restore, scope);
  };
  pending_events_.push_back({JsEventType::RESTORE, f});
}

void Main::OnTitleChanged() {
  if (console_) {
    std::string json = "{\"type\":\"titleChanged\", \"title\":" +
                       Json::EscapeString(window_.title()) + "}";
    console_->SendMessage(0, std::move(json));
  }
}
