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
#include "wifi_log_helper_test.h"
#include "log_helper.h"

using namespace testing::ext;

namespace OHOS {
namespace Wifi {
HWTEST_F(WifiLogHelperTest, EncryptLogMsgTest, TestSize.Level1)
{
    char msg[32] = {0};
    EncryptLogMsg("", msg, sizeof(msg));
    EXPECT_TRUE(msg == std::string(""));
    EncryptLogMsg("a", msg, sizeof(msg));
    EXPECT_TRUE(msg == std::string("*"));
    EncryptLogMsg("ab", msg, sizeof(msg));
    EXPECT_TRUE(msg == std::string("**"));
    EncryptLogMsg("abc", msg, sizeof(msg));
    EXPECT_TRUE(msg == std::string("a*c"));
    EncryptLogMsg("abcd", msg, sizeof(msg));
    EXPECT_TRUE(msg == std::string("a**d"));
    EncryptLogMsg("abcde", msg, sizeof(msg));
    EXPECT_TRUE(msg == std::string("ab*de"));
    EncryptLogMsg("abcdef", msg, sizeof(msg));
    EXPECT_TRUE(msg == std::string("ab**ef"));
    EncryptLogMsg("abcdefg", msg, sizeof(msg));
    EXPECT_TRUE(msg == std::string("abc*efg"));
    EncryptLogMsg("abcdefgh", msg, sizeof(msg));
    EXPECT_TRUE(msg == std::string("abc**fgh"));
    EncryptLogMsg("abcdefghi", msg, sizeof(msg));
    EXPECT_TRUE(msg == std::string("abc***ghi"));
    EncryptLogMsg("abcdefghij", msg, sizeof(msg));
    EXPECT_TRUE(msg == std::string("abc****hij"));
}
}  // namespace Wifi
}  // namespace OHOS