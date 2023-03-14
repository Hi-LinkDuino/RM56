/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "ast.h"

#include <iomanip>
#include <set>
#include <sstream>

#include "logger.h"

using namespace OHOS::Hardware;

AstObject::AstObject(const AstObject &obj) : AstObject(obj.name_, obj.type_, obj.stringValue_)
{
    integerValue_ = obj.integerValue_;
}

AstObject::AstObject(std::string name, uint32_t type, uint64_t value) :
    type_(type),
    name_(std::move(name)),
    parent_(nullptr),
    lineno_(0),
    opCode_(0),
    size_(0),
    subSize_(0),
    hash_(0),
    integerValue_(value)
{
}

AstObject::AstObject(std::string name, uint32_t type, std::string value) : AstObject(std::move(name), type, 0)
{
    stringValue_ = std::move(value);
}

AstObject::AstObject(std::string name, uint32_t type, uint64_t value, const Token &bindToken) :
    AstObject(std::move(name), type, value)
{
    lineno_ = bindToken.lineNo;
    src_ = bindToken.src;

    switch (type) {
        case PARSEROP_UINT8:  /* fall-through */
        case PARSEROP_UINT16: /* fall-through */
        case PARSEROP_UINT32: /* fall-through */
        case PARSEROP_UINT64:
            this->type_ = FitIntegerValueType(value);
            break;
        default:
            break;
    }
}

AstObject::AstObject(std::string name, uint32_t type, std::string value, const Token &bindToken) :
    AstObject(std::move(name), type, 0, bindToken)
{
    stringValue_ = std::move(value);
}

AstObject::~AstObject()
{
    parent_ = nullptr;
    next_ = nullptr;
    child_ = nullptr;
}

AstObject& AstObject::operator=(const AstObject &obj)
{
    if (this != &obj) {
        type_ = obj.type_;
        name_ = obj.name_;
        parent_ = obj.parent_;
        next_ = obj.next_;
        child_ = obj.child_;
        lineno_ = obj.lineno_;
        src_ = obj.src_;
        opCode_ = obj.opCode_;
        size_ = obj.size_;
        subSize_ = obj.subSize_;
        hash_ = obj.hash_;
        integerValue_ = obj.integerValue_;
        stringValue_ = obj.stringValue_;
    }

    return *this;
}

uint32_t AstObject::FitIntegerValueType(uint64_t value)
{
    if (value <= UINT8_MAX) {
        return PARSEROP_UINT8;
    } else if (value <= UINT16_MAX) {
        return PARSEROP_UINT16;
    } else if (value <= UINT32_MAX) {
        return PARSEROP_UINT32;
    } else {
        return PARSEROP_UINT64;
    }
}

bool AstObject::AddChild(const std::shared_ptr<AstObject> &childObj)
{
    if (childObj == nullptr) {
        return false;
    }
    if (child_ == nullptr) {
        child_ = childObj;
        std::shared_ptr<AstObject> childNext = childObj;
        while (childNext != nullptr) {
            childNext->parent_ = this;
            childNext = childNext->next_;
        }
    } else {
        return child_->AddPeer(childObj);
    }

    return true;
}

bool AstObject::AddPeer(std::shared_ptr<AstObject> peerObject)
{
    if (peerObject == nullptr) {
        return false;
    }

    if (this == peerObject.get()) {
        Logger().Error() << "add self as peer";
        return false;
    }

    if (next_ == nullptr) {
        next_ = peerObject;
    } else {
        std::shared_ptr<AstObject> lastNode = next_;
        while (lastNode->next_ != nullptr) {
            lastNode = lastNode->next_;
        }
        lastNode->next_ = peerObject;
    }

    std::shared_ptr<AstObject> peer = peerObject;
    while (peer) {
        peer->parent_ = parent_;
        peer = peer->next_;
    }

    return true;
}

