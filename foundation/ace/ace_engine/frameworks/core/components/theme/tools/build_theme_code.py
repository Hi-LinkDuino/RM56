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

import sys
import os

theme_list = {}


def read_row_value(row, param, offset, platform):
    if row[offset] == '':
        return
    item = {}
    item['.type'] = row[offset]
    item['.value'] = row[offset + 1]
    if item['.type'] == 'ThemeConstantsType::DOUBLE':
        if item['.value'].find('.') == -1:
            item['.value'] = item['.value'] + '.0'
    if row[offset + 2] != '':
        item['.isPublic'] = (row[offset + 2]).lower()
    if row[offset + 3] != '':
        item['.blendAlpha'] = row[offset + 3]
    param[platform] = item


def spitrow(row):
    values = []
    while True:
        if len(row) == 0:
            values.append(row)
            break
        pos = -1
        if row[0] == '\"':
            pos = row.find('\"', 1)
            if pos == -1:
                break
            pos = row.find(',', pos + 1)
        else:
            pos = row.find(',')
        value = ''
        if pos != -1:
            value = row[:pos]
            row = row[pos+1:]
        else:
            value = row

        if len(value) > 6 and value[0:3] == '\"\"\"' and \
                value[-3:] == '\"\"\"':
            values.append(value[2:-2])
        else:
            if len(value) > 2 and value[0] == '\"' and value[-1] == '\"':
                values.append(value[1:-1])
            else:
                values.append(value)
        if pos == -1:
            break
    return values


def read_row(row):
    if row[-1] == '\n':
        row = row[:-1]
        values = spitrow(row)
    param = {}
    if len(values) != 16:
        print('error')
        print(row)
        return

    if values[1] != '':
        param['base'] = values[1]
    if values[2] != '':
        param['offset'] = int(values[2])
    param['value'] = int(values[3])
    read_row_value(values, param, 4, 'default')
    read_row_value(values, param, 8, 'tv')
    read_row_value(values, param, 12, 'watch')
    theme_list[values[0]] = param


def read_file(file_path):
    with open(file_path) as file_read:
        content = file_read.read()
    return content


def write_file(file_path, content):
    with open(file_path, 'wb+') as file_write:
        file_write.write(content.encode())


def build_header(file_path):
    define = 'FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS'\
        '_THEME_THEME_CONSTANTS_DEFINES_H'
    begin_str = '/*\n'\
        ' * Copyright (c) 2021 Huawei Device Co., Ltd.\n'\
        ' * Licensed under the Apache License, Version 2.0 (the "License");\n'\
        ' * you may not use this file except in compliance with the License.\n'\
        ' * You may obtain a copy of the License at\n'\
        ' *\n'\
        ' *     http://www.apache.org/licenses/LICENSE-2.0\n'\
        ' *\n'\
        ' * Unless required by applicable law or agreed to in writing, software\n'\
        ' * distributed under the License is distributed on an "AS IS" BASIS,\n'\
        ' * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n'\
        ' * See the License for the specific language governing permissions and\n'\
        ' * limitations under the License.\n'\
        ' */\n'\
        '\n'\
        '#ifndef {}\n'\
        '#define {}\n'\
        '\n'\
        'namespace OHOS::Ace {{\n'\
        '\n'

    new_content = begin_str.format(define, define)
    for key, values in theme_list.items():
        row = 'constexpr uint32_t {} = '.format(key)
        has_base = False
        if 'base' in values:
            row = ''.join((row, values['base']))
            has_base = True
        if 'offset' in values:
            if has_base == True:
                row = ''.join((row, ' + '))
            row = ''.join((row, str(values['offset'])))
        row = ''.join((row, ';\n'))
        new_content = ''.join((new_content, row))
    end_str = '{}\n}} // namespace OHOS::Ace\n\n'\
        '#endif // {}'
    write_file(file_path, end_str.format(new_content, define))


def get_global_name(name):
    offset = 0
    char = name.split('_')
    ret = ''
    for i in range(0, len(char)):
        if i == 0:
            ret = ''.join((ret, char[i]))
        else:
            ret = ''.join((ret, char[i].capitalize()))
    return ret


def make_table(param_map, platform):
    content = 'const ResValueWrapper* ThemeConstants::styleMap{}[] = '.format(
        platform.capitalize())

    table = {}
    maxindex = 0
    for key, values in param_map.items():
        if maxindex < int(key):
            maxindex = int(key)

    content = ''.join((content, '{'))
    for i in range(0, maxindex + 1):
        if str(i) in param_map:
            content = '{} &{},'.format(content, param_map[str(i)])
        else:
            content = ''.join((content, ' nullptr,'))
    content = content[0:len(content) - 1]
    content = ''.join((content, '};\n'))
    return content


def build_cpp(file_path, platform):
    begin_str = '/*\n'\
        ' * Copyright (c) 2021 Huawei Device Co., Ltd.\n'\
        ' * Licensed under the Apache License, Version 2.0 (the "License");\n'\
        ' * you may not use this file except in compliance with the License.\n'\
        ' * You may obtain a copy of the License at\n'\
        ' *\n'\
        ' *     http://www.apache.org/licenses/LICENSE-2.0\n'\
        ' *\n'\
        ' * Unless required by applicable law or agreed to in writing, software\n'\
        ' * distributed under the License is distributed on an "AS IS" BASIS,\n'\
        ' * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n'\
        ' * See the License for the specific language governing permissions and\n'\
        ' * limitations under the License.\n'\
        ' */\n'\
        '\n'\
        '#include \"core/components/theme/theme_constants.h\"\n'\
        '#include \"core/components/theme/theme_constants_defines.h\"\n'\
        '\n'\
        '#include \"core/components/common/properties/scroll_bar.h\"\n'\
        '\n'\
        'namespace OHOS::Ace {\n'
    content = begin_str
    table = {}
    for key, values in theme_list.items():
        if platform in values:
            name = 'g_{}{}'.format(get_global_name(
                key.lower()), platform.capitalize())
            value = 'const ResValueWrapper {} {{'.format(name)
            table[str(values['value'])] = name
            for key1, values1 in values[platform].items():
                value = '{} {} = {},'.format(value, key1, values1)
            value = value[0: len(value) - 1]
            value = ''.join((value, ' };\n'))
            content = ''.join((content, value))
    content = '{}\n'\
              '{}\n'\
              'uint32_t ThemeConstants::{}MapCount = sizeof(styleMap{}) / sizeof(styleMap{}[0]);\n'\
              '\n}} // namespace OHOS::Ace'.format(content, make_table(table, platform),
                platform.capitalize(), platform.capitalize(), platform.capitalize())
    file_name = '{}theme_constants_{}.cpp'.format(file_path, platform)
    write_file(file_name, content)


def read_file_lines(file_path):
    with open(file_path) as file_read:
        content = file_read.readlines()
    return content


def main():
    if len(sys.argv) != 3:
        print('error, missing required parameters!')
        print('\tbuild_theme_code theme.xls theme/')
        return 1

    folder = os.path.exists(sys.argv[2])
    if not folder:
        os.makedirs(sys.argv[2])

    cvs = read_file_lines(sys.argv[1])

    for i in range(len(cvs)):
        if i == 0:
            continue
        read_row(cvs[i])

    dist_path = sys.argv[2]

    build_header(''.join((dist_path, 'theme_constants_defines.h')))
    build_cpp(dist_path, 'tv')
    build_cpp(dist_path, 'default')
    build_cpp(dist_path, 'watch')

if __name__ == '__main__':
    main()
