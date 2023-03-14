#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright 2013 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import difflib
import hashlib
import itertools
import json
import os
import zipfile
from .pycache import pycache_enabled
from .pycache import pycache

# When set and a difference is detected, a diff of what changed is printed.
PRINT_EXPLANATIONS = int(os.environ.get('PRINT_BUILD_EXPLANATIONS', 0))

# An escape hatch that causes all targets to be rebuilt.
_FORCE_REBUILD = int(os.environ.get('FORCE_REBUILD', 0))


def get_new_metadata(input_strings, input_paths):
    new_metadata = _Metadata()
    new_metadata.add_strings(input_strings)

    for path in input_paths:
        if _is_zip_file(path):
            entries = _extract_zip_entries(path)
            new_metadata.add_zip_file(path, entries)
        else:
            new_metadata.add_file(path, _md5_for_path(path))
    return new_metadata


def get_old_metadata(record_path):
    old_metadata = None
    if os.path.exists(record_path):
        with open(record_path, 'r') as jsonfile:
            try:
                old_metadata = _Metadata.from_file(jsonfile)
            except:  # noqa: E722 pylint: disable=bare-except
                pass
    return old_metadata


def print_explanations(record_path, changes):
    if PRINT_EXPLANATIONS:
        print('=' * 80)
        print('Target is stale: %s' % record_path)
        print(changes.describe_difference())
        print('=' * 80)


def call_and_record_if_stale(
        function,  # pylint: disable=invalid-name
        record_path=None,
        input_paths=None,
        input_strings=None,
        output_paths=None,
        force=False,
        pass_changes=False):
    """Calls function if outputs are stale.

    Outputs are considered stale if:
    - any output_paths are missing, or
    - the contents of any file within input_paths has changed, or
    - the contents of input_strings has changed.

    To debug which files are out-of-date, set the environment variable:
        PRINT_MD5_DIFFS=1

    Args:
      function: The function to call.
      record_path: Path to record metadata.
        Defaults to output_paths[0] + '.md5.stamp'
      input_paths: List of paths to calculate a md5 sum on.
      input_strings: List of strings to record verbatim.
      output_paths: List of output paths.
      force: Whether to treat outputs as missing regardless of whether they
        actually are.
      pass_changes: Whether to pass a Changes instance to |function|.
    """
    assert record_path or output_paths
    input_paths = input_paths or []
    input_strings = input_strings or []
    output_paths = output_paths or []

    new_metadata = get_new_metadata(input_strings, input_paths)
    force = force or _FORCE_REBUILD
    missing_outputs = [
        x for x in output_paths if force or not os.path.exists(x)
    ]

    if pycache_enabled:
        # Input strings, input files and outputs names together compose
        # cache manifest, which is the only identifier of a python action.
        manifest = '-'.join(
            [new_metadata.strings_md5(),
             new_metadata.files_md5()] + sorted(output_paths))
        record_path = pycache.get_manifest_path('{}.manifest'.format(manifest))
        old_metadata = get_old_metadata(record_path)
    else:
        record_path = record_path or output_paths[0] + '.md5.stamp'
        # When outputs are missing, don't bother gathering change information.
        if not missing_outputs:
            old_metadata = get_old_metadata(record_path)
        else:
            old_metadata = None

    changes = Changes(old_metadata, new_metadata, force, missing_outputs)
    if not changes.has_changes():
        if not pycache_enabled:
            return
        if pycache_enabled and pycache.retrieve(output_paths, prefix=manifest):
            return

    print_explanations(record_path, changes)

    args = (changes, ) if pass_changes else ()
    function(*args)
    if pycache_enabled:
        try:
            pycache.report_cache_stat('cache_miss')
        except:  # noqa: E722 pylint: disable=bare-except
            pass
        pycache.save(output_paths, prefix=manifest)

    with open(record_path, 'w') as record:
        new_metadata.to_file(record)


