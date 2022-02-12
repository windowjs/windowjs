# This script is executed by 'gclient sync' or 'gclient runhooks' after
# pulling in dependencies.

import os

# This file could be patched in, but then subsequent syncs won't remove it
# and subsequent patching via hooks fails.
with open('libraries/angle/build/config/gclient_args.gni', 'wb') as f:
  f.write('generate_location_tags = false\n')