std::ostream &OHOS::Hardware::operator<<(std::ostream &stream, const AstObject &t)
{
    if (t.type_ == PARSEROP_CONFNODE) {
        auto node = static_cast<const ConfigNode *>(&t);
        stream << *node;
        return stream;
    } else if (t.type_ == PARSEROP_CONFTERM) {
        auto term = static_cast<const ConfigTerm *>(&t);
        stream << *term;
        return stream;
    }

    if (t.name_.empty()) {
        stream << "|_";
    } else {
        stream << t.name_;
    }
    switch (t.type_) {
        case PARSEROP_UINT8:
            stream << "uint8 0x" << std::hex << t.integerValue_;
            break;
        case PARSEROP_UINT16:
            stream << "uint16 0x" << std::hex << t.integerValue_;
            break;
        case PARSEROP_UINT32:
            stream << "uint32 0x" << std::hex << t.integerValue_;
            break;
        case PARSEROP_UINT64:
            stream << "uint64 0x" << std::hex << t.integerValue_;
            break;
        case PARSEROP_STRING:
            stream << "string \"" << t.stringValue_ << "\"";
            break;
        case PARSEROP_ARRAY:
            stream << "array";
            break;
        case PARSEROP_NODEREF:
            stream << "noderef " << t.stringValue_;
            break;
        case PARSEROP_DELETE:
            stream << "delete";
            break;
        default:
            break;
    }
    return stream;
}

bool AstObject::Merge(std::shared_ptr<AstObject> &srcObj)
{
    if (srcObj->name_ != name_) {
        Logger().Error() << this->SourceInfo() << "merge different node to" << srcObj->SourceInfo();
        return false;
    }

    if (srcObj->type_ != type_) {
        Logger().Error() << this->SourceInfo() << "conflict type with " << srcObj->SourceInfo();
        return false;
    }

    src_ = srcObj->src_;
    lineno_ = srcObj->lineno_;
    stringValue_ = srcObj->stringValue_;
    integerValue_ = srcObj->integerValue_;

    return true;
}

bool AstObject::Copy(std::shared_ptr<AstObject> src, bool overwrite)
{
    if (src == nullptr) {
        return false;
    }

    if (overwrite) {
        src_ = src->src_;
        lineno_ = src->lineno_;
        integerValue_ = src->integerValue_;
        stringValue_ = src->stringValue_;
    }

    return true;
}

bool AstObject::Move(std::shared_ptr<AstObject> src)
{
    if (!Copy(src, true)) {
        return false;
    }
    src->Separate();
    return true;
}

std::string AstObject::SourceInfo()
{
    std::stringstream o;
    o << src_->c_str() << ":" << lineno_ << " ";
    return o.str();
}

void AstObject::Remove()
{
    Separate();
    child_ = nullptr;
    next_ = nullptr;
}

std::shared_ptr<AstObject> AstObject::Lookup(const std::string &name, uint32_t type) const
{
    auto peer = child_;
    while (peer != nullptr) {
        if (peer->name_ == name && (type == 0 || peer->type_ == type)) {
            return peer;
        }

        peer = peer->next_;
    }

    return nullptr;
}

bool AstObject::IsNumber() const
{
    return type_ >= PARSEROP_UINT8 && type_ <= PARSEROP_UINT64;
}

bool AstObject::IsNode() const
{
    return type_ == PARSEROP_CONFNODE;
}

bool AstObject::IsTerm() const
{
    return type_ == PARSEROP_CONFTERM;
}

bool AstObject::IsArray() const
{
    return type_ == PARSEROP_ARRAY;
}

void AstObject::Separate()
{
    if (parent_ == nullptr) {
        return;
    }
    if (parent_->child_.get() == this) {
        parent_->child_ = next_; // decref this
        next_ = nullptr;
        return;
    }

    auto pre = parent_->child_;
    while (pre != nullptr) {
        if (pre->next_.get() == this) {
            pre->next_ = pre->next_->next_; // decref this
            break;
        }

        pre = pre->next_;
    }
    next_ = nullptr;
}

void AstObject::SetParent(AstObject *parent)
{
    parent_ = parent;
}

void AstObject::SetSize(uint32_t size)
{
    size_ = size;
}

void AstObject::SetSubSize(uint32_t size)
{
    subSize_ = size;
}