class Changes(object):
    """Provides and API for querying what changed between runs."""
    def __init__(self, old_metadata, new_metadata, force, missing_outputs):
        self.old_metadata = old_metadata
        self.new_metadata = new_metadata
        self.force = force
        self.missing_outputs = missing_outputs

    def _get_old_tag(self, path, subpath=None):
        return self.old_metadata and self.old_metadata.get_tag(path, subpath)

    def has_changes(self):
        """Returns whether any changes exist."""
        return (
            self.force or not self.old_metadata or
            self.old_metadata.strings_md5() != self.new_metadata.strings_md5()
            or self.old_metadata.files_md5() != self.new_metadata.files_md5())

    def added_or_modified_only(self):
        """Returns whether the only changes were from added or modified (sub)files.

        No missing outputs, no removed paths/subpaths.
        """
        if (self.force or not self.old_metadata
                or self.old_metadata.strings_md5() !=
                self.new_metadata.strings_md5()):
            return False
        if any(self.iter_removed_paths()):
            return False
        for path in self.iter_modified_paths():
            if any(self.iter_removed_subpaths(path)):
                return False
        return True

    def iter_all_paths(self):
        """Generator for paths."""
        return self.new_metadata.iter_paths()

    def iter_all_subpaths(self, path):
        """Generator for subpaths."""
        return self.new_metadata.iter_subpaths(path)

    def iter_added_paths(self):
        """Generator for paths that were added."""
        for path in self.new_metadata.iter_paths():
            if self._get_old_tag(path) is None:
                yield path

    def iter_added_subpaths(self, path):
        """Generator for paths that were added within the given zip file."""
        for subpath in self.new_metadata.iter_subpaths(path):
            if self._get_old_tag(path, subpath) is None:
                yield subpath

    def iter_removed_paths(self):
        """Generator for paths that were removed."""
        if self.old_metadata:
            for path in self.old_metadata.iter_paths():
                if self.new_metadata.get_tag(path) is None:
                    yield path

    def iter_removed_subpaths(self, path):
        """Generator for paths that were removed within the given zip file."""
        if self.old_metadata:
            for subpath in self.old_metadata.iter_subpaths(path):
                if self.new_metadata.get_tag(path, subpath) is None:
                    yield subpath

    def iter_modified_paths(self):
        """Generator for paths whose contents have changed."""
        for path in self.new_metadata.iter_paths():
            old_tag = self._get_old_tag(path)
            new_tag = self.new_metadata.get_tag(path)
            if old_tag is not None and old_tag != new_tag:
                yield path

    def iter_modified_subpaths(self, path):
        """Generator for paths within a zip file whose contents have changed."""
        for subpath in self.new_metadata.iter_subpaths(path):
            old_tag = self._get_old_tag(path, subpath)
            new_tag = self.new_metadata.get_tag(path, subpath)
            if old_tag is not None and old_tag != new_tag:
                yield subpath

    def iter_changed_paths(self):
        """Generator for all changed paths (added/removed/modified)."""
        return itertools.chain(self.iter_removed_paths(),
                               self.iter_modified_paths(),
                               self.iter_added_paths())

    def iter_changed_subpaths(self, path):
        """Generator for paths within a zip that were added/removed/modified."""
        return itertools.chain(self.iter_removed_subpaths(path),
                               self.iter_modified_subpaths(path),
                               self.iter_added_subpaths(path))

    def describe_difference(self):
        """Returns a human-readable description of what changed."""
        if self.force:
            return 'force=True'
        elif self.old_metadata is None:
            return 'Previous stamp file not found.'

        if self.old_metadata.strings_md5() != self.new_metadata.strings_md5():
            ndiff = difflib.ndiff(self.old_metadata.get_strings(),
                                  self.new_metadata.get_strings())
            changed = [s for s in ndiff if not s.startswith(' ')]
            return 'Input strings changed:\n  ' + '\n  '.join(changed)

        if self.old_metadata.files_md5() == self.new_metadata.files_md5():
            return "There's no difference."

        lines = []
        lines.extend('Added: {}'.format(p for p in self.iter_added_paths()))
        lines.extend('Removed: {}'.format(p
                                          for p in self.iter_removed_paths()))
        for path in self.iter_modified_paths():
            lines.append('Modified: {}'.format(path))
            lines.extend('  -> Subpath added: {}'.format(
                p for p in self.iter_added_subpaths(path)))
            lines.extend('  -> Subpath removed: {}'.format(
                p for p in self.iter_removed_subpaths(path)))
            lines.extend('  -> Subpath modified: {}'.format(
                p for p in self.iter_modified_subpaths(path)))
        if lines:
            return 'Input files changed:\n  {}'.format('\n  '.join(lines))

        if self.missing_outputs:
            return 'Outputs do not exist:\n  {}'.format('\n  '.join(
                self.missing_outputs))

        return 'I have no idea what changed (there is a bug).'


