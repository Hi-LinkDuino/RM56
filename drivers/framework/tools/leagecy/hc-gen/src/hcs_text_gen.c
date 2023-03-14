/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <string.h>
#include <ctype.h>
#include <securec.h>
#include "hcs_ast.h"
#include "hcs_file.h"
#include "hcs_log.h"
#include "hcs_mem.h"
#include "hcs_option.h"
#include "hcs_parser.h"

#define TAB_SIZE 4
#define WRITE_BUFFER_LEN 256
#define VARIABLE_NAME_LEN 128
#define ELEMENT_PER_LINE 16
#define HCS_CONFIG_FILE_HEADER "/*\n"                                                      \
        " * This is an automatically generated HDF config file. Do not modify it manually.\n"                    \
        " */\n\n"

#define HCS_CONFIG_INCLUDE_HEADER "#include <stdint.h>\n\n"

#define DEFAULT_PREFIX "HdfConfig"

#define HCS_CONFIG_FUNC_IMPLEMENT "\nconst struct %s%sRoot* HdfGet%sModuleConfigRoot(void)\n" \
        "{\n"                                                   \
        "    return &%s;\n"                                     \
        "}\n"

static char *g_configRootVariableName = NULL;
static char *g_bigHumpModuleName = NULL;
static const char *g_namePrefix = DEFAULT_PREFIX;
static char *g_littleHumpNamePrefix = NULL;

static void ToCamelString(char *str, uint32_t strLen)
{
    if (!strLen) {
        return;
    }

    if (strchr(str, '_') == NULL) {
        return;
    }

    char *ptr = str;
    char *processed = str;
    uint32_t count = 0;
    while (*ptr && count <= strLen) {
        if (*ptr != '_') {
            *processed++ = *ptr++;
        } else {
            ptr++;
            *ptr = (char)toupper(*ptr);
        }
        count++;
    }
    *processed = '\0';
}

static void ToUpperCamelString(char *str, uint32_t strLen)
{
    if (!strLen) {
        return;
    }

    ToCamelString(str, strLen);
    str[0] = (char)toupper(str[0]);
}

static void ToLowerCamelString(char *str, uint32_t strLen)
{
    if (!strLen) {
        return;
    }

    ToCamelString(str, strLen);
    str[0] = (char)tolower(str[0]);
}

static int32_t InitConfigVariableNames()
{
    if (g_bigHumpModuleName == NULL) {
        const char *moduleName = HcsGetModuleName();
        g_bigHumpModuleName = HcsMemZalloc(strlen(moduleName) + 1);
        if (g_bigHumpModuleName == NULL) {
            return EOOM;
        }
        if (strcpy_s(g_bigHumpModuleName, strlen(moduleName) + 1, moduleName) != EOK) {
            HCS_ERROR("failed to copy string");
            return EFAIL;
        }
        ToUpperCamelString(g_bigHumpModuleName, strlen(g_bigHumpModuleName));
    }
    if (g_littleHumpNamePrefix == NULL) {
        char *littleHumpPrefix = strdup(g_namePrefix);
        if (littleHumpPrefix == NULL) {
            return EOOM;
        }
        ToLowerCamelString(littleHumpPrefix, strlen(littleHumpPrefix));
        g_littleHumpNamePrefix = littleHumpPrefix;
    }
    if (g_configRootVariableName == NULL) {
        const uint32_t nameLen = VARIABLE_NAME_LEN;
        g_configRootVariableName = HcsMemZalloc(nameLen);
        if (g_configRootVariableName == NULL) {
            return EOOM;
        }

        int32_t ret = sprintf_s(g_configRootVariableName, nameLen, "g_%s%sModuleRoot", g_littleHumpNamePrefix,
            g_bigHumpModuleName);
        if (ret <= 0) {
            return EFAIL;
        }
    }

    return NOERR;
}

static void ReleaseConfigVariableNames()
{
    if (g_configRootVariableName != NULL) {
        HcsMemFree(g_configRootVariableName);
        g_configRootVariableName = NULL;
    }

    if (g_bigHumpModuleName != NULL) {
        HcsMemFree(g_bigHumpModuleName);
        g_bigHumpModuleName = NULL;
    }

    if (g_littleHumpNamePrefix != NULL) {
        HcsMemFree(g_littleHumpNamePrefix);
        g_littleHumpNamePrefix = NULL;
    }
}

static const char *GetConfigRootVariableName()
{
    return g_configRootVariableName;
}

typedef struct HcsSymbol {
    char *name;
    const ParserObject *object;
    uint32_t duplicateCount;
    struct HcsSymbol *left;
    struct HcsSymbol *right;
    struct HcsSymbol *parent;
} HcsSymbol;

typedef struct HcsSymbolTable {
    HcsSymbol *symbols;
    uint32_t size;
} HcsSymbolTable;

static HcsSymbolTable *g_hcsSymbolTable = NULL;
static int32_t HcsImplementGenWalkCallBack(ParserObject *current, int32_t walkDepth);
static int32_t HcsImplementCloseBraceGen(ParserObject *current, int32_t walkDepth);

