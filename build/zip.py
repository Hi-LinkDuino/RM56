#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
"""
Archives a set of files.
"""

import ast
import optparse
import sys
from scripts.util import build_utils


def main():
    parser = optparse.OptionParser()
    build_utils.add_depfile_option(parser)

    parser.add_option('--inputs', help='List of files to archive.')
    parser.add_option('--output', help='Path to output archive.')
    parser.add_option('--base-dir',
                      help='If provided, the paths in the archive will be '
                      'relative to this directory',
                      default='.')

    options, _ = parser.parse_args()

    inputs = ast.literal_eval(options.inputs)
    output = options.output
    base_dir = options.base_dir

    with build_utils.atomic_output(output) as f:
        build_utils.do_zip(inputs, f, base_dir)

    if options.depfile:
        build_utils.write_depfile(options.depfile, output)


if __name__ == '__main__':
    sys.exit(main())
