//////////////////////////////////////////////////////////////////////////
//    File        	: RenderPass.h
//    Created By    : Jack Spink
//    Created On 	: [23/10/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Singleton.h"
#include "Core/TypeId.h"

//////////////////////////////////////////////////////////////////////////

#include <map>

//////////////////////////////////////////////////////////////////////////

class RenderPassBase;

//////////////////////////////////////////////////////////////////////////

class RenderPassManager : public Singleton<RenderPassManager>
{
public:
    static inline void Initialise()
    {
        if (Ref<RenderPassManager> rpm = Get())
        {
            rpm->InitialiseImpl();
        }
    }

    static inline void AddPass(Ref<RenderPassBase> pass)
    {
        if (Ref<RenderPassManager> rpm = Get())
        {
            rpm->AddPassImpl(pass);
        }
    }

    static inline void RemovePass(Ref<RenderPassBase> pass)
    {
        if (Ref<RenderPassManager> rpm = Get())
        {
            rpm->RemovePassImpl(pass);
        }
    }

    static inline void Render()
    {
        if (Ref<RenderPassManager> rpm = Get())
        {
            rpm->RenderImpl();
        }
    }

    static inline Ref<RenderPassBase> GetPass(const HashedString& id)
    {
        if (Ref<RenderPassManager> rpm = Get())
        {
            return rpm->GetPassImpl(id);
        }

        return nullptr;
    }

    static uint2 GetFramebufferSize()
    {
        if (Ref<RenderPassManager> rpm = Get())
        {
            return rpm->GetFramebufferSizeImpl();
        }

        return uint2(640, 480);
    }

    template<typename TPass>
    static inline Ref<TPass> GetPassAsType(const HashedString& id)
    {
        return std::dynamic_pointer_cast<TPass>(GetPass(id));
    }

    template<typename TPass>
    static inline Ref<TPass> GetPassAsType()
    {
        return std::dynamic_pointer_cast<TPass>(GetPass(TPass::Id));
    }

private:

    void InitialiseImpl();
    void SortPasses();
    void AddPassImpl(Ref<RenderPassBase> pass);
    void RemovePassImpl(Ref<RenderPassBase> pass);
    void RenderImpl();

    uint2 GetFramebufferSizeImpl() const;
    Ref<RenderPassBase> GetPassImpl(const HashedString& id);

private:
    // not using a hash_map (unordered_map) here so that we can sort it by render priority
    HashMap<HashedString, Ref<RenderPassBase>> passes;
    std::vector<HashedString> renderOrder;
};

//////////////////////////////////////////////////////////////////////////
