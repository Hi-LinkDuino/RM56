#!/usr/bin/env python
# -*- coding: utf-8 -*-

#
# Copyright (c) 2020 Huawei Device Co., Ltd.
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
#

from __future__ import print_function
from __future__ import unicode_literals
from collections import defaultdict

from prompt_toolkit.application import Application
from prompt_toolkit.key_binding.manager import KeyBindingManager
from prompt_toolkit.keys import Keys
from prompt_toolkit.layout.containers import Window
from prompt_toolkit.filters import IsDone
from prompt_toolkit.layout.controls import TokenListControl
from prompt_toolkit.layout.containers import ConditionalContainer
from prompt_toolkit.layout.containers import ScrollOffsets
from prompt_toolkit.layout.containers import HSplit
from prompt_toolkit.layout.dimension import LayoutDimension as D
from prompt_toolkit.token import Token

from hb_internal.cts.common import get_style
from hb_internal.cts.common import if_mousedown
from hb_internal.cts.common import select_node
from hb_internal.cts.common import deselect_node
from hb_internal.cts.common import Separator


class InquirerControl(TokenListControl):
    def __init__(self, choices, **kwargs):
        self.pointer_index = 0
        self.selected_options = []  # list of names
        self.answered = False
        self._init_choices(choices)
        self.deps = kwargs.pop('deps')
        self.nodes_from = defaultdict(list)
        super(InquirerControl, self).__init__(self._get_choice_tokens,
                                              **kwargs)

    def _init_choices(self, choices):
        # helper to convert from question format to internal format
        self.choices = []  # list (name, value)
        searching_first_choice = True
        for index, choice in enumerate(choices):
            if isinstance(choice, Separator):
                self.choices.append(choice)
            else:
                name = choice['name']
                value = choice.get('value', name)
                disabled = choice.get('disabled', None)
                if 'checked' in choice and choice['checked'] and not disabled:
                    self.selected_options.append(choice['name'])
                self.choices.append((name, value, disabled))
                if searching_first_choice and not disabled:
                    self.pointer_index = index
                    searching_first_choice = False

    @property
    def choice_count(self):
        return len(self.choices)

    def _get_choice_tokens(self, cli):
        tokens = []
        token = Token

        def append(index, line):
            if isinstance(line, Separator):
                tokens.append((token.Separator, '  %s\n' % line))
            else:
                line_name = line[0]
                line_value = line[1]
                selected = (line_value in self.selected_options)
                pointed_at = (index == self.pointer_index)

                @if_mousedown
                def select_item(cli, mouse_event):
                    # bind option with this index to mouse event
                    if line_value in self.selected_options:
                        deselect_node(line_value, self.selected_options,
                                      self.nodes_from, self.deps)
                    else:
                        select_node(line_value, self.selected_options,
                                    self.nodes_from, self.deps)

                if pointed_at:
                    tokens.append((token.Pointer, '    \u276f', select_item))
                else:
                    tokens.append((token, '     ', select_item))
                # 'o ' - FISHEYE
                if choice[2]:  # disabled
                    tokens.append((token, '- %s (%s)' %
                                  (choice[0], choice[2])))
                else:
                    if selected:
                        tokens.append((token.Selected, '\u25cf ', select_item))
                    else:
                        tokens.append((token, '\u25cb ', select_item))

                    if pointed_at:
                        tokens.append((Token.SetCursorPosition, ''))

                    tokens.append((token, line_name, select_item))
                tokens.append((token, '\n'))

        # prepare the select choices
        for i, choice in enumerate(self.choices):
            append(i, choice)
        tokens.pop()  # Remove last newline.
        return tokens

    def get_selected_values(self):
        # get values not labels
        return [c[1] for c in self.choices if not isinstance(c, Separator) and
                c[1] in self.selected_options]

    @property
    def line_count(self):
        return len(self.choices)


