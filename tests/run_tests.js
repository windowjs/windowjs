// This is the main test runner:
//
// $ out/src/windowjs.exe tests/run_tests.js
//
// The other sources under tests/ get executed in a subprocess to run their
// tests, and send their tests results to the parent process (this file).
//
// This file is also the entry point for child processes, when invoked with
// a path to a test script:
//
// $ out/src/windowjs.exe tests/run_tests.js -- tests/hello.js

import * as lib from './lib/lib.js';

function post(message) {
  if (Process.parent) {
    Process.parent.postMessage(message);
  }
}

function fixModulePath(path) {
  // Fix paths on Windows.
  if (File.sep == '\\') {
    path = path.replaceAll('\\', '/');
  }
  // Make the module path relative to this script.
  return '../' + path;
}

function exceptionToString(e) {
  if (!e) {
    return '(empty)';
  }
  if (!e.stack) {
    return e.toString();
  }
  return e.stack;
}

async function resetForNewTest(testName, tmpDirForTest) {
  lib.setTmpDirForTests(tmpDirForTest + '/' + testName);

  window.width = 400;
  window.height = 200;
  window.resizable = false;

  const canvas = window.canvas;
  canvas.resetTransform();
  canvas.antialias = true;
  canvas.fillStyle = '#000';
  canvas.strokeStyle = '#000';
  canvas.font = '24px sans';
  canvas.globalAlpha = 1.0;
  canvas.globalCompositeOperation = 'source-over';
  canvas.imageSmoothingEnabled = false;
  canvas.lineCap = 'butt';
  canvas.lineDashOffset = 0;
  canvas.lineJoin = 'miter';
  canvas.lineWidth = 1.0;
  canvas.miterLimit = 10.0;
  canvas.shadowBlur = 0;
  canvas.textAlign = 'left';
  canvas.textBaseline = 'alphabetic';
  canvas.clearRect(0, 0, canvas.width, canvas.height);
  canvas.fillRect(0, 0, canvas.width, canvas.height);
  canvas.beginPath();
}

async function runTestsInModule(path, tmpDirForTest, testPattern) {
  const fixedPath = fixModulePath(path);
  const mod = await import(fixedPath);
  const re = testPattern ? new RegExp(testPattern) : null;
  for (const f in mod) {
    if (re && !re.test(f)) {
      continue;
    }
    await resetForNewTest(f, tmpDirForTest);
    try {
      await mod[f]();
      post({type : 'test-passed'});
    } catch (e) {
      post({type : 'test-failed', test : f, failure : exceptionToString(e)});
    }
  }
}

const progress = {
  running : 0,
  done: 0,
  total: 0,
  passed: 0,
  failed: 0,
};

const failures = [];

function logProgress() {
  console.log(`Tests running: ${progress.running} passed: ${
      progress.passed} failed: ${progress.failed} files done: ${
      progress.done} of ${progress.total}`);
}

function resolveAfterTimeout(millis) {
  return new Promise(function(resolve, reject) {
    setTimeout(resolve, millis);
  });
}

class ChildProcess {
  constructor(child, path) {
    this.child = child;
    this.path = path;
    child.addEventListener('exception', this.onException.bind(this));
    child.addEventListener('exit', this.onExit.bind(this));
    child.addEventListener('message', this.onMessage.bind(this));
    this.promise = new Promise((resolve) => { this.resolve = resolve; });
    this.resolved = false;
  }

  async waitUntilDone() {
    try {
      await Promise.race([ this.promise, resolveAfterTimeout(30 * 1000) ]);
    } finally {
      this.child.close();
      if (!this.resolved) {
        failures.push('Test file "' + this.path + '" timed out.');
        progress.failed++;
        logProgress();
      }
    }
  }

  onException(event) {
    const trace = event.stacktrace || [];
    failures.push('Test file "' + this.path +
                  '" threw an uncaught exception: ' + event.message + '\n' +
                  trace.join('\n'));
    progress.failed++;
    logProgress();
    if (!this.resolved) {
      this.resolved = true;
      this.resolve();
    }
  }

