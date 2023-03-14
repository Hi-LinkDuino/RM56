#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
"""Copies files to a directory."""

import filecmp
import itertools
import optparse
import os
import shutil
import sys

from util import build_utils


def copy_tree(src,
              dest,
              follow_all_symlinks=False,
              follow_outside_symlinks=False):
    """copy src/* to dest/
  I. If follow_outside_symlinks is true,
     1. If src item is a symlink, and points to some item inside src,
        then copy the symlink to dest.
     2. If src item points to items outside src, then follow links to copy
        the original file to dest.
     3. Else copy src item to dest.
  II. If follow_all_symlinks is true,
     1. If src item is a symlink, then follow links to copy the original file
        to dest.
     2. Else copy src item to dest.
  follow_outside_symlinks is true when follow_all_symlinks is true.
  """
    with os.scandir(src) as itr:
        items = list(itr)
    return _do_copy_tree(items,
                         src,
                         dest,
                         follow_all_symlinks=follow_all_symlinks,
                         follow_outside_symlinks=follow_outside_symlinks)


def _do_copy_tree(items,
                  src,
                  dest,
                  follow_all_symlinks=False,
                  follow_outside_symlinks=False):
    os.makedirs(dest, exist_ok=True)
    for item in items:
        srcname = os.path.join(src, item.name)
        destname = os.path.join(dest, item.name)
        is_symlink = item.is_symlink()
        if is_symlink:
            org_linkto = os.readlink(srcname)
            linkto = org_linkto
            if not os.path.isabs(org_linkto):
                linkto = os.path.join(os.path.dirname(item), org_linkto)

            if not os.path.exists(linkto):
                os.symlink(org_linkto, destname)
                shutil.copymode(srcname, destname, follow_symlinks=False)
                continue

            if follow_all_symlinks:
                if item.is_dir():
                    copy_tree(item,
                              destname,
                              follow_all_symlinks=follow_all_symlinks,
                              follow_outside_symlinks=follow_outside_symlinks)
                else:
                    shutil.copy(item, destname)
                    shutil.copymode(item, destname)

            elif follow_outside_symlinks:
                if os.path.abspath(src) in os.path.abspath(
                        linkto) and not os.path.isabs(org_linkto):
                    os.symlink(org_linkto, destname)
                    shutil.copymode(srcname, destname, follow_symlinks=False)
                else:
                    if item.is_dir():
                        copy_tree(
                            item,
                            destname,
                            follow_all_symlinks=follow_all_symlinks,
                            follow_outside_symlinks=follow_outside_symlinks)
                    else:
                        shutil.copy(item, destname)
                        shutil.copymode(item, destname)
            else:
                os.symlink(org_linkto, destname)
                shutil.copymode(srcname, destname, follow_symlinks=False)
        elif item.is_dir():
            copy_tree(item,
                      destname,
                      follow_all_symlinks=follow_all_symlinks,
                      follow_outside_symlinks=follow_outside_symlinks)
        else:
            shutil.copy(item, destname)
            shutil.copymode(item, destname)
    shutil.copystat(src, dest)
    return dest


def copy_file(f,
              dest,
              deps,
              follow_all_symlinks=False,
              follow_outside_symlinks=False):
    """Copy file or directory and update deps."""
    if os.path.isdir(f):
        copy_tree(f,
                  os.path.join(dest, os.path.basename(f)),
                  follow_all_symlinks=follow_all_symlinks,
                  follow_outside_symlinks=follow_outside_symlinks)
        deps.extend(build_utils.get_all_files(f))
    else:
        if os.path.isfile(os.path.join(dest, os.path.basename(f))):
            dest = os.path.join(dest, os.path.basename(f))

        deps.append(f)

        if os.path.isfile(dest):
            if filecmp.cmp(dest, f, shallow=False):
                return
            # The shutil.copy() below would fail if the file does not
            # have write permissions. Deleting the file
            # has similar costs to modifying the permissions.
            os.unlink(dest)

        shutil.copy(f, dest)


def do_copy(options, deps):
    """Copy files or directories given in options.files and update deps."""
    files = list(
        itertools.chain.from_iterable(
            build_utils.parse_gn_list(f) for f in options.files))

    for f in files:
        if not options.ignore_stale:
            if os.path.isdir(f) and not options.clear:
                print('To avoid stale files you must use --clear when copying '
                      'directories')
                sys.exit(-1)
        copy_file(f,
                  options.dest,
                  deps,
                  follow_all_symlinks=options.follow_all_symlinks,
                  follow_outside_symlinks=options.follow_outside_symlinks)


def do_renaming(options, deps):
    """Copy and rename files given in options.renaming_sources
    and update deps.
    """
    src_files = list(
        itertools.chain.from_iterable(
            build_utils.parse_gn_list(f) for f in options.renaming_sources))

    dest_files = list(
        itertools.chain.from_iterable(
            build_utils.parse_gn_list(f)
            for f in options.renaming_destinations))

    if (len(src_files) != len(dest_files)):
        print('Renaming source and destination files not match.')
        sys.exit(-1)

    for src, dest in zip(src_files, dest_files):
        if os.path.isdir(src):
            print('renaming directory is not supported.')
            sys.exit(-1)
        else:
            copy_file(src, os.path.join(options.dest, dest), deps)


def main(args):
    args = build_utils.expand_file_args(args)

    parser = optparse.OptionParser()
    build_utils.add_depfile_option(parser)

    parser.add_option('--dest', help='Directory to copy files to.')
    parser.add_option('--files',
                      action='append',
                      help='List of files to copy.')
    parser.add_option(
        '--clear',
        action='store_true',
        help='If set, the destination directory will be deleted '
        'before copying files to it. This is highly recommended to '
        'ensure that no stale files are left in the directory.')
    parser.add_option('--ignore-stale',
                      action='store_true',
                      help='Do copy even there may be stale files. '
                      'If set, overrule options.clear')
    parser.add_option('--stamp', help='Path to touch on success.')
    parser.add_option('--follow-all-symlinks',
                      action='store_true',
                      help='whether to follow symlinks')
    parser.add_option(
        '--follow-outside-symlinks',
        action='store_true',
        help='whether to follow symlinks which points to targets outside of'
        'source directory')
    parser.add_option('--renaming-sources',
                      action='append',
                      help='List of files need to be renamed while being '
                      'copied to dest directory')
    parser.add_option('--renaming-destinations',
                      action='append',
                      help='List of destination file name without path, the '
                      'number of elements must match rename-sources.')

    options, _ = parser.parse_args(args)
    if options.follow_all_symlinks:
        options.follow_outside_symlinks = True

    if options.clear and not options.ignore_stale:
        build_utils.delete_directory(options.dest)
        build_utils.make_directory(options.dest)

    deps = []

    if options.files:
        do_copy(options, deps)

    if options.renaming_sources:
        do_renaming(options, deps)

    if options.depfile:
        build_utils.write_depfile(options.depfile,
                                  options.stamp,
                                  deps,
                                  add_pydeps=False)

    if options.stamp:
        build_utils.touch(options.stamp)


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