static bool HcsIsInSubClassNode(const ParserObject *object)
{
    while (object != NULL) {
        if (object->configNode.nodeType == CONFIG_NODE_INHERIT) {
            return true;
        }
        object = (ParserObject *)object->objectBase.parent;
    }
    return false;
}

static bool HcsIsInTemplateNode(const ParserObject *object)
{
    while (object != NULL) {
        if (object->configNode.nodeType == CONFIG_NODE_TEMPLATE) {
            return true;
        }
        object = (ParserObject *)object->objectBase.parent;
    }
    return false;
}

static void HcsReleaseSymbol(struct HcsSymbol *s)
{
    if (s == NULL) {
        return;
    }
    if (s->left != NULL) {
        HcsReleaseSymbol(s->left);
        s->left = NULL;
    }
    if (s->right != NULL) {
        HcsReleaseSymbol(s->right);
        s->right = NULL;
    }
    HcsMemFree(s->name);
    HcsMemFree(s);
}

static void HcsSymbolTableRelease()
{
    HcsReleaseSymbol(g_hcsSymbolTable->symbols);
    HcsMemFree(g_hcsSymbolTable);
    g_hcsSymbolTable = NULL;
}

static HcsSymbol *HcsNewSymbol(const char *name, const ParserObject *object)
{
    HcsSymbol *newSymbol = HcsMemZalloc(sizeof(HcsSymbol));
    if (newSymbol == NULL) {
        HCS_ERROR("oom");
        return NULL;
    }
    char *symbolName = strdup(name);
    if (symbolName == NULL) {
        HcsMemFree(newSymbol);
        return NULL;
    }
    newSymbol->name = symbolName;
    newSymbol->object = object;
    newSymbol->duplicateCount = 1;

    return newSymbol;
}
static int32_t HcsSymbolTableInit()
{
    if (g_hcsSymbolTable != NULL) {
        HcsSymbolTableRelease();
    }

    g_hcsSymbolTable = HcsMemZalloc(sizeof(HcsSymbolTable));
    return (g_hcsSymbolTable != NULL) ? NOERR : EOOM;
}

static int32_t HcsSymbolTableReset()
{
    HcsSymbolTableRelease();
    return HcsSymbolTableInit();
}

int32_t HcsSymbolTableAdd(const char *name, const ParserObject *object)
{
    HcsSymbol *symbolRoot = g_hcsSymbolTable->symbols;
    HcsSymbol *newSymbol = HcsNewSymbol(name, object);
    if (newSymbol == NULL) {
        return EOOM;
    }
    int32_t cmpRes;
    HcsSymbol *insert = NULL;
    HcsSymbol *p = symbolRoot;
    while (p != NULL) {
        insert = p;
        cmpRes = strcasecmp(name, p->name);
        if (!cmpRes) {
            HCS_OBJECT_ERROR(object,
                "duplicate node name at %s:%u\n To avoid redefining structures,"
                " not allow duplicate node name at text config mode",
                p->object->objectBase.src, p->object->objectBase.lineno);
            HcsReleaseSymbol(newSymbol);
            return EINVALARG;
        }
        p = cmpRes < 0 ? p->left : p->right;
    }

    if (insert == NULL) {
        g_hcsSymbolTable->symbols = newSymbol;
    } else if (cmpRes < 0) {
        insert->left = newSymbol;
    } else {
        insert->right = newSymbol;
    }

    g_hcsSymbolTable->size++;

    return NOERR;
}

HcsSymbol *HcsSymbolTableFind(const char *name)
{
    HcsSymbol *p = g_hcsSymbolTable->symbols;
    while (p != NULL) {
        int32_t cmpRes = strcasecmp(name, p->name);
        if (cmpRes == 0) {
            return p;
        }
        p = cmpRes < 0 ? p->left : p->right;
    }

    return NULL;
}

const char *g_typeMap[PARSEROP_COUNT] = {
    [PARSEROP_UINT8] = "uint8_t",
    [PARSEROP_UINT16] = "uint16_t",
    [PARSEROP_UINT32] = "uint32_t",
    [PARSEROP_UINT64] = "uint64_t",
    [PARSEROP_STRING] = "const char*",
};

static int32_t GenConfigStructName(const ParserObject *node, char *name, uint32_t nameBuffLen)
{
    char nameBuffer[OBJECT_NAME_MAX_LEN] = {'\0'};
    if (strcpy_s(nameBuffer, OBJECT_NAME_MAX_LEN, HcsGetModuleName()) != EOK) {
        return EOUTPUT;
    }
    ToUpperCamelString(nameBuffer, strlen(nameBuffer));
    int32_t res = sprintf_s(name, nameBuffLen, "%s%s", g_namePrefix, nameBuffer);
    PRINTF_CHECK_AND_RETURN(res);

    if (strcpy_s(nameBuffer, OBJECT_NAME_MAX_LEN, node->configNode.name) != EOK) {
        return EOUTPUT;
    }
    ToUpperCamelString(nameBuffer, strlen(nameBuffer));
    res = sprintf_s(name + strlen(name), nameBuffLen - strlen(name), "%s", nameBuffer);
    PRINTF_CHECK_AND_RETURN(res);

    return NOERR;
}

