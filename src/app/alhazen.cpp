#include "pch.hpp"
#include "core/image.hpp"
#include "core/math.hpp"
#include "core/scene.hpp"
#include "core/taskscheduler.hpp"
#include "core/util.hpp"
#include "app/app.hpp"
#include "app/alhazen.hpp"
#include "integrator/integrator.hpp"
#include "sensor/sensor.hpp"


/*
-------------------------------------------------
-------------------------------------------------
*/
int32_t Alhazen::runApp(const ArgConfig& config)
{
    //
    SimpleTaskScheduler taskScheduler;
    const int32_t grainSize = 1;
    taskScheduler.start(grainSize);
    // シーンの構築
    ObjectProp sceneProp;
    sceneProp.load(config.sceneFilePath);
    Scene scene(sceneProp);
    
    //
    std::vector<ImageLDR> ldrImages;
    ldrImages.resize(taskScheduler.numThread());
    //
    const int32_t totalTaskNum = scene.totalTaskNum();
    const int32_t taskNumPerLoop = scene.taskNumPerLoop();
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
    uint32_t nextDevelopTime = g_timeUtil.elapseTimeInMs() + developIntervalInMs;
    for (;;)
    {
        // 現在実行中のタスクが全て消費されるまで待つ
        while (!taskScheduler.isTaskConsumed())
        {
            //
            if (g_timeUtil.elapseTimeInMs() >= timeOutInMs)
            {
                //logging("Time out.");
                goto EXIT;
            }
            // 
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(10ms);
        }
        //
        if (taskNo >= totalTaskNum)
        {
            logging("All tasks were consumed.");
            break;
        }
        // レンダリング
        const int32_t TASK_NUM_UNTILL_BY_JOIN = 2048;
        for (int32_t taskNoOffset = 0; taskNoOffset < TASK_NUM_UNTILL_BY_JOIN; ++taskNoOffset)
        {
            taskScheduler.add([
                    taskNo,
                    totalTaskNum,
                    taskNoOffset,
                    &nextDevelopTime,
                    developIntervalInMs,
                    &filmNo,
                    &scene,
                    &taskNumPerLoop](int32_t threadNo)
            {
                const int32_t taskNoLocal = taskNo + taskNoOffset;
                if (taskNoLocal >= totalTaskNum)
                {
                    return;
                }
                scene.render(taskNoLocal);
                // マスタースレッドでのみ実行する
                if (threadNo == 0)
                {
                    // 現像
                    if (nextDevelopTime < g_timeUtil.elapseTimeInMs())
                    {
                        nextDevelopTime += developIntervalInMs;
                        filmNo++;
                        // 最終ショット以外はdenoiseは走らせない
                        std::ostringstream ss;
                        ss << std::setfill('0') << std::setw(3) << filmNo;
                        scene.developLDR(ss.str() + ".png", false);
                        scene.dumpHDR(ss.str() + ".bhdr");
                    }
                }
            });
        }
        logging("Render Task pushed (%08d->%08d)", taskNo, taskNo + TASK_NUM_UNTILL_BY_JOIN);
        //
        taskNo += TASK_NUM_UNTILL_BY_JOIN;
    }

EXIT:
    // スケジューラーを終了
    taskScheduler.shutdown();
    // 最後まで来たら、デノイズを行い最終イメージを出力
    filmNo++;
    std::ostringstream ss;
    ss << std::setfill('0') << std::setw(3) << filmNo;
    scene.developLDR(ss.str() + ".png", true);
    // masterScene.dumpHDR("result.hdr");
    return 0;
}
