#include "pch.hpp"
#include "core/sysinfo.hpp"
#include "core/logging.hpp"

//-------------------------------------------------
// printCacheInfo()
//-------------------------------------------------
bool printCacheInfo()
{
#if defined(WINDOWS)
    std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> infos;
    infos.resize(1);
    for (;;)
    {
        // 取得に成功したら次のフェイズへ移行
        DWORD dataSize = (DWORD)infos.size() * sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
        if (GetLogicalProcessorInformation(infos.data(), &dataSize))
        {
            break;
        }
        // エラーが情報書き込み先ヒープ不足であれば一つ分だけ拡張する
        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
        {
            infos.resize(infos.size() + 1);
        }
        // ERROR_INSUFFICIENT_BUFFER以外の場合は対処不能なので終了
        else
        {
            return false;
        }
    }
    // 専用のデータに入れ直す
    for (const auto& info : infos)
    {
        // Core1のL1キャッシュについて抽出する
        const CACHE_DESCRIPTOR& cache = info.Cache;
        if (!(info.ProcessorMask & 0x01) ||
                !(info.Relationship == RelationCache) ||
                !(cache.Level == 1))
        {
            continue;
        }
        //
        const char* typeStr = "";
        switch (cache.Type)
        {
        case CacheUnified:
            typeStr = "Unified";
            break;
        case CacheInstruction:
            typeStr = "Instruction";
            break;
        case CacheData:
            typeStr = "Data";
            break;
        case CacheTrace:
            typeStr = "Trace";
            break;
        }
        //
        logging("L1 Cache(%s)", typeStr);
        logging("  Size:0x%x", cache.Size);
        logging("  LineSize:0x%x", cache.LineSize);
        logging("  LineNum:%d", cache.Size / cache.LineSize);
        logging("  Associativity:%d", cache.Associativity);
    }
    // 最後まで来たので成功
    return true;
#else
    return false;
#endif
}