static int32_t GenConfigArrayName(ParserObject *array, char *name, uint32_t nameBuffLen)
{
    char buffer[OBJECT_NAME_MAX_LEN] = {'\0'};

    if (strcpy_s(buffer, sizeof(buffer), array->objectBase.name) != EOK) {
        HCS_ERROR("%s: failed to copy string", __func__);
        return EOUTPUT;
    }
    ToUpperCamelString(buffer, strlen(buffer));

    int32_t ret = sprintf_s(name, nameBuffLen, "g_hcsConfigArray%s", buffer);
    PRINTF_CHECK_AND_RETURN(ret);

    HcsSymbol *symbol = HcsSymbolTableFind(name);
    if (symbol == NULL) {
        if (HcsSymbolTableAdd(name, array)) {
            return EFAIL;
        }
        array->configTerm.signNum = 1;
    } else if (!array->configTerm.signNum) {
        array->configTerm.signNum = symbol->duplicateCount + 1;
        symbol->duplicateCount++;
    }

    ret = sprintf_s(name + strlen(name), nameBuffLen - strlen(name), "%d", array->configTerm.signNum);
    PRINTF_CHECK_AND_RETURN(ret);
    return NOERR;
}

static const char *GetArrayType(const ParserObject *array)
{
    uint32_t type = PARSEROP_UINT8;
    ParserObject *element = (ParserObject *)array->objectBase.child;
    while (element != NULL) {
        type = element->objectBase.type > type ? element->objectBase.type : type;
        element = (ParserObject *)element->objectBase.next;
    }

    return g_typeMap[type];
}

static int32_t GetArraySize(const ParserObject *array)
{
    uint32_t size = 0;
    ParserObjectBase *element = array->objectBase.child;
    while (element != NULL) {
        size++;
        element = element->next;
    }
    return size;
}

static int32_t HcsPrintTermDefinition(const ParserObject *object)
{
    ParserObject *termContext = (ParserObject *)object->objectBase.child;
    int32_t res = NOERR;
    switch (termContext->objectBase.type) {
        case PARSEROP_ARRAY:
            if (HcsIsInTemplateNode(object)) {
                res = HcsFormatOutputWrite("const %s *%s;\n", GetArrayType(termContext), object->configTerm.name);
                OUTPUT_CHECK_AND_RETURN(res);
                res = HcsFormatOutputWrite("%*cuint32_t %sSize;\n", TAB_SIZE, ' ', object->configTerm.name);
            } else {
                res = HcsFormatOutputWrite("%s %s[%d];\n", GetArrayType(termContext), object->configTerm.name,
                    GetArraySize(termContext));
            }
            break;
        case PARSEROP_UINT8:
        case PARSEROP_UINT16:
        case PARSEROP_UINT32:
        case PARSEROP_UINT64:
        case PARSEROP_STRING:
            res = HcsFormatOutputWrite("%s %s;\n", g_typeMap[termContext->objectBase.type], object->configTerm.name);
            break;
        case PARSEROP_NODEREF: {
            char refType[OBJECT_NAME_MAX_LEN] = {'\0'};
            res = GenConfigStructName((ParserObject *)termContext->objectBase.value, refType, OBJECT_NAME_MAX_LEN - 1);
            if (res) {
                return res;
            }
            res = HcsFormatOutputWrite("const struct %s* %s;\n", refType, object->configTerm.name);
        } break;
        default:
            break;
    }

    return res;
}

static int32_t HcsObjectDefinitionGen(const ParserObject *current)
{
    if (current->objectBase.type != PARSEROP_CONFNODE && current->objectBase.type != PARSEROP_CONFTERM) {
        return NOERR;
    }

    int res = NOERR;
    OUTPUT_CHECK_AND_RETURN(HcsFormatOutputWrite("%*c", TAB_SIZE, ' '));
    switch (current->objectBase.type) {
        case PARSEROP_CONFNODE: {
            char structName[OBJECT_NAME_MAX_LEN] = {'\0'};
            res = GenConfigStructName(current, structName, OBJECT_NAME_MAX_LEN - 1);
            if (res) {
                return res;
            }
            if (current->configNode.nodeType == CONFIG_NODE_TEMPLATE) {
                char nodeName[OBJECT_NAME_MAX_LEN] = {0};
                if (strcpy_s(nodeName, sizeof(nodeName), current->configNode.name) != EOK) {
                    return EOUTPUT;
                }
                ToLowerCamelString(nodeName, strlen(nodeName));
                OUTPUT_CHECK_AND_RETURN(HcsFormatOutputWrite("const struct %s* %s;\n", structName, nodeName));
                res = HcsFormatOutputWrite("%*cuint16_t %sSize;\n", TAB_SIZE, ' ', nodeName);
            } else if (current->configNode.nodeType == CONFIG_NODE_INHERIT) {
                return NOERR;
            } else {
                res = HcsFormatOutputWrite("struct %s %s;\n", structName, current->configNode.name);
            }
            break;
        }
        case PARSEROP_CONFTERM:
            res = HcsPrintTermDefinition(current);
            break;
        default:
            break;
    }

    return res;
}

