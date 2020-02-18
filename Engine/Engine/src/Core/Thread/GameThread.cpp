//////////////////////////////////////////////////////////////////////////
//    File        	: GameThread.cpp
//    Created By    : Jack Spink
//    Created On 	: [18/9/2019]
//////////////////////////////////////////////////////////////////////////

#include "Core/Thread/GameThread.h"
#include "Core/Thread/Thread.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Object.h"
#include "Core/Application.h"
#include "Core/Utility/TimeUtils.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassBase.h"

//////////////////////////////////////////////////////////////////////////

#include "imgui.h"

#ifdef _OPENGL
    #include "imgui_impl_opengl3.h"
    #include "imgui_impl_glfw.h"
#endif

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(TickFunction, RTTIObject)
    RTTI_PROPERTY(TickFunction, bool, enabled)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

void GameThread::RegisterTickFunction(TickFunction* tick)
{
    if (Ref<GameThread> gt = GetGameThread())
    {
        gt->RegisterTickFunctionImpl(tick);
    }
}

//////////////////////////////////////////////////////////////////////////

void GameThread::DeregisterTickFunction(TickFunction* tick)
{
    if (Ref<GameThread> gt = GetGameThread())
    {
        gt->DeregisterTickFunctionImpl(tick);
    }
}

//////////////////////////////////////////////////////////////////////////

void GameThread::RegisterTickFunctionImpl(TickFunction* tick)
{
    ticks.push_back(tick);
}

//////////////////////////////////////////////////////////////////////////

void GameThread::DeregisterTickFunctionImpl(TickFunction* tick)
{
    // could be slow...
    ticks.erase(std::remove(ticks.begin(), ticks.end(), tick), ticks.end());
}

//////////////////////////////////////////////////////////////////////////

void GameThread::ThreadLoop()
{
    using clock = std::chrono::high_resolution_clock;
    using time_point = std::chrono::time_point<clock>;
    using duration = std::chrono::duration<f32>;

    Thread::SetName("* Game Thread");

    GetApplication()->RenderCreate();

    time_point frameStart = clock::now();
    while (running)
    {
        time_point now = clock::now();
        const f32 dt = std::chrono::duration_cast<duration>(now - frameStart).count();
        frameStart = now;

        Time::UpdateTime(dt);

        DispatchThreadTasks();

        // update
        GetApplication()->OnPreUpdate(dt);

        for (TickFunction* tick : ticks)
        {
            if (!tick->enabled)
                continue;

            tick->ExecuteTick(dt);
        }

        GetApplication()->OnPostUpdate();

        // render
        GetApplication()->OnPreRender();

        RenderPassManager::Render();

        GetApplication()->OnPostRender();

        // do ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        GetApplication()->OnImGuiRender();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        GetApplication()->OnPresent();

        while (std::chrono::duration_cast<duration>(clock::now() - frameStart).count() < 0.016666666666667f);

        // briefly sleep so as not to run _too_ fast
        //std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // if we have thread tasks
    DispatchThreadTasks();
}

//////////////////////////////////////////////////////////////////////////

