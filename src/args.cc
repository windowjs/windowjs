#include "args.h"

#include <iostream>

#include "config.h"
#include "fail.h"

static CommandLineArgs* args = nullptr;

void InitArgs(int argc, char* argv[]) {
  ASSERT(!args);
  args = new CommandLineArgs;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--no-log") == 0) {
      args->log = false;
      continue;
    }
    if (strcmp(argv[i], "--profile-startup") == 0) {
      args->profile_startup = true;
      continue;
    }
    if (strcmp(argv[i], "--disable-dev-keys") == 0) {
      args->disable_dev_keys = true;
      continue;
    }
    if (strcmp(argv[i], "--enable-crash-keys") == 0) {
      args->enable_crash_keys = true;
      continue;
    }
    if (strcmp(argv[i], "--child") == 0) {
      args->is_child_process = true;
      continue;
    }
    if (strcmp(argv[i], "--headless") == 0) {
      args->headless = true;
      continue;
    }
    if (strcmp(argv[i], "--version") == 0) {
      args->version = true;
      continue;
    }
    if (strcmp(argv[i], "--") == 0) {
      for (i++; i < argc; i++) {
        args->args.emplace_back(argv[i]);
      }
      break;
    }

    if (argv[i][0] == '-' && strcmp(argv[i], "--console") != 0) {
      ErrorQuit("Unknown flag: %s\n", argv[i]);
    }

    if (!args->is_child_process && windowjs_config_pass_args_to_loader) {
      args->args.emplace_back(argv[i]);
    } else {
      if (!args->initial_module.empty()) {
        ErrorQuit("Only one initial module is supported.\n");
      }

      args->initial_module = argv[i];
    }
  }

  if (args->initial_module.empty()) {
    args->initial_module = "--default";
  }
}

void ShutdownArgs() {
  ASSERT(args);
  delete args;
  args = nullptr;
}

const CommandLineArgs& Args() {
  ASSERT(args);
  return *args;
}
