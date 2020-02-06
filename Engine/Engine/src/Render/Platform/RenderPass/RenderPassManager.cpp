//////////////////////////////////////////////////////////////////////////
//    File        	: RenderPassManager.cpp
//    Created By    : Jack Spink
//    Created On 	: [27/01/2020]
//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassManager.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassBase.h"
#include "Render/Platform/RenderPass/RenderPassLit.h"
#include "Render/Platform/RenderPass/RenderPassUnlit.h"
#include "Render/Platform/RenderPass/RenderPassShadow.h"
#include "Render/Platform/RenderPass/RenderPassParticle.h"
#include "Render/Platform/RenderPass/RenderPassSSL.h"
#include "Render/Platform/RenderPass/RenderPassSSAO.h"
//#include "Render/Platform/RenderPass/RenderPassDebug.h"
#include "Render/Platform/RenderPass/RenderPassFinal.h"

//////////////////////////////////////////////////////////////////////////

struct RenderPassLessComparator
{
    bool operator()(Ref<RenderPassBase> a, Ref<RenderPassBase> b)
    {
        return a->GetPriority() < b->GetPriority();
    }
};

//////////////////////////////////////////////////////////////////////////

void RenderPassManager::InitialiseImpl()
{
    passes.emplace(RenderPassLit::Id, std::make_shared<RenderPassLit>());
    passes.emplace(RenderPassUnlit::Id, std::make_shared<RenderPassUnlit>());
    passes.emplace(RenderPassShadow::Id, std::make_shared<RenderPassShadow>());
    passes.emplace(RenderPassParticle::Id, std::make_shared<RenderPassParticle>());
    passes.emplace(RenderPassSSL::Id, std::make_shared<RenderPassSSL>());
    passes.emplace(RenderPassSSAO::Id, std::make_shared<RenderPassSSAO>());

    // this should always be the last one on the pile.
    passes.emplace(RenderPassFinal::Id, std::make_shared<RenderPassFinal>());

    SortPasses();

    for (auto& it : passes)
    {
        it.second->OnRenderCreate();
    }

}

//////////////////////////////////////////////////////////////////////////

void RenderPassManager::SortPasses()
{
    // sort based on pass priority (-infinity to +infinity integer)
    // for reference the standard pass priorities are:
    // Lit      -   0
    // Unlit    -  50
    // Shadow   - 100
    // Particle - 150
    // SSL      - 200
    // SSAO     - 250 <- quite important that this one is last. mainly just for ease of use.
    // Debug    - 300

    //std::sort(passes.begin(), passes.end(), RenderPassLessComparator());
}

//////////////////////////////////////////////////////////////////////////

void RenderPassManager::AddPassImpl(Ref<RenderPassBase> pass)
{
    HashedString passId = pass->GetId();

    auto it = passes.find(passId);
    if (it == passes.end())
    {
        passes.emplace(passId, pass);
        SortPasses();
    }
}

//////////////////////////////////////////////////////////////////////////

void RenderPassManager::RemovePassImpl(Ref<RenderPassBase> pass)
{
    HashedString passId = pass->GetId();

    auto it = passes.find(passId);
    if (it != passes.end())
    {
        passes.erase(passId);
        SortPasses();
    }
}

//////////////////////////////////////////////////////////////////////////

void RenderPassManager::RenderImpl()
{
    std::vector<Ref<RenderPassBase>> sortedPasses;

    auto transformation = [](const std::pair<HashedString, Ref<RenderPassBase>>& val) -> Ref<RenderPassBase>
    {
        return val.second;
    };

    std::transform(passes.begin(), passes.end(), std::back_inserter(sortedPasses), transformation);
    std::sort(sortedPasses.begin(), sortedPasses.end(), RenderPassLessComparator());

    for (Ref<RenderPassBase> base: sortedPasses)
    {
        base->OnBegin();
        base->OnPerform();
        base->OnFinish();
    }
}

//////////////////////////////////////////////////////////////////////////

Ref<RenderPassBase> RenderPassManager::GetPassImpl(const HashedString& id)
{
    auto it = passes.find(id);
    if (it != passes.end())
    {
        return it->second;
    }

    return nullptr;
}

//////////////////////////////////////////////////////////////////////////
