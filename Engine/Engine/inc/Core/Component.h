//////////////////////////////////////////////////////////////////////////
//    File        	: Component.h
//    Created By    : Jack Spink
//    Created On 	: [17/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"
#include "Core/ObjectMacros.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/RTTI/RTTIObject.h"
#include "Core/RTTI/PropertyRegister.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Thread/GameThread.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassBase.h"

//////////////////////////////////////////////////////////////////////////

#include "pugixml.hpp"

//////////////////////////////////////////////////////////////////////////

class Component;

//////////////////////////////////////////////////////////////////////////

class ComponentTickFunction : public TickFunction
{
    DECLARE_DERIVED(ComponentTickFunction, TickFunction)
public:
    virtual void ExecuteTick(const f32 dt) override;

public:
    Component* component;
};

//////////////////////////////////////////////////////////////////////////

class Component
    : public RTTIObject
    , public IPropertyChangedListener
{
    DECLARE_COMPONENT(Component, RTTIObject)
public:
    Component();
    virtual ~Component();

    virtual void OnConstruct();
    virtual void OnDestruct();
    virtual void OnTick(const f32 dt);
    virtual void OnRender(RenderPassType::Enum pass, Ref<Material> materialOverride = nullptr) {}

public:
    inline Object* GetOwner() const { return owner; }
    inline void SetOwner(Object* newOwner) { owner = newOwner; }

protected:
    Object* owner = nullptr;

public:
    inline const ComponentId& GetId() const
    {
        return id;
    }

    const std::string GetInstanceName() const;
    void SetId(const std::string& newStringId);

public:
    ComponentTickFunction tickFunction;

private:
    ComponentId id;
};

//////////////////////////////////////////////////////////////////////////

template<typename TComponent>
Ref<TComponent> CreateComponent(Object* owner, const std::string& id)
{
    Ref<TComponent> result = std::make_shared<TComponent>();
    result->SetOwner(owner);
    result->SetId(id);
    result->OnConstruct();

    return result;
}

//////////////////////////////////////////////////////////////////////////
