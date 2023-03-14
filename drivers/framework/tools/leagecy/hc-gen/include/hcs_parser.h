/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HCS_COMPILER_PARSER_H
#define HCS_COMPILER_PARSER_H

#include <stdio.h>
#include "hcs_types.h"

#define OBJECT_NAME_MAX_LEN 100

enum ParserObjectType {
    PARSEROP_UINT8 = 0x01,
    PARSEROP_UINT16,
    PARSEROP_UINT32,
    PARSEROP_UINT64,
    PARSEROP_STRING,
    PARSEROP_CONFNODE,
    PARSEROP_CONFTERM,
    PARSEROP_ARRAY,
    PARSEROP_NODEREF,
    PARSEROP_DELETE,
    PARSEROP_COUNT,
};

enum NodeRefType {
    CONFIG_NODE_NOREF = 0,
    CONFIG_NODE_COPY,
    CONFIG_NODE_REF,
    CONFIG_NODE_DELETE,
    CONFIG_NODE_TEMPLATE,
    CONFIG_NODE_INHERIT,
    CONFIG_NODE_TYPE_COUNT
};
enum ParserObjectStatus {
    PARSER_OBJ_IDLE = 0,
    PARSER_OBJ_PROCESSING,
};
struct ParserObjectBase;
typedef struct ParserObjectBase ParserObjectBase;

#define PARSER_OBJECT_COMMON   \
    uint32_t type;             \
    uint32_t status;           \
    const char *name;          \
    ParserObjectBase *parent;  \
    ParserObjectBase *next;    \
    ParserObjectBase *child;   \
    uint32_t lineno;           \
    const char *src;           \
    uint8_t opCode;            \
    uint32_t size;             \
    uint32_t subSize;          \
    uint32_t hash;             \
    union {                    \
        uint64_t value;        \
        uint64_t integerValue; \
        char *stringValue;     \
    };

struct ParserObjectBase {
    PARSER_OBJECT_COMMON;
};

typedef struct TemplateNodeInstance {
    struct ConfigNode *nodeObject;
    struct TemplateNodeInstance *next;
} TemplateNodeInstance;

typedef struct ConfigNode {
    PARSER_OBJECT_COMMON;
    const char *refNodePath;
    uint32_t nodeType;
    struct ConfigNode *inheritNode;
    uint32_t inheritIndex;
    uint32_t inheritCount;
    uint32_t templateSignNum;
    TemplateNodeInstance *subClasses;
} ConfigNode;

typedef struct {
    PARSER_OBJECT_COMMON;
    uint32_t flag; // for array or other assembly type
    uint32_t signNum;
} ConfigTerm;

typedef union {
    ParserObjectBase objectBase;
    ConfigNode configNode;
    ConfigTerm configTerm;
} ParserObject;

enum AstWalkDirection {
    AST_WALK_FORWARD = 0x01,
    AST_WALK_BACKEND,
    AST_WALK_ROUND,
};

enum AstCopyOverWriteFlag {
    AST_COPY_SRC_OVER_DST = 0,  // source term overwrite destination terms
    AST_COPY_DST_OVER_SRC,      // destination term overwrite source terms
};

/* lexer and parser auto-gen api start */
void HcsParserRestart(FILE *inputFile);

int32_t HcsCompilerparse(void);

/* lexer and parser auto-gen api end */
bool HcsIsAnonymousObject(const ParserObject *obj);

int32_t HcsProcessInclude(char *includePath, uint32_t lineNumber);

bool HcsIsSameTypeObject(const ParserObject *cs, const ParserObject *ct);

bool HcsIsNumberObject(const ParserObject *obj);

bool HcsIsStringObject(const ParserObject *obj);

const char *HcsParserNodeTypeToStr(uint32_t nodeType);

void HcsAstFreeObject(ParserObject *object);

int32_t HcsGetCurrentSourceLine(void);

void HcsSetCurrentSourceLine(int32_t lineNumber);

uint32_t HcsAlign(uint32_t size);

#endif // HCS_COMPILER_PARSER_H
