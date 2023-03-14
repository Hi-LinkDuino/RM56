#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright (c) 2021 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""

Usage: gen_notice_file --output-image-name system \
               --notice-file-root xx/NOTICE_FILE \
               --notice-file-install-path xx/system \
               --output-title notice_title_string

Generate the project notice files, including both text and xml files.

"""
from collections import defaultdict
import argparse
import hashlib
import os
import os.path
import sys
import gzip
import shutil
import glob
import re

sys.path.append(
    os.path.dirname(os.path.dirname(os.path.dirname(
        os.path.abspath(__file__)))))
from scripts.util import build_utils  # noqa: E402
from scripts.util.file_utils import write_json_file  # noqa: E402

XML_ESCAPE_TABLE = {
    "&": "&amp;",
    '"': "&quot;",
    "'": "&apos;",
    ">": "&gt;",
    "<": "&lt;",
}


def copy_static_library_notices(options, depfiles):
    valid_notices = []
    basenames = []
    for file in build_utils.get_all_files(options.static_library_notice_dir):
        if os.stat(file).st_size == 0:
            continue
        if not file.endswith('.a.txt'):
            continue
        notice_file_name = os.path.basename(file)
        if file not in basenames:
            basenames.append(notice_file_name)
            valid_notices.append(file)
            depfiles.append(file)

    for file in valid_notices:
        if options.image_name == "system":
            if options.target_cpu == "arm64" or options.target_cpu == "x64":
                install_dir = "system/lib64"
            elif options.target_cpu == "arm":
                install_dir = "system/lib"
            else:
                continue
        elif options.image_name == "sdk":
            install_dir = "toolchains/lib"
        elif options.image_name == "ndk":
            install_dir = "sysroot/usr/lib"
        else:
            continue
        dest = os.path.join(options.notice_root_dir, install_dir,
                            os.path.basename(file))
        os.makedirs(os.path.dirname(dest), exist_ok=True)
        shutil.copyfile(file, dest)


def write_file(file, string):
    print(string, file=file)


def compute_hash(file):
    sha256 = hashlib.sha256()
    with open(file, 'rb') as file_fd:
        for line in file_fd:
            sha256.update(line)
    return sha256.hexdigest()


def get_entity(text):
    return "".join(XML_ESCAPE_TABLE.get(c, c) for c in text)


def generate_txt_notice_files(file_hash, input_dir, output_filename,
                              notice_title):
    with open(output_filename, "w") as output_file:
        write_file(output_file, notice_title)
        for value in file_hash:
            write_file(output_file, '=' * 60)
            write_file(output_file, "Notices for file(s):")
            for filename in value:
                write_file(
                    output_file, '/{}'.format(
                        re.sub('.txt.*', '',
                               os.path.relpath(filename, input_dir))))
            write_file(output_file, '-' * 60)
            with open(value[0], errors='ignore') as temp_file_hd:
                write_file(output_file, temp_file_hd.read())


def generate_xml_notice_files(files_with_same_hash, input_dir,
                              output_filename):
    id_table = {}
    for file_key in files_with_same_hash.keys():
        for filename in files_with_same_hash[file_key]:
            id_table[filename] = file_key

    with open(output_filename, "w") as output_file:
        write_file(output_file, '<?xml version="1.0" encoding="utf-8"?>')
        write_file(output_file, "<licenses>")

        # Flatten the lists into a single filename list
        sorted_filenames = sorted(id_table.keys())

        # write out a table of contents
        for filename in sorted_filenames:
            stripped_filename = re.sub('.txt.*', '',
                                       os.path.relpath(filename, input_dir))
            write_file(
                output_file, '<file-name contentId="%s">%s</file-name>' %
                             (id_table.get(filename), stripped_filename))

        write_file(output_file, '')
        write_file(output_file, '')

        processed_file_keys = []
        # write the notice file lists
        for filename in sorted_filenames:
            file_key = id_table.get(filename)
            if file_key in processed_file_keys:
                continue
            processed_file_keys.append(file_key)

            with open(filename, errors='ignore') as temp_file_hd:
                write_file(
                    output_file,
                    '<file-content contentId="{}"><![CDATA[{}]]></file-content>'
                        .format(file_key, get_entity(temp_file_hd.read())))
            write_file(output_file, '')

        # write the file complete node.
        write_file(output_file, "</licenses>")


def compress_file_to_gz(src_file_name, gz_file_name):
    with open(src_file_name, mode='rb') as src_file_fd:
        with gzip.open(gz_file_name, mode='wb') as gz_file_fd:
            gz_file_fd.writelines(src_file_fd)


def handle_zipfile_notices(zip_file):
    notice_file = '{}.txt'.format(zip_file[:-4])
    with build_utils.temp_dir() as tmp_dir:
        build_utils.extract_all(zip_file, tmp_dir, no_clobber=False)
        files = build_utils.get_all_files(tmp_dir)
        contents = []
        for file in files:
            with open(file, 'r') as fd:
                data = fd.read()
                if data not in contents:
                    contents.append(data)
        with open(notice_file, 'w') as merged_notice:
            merged_notice.write('\n\n'.join(contents))
    return notice_file


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--image-name')
    parser.add_argument('--collected-notice-zipfile',
                        action='append',
                        help='zipfile stors collected notice files')
    parser.add_argument('--notice-root-dir', help='where notice files store')
    parser.add_argument('--output-notice-txt', help='output notice.txt')
    parser.add_argument('--output-notice-gz', help='output notice.txt')
    parser.add_argument('--notice-title', help='title of notice.txt')
    parser.add_argument('--static-library-notice-dir',
                        help='path to static library notice files')
    parser.add_argument('--target-cpu', help='cpu arch')
    parser.add_argument('--depfile', help='depfile')
    parser.add_argument('--notice-module-info',
                        help='module info file for notice target')
    parser.add_argument('--notice-install-dir',
                        help='install directories of notice file')

    args = parser.parse_args()

    notice_dir = args.notice_root_dir
    depfiles = []
    if args.collected_notice_zipfile:
        for zip_file in args.collected_notice_zipfile:
            build_utils.extract_all(zip_file, notice_dir, no_clobber=False)
    else:
        depfiles += build_utils.get_all_files(notice_dir)
    # Copy notice of static targets to notice_root_dir
    copy_static_library_notices(args, depfiles)

    zipfiles = glob.glob('{}/**/*.zip'.format(notice_dir), recursive=True)

    txt_files = glob.glob('{}/**/*.txt'.format(notice_dir), recursive=True)
    txt_files += glob.glob('{}/**/*.txt.?'.format(notice_dir), recursive=True)

    outputs = [args.output_notice_txt, args.output_notice_gz]
    if args.notice_module_info:
        outputs.append(args.notice_module_info)
    build_utils.call_and_write_depfile_if_stale(
        lambda: do_merge_notice(args, zipfiles, txt_files),
        args,
        depfile_deps=depfiles,
        input_paths=depfiles,
        input_strings=args.notice_title + args.target_cpu,
        output_paths=(outputs))


def do_merge_notice(args, zipfiles, txt_files):
    notice_dir = args.notice_root_dir
    notice_txt = args.output_notice_txt
    notice_gz = args.output_notice_gz
    notice_title = args.notice_title

    if not notice_txt.endswith('.txt'):
        raise Exception(
            'Error: input variable output_notice_txt must ends with .txt')
    if not notice_gz.endswith('.xml.gz'):
        raise Exception(
            'Error: input variable output_notice_gz must ends with .xml.gz')

    notice_xml = notice_gz.replace('.gz', '')

    files_with_same_hash = defaultdict(list)
    for file in zipfiles:
        txt_files.append(handle_zipfile_notices(file))

    for file in txt_files:
        if os.stat(file).st_size == 0:
            continue
        file_hash = compute_hash(file)
        files_with_same_hash[file_hash].append(file)

    file_sets = [
        sorted(files_with_same_hash[hash])
        for hash in sorted(files_with_same_hash.keys())
    ]

    if file_sets is not None:
        generate_txt_notice_files(file_sets, notice_dir, notice_txt,
                                  notice_title)

    if files_with_same_hash is not None:
        generate_xml_notice_files(files_with_same_hash, notice_dir, notice_xml)
        compress_file_to_gz(notice_xml, args.output_notice_gz)

    if args.notice_module_info:
        module_install_info_list = []
        module_install_info = {}
        module_install_info['type'] = 'notice'
        module_install_info['source'] = args.output_notice_txt
        module_install_info['install_enable'] = True
        module_install_info['dest'] = [
            os.path.join(args.notice_install_dir,
                         os.path.basename(args.output_notice_txt))
        ]
        module_install_info_list.append(module_install_info)
        write_json_file(args.notice_module_info, module_install_info_list)


if __name__ == "__main__":
    main()
