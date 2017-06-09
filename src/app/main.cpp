#include "pch.hpp"
#include "app/alhazen.hpp"
#include "bsdf/bsdf.hpp"
#include "core/rng.hpp"
#include "core/util.hpp"
#include "core/unittest.hpp"

/*
-------------------------------------------------
main
-------------------------------------------------
*/
int32_t main(int32_t argc, char* argv[])
{    
#if defined(UNIT_TEST)
    doTest();
    return 0;
#else
    Alhazen app;
    return app.run(argc, argv);
#endif
}
