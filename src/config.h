#ifndef WINDOWJS_CONFIG_H
#define WINDOWJS_CONFIG_H

// Whether arguments get interpreted by Window.js itself, or get passed to
// the default initial_module.
//
// In windowjs.exe, this is false and Window.js loads the initial_module.
// If the initial_module is missing then the welcome screen in src/default.js
// gets shown.
//
// In windowjs-p5.exe this is true, and src/p5.js is always loaded.
// Any arguments and/or the initial_module get passed to src/p5.js.
extern bool windowjs_config_pass_args_to_loader;

#endif  // WINDOWJS_CONFIG_H