uint32_t AstObject::GetSubSize()
{
    return subSize_;
}

void AstObject::SetHash(uint32_t hash)
{
    hash_ = hash;
}

uint32_t AstObject::GetSize()
{
    return size_;
}

uint32_t AstObject::GetHash()
{
    return hash_;
}

std::shared_ptr<AstObject> AstObject::Next()
{
    return next_;
}

std::shared_ptr<AstObject> AstObject::Child()
{
    return child_;
}

const std::string &AstObject::Name()
{
    return name_;
}

const std::string &AstObject::StringValue()
{
    return stringValue_;
}

uint64_t AstObject::IntegerValue()
{
    return integerValue_;
}

uint32_t AstObject::Type()
{
    return type_;
}

uint8_t AstObject::OpCode()
{
    return opCode_;
}

void AstObject::SetOpCode(uint8_t opcode)
{
    opCode_ = opcode;
}

bool AstObject::HasDuplicateChild()
{
    return false;
}

bool AstObject::IsElders(const std::shared_ptr<AstObject> &child)
{
    auto p = child.get();
    while (p != nullptr) {
        if (p == this) {
            return true;
        }
        p = p->parent_;
    }
    return false;
}

std::shared_ptr<AstObject> AstObject::Parent()
{
    return std::shared_ptr<AstObject>(parent_, [](auto p) {});
}

ConfigNode::ConfigNode(const ConfigNode &node) : ConfigNode(node.name_, node.nodeType_, node.refNodePath_)
{
    auto child = node.child_;
    while (child != nullptr) {
        AstObject::AddChild(AstObjectFactory::Build(child));
        child = child->Next();
    }
}

ConfigNode::ConfigNode(std::string name, uint32_t nodeType, std::string refName) :
    AstObject(std::move(name), PARSEROP_CONFNODE, ""),
    refNodePath_(std::move(refName)),
    nodeType_(nodeType),
    inheritIndex_(0),
    inheritCount_(0),
    templateSignNum_(0)
{
}

ConfigNode::ConfigNode(Token &name, uint32_t nodeType, std::string refName) :
    AstObject(name.strval, PARSEROP_CONFNODE, 0, name),
    refNodePath_(std::move(refName)),
    nodeType_(nodeType),
    inheritIndex_(0),
    inheritCount_(0),
    templateSignNum_(0)
{
}

ConfigNode& ConfigNode::operator=(const ConfigNode &node)
{
    if (this != &node) {
        refNodePath_ = node.refNodePath_;
        nodeType_ = node.nodeType_;
        inheritIndex_ = node.inheritIndex_;
        inheritCount_ = node.inheritCount_;
        templateSignNum_ = node.templateSignNum_;
        subClasses_ = node.subClasses_;
    }

    return *this;
}

const std::string &ConfigNode::NodeTypeToStr(uint32_t type)
{
    static std::map<uint32_t, std::string> type2StringMap = {
        {NODE_NOREF,     ""             },
        { NODE_COPY,     "NodeCopy"     },
        { NODE_REF,      "NodeReference"},
        { NODE_DELETE,   "NodeDelete"   },
        { NODE_INHERIT,  "NodeInherit"  },
        { NODE_TEMPLATE, "NodeTemplate" },
    };
    return type2StringMap[type];
}

std::ostream &OHOS::Hardware::operator<<(std::ostream &stream, const ConfigNode &t)
{
    stream << "[node] " << t.name_.data() << " " << ConfigNode::NodeTypeToStr(t.nodeType_).data() << " "
           << t.refNodePath_.data();
    return stream;
}

ConfigNode *ConfigNode::CastFrom(const std::shared_ptr<AstObject> &astObject)
{
    return dynamic_cast<ConfigNode *>(astObject.get());
}

uint32_t ConfigNode::GetNodeType() const
{
    return nodeType_;
}

const std::string &ConfigNode::GetRefPath()
{
    return refNodePath_;
}

