#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright 2013 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
"""Contains common helpers for GN action()s."""

import collections
import contextlib
from distutils import extension
import filecmp
import fnmatch
import json
import os
import pipes
import re
import shutil
import stat
import subprocess
import sys
import tempfile
import zipfile
import optparse

# Any new non-system import must be added to:

# Some clients do not add //build/scripts/util to PYTHONPATH.
from . import md5_check  # pylint: disable=relative-import

sys.path.append(os.path.join(os.path.dirname(__file__), os.pardir, os.pardir))
import gn_helpers

# Definition copied from pylib/constants/__init__.py to avoid adding
# a dependency on pylib.
DIR_SOURCE_ROOT = os.environ.get(
    'CHECKOUT_SOURCE_ROOT',
    os.path.abspath(
        os.path.join(os.path.dirname(__file__), os.pardir, os.pardir,
                     os.pardir, os.pardir)))

HERMETIC_TIMESTAMP = (2001, 1, 1, 0, 0, 0)
_HERMETIC_FILE_ATTR = (0o644 << 16)


@contextlib.contextmanager
def temp_dir():
    dirname = tempfile.mkdtemp()
    try:
        yield dirname
    finally:
        shutil.rmtree(dirname)


def make_directory(dir_path):
    try:
        os.makedirs(dir_path)
    except OSError:
        pass


def delete_directory(dir_path):
    if os.path.exists(dir_path):
        shutil.rmtree(dir_path)


def touch(path, fail_if_missing=False):
    if fail_if_missing and not os.path.exists(path):
        raise Exception(path + ' doesn\'t exist.')

    make_directory(os.path.dirname(path))
    with open(path, 'a'):
        os.utime(path, None)


def find_in_directory(directory, filename_filter):
    files = []
    for root, _dirnames, filenames in os.walk(directory):
        matched_files = fnmatch.filter(filenames, filename_filter)
        files.extend((os.path.join(root, f) for f in matched_files))
    return files


def read_build_vars(path):
    """Parses a build_vars.txt into a dict."""
    with open(path) as f:
        return dict(l.rstrip().split('=', 1) for l in f)


def parse_gn_list(gn_string):
    """Converts a command-line parameter into a list.

    If the input starts with a '[' it is assumed to be a GN-formatted list and
    it will be parsed accordingly. When empty an empty list will be returned.
    Otherwise, the parameter will be treated as a single raw string (not
    GN-formatted in that it's not assumed to have literal quotes that must be
    removed) and a list will be returned containing that string.

    The common use for this behavior is in the ohos build where things can
    take lists of @FileArg references that are expanded via expand_file_args.
    """
    if gn_string.startswith('['):
        parser = gn_helpers.GNValueParser(gn_string)
        return parser.ParseList()
    if len(gn_string):
        return [gn_string]
    return []


def parse_and_flatten_gn_lists(gn_lists):
    ret = []
    for arg in gn_lists:
        ret.extend(parse_gn_list(arg))
    return ret


def check_options(options, parser, required=None):
    if not required:
        return
    for option_name in required:
        if getattr(options, option_name) is None:
            parser.error('--%s is required' % option_name.replace('_', '-'))


def write_json(obj, path, only_if_changed=False):
    old_dump = None
    if os.path.exists(path):
        with open(path, 'r') as oldfile:
            old_dump = oldfile.read()

    new_dump = json.dumps(obj,
                          sort_keys=True,
                          indent=2,
                          separators=(',', ': '))

    if not only_if_changed or old_dump != new_dump:
        with open(path, 'w') as outfile:
            outfile.write(new_dump)


@contextlib.contextmanager
def atomic_output(path, only_if_changed=True):
    """Helper to prevent half-written outputs.

    Args:
      path: Path to the final output file, which will be written atomically.
      only_if_changed: If True (the default), do not touch the filesystem
        if the content has not changed.
    Returns:
      A python context manager that yields a NamedTemporaryFile instance
      that must be used by clients to write the data to. On exit, the
      manager will try to replace the final output file with the
      temporary one if necessary. The temporary file is always destroyed
      on exit.
    Example:
      with build_utils.atomic_output(output_path) as tmp_file:
        subprocess.check_call(['prog', '--output', tmp_file.name])
    """
    # Create in same directory to ensure same filesystem when moving.
    with tempfile.NamedTemporaryFile(suffix=os.path.basename(path),
                                     dir=os.path.dirname(path),
                                     delete=False) as f:
        try:
            # Change tempfile permission to 664
            os.fchmod(f.fileno(), 0o664)
            yield f

            # file should be closed before comparison/move.
            f.close()
            if not (only_if_changed and os.path.exists(path)
                    and filecmp.cmp(f.name, path)):
                shutil.move(f.name, path)
        finally:
            if os.path.exists(f.name):
                os.unlink(f.name)