static int32_t HcsDuplicateCheckWalkCallBack(ParserObject *current, int32_t walkDepth)
{
    (void)walkDepth;
    if (current->objectBase.type != PARSEROP_CONFNODE || HcsIsInSubClassNode(current)) {
        return NOERR;
    }

    return HcsSymbolTableAdd(current->objectBase.name, current);
}

static int32_t HcsGenNormalNodeDefinition(ParserObject *object, int32_t walkDepth)
{
    (void)walkDepth;
    char structName[OBJECT_NAME_MAX_LEN] = {'\0'};
    int32_t res = GenConfigStructName(object, structName, OBJECT_NAME_MAX_LEN - 1);
    if (res) {
        return res;
    }
    if (HcsSymbolTableFind(structName)) {
        return NOERR;
    } else {
        HcsSymbolTableAdd(structName, object);
    }

    OUTPUT_CHECK_AND_RETURN(HcsFormatOutputWrite("struct %s {\n", structName));

    ParserObject *terms = (ParserObject *)object->objectBase.child;
    while (terms != NULL) {
        res = HcsObjectDefinitionGen(terms);
        if (res) {
            return res;
        }
        terms = (ParserObject *)terms->objectBase.next;
    }

    return HcsFormatOutputWrite("};\n\n");
}

static int32_t HcsGenNodeTemplateDefinition(ParserObject *object, int32_t walkDepth)
{
    return HcsGenNormalNodeDefinition(object, walkDepth);
}

static int32_t HcsDefinitionGenWalkCallBack(ParserObject *current, int32_t walkDepth)
{
    (void)walkDepth;
    if (current->objectBase.type != PARSEROP_CONFNODE) {
        return NOERR;
    }
    if (current->configNode.nodeType == CONFIG_NODE_TEMPLATE) {
        return HcsGenNodeTemplateDefinition(current, walkDepth);
    } else if (current->configNode.nodeType == CONFIG_NODE_INHERIT) {
        return NOERR;
    } else {
        return HcsGenNormalNodeDefinition(current, walkDepth);
    }
}

static uint32_t HcsPrintTermValue(const ParserObject *object)
{
    int32_t res = NOERR;
    switch (object->objectBase.type) {
        case PARSEROP_UINT8:  /* fallthrough */
        case PARSEROP_UINT16: /* fallthrough */
        case PARSEROP_UINT32: /* fallthrough */
        case PARSEROP_UINT64:
            res = HcsFormatOutputWrite("0x%" PRIx64, object->objectBase.integerValue);
            break;
        case PARSEROP_STRING:
            res = HcsFormatOutputWrite("\"%s\"", object->objectBase.stringValue);
            break;
        default:
            break;
    }

    return res;
}

#define MAX_CONFIG_TREE_DEPTH 100
static char *HcsBuildObjectPath(const ParserObject *refObject)
{
    const char *pathArray[MAX_CONFIG_TREE_DEPTH] = {0};
    int32_t count = 0;
    size_t pathStringLen = strlen(GetConfigRootVariableName()) + 1;
    const ParserObject *it = refObject;

    while (it != HcsGetParserRoot() && count < MAX_CONFIG_TREE_DEPTH) {
        pathArray[count++] = it->objectBase.name;
        pathStringLen += strlen(it->objectBase.name) + 1;
        it = (ParserObject *)it->objectBase.parent;
    }
    if (count >= MAX_CONFIG_TREE_DEPTH) {
        HCS_ERROR("reference a config node nested too deep over %u", MAX_CONFIG_TREE_DEPTH);
        return NULL;
    }

    char *path = HcsMemZalloc(pathStringLen);
    if (path == NULL) {
        HCS_ERROR("%s:oom", __func__);
        return NULL;
    }

    int32_t res = strcat_s(path, pathStringLen, GetConfigRootVariableName());
    if (res) {
        HcsMemFree(path);
        HCS_ERROR("%s:string cat fail", __func__);
        return NULL;
    }

    for (int32_t i = count - 1; i >= 0; --i) {
        res = strcat_s(path, pathStringLen, ".");
        if (res) {
            HcsMemFree(path);
            HCS_ERROR("%s:string cat fail", __func__);
            return NULL;
        }
        res = strcat_s(path, pathStringLen, pathArray[i]);
        if (res) {
            HcsMemFree(path);
            HCS_ERROR("%s:string cat fail", __func__);
            return NULL;
        }
    }

    return path;
}

static int32_t HcsPrintArrayImplInSubClass(ParserObject *object, uint8_t tabSize)
{
    char arrayName[VARIABLE_NAME_LEN] = {0};
    if (GenConfigArrayName(object, arrayName, VARIABLE_NAME_LEN)) {
        return EOUTPUT;
    }
    OUTPUT_CHECK_AND_RETURN(HcsFormatOutputWrite(".%s = %s,\n", object->objectBase.name, arrayName));
    return HcsFormatOutputWrite("%*c.%sSize = %d,\n", tabSize, ' ',
                                object->configTerm.name, GetArraySize((ParserObject *)object->objectBase.child));
}

