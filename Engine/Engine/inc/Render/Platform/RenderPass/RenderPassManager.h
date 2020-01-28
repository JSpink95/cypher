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

    template<typename TPass>
    static inline Ref<TPass> GetPassAsType(const HashedString& id)
    {
        return std::dynamic_pointer_cast<TPass>(GetPass(id));
    }

private:

    void InitialiseImpl();
    void SortPasses();
    void AddPassImpl(Ref<RenderPassBase> pass);
    void RemovePassImpl(Ref<RenderPassBase> pass);
    void RenderImpl();

    Ref<RenderPassBase> GetPassImpl(const HashedString& id);

private:
    // not using a hash_map (unordered_map) here so that we can sort it by render priority
    std::map<HashedString, Ref<RenderPassBase>> passes;
};

//////////////////////////////////////////////////////////////////////////
