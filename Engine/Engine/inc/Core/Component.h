//////////////////////////////////////////////////////////////////////////
//    File        	: Component.h
//    Created By    : Jack Spink
//    Created On 	: [17/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"
#include "Core/ObjectMacros.h"
#include "Render/Platform/RenderPass.h"
#include "pugixml.hpp"

//////////////////////////////////////////////////////////////////////////

#include <guiddef.h>

//////////////////////////////////////////////////////////////////////////

class Object;

//////////////////////////////////////////////////////////////////////////

class Component
{
    DECLARE_BASE_COMPONENT(Component)
public:
    // {0D2B9F85-E5F5-40C7-AF8F-FEACFFB730DD}
    static inline const GUID ClassId = { 0xd2b9f85, 0xe5f5, 0x40c7, { 0xaf, 0x8f, 0xfe, 0xac, 0xff, 0xb7, 0x30, 0xdd } };

public:
    Component();
    virtual ~Component();

    virtual void OnConstruct() {}
    virtual void OnDestruct() {}
    virtual void OnUpdate(const f32 dt) {}
    virtual void OnRender(RenderPassType::Enum pass, Ref<Material> materialOverride = nullptr) {}

#ifdef DEBUG
    virtual void OnImGui() {}
#endif

public:
    inline Object* GetOwner() const { return owner; }
    inline void SetOwner(Object* newOwner) { owner = newOwner; }

protected:
    Object* owner = nullptr;

public:
    inline GUID GetInstanceId() const { return instanceId; }

private:
    GUID instanceId;
};

//////////////////////////////////////////////////////////////////////////

template<typename TComponent>
Ref<TComponent> CreateComponent(Object* owner)
{
    Ref<TComponent> result = std::make_shared<TComponent>();
    result->SetOwner(owner);
    result->OnConstruct();

    return result;
}

//////////////////////////////////////////////////////////////////////////
