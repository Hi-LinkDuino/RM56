/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HCS_COMPILER_AST_H
#define HCS_COMPILER_AST_H

#include "hcs_parser.h"

typedef int32_t (*AstWalkCallBack)(ParserObject *current, int32_t walkDepth);

ParserObject *HcsGetParserRoot(void);

void HcsSetParserRoot(ParserObject *newRoot);

ParserObject *HcsNewConfigNode(const char *nodeName, uint32_t nodeType, const char *refName);

ParserObject *HcsNewConfigTerm(const char *name, ParserObject *value);

ParserObject *HcsNewParserObject(const char *name, uint32_t type, uint64_t value);

ParserObject *HcsAstAddChild(ParserObject *parent, ParserObject *child);

void HcsAstRemoveChildLink(ParserObjectBase *parent, ParserObjectBase *child);

ParserObject *HcsAstAddPeer(ParserObject *forward, ParserObject *peer);

int32_t HcsWalkAst(ParserObject *start, uint32_t walkDirection, AstWalkCallBack forwardCallback,
    AstWalkCallBack backwardCallback);

void HcsDumpAst(const char *label);

ParserObject *HcsAstLookupObjectInPeers(const ParserObject *current, const char *name);

ParserObject *HcsAstLookupObjectInChildren(const ParserObject *current, const char *name);

int32_t HcsAstCopySubTree(ParserObject *src, ParserObject *dst, uint32_t overWriteFlag);

int32_t HcsAstCopyArray(const ParserObject *src, ParserObject *dst);

void HcsDeleteParserObjectTree(ParserObject *object);

uint16_t HcsCountArraySize(const ParserObject *arrayObj);

uint16_t HcsGetTermType(ParserObject *object);

const char *HcsGetModuleName(void);

#endif // HCS_COMPILER_AST_H
