/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "sysversion.h"

/* *
 * Major(M) version number.
 */
static int g_majorVersion = 2;

/* *
 * Senior(S) version number.
 */
static int g_seniorVersion = 2;

/* *
 * Feature(F) version number.
 */
static int g_featureVersion = 0;

/* *
 * Build(B) version number.
 */
static int g_buildVersion = 0;

/* *
 * Obtains the major (M) version number, which increases with any updates to the overall architecture.
 * <p>The M version number monotonically increases from 1 to 99.
 *
 * @return Returns the M version number.
 * @since 4
 */
int GetMajorVersion()
{
    return g_majorVersion;
}

/* *
 * Obtains the senior (S) version number, which increases with any updates to the partial
 * architecture or major features.
 * <p>The S version number monotonically increases from 0 to 99.
 *
 * @return Returns the S version number.
 * @since 4
 */
int GetSeniorVersion()
{
    return g_seniorVersion;
}

/* *
 * Obtains the feature (F) version number, which increases with any planned new features.
 * <p>The F version number monotonically increases from 0 or 1 to 99.
 *
 * @return Returns the F version number.
 * @since 3
 */
int GetFeatureVersion()
{
    return g_featureVersion;
}

/* *
 * Obtains the build (B) version number, which increases with each new development build.
 * <p>The B version number monotonically increases from 0 or 1 to 999.
 *
 * @return Returns the B version number.
 * @since 3
 */
int GetBuildVersion()
{
    return g_buildVersion;
}