bool ConfigNode::Merge(std::shared_ptr<AstObject> &srcObj)
{
    if (srcObj == nullptr) {
        return true;
    }
    if (!srcObj->IsNode() || srcObj->Name() != name_) {
        Logger().Error() << SourceInfo() << "merge conflict type with " << srcObj->SourceInfo();
        return false;
    }

    auto srcNode = ConfigNode::CastFrom(srcObj);
    if (srcNode->GetNodeType() == DELETE) {
        srcObj->Separate();
        this->Separate();
        return true;
    }

    nodeType_ = srcNode->nodeType_;
    refNodePath_ = srcNode->refNodePath_;

    auto childSrc = srcObj->Child();
    while (childSrc != nullptr) {
        auto childSrcNext = childSrc->Next();
        auto childDst = Lookup(childSrc->Name(), childSrc->Type());
        if (childDst == nullptr) {
            childSrc->Separate();
            AddChild(childSrc);
        } else {
            if (!childDst->Merge(childSrc)) {
                return false;
            }
        }
        childSrc = childSrcNext;
    }
    return true;
}

void ConfigNode::SetNodeType(uint32_t nodeType)
{
    nodeType_ = nodeType;
}

void ConfigNode::SetRefPath(std::string ref)
{
    refNodePath_ = std::move(ref);
}

bool ConfigNode::HasDuplicateChild()
{
    std::map<std::string, std::shared_ptr<AstObject>> symMap;
    auto child = child_;
    while (child != nullptr) {
        auto sym = symMap.find(child->Name());
        if (sym != symMap.end()) {
            Logger().Error() << child->SourceInfo() << "redefined, first definition at " << sym->second->SourceInfo();
            return true;
        }
        symMap[child->Name()] = child;
        child = child->Next();
    }

    return false;
}

bool ConfigNode::InheritExpand(const std::shared_ptr<AstObject> &refObj)
{
    if (refObj == nullptr) {
        Logger().Error() << SourceInfo() << "inherit invalid node: " << refNodePath_;
        return false;
    }

    if (!Copy(refObj, false)) {
        return false;
    }

    auto templateNode = ConfigNode::CastFrom(refObj);
    if (!Compare(*templateNode)) {
        return false;
    }
    inheritIndex_ = templateNode->inheritCount_++;
    templateNode->subClasses_.push_back(this);
    return true;
}

bool ConfigNode::RefExpand(const std::shared_ptr<AstObject> &refObj)
{
    if (nodeType_ == NODE_DELETE) {
        this->Separate();
        return true;
    }

    if (refObj->IsElders(std::shared_ptr<AstObject>(this, [](auto p) {}))) {
        Logger().Error() << SourceInfo() << "circular reference " << refObj->SourceInfo();
        return false;
    }

    bool ret = true;
    if (nodeType_ == NODE_REF) {
        ret = NodeRefExpand(refObj);
    } else if (nodeType_ == NODE_COPY) {
        ret = NodeCopyExpand(refObj);
    }

    return ret;
}

bool ConfigNode::Copy(std::shared_ptr<AstObject> src, bool overwrite)
{
    auto child = src->Child();
    while (child != nullptr) {
        auto dst = Lookup(child->Name(), child->Type());
        if (dst == nullptr) {
            AddChild(AstObjectFactory::Build(child));
        } else {
            if (!dst->Copy(child, overwrite)) {
                return false;
            }
        }
        child = child->Next();
    }

    return true;
}

bool ConfigNode::Move(std::shared_ptr<AstObject> src)
{
    return AstObject::Move(src);
}

bool ConfigNode::NodeRefExpand(const std::shared_ptr<AstObject> &ref)
{
    if (ref == nullptr) {
        Logger().Error() << SourceInfo() << "reference node '" << refNodePath_ << "' not exist";
        return false;
    }
    return ref->Move(std::shared_ptr<AstObject>(this, [](AstObject *p) { (void)p; }));
}

bool ConfigNode::NodeCopyExpand(const std::shared_ptr<AstObject> &ref)
{
    if (ref == nullptr) {
        Logger().Error() << SourceInfo() << "copy node '" << refNodePath_ << "' not exist";
        return false;
    }
    this->nodeType_ = NODE_NOREF;
    return Copy(ref, false);
}

