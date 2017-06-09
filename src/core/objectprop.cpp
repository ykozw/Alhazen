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
ObjectProp::ObjectProp(const std::string& tag, const std::string& name, const std::string& value)
{
    tag_ = tag;
    addAttribute("name", name);
    addAttribute("value", value);
}

/*
-------------------------------------------------
load()
-------------------------------------------------
*/
bool ObjectProp::load(const std::string& fileName)
{
    // xmlのロード
    using namespace tinyxml2;
    tinyxml2::XMLDocument xml;
    xml.LoadFile(fileName.c_str());

    // sceneノードがルート
    auto sceneNode = xml.FirstChildElement("scene");
    ObjectProp root;
    loadSub( sceneNode, root);
    *this = root.childProps_[0];
    //
    return true;
}

/*
-------------------------------------------------
load()
-------------------------------------------------
*/
void ObjectProp::loadSub(tinyxml2::XMLNode* node, ObjectProp& parentProp)
{
    ObjectProp objectProp;
    auto element = node->ToElement();
    if (element)
    {
        const char* tag = element->Name();
        objectProp.tag_ = tag;
        auto attribute = element->FirstAttribute();
        while (attribute != nullptr)
        {
            const char* name = attribute->Name();
            const char* value = attribute->Value();
            objectProp.attributes_[name] = value;
            //
            attribute = attribute->Next();
        }
    }
    // 子を巡回
    auto* child = node->FirstChild();
    auto* elm = node->FirstChildElement();
    const bool noChild = node->NoChildren();
    for (auto childNode = node->FirstChild(); childNode != nullptr;)
    {
        loadSub(childNode, objectProp);
        childNode = childNode->NextSibling();
    }
    //
    if (!objectProp.isEmpty())
    {
        parentProp.childProps_.push_back(objectProp);
    }
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
void ObjectPropString::setString(const std::string& v)
{
    value_ = v;
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
void ObjectPropString::setBool(bool v)
{
    value_ = std::to_string(v);
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
void ObjectPropString::setInt(int32_t v)
{
    value_ = std::to_string(v);
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
void ObjectPropString::setFloat(float v)
{
    value_ = std::to_string(v);
}
/*
 -------------------------------------------------
 -------------------------------------------------
 */
void ObjectPropString::setVec3(const Vec3& v)
{
    value_ =
    std::to_string(v.x) + "," +
    std::to_string(v.y) +"," +
    std::to_string(v.z);
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
    else if (sscanf(value_.c_str(), "%d", &v) == 1)
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
        tag_ == "" &&
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