static int32_t HcsPrintArrayContent(const ParserObject *object, int32_t tabSize)
{
    if (!GetArraySize(object)) {
        return NOERR;
    }

    ParserObject *element = (ParserObject *)object->objectBase.child;
    uint32_t elementCount = 0;
    while (element != NULL) {
        if (HcsPrintTermValue(element) != NOERR) {
            return EOUTPUT;
        }
        if (elementCount++ >= ELEMENT_PER_LINE) {
            OUTPUT_CHECK_AND_RETURN(HcsFormatOutputWrite("\n%*c", tabSize, ' '));
        }
        element = (ParserObject *)element->configTerm.next;
        if (element != NULL) {
            OUTPUT_CHECK_AND_RETURN(HcsFormatOutputWrite(", "));
        }
    }

    return NOERR;
}

static int32_t HcsPrintArrayImplement(ParserObject *object, uint8_t tabSize)
{
    if (HcsIsInSubClassNode(object)) {
        return HcsPrintArrayImplInSubClass(object, tabSize);
    }

    ParserObject *termContext = (ParserObject *)object->objectBase.child;
    OUTPUT_CHECK_AND_RETURN(HcsFormatOutputWrite(".%s = { ", object->configTerm.name));

    if (HcsPrintArrayContent(termContext, tabSize + TAB_SIZE)) {
        HCS_ERROR("fail to write array content");
        return EOUTPUT;
    }
    return HcsFormatOutputWrite(" },\n");
}

static int32_t HcsPrintTermImplement(ParserObject *object, int32_t tabSize)
{
    int32_t res = NOERR;
    OUTPUT_CHECK_AND_RETURN(HcsFormatOutputWrite("%*c", tabSize, ' '));
    ParserObject *termContext = (ParserObject *)object->objectBase.child;
    switch (termContext->objectBase.type) {
        case PARSEROP_UINT8:
            /* fall-through */
        case PARSEROP_UINT16:
            /* fall-through */
        case PARSEROP_UINT32:
            /* fall-through */
        case PARSEROP_UINT64:
            OUTPUT_CHECK_AND_RETURN(HcsFormatOutputWrite(".%s = ", object->configTerm.name));
            if (HcsPrintTermValue(termContext) != NOERR) {
                return EOUTPUT;
            }
            res = HcsFormatOutputWrite(",\n");
            break;
        case PARSEROP_STRING:
            OUTPUT_CHECK_AND_RETURN(HcsFormatOutputWrite(".%s = ", object->configTerm.name));
            if (HcsPrintTermValue(termContext) != NOERR) {
                return EOUTPUT;
            }
            res = HcsFormatOutputWrite(",\n");
            break;
        case PARSEROP_ARRAY:
            res = HcsPrintArrayImplement(object, tabSize);
            break;
        case PARSEROP_NODEREF: {
            char *refPath = HcsBuildObjectPath((ParserObject *)object->objectBase.child->value);
            if (refPath == NULL) {
                return EOUTPUT;
            }
            res = HcsFormatOutputWrite(".%s = &%s,\n", object->configTerm.name, refPath);
            HcsMemFree(refPath);
            break;
        }
        default:
            break;
    }

    return res;
}

static int32_t HcsGenTemplateVariableName(ParserObject *object, char *nameBuff, uint32_t nameBufferSize)
{
    char tempName[OBJECT_NAME_MAX_LEN] = {'\0'};
    if (strcpy_s(tempName, sizeof(tempName), object->objectBase.name) != EOK) {
        HCS_ERROR("failed to copy string");
        return EOUTPUT;
    }
    ToUpperCamelString(tempName, sizeof(tempName));
    HcsSymbol *sym = HcsSymbolTableFind(object->objectBase.name);
    if (sym == NULL) {
        HcsSymbolTableAdd(object->objectBase.name, object);
    } else if (sym->object != object && object->configNode.templateSignNum == 0) {
        sym->duplicateCount++;
        object->configNode.templateSignNum = sym->duplicateCount;
    }

    int32_t res;
    if (object->configNode.templateSignNum) {
        res = sprintf_s(nameBuff, nameBufferSize, "g_%s%s%u", g_littleHumpNamePrefix, tempName,
            object->configNode.templateSignNum);
    } else {
        res = sprintf_s(nameBuff, nameBufferSize, "g_%s%s", g_littleHumpNamePrefix, tempName);
    }

    return (res > 0) ? NOERR : EFAIL;
}

static int32_t HcsTemplateNodeImplGen(ParserObject *current, int32_t tabSize)
{
    char templateVariableName[VARIABLE_NAME_LEN] = {'\0'};
    if (HcsGenTemplateVariableName(current, templateVariableName, sizeof(templateVariableName))) {
        return EOUTPUT;
    }
    char nodeName[OBJECT_NAME_MAX_LEN] = {0};
    if (strcpy_s(nodeName, sizeof(nodeName), current->configNode.name) != EOK) {
        HCS_ERROR("%s: failed to copy string", __func__);
        return EOUTPUT;
    }

    ToLowerCamelString(nodeName, strlen(nodeName));
    OUTPUT_CHECK_AND_RETURN(HcsFormatOutputWrite("%*c.%s = %s,\n", tabSize, ' ', nodeName,
        current->configNode.inheritCount ? templateVariableName : "0"));

    return HcsFormatOutputWrite("%*c.%sSize = %u,\n",
        tabSize, ' ', nodeName, current->configNode.inheritCount);
}

