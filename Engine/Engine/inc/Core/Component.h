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
