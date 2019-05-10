#pragma once

#include "math.hpp"
#include "logging.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class ObjectPropString AL_FINAL
{
public:
    ObjectPropString();
    ObjectPropString(const std::string& value);
    //
    const std::string& value() const;
    bool valid();
    operator bool() const;
    // 設定系
    void setString(const std::string& v);
    void setBool(bool v);
    void setInt(int32_t v);
    void setFloat(float v);
    void setVec3(const Vec3& v);
    // プリミティブな型のみここで変換を行う
    std::string asString(const std::string& defaultValue) const;
    bool asBool(bool defaultValue) const;
    int32_t asInt(int32_t defaultValue) const;
    float asFloat(float defaultValue) const;
    Vec3 asVec3(Vec3 defaultValue) const;

private:
    bool valid_;
    std::string value_;
};

/*
-------------------------------------------------
-------------------------------------------------
*/
class ObjectProp AL_FINAL
{
public:
    typedef std::unordered_map<std::string, std::string> Attributes;
    typedef std::vector<ObjectProp> ChildProps;

public:
    //
    ObjectProp() = default;
    ObjectProp(const ObjectProp& other) = default;
    ObjectProp(const std::string& tag,
               const std::string& name,
               const std::string& value);
    // 生成関連
    bool load(const std::string& fileName);
    // 手動生成系
    void addAttribute(const std::string& tag, const std::string& value);
    void addChild(const ObjectProp& child);
    //
    const Attributes& attributes() const;
    const ChildProps& childProps() const;
    //
    std::string tag() const;
    ObjectPropString attribute(const std::string& key) const;
    ObjectProp findChildBy(const std::string& attributeName,
                           const std::string& value) const;
    ObjectProp findChildByTag(const std::string& category) const;
    bool isEmpty() const;
    // 変換
    bool asBool(bool defaultValue) const;
    int32_t asInt(int32_t defaultValue) const;
    float asFloat(float defaultValue) const;
    std::string asString(const std::string& defaultValue) const;
    Vec3 asXYZ(Vec3 defaultValue) const;
    float asAngle(float defaultValue) const;

private:
    void loadSub(tinyxml2::XMLNode* node, ObjectProp& parentProp);

private:
    std::string tag_;
    Attributes attributes_;
    ChildProps childProps_;
};

/*
-------------------------------------------------
-------------------------------------------------
*/
class Object
{
public:
    virtual ~Object() {}
};

/*
-------------------------------------------------
-------------------------------------------------
*/
void
registerObject(const std::type_index& baseClassType,
               const std::type_index& targetClassType,
               const std::string& baseClassName,
               const std::string& targetClassName,
               std::function<Object*(const ObjectProp&)> createObjectFunc);

/*
-------------------------------------------------
-------------------------------------------------
*/
std::string
typeid2name(const std::type_index& deriClass);

/*
-------------------------------------------------
-------------------------------------------------
*/
Object*
createObjectCore(const std::string& typeName, const ObjectProp& objectProp);

Object*
createObjectCore(std::type_index baseClass,
                 const std::string& targetClass,
                 const ObjectProp& objectProp);

/*
-------------------------------------------------
-------------------------------------------------
*/
template<typename BaseClass>
std::shared_ptr<BaseClass>
createObject(const std::string& targetClassName)
{
    return std::shared_ptr<BaseClass>(dynamic_cast<BaseClass*>(
      createObjectCore(typeid(BaseClass), targetClassName, ObjectProp())));
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<typename BaseClass>
std::shared_ptr<BaseClass>
createObject(const ObjectProp& objProp)
{
    //
    std::string baseClassName = typeid2name(typeid(BaseClass));
    // 同じ階層にある場合と、一つ下の階層にある場合が混在している
    const ObjectProp& targetProp = (baseClassName == objProp.tag())
                                     ? objProp
                                     : objProp.findChildByTag(baseClassName);
    const std::string& targetClassName =
      targetProp.attribute("type").asString("");
    AL_ASSERT_DEBUG(targetClassName != "");
    return std::shared_ptr<BaseClass>(dynamic_cast<BaseClass*>(
      createObjectCore(typeid(BaseClass), targetClassName, targetProp)));
}

/*
-------------------------------------------------
-------------------------------------------------
*/
#define REGISTER_OBJECT(BASE_CLASS_TYPE, CLASS_TYPE)                           \
    class Register##CLASS_TYPE##CLASS_NAME                                     \
    {                                                                          \
    public:                                                                    \
        Register##CLASS_TYPE##CLASS_NAME()                                     \
        {                                                                      \
            registerObject(typeid(BASE_CLASS_TYPE),                            \
                           typeid(CLASS_TYPE),                                 \
                           #BASE_CLASS_TYPE,                                   \
                           #CLASS_TYPE,                                        \
                           create);                                            \
        }                                                                      \
        static Object* create(const ObjectProp& prop)                          \
        {                                                                      \
            return new CLASS_TYPE(prop);                                       \
        }                                                                      \
    };                                                                         \
    static Register##CLASS_TYPE##CLASS_NAME register##CLASS_TYPE;

/*
-------------------------------------------------
-------------------------------------------------
*/
class SceneObject : public Object
{};