class called_process_error(Exception):
    """This exception is raised when the process run by check_output
    exits with a non-zero exit code."""
    def __init__(self, cwd, args, output):
        super(called_process_error, self).__init__()
        self.cwd = cwd
        self.args = args
        if isinstance(output, bytes):
            self.output = output.decode()
        else:
            self.output = output

    def __str__(self):
        # A user should be able to simply copy and paste the command that failed
        # into their shell.
        copyable_command = '( cd {}; {} )'.format(
            os.path.abspath(self.cwd), ' '.join(map(pipes.quote, self.args)))
        return 'Command failed: {}\n{}'.format(copyable_command, self.output)


def filter_lines(output, filter_string):
    """Output filter from build_utils.check_output.

    Args:
      output: Executable output as from build_utils.check_output.
      filter_string: An RE string that will filter (remove) matching
          lines from |output|.

    Returns:
      The filtered output, as a single string.
    """
    re_filter = re.compile(filter_string)
    return '\n'.join(line for line in output.splitlines()
                     if not re_filter.search(line))


# This can be used in most cases like subprocess.check_output(). The output,
# particularly when the command fails, better highlights the command's failure.
# If the command fails, raises a build_utils.called_process_error.
def check_output(args,
                 cwd=None,
                 env=None,
                 print_stdout=False,
                 print_stderr=True,
                 stdout_filter=None,
                 stderr_filter=None,
                 fail_func=lambda returncode, stderr: returncode != 0):
    if not cwd:
        cwd = os.getcwd()

    child = subprocess.Popen(args,
                             stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE,
                             cwd=cwd,
                             env=env)
    stdout, stderr = child.communicate()

    if stdout_filter is not None:
        stdout = stdout_filter(stdout)

    if stderr_filter is not None:
        stderr = stderr_filter(stderr)
    if isinstance(stdout, bytes):
        stdout = stdout.decode()
    if isinstance(stderr, bytes):
        stderr = stderr.decode()

    if fail_func(child.returncode, stderr):
        raise called_process_error(cwd, args, stdout + stderr)

    if print_stdout:
        if isinstance(stdout, bytes):
            stdout = stdout.decode()
        if stdout:
            sys.stdout.write(stdout)
    if print_stderr:
        if isinstance(stderr, bytes):
            stderr = stderr.decode()
        if stderr:
            sys.stderr.write(stderr)
    return stdout


def get_modified_time(path):
    # For a symlink, the modified time should be the greater of the link's
    # modified time and the modified time of the target.
    return max(os.lstat(path).st_mtime, os.stat(path).st_mtime)


def is_time_stale(output, inputs):
    if not os.path.exists(output):
        return True

    output_time = get_modified_time(output)
    for i in inputs:
        if get_modified_time(i) > output_time:
            return True
    return False


def _check_zip_path(name):
    if os.path.normpath(name) != name:
        raise Exception('Non-canonical zip path: %s' % name)
    if os.path.isabs(name):
        raise Exception('Absolute zip path: %s' % name)


def _is_symlink(zip_file, name):
    zi = zip_file.getinfo(name)

    # The two high-order bytes of ZipInfo.external_attr represent
    # UNIX permissions and file type bits.
    return stat.S_ISLNK(zi.external_attr >> 16)


def extract_all(zip_path,
                path=None,
                no_clobber=True,
                pattern=None,
                predicate=None):
    if path is None:
        path = os.getcwd()
    elif not os.path.exists(path):
        make_directory(path)

    if not zipfile.is_zipfile(zip_path):
        raise Exception('Invalid zip file: %s' % zip_path)

    extracted = []
    with zipfile.ZipFile(zip_path) as z:
        for name in z.namelist():
            if name.endswith('/'):
                make_directory(os.path.join(path, name))
                continue
            if pattern is not None:
                if not fnmatch.fnmatch(name, pattern):
                    continue
            if predicate and not predicate(name):
                continue
            _check_zip_path(name)
            if no_clobber:
                output_path = os.path.join(path, name)
                if os.path.exists(output_path):
                    raise Exception('Path already exists from zip: %s %s %s' %
                                    (zip_path, name, output_path))
            if _is_symlink(z, name):
                dest = os.path.join(path, name)
                make_directory(os.path.dirname(dest))
                os.symlink(z.read(name), dest)
                extracted.append(dest)
            else:
                z.extract(name, path)
                extracted.append(os.path.join(path, name))

    return extracted


