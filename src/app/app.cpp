#include "pch.hpp"
#include "core/util.hpp"
#include "core/logging.hpp"
#include "app/app.hpp"

//-------------------------------------------------
// parseArgs
//-------------------------------------------------
bool parseArgs(int32_t argc, char* argv[], ArgConfig& config)
{
    // シーンファイルの指定がない場合は終了
    if (argc < 2)
    {
        loggingError("scene file is not specified.");
        return false;
    }
    //
    config.sceneFilePath = argv[1];
    g_fileSystem.init(config.sceneFilePath.c_str());

    // オプションの解釈
    bool doTest = false;
    for (int argi = 2; argi < argc; ++argi)
    {
        const char* arg = argv[argi];
        if (strlen(arg) < 2 ||
            arg[0] != '-')
        {
            continue;
        }
        const char opt = arg[1];
        //
        switch (opt)
        {
        case 'l':
            config.dumpLensData = true; // HACK: このオプションは要らないかもしれない
            break;
        case 'f':
            config.floatException = true;
            break;
        default:
            break;
        }
    }
    return true;
}

//-------------------------------------------------
//
//-------------------------------------------------
int32_t App::run(int32_t argc, char* argv[])
{
#if defined(WINDOWS)
    // 高速化のため、Denormalized floating-pointは常に0にしておく。
    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
    _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
#endif
    //
    ArgConfig config;
    if (!parseArgs(argc, argv, config))
    {
        return 0;
    }

#if defined(WINDOWS)
    _chdir(config.baseFileDir.c_str());
#endif
    //
    initialzeLog();
    // 浮動小数関係の例外をonにするか
    if (config.floatException)
    {
#if defined(WINDOWS)
        _controlfp(0, _MCW_EM);
        _controlfp(_EM_INEXACT, _EM_INEXACT);
#endif
    }
    //
    const int32_t ret = runApp(config);
    finalizeLog();
    //
    return ret;
}
