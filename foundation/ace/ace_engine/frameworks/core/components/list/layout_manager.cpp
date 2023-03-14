/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "core/components/list/layout_manager.h"

namespace OHOS::Ace {

constexpr bool DIR_HORIZONTAL = false;
constexpr bool DIR_VERTICAL = true;
constexpr bool DIR_FORWARD = false;
constexpr bool DIR_REVERSE = true;

const std::map<bool, std::map<FlexDirection, std::map<bool, std::map<bool, KeyDirection>>>> DIRECTION_MAP = {
    { false, // RTL is false
        {
            { FlexDirection::ROW,
                {
                    { DIR_HORIZONTAL, { { DIR_FORWARD, KeyDirection::DOWN }, { DIR_REVERSE, KeyDirection::UP } } },
                    { DIR_VERTICAL, { { DIR_FORWARD, KeyDirection::RIGHT }, { DIR_REVERSE, KeyDirection::LEFT } } }
                }
            },
            { FlexDirection::COLUMN,
                {
                    { DIR_HORIZONTAL, { { DIR_FORWARD, KeyDirection::RIGHT }, { DIR_REVERSE, KeyDirection::LEFT } } },
                    { DIR_VERTICAL, { { DIR_FORWARD, KeyDirection::DOWN }, { DIR_REVERSE, KeyDirection::UP } } }
                }
            },
            { FlexDirection::ROW_REVERSE,
                {
                    { DIR_HORIZONTAL, { { DIR_FORWARD, KeyDirection::UP }, { DIR_REVERSE, KeyDirection::DOWN } } },
                    { DIR_VERTICAL, { { DIR_FORWARD, KeyDirection::RIGHT }, { DIR_REVERSE, KeyDirection::LEFT } } }
                }
            },
            { FlexDirection::COLUMN_REVERSE,
                {
                    { DIR_HORIZONTAL, { { DIR_FORWARD, KeyDirection::RIGHT }, { DIR_REVERSE, KeyDirection::LEFT } } },
                    { DIR_VERTICAL, { { DIR_FORWARD, KeyDirection::UP }, { DIR_REVERSE, KeyDirection::DOWN } } }
                }
            }
        }
    },
    { true, // RTL is true
        {
            { FlexDirection::ROW,
                {
                    { DIR_HORIZONTAL, { { DIR_FORWARD, KeyDirection::UP }, { DIR_REVERSE, KeyDirection::DOWN } } },
                    { DIR_VERTICAL, { { DIR_FORWARD, KeyDirection::RIGHT }, { DIR_REVERSE, KeyDirection::LEFT } } }
                }
            },
            { FlexDirection::COLUMN,
                {
                    { DIR_HORIZONTAL, { { DIR_FORWARD, KeyDirection::LEFT }, { DIR_REVERSE, KeyDirection::RIGHT } } },
                    { DIR_VERTICAL, { { DIR_FORWARD, KeyDirection::DOWN }, { DIR_REVERSE, KeyDirection::UP } } }
                }
            },
            { FlexDirection::ROW_REVERSE,
                {
                    { DIR_HORIZONTAL, { { DIR_FORWARD, KeyDirection::UP }, { DIR_REVERSE, KeyDirection::DOWN } } },
                    { DIR_VERTICAL, { { DIR_FORWARD, KeyDirection::RIGHT }, { DIR_REVERSE, KeyDirection::LEFT } } }
                }
            },
            { FlexDirection::COLUMN_REVERSE,
                {
                    { DIR_HORIZONTAL, { { DIR_FORWARD, KeyDirection::LEFT }, { DIR_REVERSE, KeyDirection::RIGHT } } },
                    { DIR_VERTICAL, { { DIR_FORWARD, KeyDirection::UP }, { DIR_REVERSE, KeyDirection::DOWN } } }
                }
            }
        }
    }
};

} // namespace OHOS::Ace