def add_to_zip_hermetic(zip_file,
                        zip_path,
                        src_path=None,
                        data=None,
                        compress=None):
    """Adds a file to the given ZipFile with a hard-coded modified time.

    Args:
      zip_file: ZipFile instance to add the file to.
      zip_path: Destination path within the zip file.
      src_path: Path of the source file. Mutually exclusive with |data|.
      data: File data as a string.
      compress: Whether to enable compression. Default is taken from ZipFile
          constructor.
    """
    assert (src_path is None) != (data is None), (
        '|src_path| and |data| are mutually exclusive.')
    _check_zip_path(zip_path)
    zipinfo = zipfile.ZipInfo(filename=zip_path, date_time=HERMETIC_TIMESTAMP)
    zipinfo.external_attr = _HERMETIC_FILE_ATTR

    if src_path and os.path.islink(src_path):
        zipinfo.filename = zip_path
        zipinfo.external_attr |= stat.S_IFLNK << 16  # mark as a symlink
        zip_file.writestr(zipinfo, os.readlink(src_path))
        return

    # we want to use _HERMETIC_FILE_ATTR, so manually set
    # the few attr bits we care about.
    if src_path:
        st = os.stat(src_path)
        for mode in (stat.S_IXUSR, stat.S_IXGRP, stat.S_IXOTH):
            if st.st_mode & mode:
                zipinfo.external_attr |= mode << 16

    if src_path:
        with open(src_path, 'rb') as f:
            data = f.read()

    # zipfile will deflate even when it makes the file bigger. To avoid
    # growing files, disable compression at an arbitrary cut off point.
    if len(data) < 16:
        compress = False

    # None converts to ZIP_STORED, when passed explicitly rather than the
    # default passed to the ZipFile constructor.
    compress_type = zip_file.compression
    if compress is not None:
        compress_type = zipfile.ZIP_DEFLATED if compress else zipfile.ZIP_STORED
    zip_file.writestr(zipinfo, data, compress_type)


def do_zip(inputs,
           output,
           base_dir=None,
           compress_fn=None,
           zip_prefix_path=None):
    """Creates a zip file from a list of files.

    Args:
      inputs: A list of paths to zip, or a list of (zip_path, fs_path) tuples.
      output: Destination .zip file.
      base_dir: Prefix to strip from inputs.
      compress_fn: Applied to each input to determine whether or not to compress.
          By default, items will be |zipfile.ZIP_STORED|.
      zip_prefix_path: Path prepended to file path in zip file.
    """
    input_tuples = []
    for tup in inputs:
        if isinstance(tup, str):
            tup = (os.path.relpath(tup, base_dir), tup)
        input_tuples.append(tup)

    # Sort by zip path to ensure stable zip ordering.
    input_tuples.sort(key=lambda tup: tup[0])
    with zipfile.ZipFile(output, 'w') as outfile:
        for zip_path, fs_path in input_tuples:
            if zip_prefix_path:
                zip_path = os.path.join(zip_prefix_path, zip_path)
            compress = compress_fn(zip_path) if compress_fn else None
            add_to_zip_hermetic(outfile,
                                zip_path,
                                src_path=fs_path,
                                compress=compress)


def zip_dir(output, base_dir, compress_fn=None, zip_prefix_path=None):
    """Creates a zip file from a directory."""
    inputs = []
    for root, _, files in os.walk(base_dir):
        for f in files:
            inputs.append(os.path.join(root, f))

    with atomic_output(output) as f:
        do_zip(inputs,
               f,
               base_dir,
               compress_fn=compress_fn,
               zip_prefix_path=zip_prefix_path)


def matches_glob(path, filters):
    """Returns whether the given path matches any of the given glob patterns."""
    return filters and any(fnmatch.fnmatch(path, f) for f in filters)


def _strip_dst_name(dst_name, options):
    # Strip specific directories and file if options is not None
    if options and options.stripFile:
        for f in options.stripFile:
            if fnmatch.fnmatch(dst_name, '*/' + f):
                return True
    if options and options.stripDir:
        for d in options.stripDir:
            if fnmatch.fnmatch(dst_name, d + '/*'):
                return True
    return False


