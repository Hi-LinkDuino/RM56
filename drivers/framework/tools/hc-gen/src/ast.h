/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HC_GEN_AST_H
#define HC_GEN_AST_H

#include <cstdint>
#include <list>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "token.h"
#include "types.h"

namespace OHOS {
namespace Hardware {
enum ObjectType {
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
};

enum NodeRefType {
    NODE_NOREF = 0,
    NODE_COPY,
    NODE_REF,
    NODE_DELETE,
    NODE_TEMPLATE,
    NODE_INHERIT,
};

class AstObject {
public:
    friend class Ast;

    AstObject(const AstObject &obj);

    AstObject(std::string name, uint32_t type, uint64_t value);

    AstObject(std::string name, uint32_t type, std::string value);

    AstObject(std::string name, uint32_t type, uint64_t value, const Token &bindToken);

    AstObject(std::string name, uint32_t type, std::string value, const Token &bindToken);

    virtual ~AstObject();

    AstObject& operator=(const AstObject &obj);

    virtual bool AddChild(const std::shared_ptr<AstObject> &childObj);

    virtual bool AddPeer(std::shared_ptr<AstObject> peerObject);

    friend std::ostream &operator<<(std::ostream &stream, const AstObject &t);

    virtual bool Merge(std::shared_ptr<AstObject> &srcObj);

    virtual bool Copy(std::shared_ptr<AstObject> src, bool overwrite);

    virtual bool Move(std::shared_ptr<AstObject> src);

    void Remove();

    void Separate();

    std::shared_ptr<AstObject> Lookup(const std::string &name, uint32_t type = 0) const;

    bool IsElders(const std::shared_ptr<AstObject> &child);

    bool IsNumber() const;

    bool IsNode() const;

    bool IsTerm() const;

    bool IsArray() const;

    virtual std::string SourceInfo();

    void SetParent(AstObject *parent);

    void SetSize(uint32_t size);

    void SetSubSize(uint32_t size);

    void SetHash(uint32_t hash);

    uint32_t GetSize();

    uint32_t GetSubSize();

    uint32_t GetHash();

    std::shared_ptr<AstObject> Child();

    std::shared_ptr<AstObject> Next();

    virtual const std::string &Name();

    const std::string &StringValue();

    uint64_t IntegerValue();

    virtual uint32_t Type();

    uint8_t OpCode();

    void SetOpCode(uint8_t opcode);

    virtual bool HasDuplicateChild();

    std::shared_ptr<AstObject> Parent();

protected:
    uint32_t type_;
    std::string name_;
    AstObject *parent_;
    std::shared_ptr<AstObject> next_;
    std::shared_ptr<AstObject> child_;
    uint32_t lineno_;
    std::shared_ptr<std::string> src_;
    uint8_t opCode_;
    uint32_t size_;
    uint32_t subSize_;
    uint32_t hash_;
    uint64_t integerValue_;
    std::string stringValue_;

private:
    static uint32_t FitIntegerValueType(uint64_t value);
};

class ConfigNode : public AstObject {
public:
    ConfigNode(const ConfigNode &node);

    ConfigNode(std::string name, uint32_t nodeType, std::string refName);

    ConfigNode(Token &name, uint32_t nodeType, std::string refName);

    ~ConfigNode() override = default;

    ConfigNode& operator=(const ConfigNode &node);

    friend std::ostream &operator<<(std::ostream &stream, const ConfigNode &t);

    bool Merge(std::shared_ptr<AstObject> &srcObj) override;

    static ConfigNode *CastFrom(const std::shared_ptr<AstObject> &astObject);

    uint32_t GetNodeType() const;

    const std::string &GetRefPath();

    void SetNodeType(uint32_t type);

    void SetRefPath(std::string ref);

    static const std::string &NodeTypeToStr(uint32_t type);

    bool HasDuplicateChild() override;

    bool InheritExpand(const std::shared_ptr<AstObject> &refObj);

