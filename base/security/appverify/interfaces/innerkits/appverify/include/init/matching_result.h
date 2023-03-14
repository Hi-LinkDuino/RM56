/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#ifndef HAP_MATCHING_RESULT_H
#define HAP_MATCHING_RESULT_H

namespace OHOS {
namespace Security {
namespace Verify {
enum TrustedSources {
    OTHER_TRUSTED_SOURCE = 0,
    APP_GALLARY,
    APP_SYSTEM,
    APP_THIRD_PARTY_PRELOAD,
};

enum MatchingStates {
    DO_NOT_MATCH = 0,
    MATCH_WITH_SIGN,
    MATCH_WITH_PROFILE,
    MATCH_WITH_PROFILE_DEBUG,
    MATCH_WITH_TICKET,
};

struct MatchingResult {
    MatchingStates matchState;
    TrustedSources source;
};
} // namespace Verify
} // namespace Security
} // namespace OHOS
#endif // HAP_MATCHING_RESULT_H