def merge_zips(output, input_zips, path_transform=None, merge_args=None):
    """Combines all files from |input_zips| into |output|.

    Args:
      output: Path or ZipFile instance to add files to.
      input_zips: Iterable of paths to zip files to merge.
      path_transform: Called for each entry path. Returns a new path, or None to
          skip the file.
    """
    options = None
    if merge_args:
        parser = optparse.OptionParser()
        parser.add_option('--stripDir',
                          action='append',
                          help='strip specific directory')
        parser.add_option('--stripFile',
                          action='append',
                          help='strip specific file.')

        args = expand_file_args(merge_args)
        options, _ = parser.parse_args(args)

    path_transform = path_transform or (lambda p: p)
    added_names = set()

    output_is_already_open = not isinstance(output, str)
    if output_is_already_open:
        assert isinstance(output, zipfile.ZipFile)
        out_zip = output
    else:
        out_zip = zipfile.ZipFile(output, 'w')

    try:
        for in_file in input_zips:
            with zipfile.ZipFile(in_file, 'r') as in_zip:
                # ijar creates zips with null CRCs.
                in_zip._expected_crc = None
                for info in in_zip.infolist():
                    # Ignore directories.
                    if info.filename[-1] == '/':
                        continue
                    dst_name = path_transform(info.filename)
                    if not dst_name:
                        continue
                    if _strip_dst_name(dst_name, options):
                        continue
                    already_added = dst_name in added_names
                    if not already_added:
                        add_to_zip_hermetic(
                            out_zip,
                            dst_name,
                            data=in_zip.read(info),
                            compress=info.compress_type != zipfile.ZIP_STORED)
                        added_names.add(dst_name)
    finally:
        if not output_is_already_open:
            out_zip.close()


def get_sorted_transitive_dependencies(top, deps_func):
    """Gets the list of all transitive dependencies in sorted order.

    There should be no cycles in the dependency graph (crashes if cycles exist).

    Args:
      top: A list of the top level nodes
      deps_func: A function that takes a node and returns a list of its direct
          dependencies.
    Returns:
      A list of all transitive dependencies of nodes in top, in order (a node
      will appear in the list at a higher index than all of its dependencies).
    """
    # Find all deps depth-first, maintaining original order in the case of ties.
    deps_map = collections.OrderedDict()

    def discover(nodes):
        for node in nodes:
            if node in deps_map:
                continue
            deps = deps_func(node)
            discover(deps)
            deps_map[node] = deps

    discover(top)
    return list(deps_map.keys())


def _compute_python_dependencies():
    """Gets the paths of imported non-system python modules.

    A path is assumed to be a "system" import if it is outside of chromium's
    src/. The paths will be relative to the current directory.
    """
    _force_lazy_modules_to_load()
    module_paths = (m.__file__ for m in sys.modules.values()
                    if m is not None and hasattr(m, '__file__') and m.__file__)
    abs_module_paths = list(map(os.path.abspath, module_paths))

    assert os.path.isabs(DIR_SOURCE_ROOT)
    non_system_module_paths = [
        p for p in abs_module_paths if p.startswith(DIR_SOURCE_ROOT)
    ]

    def convert_pyc_to_py(s):
        if s.endswith('.pyc'):
            return s[:-1]
        return s

    non_system_module_paths = list(
        map(convert_pyc_to_py, non_system_module_paths))
    non_system_module_paths = list(
        map(os.path.relpath, non_system_module_paths))
    return sorted(set(non_system_module_paths))


def _force_lazy_modules_to_load():
    """Forces any lazily imported modules to fully load themselves.

    Inspecting the modules' __file__ attribute causes lazily imported modules
    (e.g. from email) to get fully imported and update sys.modules. Iterate
    over the values until sys.modules stabilizes so that no modules are missed.
    """
    while True:
        num_modules_before = len(list(sys.modules.keys()))
        for m in list(sys.modules.values()):
            if m is not None and hasattr(m, '__file__'):
                _ = m.__file__
        num_modules_after = len(list(sys.modules.keys()))
        if num_modules_before == num_modules_after:
            break


def add_depfile_option(parser):
    if hasattr(parser, 'add_option'):
        func = parser.add_option
    else:
        func = parser.add_argument
    func('--depfile', help='Path to depfile (refer to `gn help depfile`)')


