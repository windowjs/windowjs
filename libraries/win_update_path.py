#!/usr/bin/env python

import os
import sys

repo = sys.argv[1]
depot_tools = '%s/libraries/depot_tools' % repo
gn = '%s/libraries/gn/out' % repo

paths = os.environ['PATH'].split(';')

PATH = ';'.join(p for p in paths if not repo in p)

print '%s;%s;%s' % (gn, depot_tools, PATH)
