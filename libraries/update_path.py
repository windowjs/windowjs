#!/usr/bin/env python

import os
import sys

repo = sys.argv[1]
depot_tools = '%s/libraries/depot_tools' % repo
gn = '%s/libraries/gn/out' % repo
ninja = '%s/libraries/ninja' % repo

paths = os.environ['PATH'].split(os.pathsep)

PATH = os.pathsep.join(p for p in paths if not repo in p and not '.vpython-root' in p)

print '%s%s%s%s%s%s%s' % (ninja, os.pathsep, gn, os.pathsep, depot_tools, os.pathsep, PATH)