def write_depfile(depfile_path, first_gn_output, inputs=None, add_pydeps=True):
    assert depfile_path != first_gn_output  # http://crbug.com/646165
    inputs = inputs or []
    if add_pydeps:
        inputs = _compute_python_dependencies() + inputs
    inputs = sorted(inputs)
    make_directory(os.path.dirname(depfile_path))
    # Ninja does not support multiple outputs in depfiles.
    with open(depfile_path, 'w') as depfile:
        depfile.write(first_gn_output.replace(' ', '\\ '))
        depfile.write(': ')
        depfile.write(' '.join(i.replace(' ', '\\ ') for i in inputs))
        depfile.write('\n')


def expand_file_args(args):
    """Replaces file-arg placeholders in args.

    These placeholders have the form:
      @FileArg(filename:key1:key2:...:keyn)

    The value of such a placeholder is calculated by reading 'filename' as json.
    And then extracting the value at [key1][key2]...[keyn].

    Note: This intentionally does not return the list of files that appear in
    such placeholders. An action that uses file-args *must* know the paths of
    those files prior to the parsing of the arguments (typically by explicitly
    listing them in the action's inputs in build files).
    """
    new_args = list(args)
    file_jsons = dict()
    r = re.compile(r'@FileArg\((.*?)\)')
    for i, arg in enumerate(args):
        match = r.search(arg)
        if not match:
            continue

        if match.end() != len(arg):
            raise Exception(
                'Unexpected characters after FileArg: {}'.format(arg))

        lookup_path = match.group(1).split(':')
        file_path = lookup_path[0]
        if file_path not in file_jsons:
            with open(file_path) as f:
                file_jsons[file_path] = json.load(f)

        expansion = file_jsons[file_path]

        for k in lookup_path[1:]:
            if k in expansion:
                expansion = expansion[k]
            else:
                expansion = ""
                print("WARNNING", lookup_path[1:], "is not in metadata file, set default ''")
        # This should match parse_gn_list. The output is either a GN-formatted list
        # or a literal (with no quotes).
        if isinstance(expansion, list):
            new_args[i] = arg[:match.start()] + gn_helpers.ToGNString(
                expansion)
        else:
            new_args[i] = arg[:match.start()] + str(expansion)

    return new_args


def read_sources_list(sources_list_file_name):
    """Reads a GN-written file containing list of file names and returns a list.

    Note that this function should not be used to parse response files.
    """
    with open(sources_list_file_name) as f:
        return [file_name.strip() for file_name in f]


def call_and_write_depfile_if_stale(function,
                                    options,
                                    record_path=None,
                                    input_paths=None,
                                    input_strings=None,
                                    output_paths=None,
                                    force=False,
                                    pass_changes=False,
                                    depfile_deps=None,
                                    add_pydeps=True):
    """Wraps md5_check.call_and_record_if_stale() and writes a depfile if applicable.

    Depfiles are automatically added to output_paths when present in the
    |options| argument. They are then created after |function| is called.

    By default, only python dependencies are added to the depfile. If there are
    other input paths that are not captured by GN deps, then they should be
    listed in depfile_deps. It's important to write paths to the depfile that
    are already captured by GN deps since GN args can cause GN deps to change,
    and such changes are not immediately reflected in depfiles
    (http://crbug.com/589311).
    """
    if not output_paths:
        raise Exception('At least one output_path must be specified.')
    input_paths = list(input_paths or [])
    input_strings = list(input_strings or [])
    output_paths = list(output_paths or [])

    python_deps = None
    if hasattr(options, 'depfile') and options.depfile:
        python_deps = _compute_python_dependencies()
        input_paths += python_deps
        output_paths += [options.depfile]

    def on_stale_md5(changes):
        args = (changes, ) if pass_changes else ()
        function(*args)
        if python_deps is not None:
            all_depfile_deps = list(python_deps) if add_pydeps else []
            if depfile_deps:
                all_depfile_deps.extend(depfile_deps)
            write_depfile(options.depfile,
                          output_paths[0],
                          all_depfile_deps,
                          add_pydeps=False)

    md5_check.call_and_record_if_stale(on_stale_md5,
                                       record_path=record_path,
                                       input_paths=input_paths,
                                       input_strings=input_strings,
                                       output_paths=output_paths,
                                       force=force,
                                       pass_changes=True)


def get_all_files(base, follow_symlinks=False):
    """Returns a list of all the files in |base|. Each entry is relative to the
    last path entry of |base|."""
    result = []
    for root, _, files in os.walk(base, followlinks=follow_symlinks):
        result.extend([os.path.join(root, f) for f in files])

    return result


def rebase_path(input, new_base=None, current_base="."):
    if new_base:
        return os.path.relpath(os.path.join(current_base, input), new_base)
    else:
        return os.path.realpath(os.path.join(current_base, input))