bool ConfigNode::Compare(ConfigNode &other)
{
    auto objChild = child_;
    while (objChild != nullptr) {
        auto baseObj = Lookup(objChild->Name(), objChild->Type());
        if (baseObj == nullptr) {
            Logger().Error() << objChild->SourceInfo() << "not in template node: " << other.SourceInfo();
            return false;
        }
        if (objChild->IsNode()) {
            return ConfigNode::CastFrom(objChild)->Compare(*ConfigNode::CastFrom(baseObj));
        }

        objChild = objChild->Next();
    }
    return true;
}

uint32_t ConfigNode::InheritIndex()
{
    return inheritIndex_;
}

uint32_t ConfigNode::InheritCount()
{
    return inheritCount_;
}

uint32_t ConfigNode::TemplateSignNum()
{
    return templateSignNum_;
}

void ConfigNode::SetTemplateSignNum(uint32_t sigNum)
{
    templateSignNum_ = sigNum;
}

const std::list<AstObject *> &ConfigNode::SubClasses()
{
    return subClasses_;
}

ConfigTerm::ConfigTerm(const ConfigTerm &term) : ConfigTerm(term.name_, nullptr)
{
    AstObject::AddChild(AstObjectFactory::Build(term.child_));
}

ConfigTerm::ConfigTerm(std::string name, const std::shared_ptr<AstObject> &value) :
    AstObject(std::move(name), PARSEROP_CONFTERM, 0),
    signNum_(0)
{
    if (value != nullptr) {
        child_ = value;
        value->SetParent(this);
    }
}

ConfigTerm::ConfigTerm(Token &name, const std::shared_ptr<AstObject> &value) :
    AstObject(name.strval, PARSEROP_CONFTERM, 0, name),
    signNum_(0)
{
    if (value != nullptr) {
        child_ = value;
        value->SetParent(this);
    }
}

ConfigTerm& ConfigTerm::operator=(const ConfigTerm &term)
{
    if (this != &term) {
        refNode_ = term.refNode_;
        signNum_ = term.signNum_;
    }

    return *this;
}

std::ostream &OHOS::Hardware::operator<<(std::ostream &stream, const ConfigTerm &t)
{
    stream << "[term] " << t.name_.data();
    return stream;
}

ConfigTerm *ConfigTerm::CastFrom(const std::shared_ptr<AstObject> &astObject)
{
    return dynamic_cast<ConfigTerm *>(astObject.get());
}

bool ConfigTerm::Merge(std::shared_ptr<AstObject> &srcObj)
{
    if (!srcObj->IsTerm()) {
        Logger().Error() << SourceInfo() << "merge conflict type with " << srcObj->SourceInfo();
        return false;
    }

    auto value = srcObj->Child();
    srcObj->Child()->Separate();
    child_ = nullptr;
    AddChild(value);
    return true;
}

bool ConfigTerm::RefExpand(const std::shared_ptr<AstObject> refObj)
{
    if (child_->Type() == PARSEROP_DELETE) {
        this->Separate();
        return true;
    }

    if (child_->Type() != PARSEROP_NODEREF) {
        return true;
    }

    if (refObj == nullptr || !refObj->IsNode() || ConfigNode::CastFrom(refObj)->GetNodeType() == NODE_REF ||
        ConfigNode::CastFrom(refObj)->GetNodeType() == NODE_TEMPLATE ||
        ConfigNode::CastFrom(refObj)->GetNodeType() == NODE_DELETE) {
        Logger().Error() << SourceInfo() << "reference invalid node '" << child_->StringValue() << '\'';
        return false;
    }

    refNode_ = refObj;
    return true;
}

bool ConfigTerm::Copy(std::shared_ptr<AstObject> src, bool overwrite)
{
    if (!overwrite) {
        return true;
    }
    if (child_->Type() != src->Child()->Type() && (!child_->IsNumber() || !src->Child()->IsNumber())) {
        Logger().Error() << src->SourceInfo() << "overwrite different type with:" << child_->SourceInfo();
        return false;
    }
    return child_->Copy(src->Child(), overwrite);
}

