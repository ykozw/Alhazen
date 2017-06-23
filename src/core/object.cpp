#include "pch.hpp"
#include "core/object.hpp"
#include "core/logging.hpp"
#include "core/util.hpp"
#include "core/threadid.hpp"

namespace
{
    /*
     -------------------------------------------------
     -------------------------------------------------
     */
    class Init
    {
    public:
        Init()
        {
            Job::threadId = 0;
        }
    }init;
    
    /*
     -------------------------------------------------
     -------------------------------------------------
     */
    typedef std::unordered_map<std::string, std::function<Object*(const ObjectProp&)>> ObjectCreateFuncs;
    typedef std::function<Object*(const ObjectProp&)> CreateObjectFun;
    typedef std::unordered_map<std::string, CreateObjectFun> CreateObjectFunList;
    
    /*
     -------------------------------------------------
     -------------------------------------------------
     */
    class Global
    {
    public:
        ObjectCreateFuncs& objectCreateFuncs()
        {
            static std::unordered_map<std::string, std::function<Object*(const ObjectProp&)>> objectCreateFuncs;
            return objectCreateFuncs;
        }
        std::unordered_map<std::type_index, CreateObjectFunList>& allObjectCreateFuncs()
        {
            static std::unordered_map<std::type_index, CreateObjectFunList> allObjectCreateFuncs;
            return allObjectCreateFuncs;
        }
        std::unordered_map<std::type_index, std::string>& type2name()
        {
            static std::unordered_map<std::type_index, std::string> type2name;
            return type2name;
        }
    }g;
};


/*
 -------------------------------------------------
 -------------------------------------------------
 */
void ObjectProp::addAttribute(const std::string& tag, const std::string& value)
{
    attributes_.insert(std::pair<std::string, std::string>(tag, value));
}


/*
 -------------------------------------------------
 -------------------------------------------------
 */
void ObjectProp::addChild(const ObjectProp& child)
{
    childProps_.push_back(child);
}

//-------------------------------------------------
//
//-------------------------------------------------
std::string typeid2name(const std::type_index& deriClass)
{
    return g.type2name()[deriClass];
}

//-------------------------------------------------
// registerObject
//-------------------------------------------------
void registerObject(
    const std::type_index& baseClassType,
    const std::type_index& targetClassType,
    const std::string& baseClassName,
    const std::string& targetClassName,
    std::function<Object*(const ObjectProp&)> createObjectFunc)
{
    printf("registerObject[%s:%s]\n", baseClassType.name(), targetClassName.c_str());
    // Global Constructor内で行われること前提
    // AL_ASSERT_DEBUG(Job::threadId == 0);
    AL_ASSERT_DEBUG(createObjectFunc != nullptr);
    // TODO: 二重登録検知ができるようにする
    //
    g.objectCreateFuncs().insert(std::pair<std::string, std::function<Object*(const ObjectProp&)>>(targetClassName, createObjectFunc));
    // 
    g.type2name().insert(std::make_pair(baseClassType, baseClassName));
    g.type2name().insert(std::make_pair(targetClassType, targetClassName));
    //
    g.allObjectCreateFuncs()[baseClassType][targetClassName] = createObjectFunc;
}

//-------------------------------------------------
// createObjectOld
//-------------------------------------------------
Object* createObjectCore(const std::string& typeName, const ObjectProp& objectProp)
{
    AL_ASSERT_DEBUG(g.objectCreateFuncs().find(typeName) != g.objectCreateFuncs().end());
    const auto& objCreateFunc = g.objectCreateFuncs()[typeName];
    AL_ASSERT_DEBUG(objCreateFunc != NULL);
    return objCreateFunc(objectProp);
}

//-------------------------------------------------
// createObjectOld
//-------------------------------------------------
Object* createObjectCore(
    std::type_index baseClassType,
    const std::string& targetClassName,
    const ObjectProp& objectProp)
{
    //
    logging("CreateObject BaseClass:%s TargetClass:%s", baseClassType.name(), targetClassName.c_str());
    //
    auto baseClassTypeIte = g.allObjectCreateFuncs().find(baseClassType);
    AL_ASSERT_DEBUG(baseClassTypeIte != g.allObjectCreateFuncs().end());
    CreateObjectFunList& createObjectFunList = baseClassTypeIte->second;
    auto createObjectIte = createObjectFunList.find(targetClassName);
    AL_ASSERT_DEBUG(createObjectIte != createObjectFunList.end());
    const auto& objCreateFunc = createObjectIte->second;
    return objCreateFunc(objectProp);
}