static int32_t HcsInheritObjectImplGen(ParserObject *current, int32_t tabSize)
{
    if (current->configNode.nodeType == CONFIG_NODE_TEMPLATE) {
        return HcsTemplateNodeImplGen(current, tabSize);
    }

    return NOERR;
}

static int32_t HcsObjectImplementGen(ParserObject *current, int32_t tabSize)
{
    int32_t res = NOERR;
    switch (current->objectBase.type) {
        case PARSEROP_CONFNODE:
            if (current->configNode.nodeType != CONFIG_NODE_NOREF) {
                res = HcsInheritObjectImplGen(current, tabSize);
                return res ? res : EASTWALKBREAK;
            }
            OUTPUT_CHECK_AND_RETURN(HcsFormatOutputWrite("%*c.%s = {\n", tabSize, ' ', current->configNode.name));
            if (current->objectBase.child == NULL) {
                res = HcsFormatOutputWrite("%*c},\n", tabSize, ' ');
            }
            break;
        case PARSEROP_CONFTERM:
            res = HcsPrintTermImplement(current, tabSize);
            break;
        default:
            return NOERR;
    }

    return res;
}

static int32_t HcsImplementGenWalkCallBack(ParserObject *current, int32_t walkDepth)
{
    if (current->objectBase.type != PARSEROP_CONFNODE && current->objectBase.type != PARSEROP_CONFTERM) {
        return NOERR;
    }

    if (current->objectBase.type != PARSEROP_CONFNODE && HcsIsInSubClassNode(current)) {
        return NOERR;
    }

    if (current == HcsGetParserRoot()) {
        char structName[OBJECT_NAME_MAX_LEN] = {'\0'};
        int32_t res = GenConfigStructName(current, structName, OBJECT_NAME_MAX_LEN - 1);
        if (res) {
            return res;
        }
        OUTPUT_CHECK_AND_RETURN(HcsFormatOutputWrite("static const struct %s %s = {\n",
            structName, GetConfigRootVariableName()));
        if (current->objectBase.child == NULL) {
            res = HcsFormatOutputWrite("};\n");
        }
        return res;
    }
    return HcsObjectImplementGen(current, walkDepth * TAB_SIZE);
}

static int32_t HcsImplementCloseBraceGen(ParserObject *current, int32_t walkDepth)
{
    if (current->objectBase.type != PARSEROP_CONFNODE) {
        return NOERR;
    }

    if (current->configNode.nodeType == CONFIG_NODE_INHERIT) {
        return NOERR;
    }

    int32_t tabSize = walkDepth * TAB_SIZE;

    if (current != HcsGetParserRoot()) {
        return HcsFormatOutputWrite("%*c},\n", tabSize, ' ');
    } else {
        return HcsFormatOutputWrite("};\n");
    }
}

static void ToUpperString(char *str, uint32_t strLen)
{
    for (uint32_t i = 0; i < strLen; ++i) {
        str[i] = (char)toupper(str[i]);
    }
}

#define HCS_HEADER_MACRO_MAX_LEN 150
static char *GenHeaderProtectionMacro()
{
    char *fileName = HcsGetOutputFileNameWithoutSuffix();
    if (fileName == NULL) {
        return NULL;
    }
    ToUpperString(fileName, strlen(fileName));

    char *macro = HcsMemZalloc(sizeof(char) * HCS_HEADER_MACRO_MAX_LEN);
    if (macro == NULL) {
        HCS_ERROR("oom");
        HcsMemFree(fileName);
        return NULL;
    }

    int32_t res = sprintf_s(macro, HCS_HEADER_MACRO_MAX_LEN, "HCS_CONFIG_%s_HEADER_H", fileName);
    HcsMemFree(fileName);
    if (res <= 0) {
        HcsMemFree(macro);
        return NULL;
    }
    return macro;
}

static int32_t HcsWriteHeaderFileHead()
{
    /* Write copyright info */
    OUTPUT_CHECK_AND_RETURN(HcsFormatOutputWrite(HCS_CONFIG_FILE_HEADER));

    /* Write header protection macro */
    char *headerProtectMacro = GenHeaderProtectionMacro();
    if (headerProtectMacro == NULL) {
        return EOUTPUT;
    }
    OUTPUT_CHECK_AND_RETURN(HcsFormatOutputWrite("#ifndef %s\n#define %s\n\n",
        headerProtectMacro, headerProtectMacro));

    /* Write include header file */
    return HcsFormatOutputWrite(HCS_CONFIG_INCLUDE_HEADER);
}

