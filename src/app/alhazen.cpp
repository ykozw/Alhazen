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


//-------------------------------------------------
//
//-------------------------------------------------
int32_t Alhazen::runApp(const ArgConfig& config)
{
    //
    SimpleTaskScheduler taskScheduler;
    const int32_t grainSize = 1;
    taskScheduler.start(grainSize);
    // シーンの構築
    ObjectProp sceneProp;
    sceneProp.load(config.sceneFilePath);
    Scene masterScene(sceneProp);
    
    // スレッド毎にシーンのcloneを作成する
    std::vector<ScenePtr> scenes;
    scenes.resize(taskScheduler.numThread());
    for (auto& scene : scenes)
    {
        scene = masterScene.cloneForWorker();
    }
    //
    std::vector<ImageLDR> ldrImages;
    ldrImages.resize(taskScheduler.numThread());
    //
    const bool isPreview = false;
    const int32_t totalTaskNum = masterScene.totalTaskNum();
    const int32_t taskNumPerLoop = masterScene.taskNumPerLoop();
    const uint32_t developIntervalInMs = masterScene.developIntervalInMs();
    const uint32_t timeOutInMs = masterScene.timeOutInMs();

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
        const int32_t TASK_NUM_UNTILL_BY_JOIN = 256;
        for (int32_t taskNoOffset = 0; taskNoOffset < TASK_NUM_UNTILL_BY_JOIN; ++taskNoOffset)
        {
            taskScheduler.add([
                &scenes, taskNo,
                    totalTaskNum,
                    taskNoOffset,
                    &nextDevelopTime,
                    developIntervalInMs,
                    isPreview,
                    &filmNo,
                    &masterScene,
                    &taskNumPerLoop](int32_t threadNo)
            {
                const int32_t taskNoLocal = taskNo + taskNoOffset;
                if (taskNoLocal >= totalTaskNum)
                {
                    return;
                }
                auto& scene = scenes[threadNo];
                SubFilm& subFilm = scene->render(taskNoLocal);
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
                        masterScene.developLDR(ss.str() + ".bmp", false, isPreview);
                        masterScene.dumpHDR(ss.str() + ".bhdr");
                    }
                }
            });
        }
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
    masterScene.developLDR(ss.str() + ".bmp", true, isPreview);
    // masterScene.dumpHDR("result.hdr");
    //
    StatCounter::print();
    return 0;
}
