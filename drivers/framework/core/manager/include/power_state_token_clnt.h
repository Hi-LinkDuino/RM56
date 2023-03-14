/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */


#ifndef POWER_STATE_TOKEN_CLNT_H
#define POWER_STATE_TOKEN_CLNT_H

#include "hdf_slist.h"
#include "power_state_token_if.h"

struct PowerStateTokenClnt {
    struct HdfSListNode entry;
    HdfPsmState powerState;
    struct IPowerStateToken *tokenIf;
};

struct PowerStateTokenClnt *PowerStateTokenClntNewInstance(struct IPowerStateToken *tokenIf);
void PowerStateTokenClntFreeInstance(struct PowerStateTokenClnt *tokenClnt);

#endif /* POWER_STATE_TOKEN_CLNT_H */