static int32_t HcsWriteHeaderFileEnd()
{
    /* Write header protection macro */
    char *headerMacro = GenHeaderProtectionMacro();
    if (headerMacro == NULL) {
        return EOUTPUT;
    }
    int32_t ret = HcsFormatOutputWrite("\n\n#endif // %s", headerMacro);
    HcsMemFree(headerMacro);

    return ret;
}

static int32_t HcsWriteFunctionDeclaration()
{
    return HcsFormatOutputWrite("const struct %s%sRoot* HdfGet%sModuleConfigRoot(void);", g_namePrefix,
        g_bigHumpModuleName, g_bigHumpModuleName);
}

static int32_t HcsWriteFunctionImplement()
{
    return HcsFormatOutputWrite(HCS_CONFIG_FUNC_IMPLEMENT, g_namePrefix,
        g_bigHumpModuleName, g_bigHumpModuleName, GetConfigRootVariableName());
}

static int32_t HcsOutputHeaderFile()
{
    struct HcsFile *outputFIle = NULL;
    ParserObject *astRoot = HcsGetParserRoot();
    if (astRoot == NULL) {
        return EOUTPUT;
    }

    /* Duplicate node name check */
    if (HcsWalkAst(astRoot, AST_WALK_BACKEND, NULL, HcsDuplicateCheckWalkCallBack)) {
        return EOUTPUT;
    }

    outputFIle = HcsOpenOutputFile(".h");
    if (outputFIle == NULL) {
        return EINVALF;
    }

    int32_t ret = HcsWriteHeaderFileHead();
    if (ret) {
        goto OUT;
    }
    /* Generate C header file containing the structure definition of config */
    if (!HcsWalkAst(astRoot, AST_WALK_BACKEND, NULL, HcsDefinitionGenWalkCallBack)) {
        ret = HcsWriteFunctionDeclaration();
        if (ret) {
            goto OUT;
        }
    }

    ret = HcsWriteHeaderFileEnd();
OUT:
    HcsCloseOutput(outputFIle);
    return ret;
}


static int32_t g_baseTabsize = 0;

static int32_t HcsTemplateObjectGenWalk(ParserObject *current, int32_t walkDepth)
{
    return HcsObjectImplementGen(current, walkDepth * TAB_SIZE + g_baseTabsize);
}

static int32_t HcsTemplateCloseBraceGen(ParserObject *current, int32_t walkDepth)
{
    return HcsImplementCloseBraceGen(current, walkDepth + g_baseTabsize / TAB_SIZE);
}

static int32_t HcsTemplateVariableGen(ParserObject *current)
{
    ParserObject *child = (ParserObject *)current->objectBase.child;
    while (child != NULL) {
        int32_t res = HcsWalkAst(child, AST_WALK_ROUND, HcsTemplateObjectGenWalk, HcsTemplateCloseBraceGen);
        if (res) {
            return res;
        }
        child = (ParserObject *)child->objectBase.next;
    }
    return NOERR;
}

static int32_t HcsTemplateImplGenWalkCallBack(ParserObject *current, int32_t walkDepth)
{
    (void)walkDepth;
    if (current->objectBase.type != PARSEROP_CONFNODE || current->configNode.nodeType != CONFIG_NODE_TEMPLATE) {
        return NOERR;
    }

    if (!current->configNode.inheritCount) {
        return NOERR;
    }

    char nameBuff[OBJECT_NAME_MAX_LEN] = {'\0'};
    int32_t res = GenConfigStructName(current, nameBuff, OBJECT_NAME_MAX_LEN - 1);
    if (res) {
        return res;
    }

    OUTPUT_CHECK_AND_RETURN(HcsFormatOutputWrite("static const struct %s ", nameBuff));
    if (HcsGenTemplateVariableName(current, nameBuff, sizeof(nameBuff))) {
        return EOUTPUT;
    }
    OUTPUT_CHECK_AND_RETURN(HcsFormatOutputWrite("%s[] = {\n", nameBuff));

    /* Generate C global variables definition file */
    TemplateNodeInstance *subClasses = current->configNode.subClasses;
    g_baseTabsize = TAB_SIZE + TAB_SIZE;
    while (subClasses != NULL) {
        OUTPUT_CHECK_AND_RETURN(HcsFormatOutputWrite("%*c[%d] = {\n", TAB_SIZE, ' ',
            subClasses->nodeObject->inheritIndex));

        if (HcsTemplateVariableGen((ParserObject *)subClasses->nodeObject)) {
            return EOUTPUT;
        }
        OUTPUT_CHECK_AND_RETURN(HcsFormatOutputWrite("%*c},\n", TAB_SIZE, ' '));
        subClasses = subClasses->next;
    }

    return HcsFormatOutputWrite("};\n\n");
}

static int32_t HcsArrayVariablesDeclareGen(ParserObject *term)
{
    if (!HcsIsInSubClassNode(term)) {
        return NOERR;
    }

    char nameBuff[OBJECT_NAME_MAX_LEN] = {'\0'};
    int32_t res = GenConfigArrayName(term, nameBuff, OBJECT_NAME_MAX_LEN - 1);
    if (res) {
        return res;
    }
    ParserObject *array = (ParserObject *)term->configTerm.child;
    res = HcsFormatOutputWrite("\nstatic const %s %s[%d] = {\n%*c", GetArrayType(array),
        nameBuff, GetArraySize(array), TAB_SIZE, ' ');
    OUTPUT_CHECK_AND_RETURN(res);

    if (HcsPrintArrayContent(array, TAB_SIZE)) {
        HCS_ERROR("fail to write array content");
        return EOUTPUT;
    }
    return HcsFormatOutputWrite("\n};\n");
}

