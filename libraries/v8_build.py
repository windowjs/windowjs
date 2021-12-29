# This script is executed by 'gclient sync' or 'gclient runhooks' after
# pulling in dependencies.

import os

# This file could be patched in, but then subsequent syncs won't remove it
# and subsequent patching via hooks fails.
with open('libraries/v8/build/config/gclient_args.gni', 'wb') as f:
  f.write('checkout_fuchsia_for_arm64_host = false\n')
