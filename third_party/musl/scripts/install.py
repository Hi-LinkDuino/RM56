#!/usr/bin/env python

import os
import sys
import argparse
from shutil import copy

def musl_copy_file(src, dest):
    dest_dir = os.path.dirname(dest)
    if not os.path.exists(dest_dir):
        os.makedirs(dest_dir)
    copy(src, dest)

def main():
    parser = argparse.ArgumentParser(description=__doc__)

    parser.add_argument('--input',
                        required = True,
                        help = 'The header source path',
                        metavar = 'FILE')

    parser.add_argument('--output',
                        required = True,
                        help = 'The output directory',
                        metavar = 'FILE')

    args = parser.parse_args()

    musl_copy_file(args.input, args.output)

if __name__ == "__main__":
    sys.exit(main())