    bool RefExpand(const std::shared_ptr<AstObject> &refObj);

    bool Copy(std::shared_ptr<AstObject> src, bool overwrite) override;

    bool Move(std::shared_ptr<AstObject> src) override;

    bool Compare(ConfigNode &other);

    uint32_t InheritIndex();

    uint32_t InheritCount();

    uint32_t TemplateSignNum();

    void SetTemplateSignNum(uint32_t sigNum);

    const std::list<AstObject *> &SubClasses();

private:
    bool NodeRefExpand(const std::shared_ptr<AstObject> &ref);

    bool NodeCopyExpand(const std::shared_ptr<AstObject> &ref);

    std::string refNodePath_;
    uint32_t nodeType_;
    uint32_t inheritIndex_;
    uint32_t inheritCount_;
    uint32_t templateSignNum_;
    std::list<AstObject *> subClasses_;
};

class ConfigTerm : public AstObject {
public:
    ConfigTerm(const ConfigTerm &term);

    ConfigTerm(std::string name, const std::shared_ptr<AstObject> &value);

    ConfigTerm(Token &name, const std::shared_ptr<AstObject> &value);

    ~ConfigTerm() override = default;

    ConfigTerm& operator=(const ConfigTerm &term);

    static ConfigTerm *CastFrom(const std::shared_ptr<AstObject> &astObject);

    bool Merge(std::shared_ptr<AstObject> &srcObj) override;

    friend std::ostream &operator<<(std::ostream &stream, const ConfigTerm &t);

    bool RefExpand(const std::shared_ptr<AstObject> refObj);

    bool Copy(std::shared_ptr<AstObject> src, bool overwrite) override;

    bool Move(std::shared_ptr<AstObject> src) override;

    std::weak_ptr<AstObject> RefNode();

    uint32_t SigNum();

    void SetSigNum(uint32_t sigNum);

private:
    std::weak_ptr<AstObject> refNode_;
    uint32_t signNum_;
};

class ConfigArray : public AstObject {
public:
    ConfigArray();

    ConfigArray(const ConfigArray &array);

    explicit ConfigArray(const Token &bindToken);

    ~ConfigArray() override = default;

    ConfigArray& operator=(const ConfigArray &array);

    static ConfigArray *CastFrom(const std::shared_ptr<AstObject> &astObject);

    bool AddChild(const std::shared_ptr<AstObject> &childObj) override;

    bool Merge(std::shared_ptr<AstObject> &srcObj) override;

    bool Copy(std::shared_ptr<AstObject> src, bool overwrite) override;

    uint16_t ArraySize();

    uint16_t ArrayType();

private:
    uint32_t arrayType_;
    uint32_t arraySize_;
};

class AstObjectFactory {
public:
    static std::shared_ptr<AstObject> Build(std::shared_ptr<AstObject> object);
};

class Ast {
public:
    Ast() = default;

    explicit Ast(std::shared_ptr<AstObject> astRoot) : astRoot_(std::move(astRoot)), redefineChecked_(false) {}

    ~Ast() = default;

    std::shared_ptr<AstObject> GetAstRoot();

    bool Merge(const std::list<std::shared_ptr<Ast>> &astList);

    bool Expand();

    std::shared_ptr<AstObject> Lookup(const std::shared_ptr<AstObject> &startObj, const std::string &path);

    template <typename T>
    static bool WalkForward(const std::shared_ptr<AstObject> &startObject, T callback)
    {
        std::shared_ptr<AstObject> forwardWalkObj = startObject;
        int32_t walkDepth = 0;
        bool preVisited = false;

        while (forwardWalkObj != nullptr) {
            if (!preVisited) {
                int32_t ret = (int32_t)callback(forwardWalkObj, walkDepth);
                if (ret && ret != EASTWALKBREAK) {
                    return false;
                } else if (ret != EASTWALKBREAK && forwardWalkObj->child_ != nullptr) {
                    /* when callback return EASTWALKBREAK, not walk current's child */
                    walkDepth++;
                    forwardWalkObj = forwardWalkObj->child_;
                    continue;
                }
            }
            if (forwardWalkObj == startObject) {
                break;
            }

            if (forwardWalkObj->next_ != nullptr) {
                forwardWalkObj = forwardWalkObj->next_;
                preVisited = false;
            } else {
                forwardWalkObj = forwardWalkObj->Parent();
                preVisited = true;
                walkDepth--;
            }
        }

        return true;
    }