  onExit(event) {
    if (event.status != 0 || event.error) {
      failures.push('Test file "' + this.path +
                    '" exited with an error status: ' + event.error);
      progress.failed++;
      logProgress();
    }
    if (!this.resolved) {
      this.resolved = true;
      this.resolve();
    }
  }

  onMessage(event) {
    if (event.type == 'test-passed') {
      progress.passed++;
      logProgress();
    } else if (event.type == 'test-failed') {
      failures.push('Test "' + event.test + '" in "' + this.path +
                    '" failed:\n' + event.failure);
      progress.failed++;
      logProgress();
    }
  }
}

async function spawnTest(path, testPattern, index, runningMap, tmpDirForTest) {
  try {
    const importPath = './tests/' + path;
    const args = [importPath, tmpDirForTest];
    if (testPattern) {
      args.push(testPattern);
    }
    const options = {
      log : true,
    };
    const process = Process.spawn(__filename, args, options);
    const child = new ChildProcess(process, path);
    await child.waitUntilDone();
  } finally {
    runningMap.delete(index);
  }
}

async function isTestFile(path) {
  const filename = File.basename(path);
  if (filename.startsWith('.') || !filename.endsWith('.js')) {
    // Skip dot files and non-JS files.
    return false;
  }
  if (path == 'run_tests.js' || path == 'lib.js') {
    // Skip test runner and library.
    return false;
  }
  return true;
}

async function runAllTestsInternal(pathPattern, testPattern, tmpDirForTests) {
  const pathRe = pathPattern ? new RegExp(pathPattern) : null;
  const allFiles = [];

  for (const file of await File.list(__dirname)) {
    if (pathRe && !pathRe.test(file)) {
      continue;
    }
    if (await isTestFile(file)) {
      allFiles.push(file);
    }
  }

  progress.total = allFiles.length;

  const cpus = Math.max(Process.cpus, 2);
  const running = new Map();

  for (let i = 0; i < allFiles.length; i++) {
    const nextFile = allFiles[i];
    const tmpDirForTest = tmpDirForTests + '/' + i;
    const promise = spawnTest(nextFile, testPattern, i, running, tmpDirForTest);
    running.set(i, promise);
    progress.running++;

    if (running.size == cpus) {
      logProgress();
      // Wait for one to finish before scheduling the next one.
      // Each of these Promises removes itself from the "running" Map before
      // it resolves.
      await Promise.race(running.values());
      progress.done++;
      progress.running--;
    }
  }

  while (running.size > 0) {
    logProgress();
    await Promise.race(running.values());
    progress.done++;
    progress.running--;
  }
  logProgress();

  if (failures.length == 0) {
    console.log('\n\n\nAll tests passed.\n\n');
    return 0;
  }

  console.log('\n\n\n' + failures.length + ' test failures:\n\n\n');
  for (let i = 0; i < failures.length; i++) {
    console.log('[' + (i + 1) + '] ' + failures[i] + '\n\n');
  }

  return 1;
}

async function runAllTests(pathPattern, testPattern) {
  const tmp = File.tmp + '/windowjs-tests-' + Date.now();
  await File.mkdirs(tmp);

  try {
    return await runAllTestsInternal(pathPattern, testPattern, tmp);
  } finally {
    await File.removeTree(tmp);
  }
}

async function main() {
  window.visible = false;
  window.width = 400;
  window.height = 200;

  const start = performance.now();

  if (Process.parent == null) {
    const status = await runAllTests(Process.args[0], Process.args[1]);
    const elapsed = (performance.now() - start) / 1000.0;
    console.log('Finished in ' + elapsed.toFixed(1) + ' seconds.');
    Process.exit(status);
  } else {
    await runTestsInModule(Process.args[0], Process.args[1], Process.args[2]);
  }

  window.close();
}

main();
