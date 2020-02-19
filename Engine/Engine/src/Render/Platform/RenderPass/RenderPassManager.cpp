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
#include "Render/Platform/RenderPass/RenderPassCoreCombiner.h"
#include "Render/Platform/RenderPass/RenderPassBloom.h"
#include "Render/Platform/RenderPass/RenderPassSSAO.h"
//#include "Render/Platform/RenderPass/RenderPassDebug.h"
#include "Render/Platform/RenderPass/RenderPassFinal.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Window.h"

//////////////////////////////////////////////////////////////////////////

struct RenderPassLessComparator
{
    RenderPassLessComparator(const HashMap<HashedString, Ref<RenderPassBase>>& inData)
        : data(inData)
    {}

private:
    const HashMap<HashedString, Ref<RenderPassBase>>& data;

public:

    bool operator()(const HashedString& a, const HashedString& b)
    {
        auto it1 = data.find(a);
        auto it2 = data.find(b);

        if (it1 == data.end())
            return false;

        if (it2 == data.end())
            return true;

        return it1->second->GetPriority() < it2->second->GetPriority();
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
    passes.emplace(RenderPassCoreCombiner::Id, std::make_shared<RenderPassCoreCombiner>());
    passes.emplace(RenderPassBloom::Id, std::make_shared<RenderPassBloom>());
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
    // Lit          -   0
    // Unlit        -  50
    // Shadow       - 100
    // Particle     - 150
    // SSL          - 200
    // CoreCombiner - 201 <- combines previous phases (SSL + Unlit + Particle)
    // Bloom        - 225
    // SSAO         - 250 <- quite important that this one is last. mainly just for ease of use.
    // Debug        - 300

    renderOrder.clear();
    auto transformer = [](const std::pair<const HashedString, Ref<RenderPassBase>>& pass) -> HashedString { return pass.first; };
    std::transform(passes.begin(), passes.end(), std::back_inserter(renderOrder), transformer);

    std::sort(renderOrder.begin(), renderOrder.end(), RenderPassLessComparator(passes));
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
    for (const HashedString& id : renderOrder)
    {
        auto it = passes.find(id);
        if (it != passes.end())
        {
            it->second->OnBegin();
            it->second->OnPerform();
            it->second->OnFinish();
        }
    }
}

//////////////////////////////////////////////////////////////////////////

uint2 RenderPassManager::GetFramebufferSizeImpl() const
{
    return (uint2)(float2(Display::GetSize()) / 2.0f);
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
