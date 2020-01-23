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

#include "Render/Platform/RenderPass.h"

//////////////////////////////////////////////////////////////////////////

#include "pugixml.hpp"

//////////////////////////////////////////////////////////////////////////

class Component
    : public RTTIObject
    , public IPropertyChangedListener
{
    DECLARE_COMPONENT(Component, RTTIObject)
public:
    Component();
    virtual ~Component();

    virtual void OnConstruct() {}
    virtual void OnDestruct() {}
    virtual void OnUpdate(const f32 dt) {}
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

    void SetId(const std::string& newStringId);

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
