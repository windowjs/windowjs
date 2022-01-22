#ifndef WINDOWJS_ARGS_H
#define WINDOWJS_ARGS_H

#include <string>
#include <vector>

#include "string.h"

struct CommandLineArgs {
  std::string initial_module;
  bool log = true;
  bool profile_startup = false;
  bool is_child_process = false;
  bool disable_dev_keys = false;
  bool enable_crash_keys = false;
  bool version = false;
  bool headless = false;
  std::vector<std::string> args;
};

void InitArgs(int argc, char* argv[]);
void ShutdownArgs();

const CommandLineArgs& Args();

#endif  // WINDOWJS_ARGS_H
