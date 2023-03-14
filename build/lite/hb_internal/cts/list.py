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
import sys
from prompt_toolkit.application import Application
from prompt_toolkit.key_binding.manager import KeyBindingManager
from prompt_toolkit.keys import Keys
from prompt_toolkit.layout.containers import Window
from prompt_toolkit.filters import IsDone
from prompt_toolkit.layout.controls import TokenListControl
from prompt_toolkit.layout.containers import ConditionalContainer
from prompt_toolkit.layout.containers import HSplit
from prompt_toolkit.layout.dimension import LayoutDimension as D
from prompt_toolkit.token import Token
from hb_internal.cts.common import Separator
from hb_internal.cts.common import if_mousedown
from hb_internal.cts.common import get_style
from hb_internal.common.utils import OHOSException


class InquirerControl(TokenListControl):
    def __init__(self, choices, **kwargs):
        self.selected_option_index = 0
        self.answered = False
        self.choices = choices
        self._init_choices(choices)
        super(InquirerControl, self).__init__(self._get_choice_tokens,
                                              **kwargs)

    def _init_choices(self, choices, default=None):
        # helper to convert from question format to internal format
        self.choices = []  # list (name, value, disabled)
        searching_first_choice = True
        for index, choice in enumerate(choices):
            if isinstance(choice, Separator):
                self.choices.append((choice, None, None))
            else:
                base_string = str if sys.version_info[0] >= 3 else None
                if isinstance(choice, base_string):
                    self.choices.append((choice, choice, None))
                else:
                    name = choice.get('name')
                    value = choice.get('value', name)
                    disabled = choice.get('disabled', None)
                    self.choices.append((name, value, disabled))
                if searching_first_choice:
                    self.selected_option_index = index
                    searching_first_choice = False

    @property
    def choice_count(self):
        return len(self.choices)

    def _get_choice_tokens(self, cli):
        tokens = []
        token = Token

        def append(index, choice):
            selected = (index == self.selected_option_index)

            @if_mousedown
            def select_item(cli, mouse_event):
                # bind option with this index to mouse event
                self.selected_option_index = index
                self.answered = True

            tokens.append((token.Pointer if selected else token, ' \u276f '
                          if selected else '   '))
            if selected:
                tokens.append((Token.SetCursorPosition, ''))
            if choice[2]:  # disabled
                tokens.append((token.Selected if selected else token,
                               '- %s (%s)' % (choice[0], choice[2])))
            else:
                if isinstance(choice[0], Separator):
                    tokens.append((token.Separator,
                                  str(choice[0]),
                                  select_item))
                else:
                    try:
                        tokens.append((token.Selected if selected else token,
                                      str(choice[0]), select_item))
                    except Exception:
                        tokens.append((token.Selected if selected else
                                      token, choice[0], select_item))
            tokens.append((token, '\n'))

        # prepare the select choices
        for i, choice in enumerate(self.choices):
            append(i, choice)
        tokens.pop()  # Remove last newline.
        return tokens

    def get_selection(self):
        return self.choices[self.selected_option_index]


def question(message, **kwargs):
    if 'choices' not in kwargs:
        raise OHOSException("You must choose one platform.")

    choices = kwargs.pop('choices', None)
    qmark = kwargs.pop('qmark', '?')
    style = kwargs.pop('style', get_style('terminal'))

    inquirer_control = InquirerControl(choices)

    def get_prompt_tokens(cli):
        tokens = []

        tokens.append((Token.QuestionMark, qmark))
        tokens.append((Token.Question, ' %s ' % message))
        if inquirer_control.answered:
            tokens.append((Token.Answer, ' ' +
                          inquirer_control.get_selection()[0]))
        else:
            tokens.append((Token.Instruction, ' (Use arrow keys)'))
        return tokens

    # assemble layout
    layout = HSplit([
        Window(height=D.exact(1),
               content=TokenListControl(get_prompt_tokens)),
        ConditionalContainer(
            Window(inquirer_control),
            filter=~IsDone()
        )
    ])

    # key bindings
    manager = KeyBindingManager.for_prompt()

    @manager.registry.add_binding(Keys.ControlQ, eager=True)
    @manager.registry.add_binding(Keys.ControlC, eager=True)
    def _(event):
        raise KeyboardInterrupt()

    @manager.registry.add_binding(Keys.Down, eager=True)
    def move_cursor_down(event):
        def _next():
            inquirer_control.selected_option_index = (
                (inquirer_control.selected_option_index + 1) %
                inquirer_control.choice_count)
        _next()
        while isinstance(inquirer_control.choices[
            inquirer_control.selected_option_index][0], Separator) \
                or inquirer_control.choices[
                    inquirer_control.selected_option_index][2]:
            _next()

    @manager.registry.add_binding(Keys.Up, eager=True)
    def move_cursor_up(event):
        def _prev():
            inquirer_control.selected_option_index = (
                (inquirer_control.selected_option_index - 1) %
                inquirer_control.choice_count)
        _prev()
        while isinstance(inquirer_control.choices[
            inquirer_control.selected_option_index][0], Separator) \
                or inquirer_control.choices[
                    inquirer_control.selected_option_index][2]:
            _prev()

    @manager.registry.add_binding(Keys.Enter, eager=True)
    def set_answer(event):
        inquirer_control.answered = True
        event.cli.set_return_value(inquirer_control.get_selection())

    return Application(
        layout=layout,
        key_bindings_registry=manager.registry,
        mouse_support=True,
        style=style
    )
