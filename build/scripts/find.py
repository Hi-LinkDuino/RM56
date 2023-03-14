#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
"""Finds files in directories.
"""

import fnmatch
import optparse
import os
import sys


def main(argv):
    parser = optparse.OptionParser()
    parser.add_option('--pattern', default='*', help='File pattern to match.')
    parser.add_option('--base-dir', help='base directory')
    parser.add_option('--return-relpath',
                      action='store_true',
                      help='return relative address from base directory')
    parser.add_option('--follow-symlinks',
                      action='store_true',
                      help='whether to follow links')
    options, directories = parser.parse_args(argv)

    for d in directories:
        if not os.path.exists(d):
            print('%s does not exist' % d)
            return 1
        if os.path.isfile(d):
            if options.return_relpath:
                if options.base_dir is not None:
                    if fnmatch.filter(d, options.pattern):
                        print(os.path.relpath(d, options.base_dir))
                else:
                    print("Please specify the relative base directory")
                    return 1
            else:
                if fnmatch.filter(d, options.pattern):
                    print(d)
            return 0
        elif not os.path.isdir(d):
            # if input path is not a directory nor a normal file, return error.
            print('%s is not a directory or a file' % d)
            return 1
        for root, _, files in os.walk(d, followlinks=options.follow_symlinks):
            for f in fnmatch.filter(files, options.pattern):
                if options.return_relpath:
                    if options.base_dir is not None:
                        print(
                            os.path.relpath(os.path.join(root, f),
                                            options.base_dir))
                    else:
                        print("Please specify the relative base directory")
                        return 1
                else:
                    print(os.path.join(root, f))


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
