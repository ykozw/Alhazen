#include "pch.hpp"
#include "core/object.hpp"
#include "core/util.hpp"

/*
-------------------------------------------------
ObjectPropString()
-------------------------------------------------
*/
ObjectPropString::ObjectPropString()
    :valid_(false)
{

}

/*
-------------------------------------------------
ObjectPropString()
-------------------------------------------------
*/
ObjectPropString::ObjectPropString(const std::string& value)
    :valid_((value != "")),
    value_(value)
{
}

/*
-------------------------------------------------
-------------------------------------------------
*/
const std::string& ObjectPropString::value()const
{
    return value_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool ObjectPropString::valid()
{
    return valid_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
ObjectPropString::operator bool()const
{
    return valid_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
std::string ObjectPropString::asString(const std::string& defaultValue) const
{
    if (valid_)
    {
        return value_;
    }
    else
    {
        return defaultValue;
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
ObjectProp::ObjectProp(const std::string& str)
{
    createFromString(str);
}

/*
-------------------------------------------------
createFromFile()
-------------------------------------------------
*/
bool ObjectProp::createFromFile(const std::string& fileName)
{
    //
    IStream* stream = NULL;
    AL_VALID(SUCCEEDED(SHCreateStreamOnFile(fileName.c_str(), STGM_READ, &stream)));
    return load(stream);
}

/*
-------------------------------------------------
createFromString()
-------------------------------------------------
*/
bool ObjectProp::createFromString(const std::string& str)
{
    //
    IStream* stream = SHCreateMemStream((BYTE*)str.c_str(), static_cast<UINT>(str.size()));
    return load(stream);
}

/*
-------------------------------------------------
load()
-------------------------------------------------
*/
bool ObjectProp::load2(const std::string& fileName)
{
    // TODO: xmlのロード
    using namespace tinyxml2;
    tinyxml2::XMLDocument xml;
    xml.LoadFile(fileName.c_str());

    // sceneノードがルート
    auto sceneNode = xml.FirstChildElement("scene");
    つぎはここから
    auto child = sceneNode->FirstChild();
    while (child != nullptr)
    {
        const char* v = child->Value();
        auto element = child->ToElement();
        const char* name = element->Name();
        auto attribute = element->FirstAttribute();
        while (attribute != nullptr)
        {
            const char* name = attribute->Name();
            const char* value = attribute->Value();
            // 次のアトリビュートへ
            attribute = attribute->Next();
        }

        // 次の子へ。
        child = child->NextSibling();
    }

    {
        const char* value = sceneNode->Value();
        auto dec = sceneNode->ToDeclaration();
        auto doc = sceneNode->ToDocument();
        auto elem = sceneNode->ToElement();
        auto text = sceneNode->ToText();
    }
    auto* firstNode = xml.FirstChild();
    {
        const char* value = firstNode->Value();
        auto dec = firstNode->ToDeclaration();
        auto doc = firstNode->ToDocument();
        auto elem = firstNode->ToElement();
        auto text = firstNode->ToText();
    }
    //
    return true;
}

/*
-------------------------------------------------
load()
-------------------------------------------------
*/
bool ObjectProp::load(IStream* stream)
{
    //-------------------------------------------------
    // SceneFileNode
    //-------------------------------------------------
    struct SceneDocumentNode
    {
    public:
        std::string category;
        int32_t depth;
        std::vector<std::pair<std::string, std::string>> attributes;
        SceneDocumentNode* parent;
        SceneDocumentNode* firstChild;
        SceneDocumentNode* nextSibling;
        //
        class SceneDocument* sceneFile;
    public:
        SceneDocumentNode()
            :category("none"),
            depth(-1),
            parent(NULL),
            firstChild(NULL),
            nextSibling(NULL),
            sceneFile(NULL)
        {}
    };
    //
    IXmlReader* reader = NULL;
    AL_VALID(SUCCEEDED(CreateXmlReader(__uuidof(IXmlReader), reinterpret_cast<void**>(&reader), 0)));
    AL_VALID(SUCCEEDED(reader->SetInput(stream)));
    //
    std::vector<SceneDocumentNode> nodes_;
    const auto w2c = [](LPCWSTR src)
    {
        const int32_t MAX_COUNT = 0xFF * 16;
        size_t convNum = 0;
        char converted[MAX_COUNT];
        wcstombs_s(&convNum, converted, src, MAX_COUNT);
        return std::string(converted);
    };
    //
    XmlNodeType nodeType;
    reader->MoveToNextAttribute();
    while (reader->Read(&nodeType) == S_OK)
    {
        switch (nodeType)
        {
        case XmlNodeType_None:
            break;
        case XmlNodeType_Element:
        {
            SceneDocumentNode newNode;
            LPCWSTR localName;
            AL_VALID(SUCCEEDED(reader->GetLocalName(&localName, NULL)));
            newNode.category = w2c(localName);
            HRESULT hr = reader->MoveToFirstAttribute();
            if (S_FALSE == hr)
            {
                break;
            }
            AL_ASSERT_DEBUG(S_OK == hr);
            UINT atributeCount = 0;
            UINT depth;
            reader->GetAttributeCount(&atributeCount);
            reader->GetDepth(&depth);
            newNode.depth = (int32_t)depth;
            LPCWSTR pwszAttributeName;
            LPCWSTR pwszAttributeValue;
            do
            {
                AL_VALID(SUCCEEDED(reader->GetLocalName(&pwszAttributeName, NULL)));
                AL_VALID(SUCCEEDED(reader->GetValue(&pwszAttributeValue, NULL)));
                const std::string key = w2c(pwszAttributeName);
                const std::string value = w2c(pwszAttributeValue);
                newNode.attributes.emplace_back(std::make_pair(key,value));
            }
            while (reader->MoveToNextAttribute() == S_OK);
            nodes_.emplace_back(newNode);
        }
        break;
        case XmlNodeType_Attribute:
        case XmlNodeType_Text:
        case XmlNodeType_CDATA:
        case XmlNodeType_ProcessingInstruction:
        case XmlNodeType_Comment:
        case XmlNodeType_DocumentType:
        case XmlNodeType_Whitespace:
        case XmlNodeType_EndElement:
        case XmlNodeType_XmlDeclaration:
            break;
        default:
            AL_ASSERT_DEBUG(false);
            break;
        }
    }
    reader->Release();
    stream->Release();
    //
    std::array<int32_t, 16> lastNodePerDepth;
    lastNodePerDepth.fill(0);
    for (int32_t nodeNo = 1; nodeNo < nodes_.size(); ++nodeNo)
    {
        const int32_t depth = nodes_[nodeNo].depth;
        // 「一つ上の深さのノードのインデックス」 < 「最後に同じ深さをもつノードのインデックス」のときは同じ深さが兄弟になっている
        if (lastNodePerDepth[depth - 1] < lastNodePerDepth[depth])
        {
            nodes_[lastNodePerDepth[depth]].nextSibling = &nodes_[nodeNo];
        }
        lastNodePerDepth[depth] = nodeNo;
        const int32_t parentIndex = lastNodePerDepth[depth - 1];
        nodes_[nodeNo].parent = &nodes_[parentIndex];
        // ひとつ前が一段上であればそれは第一子
        auto& prevNode = nodes_[nodeNo - 1];
        if (depth - prevNode.depth == 1)
        {
            prevNode.firstChild = &nodes_[nodeNo];
        }
        // 同じ深さであれば兄弟
        else if (depth - prevNode.depth == 0)
        {
            prevNode.nextSibling = &nodes_[nodeNo];
        }
    }
    //
    const std::function<void(ObjectProp*,SceneDocumentNode*)>constructObjectProp =
        [&constructObjectProp](ObjectProp* objProp, SceneDocumentNode* node)
    {
        objProp->tag_ = node->category;
        // アトリビュートのコピー
        for (const auto& att : node->attributes)
        {
            objProp->attributes_[att.first] = att.second;
        }
        // 子のコピー
        auto* child = node->firstChild;
        while (child != NULL)
        {
            //
            ObjectProp childProp;
            constructObjectProp(&childProp, child);
            objProp->childProps_.push_back(childProp);
            // 次の兄弟に移動
            child = child->nextSibling;
        }
    };
    // ObjectPropとして構築しなおす
    ObjectProp objectProp;
    constructObjectProp(this, &nodes_[0]);
    return true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool ObjectPropString::asBool(bool defaultValue) const
{
    if (!valid_)
    {
        // TODO: warning
        return defaultValue;
    }
    int32_t v = false;
    if (value_ == "true")
    {
        return true;
    }
    else if (value_ == "false")
    {
        return false;
    }
    else if (sscanf_s(value_.c_str(), "%d", &v) == 1)
    {
        return v != 0;
    }
    else
    {
        // TODO: warning
        return defaultValue;
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
int32_t ObjectPropString::asInt(int32_t defaultValue) const
{
    int32_t v = 0;
    if (sscanf(value_.c_str(), "%d", &v) != 1)
    {
        // TODO: warning
        return defaultValue;
    }
    return v;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float ObjectPropString::asFloat(float defaultValue) const
{
    float v = 0;
    if (sscanf(value_.c_str(), "%f", &v) != 1)
    {
        // TODO: warning
        return defaultValue;
    }
    return v;
}

/*
-------------------------------------------------
asVec3()
-------------------------------------------------
*/
Vec3 ObjectPropString::asVec3(const Vec3& defaultValue) const
{
    if (valid_)
    {
        Vec3 ret;
        if (sscanf(value_.c_str(), "%f, %f, %f", &ret.x, &ret.y, &ret.z) != 3)
        {
            // TODO: warning
            return defaultValue;
        }
        return ret;
    }
    else
    {
        return defaultValue;
    }
}

/*
-------------------------------------------------
attribute()
-------------------------------------------------
*/
ObjectPropString  ObjectProp::attribute(const std::string& attributeName) const
{
    auto ite = attributes_.find(attributeName);
    if (ite == attributes_.end())
    {
        return ObjectPropString();
    }
    return ObjectPropString(ite->second);
}

/*
-------------------------------------------------
findChildBy()
-------------------------------------------------
*/
ObjectProp ObjectProp::findChildBy(const std::string& attributeName, const std::string& attributeValue) const
{
    for (const auto& cp : childProps_)
    {
        const auto attr = cp.attribute(attributeName);
        if (attr && attr.asString("") == attributeValue)
        {
            return cp;
        }
    }
    return ObjectProp();
}

/*
-------------------------------------------------
findChildByTag()
-------------------------------------------------
*/
ObjectProp ObjectProp::findChildByTag(const std::string& tagName) const
{
    for (const auto& cp : childProps_)
    {
        if (cp.tag() == tagName)
        {
            return cp;
        }
    }
    loggingWarning("Typename:%s is not found.", tagName.c_str());
    return ObjectProp();
}

/*
-------------------------------------------------
-------------------------------------------------
*/
const ObjectProp::Attributes& ObjectProp::attributes() const
{
    return attributes_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
const ObjectProp::ChildProps& ObjectProp::childProps() const
{
    return childProps_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
std::string ObjectProp::tag() const
{
    return tag_;
}

/*
-------------------------------------------------
isEmpty()
-------------------------------------------------
*/
bool ObjectProp::isEmpty() const
{
    return
        attributes_.empty() &&
        childProps_.empty();
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool ObjectProp::asBool(bool defaultValue) const
{
    return attribute("value").asBool(defaultValue);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
int32_t ObjectProp::asInt(int32_t defaultValue) const
{
    return attribute("value").asInt(defaultValue);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float ObjectProp::asFloat(float defaultValue) const
{
    const ObjectPropString attr = attribute("value");
    int32_t v = 0;
    if (!attr)
    {
        // TODO: warning
        return defaultValue;
    }
    const auto value = attr.value();
    float ret;
    if (sscanf(value.c_str(), "%f", &ret) != 1)
    {
        // TODO: warning
        return defaultValue;
    }
    return ret;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
std::string ObjectProp::asString(const std::string& defaultValue) const
{
    return attribute("value").asString(defaultValue);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Vec3 ObjectProp::asXYZ(const Vec3& defaultValue) const
{
    //
    ObjectPropString ax = attribute("x");
    ObjectPropString ay = attribute("y");
    ObjectPropString az = attribute("z");
    if (!ax || !ay || !az)
    {
        return defaultValue;
    }
    //
    Vec3 ret;
    if (sscanf(ax.value().c_str(), "%f", &ret.x) != 1 ||
        sscanf(ay.value().c_str(), "%f", &ret.y) != 1 ||
        sscanf(az.value().c_str(), "%f", &ret.z) != 1)
    {
        // TODO: warning
        return defaultValue;
    }
    return ret;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float ObjectProp::asAngle(float defaultValue) const
{
    return attribute("angle").asFloat(defaultValue);
}
