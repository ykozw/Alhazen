#include "pch.hpp"
#include "app/alhazen.hpp"
#include "core/unittest.hpp"

#include "core/rng.hpp"
#include "bsdf/bsdf.hpp"

void test()
{
    ObjectProp obj;
    obj.load2("C:\\dev\\alhazen\\asset\\pt\\scene.xml");
}
/*
-------------------------------------------------
main
-------------------------------------------------
*/
int32_t main(int32_t argc, char* argv[])
{
    test();
#if defined(UNIT_TEST)
    doTest();
    return 0;
#else
    Alhazen app;
    return app.run(argc, argv);
#endif
}
