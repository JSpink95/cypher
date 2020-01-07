//////////////////////////////////////////////////////////////////////////
//    File        	: Object.h
//    Created By    : Jack Spink
//    Created On 	: [17/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Core.h"
#include "Core/Types.h"
#include "Core/TypeId.h"
#include "Core/Property.h"
#include "Core/ObjectMacros.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Component.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass.h"

//////////////////////////////////////////////////////////////////////////

#include <vector>

//////////////////////////////////////////////////////////////////////////

using ComponentHashMap = ObjectIdHashMap<Ref<Component>>;

//////////////////////////////////////////////////////////////////////////

class Material;

//////////////////////////////////////////////////////////////////////////

class Object: public PropertyContainer
{
    DECLARE_BASE_OBJECT(Object)
public:
    Object();
    virtual ~Object();

    virtual void OnConstruct();
    virtual void OnDestruct();
    virtual void OnUpdate(const f32 dt);                                // called from the game thread
    virtual void OnPhysicsUpdate(const f32 dt);                         // called from the physics thread
    virtual void OnRender(RenderPassType::Enum pass, Ref<Material> overrideMaterial = nullptr);    // called from the render thread

    virtual void Serialise(std::ofstream& output) {}
    virtual void Deserialise(const std::ifstream& input) {}

public:
    void SetTickEnabled(bool const enabled);

public:

    template<typename TComponent>
    Ref<TComponent> NewComponent();

    template<typename TComponent>
    Ref<TComponent> FindFirstComponentOfType();

    template<typename TComponent>
    void FindAllComponentsOfType(std::vector<Ref<TComponent>>& outComponents);

private:
    ComponentHashMap components;

public:
    inline ObjectGuid GetInstanceId() const { return instanceId; }

private:
    ObjectGuid instanceId;
};

//////////////////////////////////////////////////////////////////////////

template<typename TComponent>
inline Ref<TComponent> Object::NewComponent()
{
    Ref<TComponent> newComponent = CreateComponent<TComponent>(this);
    components.emplace(std::make_pair(newComponent->GetInstanceId(), newComponent));
    
    return newComponent;
}

//////////////////////////////////////////////////////////////////////////

template<typename TComponent>
inline Ref<TComponent> Object::FindFirstComponentOfType()
{
    Ref<TComponent> result = nullptr;

    for (auto& kv : components)
    {
        Ref<Component> component = kv.second;
        if (component != nullptr && component->GetClassUID() == TComponent::ClassUID())
        {
            result = std::dynamic_pointer_cast<TComponent>(component);
            break;
        }
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////

template<typename TComponent>
inline void Object::FindAllComponentsOfType(std::vector<Ref<TComponent>>& outComponents)
{
    outComponents.clear();
    for (auto& kv : components)
    {
        Ref<Component> component = kv.second;
        if (component != nullptr && component->GetClassUID() == TComponent::ClassUID())
        {
            outComponents.push_back(std::dynamic_pointer_cast<TComponent>(component));
        }
    }
}

//////////////////////////////////////////////////////////////////////////