    template <typename T>
    static bool WalkBackward(const std::shared_ptr<AstObject> &startObject, T callback)
    {
        std::shared_ptr<AstObject> backWalkObj = startObject;
        std::shared_ptr<AstObject> next = nullptr;
        std::shared_ptr<AstObject> parent = nullptr;
        int32_t walkDepth = 0;
        bool preVisited = false;

        while (backWalkObj != nullptr) {
            if (backWalkObj->child_ == nullptr || preVisited) {
                next = backWalkObj->next_;
                parent = backWalkObj->Parent();
                /* can safe delete current in callback */
                if (callback(backWalkObj, walkDepth) != NOERR) {
                    return false;
                }
            } else {
                if (backWalkObj->child_) {
                    walkDepth++;
                    backWalkObj = backWalkObj->child_;
                    continue;
                }
            }
            if (backWalkObj == startObject) {
                break;
            }

            if (next != nullptr) {
                backWalkObj = next;
                preVisited = false;
            } else {
                backWalkObj = parent;
                preVisited = true;
                walkDepth--;
            }
        }

        return true;
    }

    template <typename T1, typename T2>
    static bool WalkRound(const std::shared_ptr<AstObject> &startObject, T1 forwardCallback, T2 backwardCallback)
    {
        std::shared_ptr<AstObject> roundWalkObj = startObject;
        int32_t walkDepth = 0;
        bool preVisited = false;

        while (roundWalkObj != nullptr) {
            if (preVisited) {
                if (backwardCallback(roundWalkObj, walkDepth) != NOERR) {
                    return false;
                }
            } else {
                int32_t ret = forwardCallback(roundWalkObj, walkDepth);
                /* when callback return EASTWALKBREAK, not walk current's child */
                if (ret && ret != EASTWALKBREAK) {
                    return false;
                } else if (!ret && roundWalkObj->child_ != nullptr) {
                    walkDepth++;
                    roundWalkObj = roundWalkObj->child_;
                    continue;
                }
            }
            if (roundWalkObj == startObject) {
                break;
            }

            if (roundWalkObj->next_) {
                roundWalkObj = roundWalkObj->next_;
                preVisited = false;
            } else {
                roundWalkObj = roundWalkObj->Parent();
                preVisited = true;
                walkDepth--;
            }
        }

        return true;
    }

    template <typename T>
    bool WalkForward(T callback)
    {
        return WalkForward(astRoot_, callback);
    }

    template <typename T>
    bool WalkBackward(T callback)
    {
        return WalkBackward(astRoot_, callback);
    }

    template <typename T1, typename T2>
    bool WalkRound(T1 forwardCallback, T2 backwardCallback)
    {
        return WalkRound(astRoot_, forwardCallback, backwardCallback);
    }

    void Dump(const std::string &prefix = std::string());

private:
    bool RedefineCheck();

    bool NodeExpand();

    bool InheritExpand();

    std::list<std::string> SplitNodePath(const std::string &path, char separator);

    std::shared_ptr<AstObject> astRoot_;
    bool redefineChecked_;
};

std::ostream &operator<<(std::ostream &s, const AstObject &obj);
std::ostream &operator<<(std::ostream &s, const ConfigNode &obj);
std::ostream &operator<<(std::ostream &s, const ConfigTerm &obj);

} // namespace Hardware
} // namespace OHOS

#endif // HC_GEN_AST_H
