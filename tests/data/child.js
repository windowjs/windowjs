function handleParentCommand(command) {
  switch (command) {

  case 'exit-123':
    Process.exit(123);
    break;

  case 'receive-ping':
    Process.parent.addEventListener('message', function(event) {
      Process.parent.postMessage({'type' : 'pong', 'payload' : event.payload});
      Process.exit(0);
    });
    break;

  case 'logs':
    console.log('log log');
    console.debug('debug log');
    console.info('info log');
    console.warn('warn log');
    console.error('error log');
    Process.exit(0);

  case 'receive-command':
    Process.parent.addEventListener('message', function(event) {
      handleParentCommand(event.command);
    });
    break;

  case 'throw-exception':
    throw new Error('oh no');
    break;

  default:
    Process.exit(1);
  }
}

window.visible = false;
handleParentCommand(Process.args[0]);
