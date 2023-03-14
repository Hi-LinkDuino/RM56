/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "power_state_token_clnt.h"
#include "osal_mem.h"

static void PowerStateTokenClntConstruct(struct PowerStateTokenClnt *clnt, struct IPowerStateToken *tokenIf)
{
    clnt->tokenIf = tokenIf;
    clnt->powerState = PSM_STATE_INACTIVE;
}

struct PowerStateTokenClnt *PowerStateTokenClntNewInstance(struct IPowerStateToken *tokenIf)
{
    struct PowerStateTokenClnt *tokenClnt =
        (struct PowerStateTokenClnt *)OsalMemCalloc(sizeof(struct PowerStateTokenClnt));
    if (tokenClnt != NULL) {
        PowerStateTokenClntConstruct(tokenClnt, tokenIf);
        return tokenClnt;
    }
    return tokenClnt;
}

void PowerStateTokenClntFreeInstance(struct PowerStateTokenClnt *tokenClnt)
{
    if (tokenClnt != NULL) {
        OsalMemFree(tokenClnt);
    }
}