static int32_t HcsTemplateVariablesDeclareGenWalk(ParserObject *current, int32_t walkDepth)
{
    (void)walkDepth;
    if (current->objectBase.type == PARSEROP_CONFTERM && HcsGetTermType(current) == PARSEROP_ARRAY) {
        return HcsArrayVariablesDeclareGen(current);
    } else if (current->objectBase.type != PARSEROP_CONFNODE || current->configNode.nodeType != CONFIG_NODE_TEMPLATE) {
        return NOERR;
    }

    if (!current->configNode.inheritCount) {
        return NOERR;
    }
    char writeBuffer[WRITE_BUFFER_LEN] = {'\0'};
    char nameBuff[OBJECT_NAME_MAX_LEN] = {'\0'};
    int32_t res = GenConfigStructName(current, nameBuff, OBJECT_NAME_MAX_LEN - 1);
    if (res) {
        return res;
    }

    res = sprintf_s(writeBuffer, sizeof(writeBuffer), "static const struct %s ", nameBuff);
    PRINTF_CHECK_AND_RETURN(res);
    if (HcsGenTemplateVariableName(current, nameBuff, sizeof(nameBuff))) {
        return EOUTPUT;
    }
    res = sprintf_s(writeBuffer + strlen(writeBuffer), sizeof(writeBuffer) - strlen(writeBuffer), "%s[];\n", nameBuff);
    PRINTF_CHECK_AND_RETURN(res);
    return HcsOutputWrite(writeBuffer, strlen(writeBuffer));
}

static int32_t HcsTemplateVariablesDeclareGen(ParserObject *astRoot)
{
    uint32_t writeCount = HcsGetOutputCurrentCount();
    if (HcsWalkAst(astRoot, AST_WALK_BACKEND, NULL, HcsTemplateVariablesDeclareGenWalk)) {
        return EOUTPUT;
    }
    if (writeCount == HcsGetOutputCurrentCount()) {
        /* if no template variable generated, should not output end line */
        return NOERR;
    }
    const char *lineEnd = "\n";
    return HcsOutputWrite(lineEnd, strlen(lineEnd));
}

static int32_t HcsOutputTemplateImplement(ParserObject *astRoot)
{
    if (HcsTemplateVariablesDeclareGen(astRoot)) {
        return EOUTPUT;
    }

    /* all template node will be global variables */
    return HcsWalkAst(astRoot, AST_WALK_BACKEND, NULL, HcsTemplateImplGenWalkCallBack);
}

static int32_t HcsOutputCFile()
{
    ParserObject *astRoot = HcsGetParserRoot();
    if (astRoot == NULL) {
        return EOUTPUT;
    }

    if (HcsSymbolTableReset()) {
        return EOUTPUT;
    }

    struct HcsFile *outputFIle = HcsOpenOutputFile(".c");
    if (outputFIle == NULL) {
        return EINVALF;
    }

    if (HcsFormatOutputWrite(HCS_CONFIG_FILE_HEADER)) {
        HcsCloseOutput(outputFIle);
        return EOUTPUT;
    }

    char *fileName = HcsGetOutputFileNameWithoutSuffix();
    if (fileName == NULL) {
        return EOUTPUT;
    }
    int32_t res = HcsFormatOutputWrite("#include \"%s.h\"\n\n", fileName);
    HcsMemFree(fileName);
    if (res != NOERR) {
        HcsCloseOutput(outputFIle);
        return EOUTPUT;
    }

    res = HcsOutputTemplateImplement(astRoot);
    if (res != NOERR) {
        HcsCloseOutput(outputFIle);
        return EOUTPUT;
    }

    /* Generate C global variables definition file */
    res = HcsWalkAst(astRoot, AST_WALK_ROUND, HcsImplementGenWalkCallBack, HcsImplementCloseBraceGen);
    if (res) {
        HcsCloseOutput(outputFIle);
        return res;
    }

    res = HcsWriteFunctionImplement();
    HcsCloseOutput(outputFIle);
    return res;
}

int32_t HcsTextCodeOutput(void)
{
    int32_t ret = HcsSymbolTableInit();
    if (ret) {
        return ret;
    }

    const char *symbolPrefix = HcsOptGetSymbolNamePrefix();
    if (symbolPrefix != NULL) {
        g_namePrefix = symbolPrefix;
    }

    do {
        ret = InitConfigVariableNames();
        if (ret) {
            break;
        }
        ret = HcsOutputHeaderFile();
        if (ret) {
            break;
        }

        ret = HcsOutputCFile();
    } while (false);

    ReleaseConfigVariableNames();
    HcsSymbolTableRelease();
    return ret;
}
