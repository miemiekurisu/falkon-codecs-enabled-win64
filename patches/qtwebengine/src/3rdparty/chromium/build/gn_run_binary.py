# Copyright 2014 The Chromium Authors
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""Helper script for GN to run an arbitrary binary. See compiled_action.gni.

Run with:
  python gn_run_binary.py <binary_name> [args ...]
"""


import os
import shutil
import subprocess
import sys

# This script is designed to run binaries produced by the current build. We
# may prefix it with "./" to avoid picking up system versions that might
# also be on the path.
path = sys.argv[1]
if not os.path.isabs(path):
  path = './' + path


# [PROXYSWITCHER-PATCH]
# v8_context_snapshot_generator.exe deadlocks (0% CPU, before main) on this
# particular virtualized build host, so the snapshot action never finishes.
# The V8/Blink sources are untouched by this codec build (only Qt's
# proxy_config_service_qt.cpp is patched), so the snapshot produced by the
# earlier successful build of the same sources is still valid. Instead of
# running the hanging tool, copy the pre-built snapshot onto --output_file.
# Remove this block once the generator works again on the build host.
def _proxy_patched_snapshot():
  import re
  name = os.path.basename(sys.argv[1]).lower()
  if name.endswith('.exe'):
    name = name[:-4]
  if name != 'v8_context_snapshot_generator':
    return None
  try:
    idx = next(i for i, a in enumerate(sys.argv) if a.startswith('--output_file='))
    out = sys.argv[idx].split('=', 1)[1]
  except StopIteration:
    return None
  cwd = os.getcwd()
  is_debug = re.search(r'[\\/]Debug[\\/]', cwd) is not None
  base = r'C:\bd\_qtcodec\resources'
  src = os.path.join(base, 'v8_context_snapshot.debug.bin' if is_debug
                     else 'v8_context_snapshot.bin')
  if not os.path.isfile(src):
    # Pre-built snapshot unavailable: fall back to running the real tool.
    return None
  try:
    shutil.copyfile(src, out)
    os.utime(out, None)
  except OSError:
    return None
  sys.stderr.write('[ProxySwitcher] used pre-built %s for %s\n' % (src, out))
  return 0


_pre = _proxy_patched_snapshot()
if _pre is not None:
  sys.exit(_pre)
# [PROXYSWITCHER-PATCH] end


# The rest of the arguments are passed directly to the executable.
args = [path] + sys.argv[2:]

ret = subprocess.call(args)
if ret != 0:
  if ret <= -100:
    # Windows error codes such as 0xC0000005 and 0xC0000409 are much easier to
    # recognize and differentiate in hex. In order to print them as unsigned
    # hex we need to add 4 Gig to them.
    print('%s failed with exit code 0x%08X' % (sys.argv[1], ret + (1 << 32)))
  else:
    print('%s failed with exit code %d' % (sys.argv[1], ret))
sys.exit(ret)
