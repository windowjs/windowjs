// Tests for the Process API: https://windowjs.org/doc/process

import {assert, assertEquals} from './lib/lib.js';

function waitUntilChildExit(child) {
  return new Promise(function(resolve) {
    child.addEventListener('exit', resolve);
  });
}

function spawnChild(args, options) {
  options = options || {
    log : true,
  };
  return Process.spawn(__dirname + '/data/child.js', args, options);
}

export async function cpus() {
  assert(Process.cpus >= 1);
}

export async function childProcessExitCode() {
  const child = spawnChild([ 'exit-123' ]);
  const exit = await waitUntilChildExit(child);
  assertEquals(exit.status, 123);
}

export async function childProcessMessaging() {
  const child = spawnChild([ 'receive-ping' ])
  const payload = {
    string : 'hello world',
    number : 123.456,
    'true' : true,
    'false' : false,
    'null' : null,
    array : [ 'hello world', 123.456, true, false, null ],
    object : {},
  };
  const replyPromise = new Promise((resolve) => {
    child.addEventListener('message', resolve);
  });
  child.postMessage({'type' : 'ping', 'payload' : payload});
  const reply = await replyPromise;
  assertEquals(reply.type, 'pong');
  assert(reply.payload);
  assertEquals(reply.payload['string'], 'hello world');
  assertEquals(reply.payload['number'], 123.456);
  assertEquals(reply.payload['true'], true);
  assertEquals(reply.payload['false'], false);
  assertEquals(reply.payload['null'], null);
  assertEquals(reply.payload['array'].length, 5);
  assertEquals(reply.payload['array'][0], 'hello world');
  assertEquals(reply.payload['array'][1], 123.456);
  assertEquals(reply.payload['array'][2], true);
  assertEquals(reply.payload['array'][3], false);
  assertEquals(reply.payload['array'][4], null);
  assert(reply.payload['object'] instanceof Object);
  const exit = await waitUntilChildExit(child);
  assertEquals(exit.status, 0);
}

export async function childLogs() {
  const child = spawnChild([ 'receive-command' ], {log : false});
  const expected = ['log', 'debug', 'info', 'warn', 'error'];
  return new Promise(function(resolve, reject) {
    child.addEventListener('log', function(event) {
      const next = expected.shift();
      if (event.level != next) {
        reject(`Unexpected log level: ${event.level} != ${next}`);
        return;
      }
      if (event.message != next + ' log') {
        reject(`Unexpected log message: "${event.message}" != "${next} log"`);
        return;
      }
      if (expected.length == 0) {
        resolve();
      }
    });
    child.postMessage({'command' : 'logs'})
  });
}

export async function childException() {
  return new Promise(function(resolve, reject) {
    const child = spawnChild([ 'throw-exception' ]);
    child.addEventListener('exception', function(event) {
      const expected = 'Uncaught Error: oh no';
      if (event.message != expected) {
        reject(`Unexpected exception message: "${event.message}" != "${
            expected}"`);
      } else {
        child.close();
        resolve();
      }
    });
  });
}