bool ConfigTerm::Move(std::shared_ptr<AstObject> src)
{
    return child_->Move(src->Child());
}

std::weak_ptr<AstObject> ConfigTerm::RefNode()
{
    return refNode_;
}

void ConfigTerm::SetSigNum(uint32_t sigNum)
{
    signNum_ = sigNum;
}

uint32_t ConfigTerm::SigNum()
{
    return signNum_;
}

ConfigArray::ConfigArray() : AstObject("", PARSEROP_ARRAY, 0), arrayType_(0), arraySize_(0)
{
}

ConfigArray::ConfigArray(const ConfigArray &array) : ConfigArray()
{
    auto child = array.child_;
    while (child != nullptr) {
        AstObject::AddChild(AstObjectFactory::Build(child));
        child = child->Next();
    }
    arraySize_ = array.arraySize_;
    arrayType_ = array.arrayType_;
}

ConfigArray::ConfigArray(const Token &bindToken) :
    AstObject("", PARSEROP_ARRAY, 0, bindToken),
    arrayType_(0),
    arraySize_(0) {};

ConfigArray& ConfigArray::operator=(const ConfigArray &array)
{
    if (this != &array) {
        arrayType_ = array.arrayType_;
        arraySize_ = array.arraySize_;
    }

    return *this;
}

bool ConfigArray::AddChild(const std::shared_ptr<AstObject> &childObj)
{
    if (AstObject::AddChild(childObj)) {
        arraySize_++;
        arrayType_ = std::max(arrayType_, childObj->Type());
        return true;
    } else {
        return false;
    }
}

bool ConfigArray::Merge(std::shared_ptr<AstObject> &srcObj)
{
    if (!srcObj->IsArray()) {
        Logger().Error() << SourceInfo() << "merge conflict type with " << srcObj->SourceInfo();
        return false;
    }

    auto value = srcObj->Child();
    value->Separate();
    child_ = value;
    return true;
}

bool ConfigArray::Copy(std::shared_ptr<AstObject> src, bool overwrite)
{
    if (!overwrite) {
        return true;
    }
    auto array = ConfigArray::CastFrom(src);
    child_ = nullptr;
    auto t = array->child_;
    while (t != nullptr) {
        AddChild(AstObjectFactory::Build(t));
    }
    return true;
}

ConfigArray *ConfigArray::CastFrom(const std::shared_ptr<AstObject> &astObject)
{
    return static_cast<ConfigArray *>(astObject.get());
}

uint16_t ConfigArray::ArraySize()
{
    return arraySize_;
}

uint16_t ConfigArray::ArrayType()
{
    return arrayType_;
}

std::shared_ptr<AstObject> AstObjectFactory::Build(std::shared_ptr<AstObject> object)
{
    switch (object->Type()) {
        case PARSEROP_CONFNODE:
            return std::make_shared<ConfigNode>(*ConfigNode::CastFrom(object));
        case PARSEROP_CONFTERM:
            return std::make_shared<ConfigTerm>(*ConfigTerm::CastFrom(object));
        case PARSEROP_ARRAY:
            return std::make_shared<ConfigArray>(*ConfigArray::CastFrom(object));
        default:
            return std::make_shared<AstObject>(*object);
    }
}

void Ast::Dump(const std::string &prefix)
{
    Logger().Debug() << "Dump " << prefix << " AST:";
    WalkForward([](const std::shared_ptr<AstObject> &current, int32_t walkDepth) -> int32_t {
        Logger().Debug() << ::std::setw(walkDepth * 4) << " " << *current;
        return NOERR;
    });
}

std::shared_ptr<AstObject> Ast::GetAstRoot()
{
    return astRoot_;
}

bool Ast::Merge(const std::list<std::shared_ptr<Ast>> &astList)
{
    if (!RedefineCheck()) {
        return false;
    }
    for (auto &astIt : astList) {
        if (!astIt->RedefineCheck()) {
            return false;
        }
        if (astRoot_ == nullptr) {
            astRoot_ = astIt->GetAstRoot();
            continue;
        }
        Dump("merge this");
        astIt->Dump("tobe merge");
        if (!astRoot_->Merge(astIt->astRoot_)) {
            return false;
        }
        astIt->Dump("merged");
    }
    return true;
}