def question(message, **kwargs):
    if 'default' in kwargs:
        raise ValueError('Checkbox does not implement \'default\' '
                         'use \'checked\':True\' in choice!')

    deps = kwargs.pop('deps')
    choices = kwargs.pop('choices', None)
    style = kwargs.pop('style', get_style('terminal'))

    inquirer_control = kwargs.pop('inquirer_control', None)
    if inquirer_control is None:
        inquirer_control = InquirerControl(choices, deps=deps)

    qmark = kwargs.pop('qmark', '?')

    def get_prompt_tokens(cli):
        tokens = []

        tokens.append((Token.QuestionMark, qmark))
        tokens.append((Token.Question, ' %s ' % message))
        if inquirer_control.answered:
            nbr_selected = len(inquirer_control.selected_options)
            if nbr_selected == 0:
                tokens.append((Token.Answer, ' done'))
            elif nbr_selected == 1:
                tokens.append((Token.Answer, ' [%s]' %
                               inquirer_control.selected_options[0]))
            else:
                tokens.append((Token.Answer,
                               ' done (%d selections)' % nbr_selected))
        else:
            tokens.append((Token.Instruction,
                           ' (<up>, <down> to move, <space> to select, <a> '
                           'to toggle, <i> to invert)'))
        return tokens

    # assemble layout
    layout = HSplit([
        Window(height=D.exact(1),
               content=TokenListControl(get_prompt_tokens,
                                        align_center=False)),
        ConditionalContainer(
            Window(
                inquirer_control,
                width=D.exact(43),
                height=D(min=3),
                scroll_offsets=ScrollOffsets(top=1, bottom=1)
            ),
            filter=~IsDone()
        )
    ])

    # key bindings
    manager = KeyBindingManager.for_prompt()

    @manager.registry.add_binding(Keys.ControlQ, eager=True)
    @manager.registry.add_binding(Keys.ControlC, eager=True)
    def _(event):
        raise KeyboardInterrupt()

    @manager.registry.add_binding(' ', eager=True)
    def toggle(event):
        pointer_index = inquirer_control.pointer_index
        pointed_choice = inquirer_control.choices[pointer_index][1]  # value
        if pointed_choice in inquirer_control.selected_options:
            deselect_node(pointed_choice, inquirer_control.selected_options,
                          inquirer_control.nodes_from, deps)
        else:
            select_node(pointed_choice, inquirer_control.selected_options,
                        inquirer_control.nodes_from, deps)

    @manager.registry.add_binding('i', eager=True)
    def invert(event):
        inverted_selection = [c[1] for c in inquirer_control.choices if
                              not isinstance(c, Separator) and
                              c[1] not in inquirer_control.selected_options and
                              not c[2]]
        inquirer_control.selected_options = inverted_selection

    @manager.registry.add_binding('a', eager=True)
    def select_all(event):
        all_selected = True  # all choices have been selected
        for choice in inquirer_control.choices:
            if not isinstance(choice, Separator) and \
                    choice[1] not in inquirer_control.selected_options and \
                    not choice[2]:
                # add missing ones
                inquirer_control.selected_options.append(choice[1])
                all_selected = False
        if all_selected:
            inquirer_control.selected_options = []

    @manager.registry.add_binding(Keys.Down, eager=True)
    def move_cursor_down(event):
        def _next():
            inquirer_control.pointer_index = \
                ((inquirer_control.pointer_index + 1) %
                 inquirer_control.line_count)
        _next()
        while isinstance(inquirer_control.choices[
            inquirer_control.pointer_index], Separator) or \
                inquirer_control.choices[inquirer_control.pointer_index][2]:
            _next()

    @manager.registry.add_binding(Keys.Up, eager=True)
    def move_cursor_up(event):
        def _prev():
            inquirer_control.pointer_index = \
                ((inquirer_control.pointer_index - 1) %
                 inquirer_control.line_count)
        _prev()
        while isinstance(inquirer_control.choices[
            inquirer_control.pointer_index], Separator) or \
                inquirer_control.choices[inquirer_control.pointer_index][2]:
            _prev()

    @manager.registry.add_binding(Keys.Enter, eager=True)
    def set_answer(event):
        inquirer_control.answered = True
        event.cli.set_return_value(inquirer_control)

    return Application(
        layout=layout,
        key_bindings_registry=manager.registry,
        mouse_support=True,
        style=style
    )
