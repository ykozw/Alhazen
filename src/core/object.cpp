#include "pch.hpp"
#include "core/object.hpp"
#include "core/logging.hpp"
#include "core/util.hpp"

namespace
{
/*
-------------------------------------------------
-------------------------------------------------
*/
typedef std::unordered_map<std::string,
                           std::function<Object *(const ObjectProp &)>>
    ObjectCreateFuncs;
typedef std::function<Object *(const ObjectProp &)> CreateObjectFun;
typedef std::unordered_map<std::string, CreateObjectFun> CreateObjectFunList;

/*
-------------------------------------------------
-------------------------------------------------
*/
class Global
{
public:
    ObjectCreateFuncs &objectCreateFuncs()
    {
        static std::unordered_map<std::string,
                                  std::function<Object *(const ObjectProp &)>>
            objectCreateFuncs;
        return objectCreateFuncs;
    }
    std::unordered_map<std::type_index, CreateObjectFunList> &
    allObjectCreateFuncs()
    {
        static std::unordered_map<std::type_index, CreateObjectFunList>
            allObjectCreateFuncs;
        return allObjectCreateFuncs;
    }
    std::unordered_map<std::type_index, std::string> &type2name()
    {
        static std::unordered_map<std::type_index, std::string> type2name;
        return type2name;
    }
} g;
}; // namespace

/*
-------------------------------------------------
-------------------------------------------------
*/
void ObjectProp::addAttribute(const std::string &tag, const std::string &value)
{
    attributes_.insert(std::pair<std::string, std::string>(tag, value));
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void ObjectProp::addChild(const ObjectProp &child)
{
    childProps_.push_back(child);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
std::string typeid2name(const std::type_index &deriClass)
{
    return g.type2name()[deriClass];
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void registerObject(
    const std::type_index &baseClassType,
    const std::type_index &targetClassType,
    const std::string &baseClassName,
    const std::string &targetClassName,
    std::function<Object *(const ObjectProp &)> createObjectFunc)
{
    logging("registerObject[%s:%s]",
            baseClassName.c_str(),
            targetClassName.c_str());
    AL_ASSERT_ALWAYS(createObjectFunc != nullptr);
    // TODO: 二重登録検知ができるようにする
    //
    g.objectCreateFuncs().insert(
        std::pair<std::string, std::function<Object *(const ObjectProp &)>>(
            targetClassName, createObjectFunc));
    //
    g.type2name().insert(std::make_pair(baseClassType, baseClassName));
    g.type2name().insert(std::make_pair(targetClassType, targetClassName));
    //
    g.allObjectCreateFuncs()[baseClassType][targetClassName] = createObjectFunc;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Object *createObjectCore(const std::string &typeName,
                         const ObjectProp &objectProp)
{
    AL_ASSERT_ALWAYS(g.objectCreateFuncs().find(typeName) !=
                     g.objectCreateFuncs().end());
    const auto &objCreateFunc = g.objectCreateFuncs()[typeName];
    AL_ASSERT_ALWAYS(objCreateFunc != nullptr);
    return objCreateFunc(objectProp);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Object *createObjectCore(std::type_index baseClassType,
                         const std::string &targetClassName,
                         const ObjectProp &objectProp)
{
    //
    logging(
        "Create Object[%s::%s]", baseClassType.name(), targetClassName.c_str());
    //
    auto baseClassTypeIte = g.allObjectCreateFuncs().find(baseClassType);
    AL_ASSERT_ALWAYS(baseClassTypeIte != g.allObjectCreateFuncs().end());
    CreateObjectFunList &createObjectFunList = baseClassTypeIte->second;
    auto createObjectIte = createObjectFunList.find(targetClassName);
    AL_ASSERT_ALWAYS(createObjectIte != createObjectFunList.end());
    const auto &objCreateFunc = createObjectIte->second;
    return objCreateFunc(objectProp);
}