bool Ast::Expand()
{
    if (!RedefineCheck()) {
        return false;
    }

    if (astRoot_->Lookup("module", PARSEROP_CONFTERM) == nullptr) {
        Logger().Error() << astRoot_->SourceInfo() << "miss 'module' attribute under root node";
        return false;
    }

    if (!NodeExpand()) {
        return false;
    }

    if (!InheritExpand()) {
        return false;
    };

    Dump("expanded");
    return true;
}

bool Ast::NodeExpand()
{
    return WalkBackward([this](const std::shared_ptr<AstObject> &current, int32_t walkDepth) -> int32_t {
        (void)walkDepth;
        if (current->IsNode()) {
            auto node = ConfigNode::CastFrom(current);
            if (node->GetNodeType() == NODE_DELETE) {
                current->Remove();
                return NOERR;
            }
            if (node->GetNodeType() != NODE_REF && node->GetNodeType() != NODE_COPY) {
                return NOERR;
            }
            // current node maybe deleted after reference expand, never use it after this
            auto ret = node->RefExpand(Lookup(current, node->GetRefPath()));
            if (!ret) {
                return EFAIL;
            }
        } else if (current->IsTerm()) {
            std::shared_ptr<AstObject> ref;
            if (current->child_->Type() == PARSEROP_DELETE) {
                current->Remove();
                return NOERR;
            }
            if (current->child_->Type() == PARSEROP_NODEREF) {
                ref = Lookup(current, current->child_->StringValue());
                if (!ConfigTerm::CastFrom(current)->RefExpand(ref)) {
                    return EFAIL;
                }
            }
        }
        return NOERR;
    });
}

bool Ast::InheritExpand()
{
    return WalkForward([this](const std::shared_ptr<AstObject> &current, int32_t) -> int32_t {
        if (current->IsNode()) {
            auto node = ConfigNode::CastFrom(current);
            if (node->GetNodeType() != NODE_INHERIT) {
                return NOERR;
            }
            auto inherit = Lookup(current, node->GetRefPath());
            if (!node->InheritExpand(inherit)) {
                return EFAIL;
            }
        }

        return NOERR;
    });
}

bool Ast::RedefineCheck()
{
    if (redefineChecked_) {
        return true;
    }

    bool ret = WalkForward([](const std::shared_ptr<AstObject> &current, int32_t) -> int32_t {
        if (current->IsNode() && current->HasDuplicateChild()) {
            return EFAIL;
        }

        return NOERR;
    });

    redefineChecked_ = true;
    return ret;
}

std::shared_ptr<AstObject> Ast::Lookup(const std::shared_ptr<AstObject> &startObj, const std::string &path)
{
    if (path.find('.') == std::string::npos) {
        return startObj->parent_->Lookup(path);
    }

    auto splitPath = SplitNodePath(path, '.');
    if (splitPath.front() != astRoot_->name_) {
        Logger().Error() << "lookup ast with invalid path, which not begin with 'root': " << path;
        return nullptr;
    }
    splitPath.erase(splitPath.begin());

    std::shared_ptr<AstObject> target = astRoot_;
    std::shared_ptr<AstObject> child = astRoot_->Child();
    for (auto &it : splitPath) {
        // look up children
        target = target->Lookup(it);
        if (target == nullptr) {
            break;
        }
    }

    return target;
}

std::list<std::string> Ast::SplitNodePath(const std::string &path, char separator)
{
    std::list<std::string> splitList;
    std::string temp;
    for (auto c : path) {
        if (c != separator) {
            temp.push_back(c);
        } else {
            if (temp.empty()) {
                splitList.clear();
                break;
            }
            splitList.push_back(temp);
            temp.clear();
        }
    }

    if (temp.empty()) {
        splitList.push_back(path);
    } else {
        splitList.push_back(temp);
    }
    return splitList;
}
