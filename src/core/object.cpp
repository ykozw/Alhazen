#include "pch.hpp"
#include "core/object.hpp"
#include "core/logging.hpp"
#include "core/util.hpp"
#include "core/threadid.hpp"

namespace
{
#pragma warning(disable:4073) // init_segの使用
#pragma warning(disable:4074) // init_segの使用
#pragma init_seg(lib)
std::unordered_map<std::string, std::function<Object*(const ObjectProp&)>> objectCreateFuncs;
std::unordered_map<std::string, std::function<Object*(const ObjectProp&)>> objectCreateFuncs2;

typedef std::function<Object*(const ObjectProp&)> CreateObjectFun;
typedef std::unordered_map<std::string, CreateObjectFun> CreateObjectFunList;
std::unordered_map<std::type_index, CreateObjectFunList> g_allObjectCreateFuncs;
std::unordered_map<std::type_index, std::string> g_type2name;

class Init
{
public:
    Init()
    {
        Job::threadId = 0;
    }
}init;
};

//
void ObjectProp::addAttribute(const std::string& tag, const std::string& value)
{
    attributes_.insert(std::pair<std::string,std::string>(tag,value));
}

//
void ObjectProp::addChild(const ObjectProp& child)
{
    childProps_.push_back(child);
}

//-------------------------------------------------
//
//-------------------------------------------------
std::string typeid2name(const std::type_index& deriClass)
{
    return g_type2name[deriClass];
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
    AL_ASSERT_DEBUG(Job::threadId == 0);
    AL_ASSERT_DEBUG(createObjectFunc != nullptr);
    // TODO: 二重登録検知ができるようにする
    //
    objectCreateFuncs.insert(std::pair<std::string, std::function<Object*(const ObjectProp&)>>(targetClassName, createObjectFunc));
    // 
    g_type2name.insert(std::make_pair(baseClassType, baseClassName));
    g_type2name.insert(std::make_pair(targetClassType, targetClassName));
    //
    g_allObjectCreateFuncs[baseClassType][targetClassName] = createObjectFunc;
}

//-------------------------------------------------
// createObjectOld
//-------------------------------------------------
Object* createObjectCore(const std::string& typeName, const ObjectProp& objectProp)
{
    AL_ASSERT_DEBUG( objectCreateFuncs.find(typeName) != objectCreateFuncs.end() );
    const auto& objCreateFunc = objectCreateFuncs[typeName];
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
    auto baseClassTypeIte = g_allObjectCreateFuncs.find(baseClassType);
    AL_ASSERT_DEBUG(baseClassTypeIte != g_allObjectCreateFuncs.end());
    CreateObjectFunList& createObjectFunList = baseClassTypeIte->second;
    auto createObjectIte = createObjectFunList.find(targetClassName);
    AL_ASSERT_DEBUG(createObjectIte != createObjectFunList.end());
    const auto& objCreateFunc = createObjectIte->second;
    return objCreateFunc(objectProp);
}