class _Metadata(object):
    """Data model for tracking change metadata."""
    def __init__(self):
        self._files_md5 = None
        self._strings_md5 = None
        self._files = []
        self._strings = []
        # Map of (path, subpath) -> entry. Created upon first call to _get_entry().
        self._file_map = None

    @classmethod
    def from_file(cls, fileobj):
        """Returns a _Metadata initialized from a file object."""
        ret = cls()
        obj = json.load(fileobj)
        ret._files_md5 = obj['files-md5']
        ret._strings_md5 = obj['strings-md5']
        ret._files = obj['input-files']
        ret._strings = obj['input-strings']
        return ret

    def to_file(self, fileobj):
        """Serializes metadata to the given file object."""
        obj = {
            "files-md5": self.files_md5(),
            "strings-md5": self.strings_md5(),
            "input-files": self._files,
            "input-strings": self._strings,
        }
        json.dump(obj, fileobj, indent=2, sort_keys=True)

    def _assert_not_queried(self):
        assert self._files_md5 is None
        assert self._strings_md5 is None
        assert self._file_map is None

    def add_strings(self, values):
        self._assert_not_queried()
        self._strings.extend(str(v) for v in values)

    def add_file(self, path, tag):
        """Adds metadata for a non-zip file.

        Args:
          path: Path to the file.
          tag: A short string representative of the file contents.
        """
        self._assert_not_queried()
        self._files.append({
            'path': path,
            'tag': tag,
        })

    def add_zip_file(self, path, entries):
        """Adds metadata for a zip file.

        Args:
          path: Path to the file.
          entries: List of (subpath, tag) tuples for entries within the zip.
        """
        self._assert_not_queried()
        tag = _compute_inline_md5(
            itertools.chain((e[0] for e in entries), (e[1] for e in entries)))
        self._files.append({
            'path':
            path,
            'tag':
            tag,
            'entries': [{
                "path": e[0],
                "tag": e[1]
            } for e in entries],
        })

    def get_strings(self):
        """Returns the list of input strings."""
        return self._strings

    def files_md5(self):
        """Lazily computes and returns the aggregate md5 of input files."""
        if self._files_md5 is None:
            # Omit paths from md5 since temporary files have random names.
            self._files_md5 = _compute_inline_md5(
                self.get_tag(p) for p in sorted(self.iter_paths()))
        return self._files_md5

    def strings_md5(self):
        """Lazily computes and returns the aggregate md5 of input strings."""
        if self._strings_md5 is None:
            self._strings_md5 = _compute_inline_md5(self._strings)
        return self._strings_md5

    def _get_entry(self, path, subpath=None):
        """Returns the JSON entry for the given path / subpath."""
        if self._file_map is None:
            self._file_map = {}
            for entry in self._files:
                self._file_map[(entry['path'], None)] = entry
                for subentry in entry.get('entries', ()):
                    self._file_map[(entry['path'],
                                    subentry['path'])] = subentry
        return self._file_map.get((path, subpath))

    def get_tag(self, path, subpath=None):
        """Returns the tag for the given path / subpath."""
        ret = self._get_entry(path, subpath)
        return ret and ret['tag']

    def iter_paths(self):
        """Returns a generator for all top-level paths."""
        return (e['path'] for e in self._files)

    def iter_subpaths(self, path):
        """Returns a generator for all subpaths in the given zip.

        If the given path is not a zip file or doesn't exist, returns an empty
        iterable.
        """
        outer_entry = self._get_entry(path)
        if not outer_entry:
            return ()
        subentries = outer_entry.get('entries', [])
        return (entry['path'] for entry in subentries)


def _update_md5_for_file(md5, path, block_size=2**16):
    # record md5 of linkto for dead link.
    if os.path.islink(path):
        linkto = os.readlink(path)
        if not os.path.exists(linkto):
            md5.update(linkto.encode())
            return

    with open(path, 'rb') as infile:
        while True:
            data = infile.read(block_size)
            if not data:
                break
            md5.update(data)


def _update_md5_for_directory(md5, dir_path):
    for root, _, files in os.walk(dir_path):
        for f in files:
            _update_md5_for_file(md5, os.path.join(root, f))


def _md5_for_path(path):
    md5 = hashlib.md5()
    if os.path.isdir(path):
        _update_md5_for_directory(md5, path)
    else:
        _update_md5_for_file(md5, path)
    return md5.hexdigest()


def _compute_inline_md5(iterable):
    """Computes the md5 of the concatenated parameters."""
    md5 = hashlib.md5()
    for item in iterable:
        md5.update(str(item).encode())
    return md5.hexdigest()


def _is_zip_file(path):
    """Returns whether to treat the given file as a zip file."""
    return path[-4:] in ('.zip')


def _extract_zip_entries(path):
    """Returns a list of (path, CRC32) of all files within |path|."""
    entries = []
    with zipfile.ZipFile(path) as zip_file:
        for zip_info in zip_file.infolist():
            # Skip directories and empty files.
            if zip_info.CRC:
                entries.append(
                    (zip_info.filename, zip_info.CRC + zip_info.compress_type))
    return entries
