#include "pch.hpp"
#include "app/app.hpp"
#include "app/alhazen.hpp"
#include "integrator/integrator.hpp"
#include "sensor/sensor.hpp"
#include "core/image.hpp"
#include "core/math.hpp"
#include "core/scene.hpp"
#include "core/util.hpp"
#include "core/parallelfor.hpp"
#include "core/stats.hpp"
#include "core/floatstreamstats.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
int32_t Alhazen::runApp(const ArgConfig& config)
{
    // シーンの構築
    ObjectProp sceneProp;
    sceneProp.load(config.sceneFilePath);
    Scene scene(sceneProp);
    //
    const int32_t totalTaskNum = scene.totalTaskNum();
    const uint32_t developIntervalInMs = scene.developIntervalInMs();
    const uint32_t timeOutInMs = scene.timeOutInMs();

#if 0
    // デバッグ用の直呼び出し
    auto& scene = scenes[0];
    scene->renderDebug(512,512 - 200);
    AL_ASSERT_ALWAYS(false);
#endif

    // 全てのタスクが消化されるか、時間切れになるまで行う
    int32_t taskNo = 0;
    int32_t filmNo = 0;
    uint32_t nextDevelopTime =
        g_timeUtil.elapseTimeInMs() + developIntervalInMs;
    //
    FloatStreamStats taskTimeStats;

    for (;;)
    {
        //
        if (taskNo >= totalTaskNum)
        {
            logging("All tasks were consumed.");
            break;
        }
        //
        const auto startRenderTime = g_timeUtil.elapseTimeInMs();
        // レンダリング
        const int32_t TASK_NUM_UNTILL_BY_JOIN = 512;
        parallelFor(
            TASK_NUM_UNTILL_BY_JOIN,
            [&](int32_t taskNoOffsetBegin, int32_t taskNoOffsetEnd) {
                CounterStats::preParallel();
                //
                if (g_timeUtil.elapseTimeInMs() >= timeOutInMs)
                {
                    return;
                }
                //
                for (int32_t taskNoOffset = taskNoOffsetBegin;
                     taskNoOffset < taskNoOffsetEnd;
                     ++taskNoOffset)
                {
                    const int32_t taskNoLocal = taskNo + taskNoOffset;
                    if (taskNoLocal >= totalTaskNum)
                    {
                        return;
                    }
                    scene.render(taskNoLocal);
                    // 現像
                    if (nextDevelopTime < g_timeUtil.elapseTimeInMs())
                    {
                        nextDevelopTime +=
                            developIntervalInMs; // TODO:
                                                 // 複数回来ることがあり得る
#if 0 // 連番で出す場合
                                filmNo++;
                                // 最終ショット以外はdenoiseは走らせない
                                std::ostringstream ss;
                                ss << std::setfill('0') << std::setw(3) << filmNo;
                                scene.developLDR(ss.str() + ".png", false);
                                scene.dumpHDR(ss.str() + ".bhdr");
#else // 同じイメージで出し続ける場合
                        scene.developLDR("out.png", false);
#endif
                    }

                    //
                    // if (nextStatPrintTime < g_timeUtil.elapseTimeInMs()) {
                    //    nextStatPrintTime +=
                    //      1000; // TODO: 複数回来ることがあり得る
                    //    CounterStats::printStats(true);
                    //}
                }
                //
                CounterStats::postParallel();
            });
        taskTimeStats.add(float(g_timeUtil.elapseTimeInMs() - startRenderTime));
        logging("Render Task pushed (%08d->%08d) %d ms",
                taskNo,
                taskNo + TASK_NUM_UNTILL_BY_JOIN,
                int32_t(taskTimeStats.mean()));
        //
        CounterStats::printStats(true);
        //
        taskNo += TASK_NUM_UNTILL_BY_JOIN;
    }
    // 最後まで来たら、デノイズを行い最終イメージを出力
    filmNo++;
    std::ostringstream ss;
    ss << std::setfill('0') << std::setw(3) << filmNo;
    scene.developLDR(ss.str() + ".png", true);
    // masterScene.dumpHDR("result.hdr");
    return 0;
}
