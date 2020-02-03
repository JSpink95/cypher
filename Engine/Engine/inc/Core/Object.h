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
#include "Core/ObjectMacros.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/RTTI/RTTIObject.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Component.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Thread/GameThread.h"

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/RenderPass/RenderPassManager.h"

//////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <vector>

//////////////////////////////////////////////////////////////////////////

class Object;
class Material;

//////////////////////////////////////////////////////////////////////////

class ObjectTickFunction : public TickFunction
{
    DECLARE_DERIVED(ObjectTickFunction, TickFunction)
public:
    virtual void ExecuteTick(const f32 dt) override;

public:
    Object* object = nullptr;
};

//////////////////////////////////////////////////////////////////////////

class Object: public RTTIObject
{
    DECLARE_OBJECT(Object, RTTIObject)
public:
    Object();
    virtual ~Object();

    void Tick(const f32 dt);

    virtual void OnConstruct();
    virtual void OnDestruct();
    virtual void OnUpdate(const f32 dt);                                // called from the game thread
    virtual void OnPhysicsUpdate(const f32 dt);                         // called from the physics thread
    virtual void OnRender(RenderPassType::Enum pass, Ref<Material> overrideMaterial = nullptr);    // called from the render thread

    virtual void Serialise(std::ofstream& output) {}
    virtual void Deserialise(const std::ifstream& input) {}

public:
    void SetTickEnabled(const bool enabled);

public:

    template<typename TComponent>
    Ref<TComponent> CreateComponent(const std::string& id);

public:

    inline Ref<Component> FindComponent(const std::string& name)
    {
        ComponentId componentId = ComponentId::Create(id, name);
        auto it = components.find(componentId);
        if (it != components.end())
        {
            return it->second;
        }

        return nullptr;
    }

    template<typename TPredicate>
    inline void ForEachComponent(TPredicate predicate)
    {
        for (auto& componentPair : components)
        {
            predicate(componentPair.first, componentPair.second);
        }
    }

public:

    template<typename TComponent>
    Ref<TComponent> FindFirstComponentOfType()
    {
        auto it = std::find_if(components.begin(), components.end(), [](const std::pair<ComponentId, Ref<Component>>& it) -> bool
        { 
            return it.second != nullptr && it.second->IsTypeOf(TComponent::ClassUID());
        });

        if (it != components.end())
        {
            return std::dynamic_pointer_cast<TComponent>(it->second);
        }

        return nullptr;
    }

    template<typename TComponent>
    Ref<TComponent> FindComponentAsType(const std::string& name)
    {
        return std::dynamic_pointer_cast<TComponent>(FindComponent(name));
    }

public:
    GenericComponentHashMap components;

public:
    inline const ObjectId& GetId() const
    {
        return id;
    }

    inline void SetId(const ObjectId& newId)
    {
        id = newId;
    }

private:
    ObjectId id;
    ObjectTickFunction tickFunction;
};

//////////////////////////////////////////////////////////////////////////

template<typename TComponent>
inline Ref<TComponent> Object::CreateComponent(const std::string& id)
{
    Ref<TComponent> newComponent = ::CreateComponent<TComponent>(this, id);
    components.emplace(newComponent->GetId(), newComponent);
    
    return newComponent;
}

//////////////////////////////////////////////////////////////////////////

namespace RTTI
{
    // xml serialisation
    Ref<Object> LoadObjectFromXML(const std::string& filepath);
    void SaveObjectToXML(Ref<Object> object, const std::string& filepath);

    // binary serialisation
    void SaveObjectToBinary(Ref<Object> object, const std::string& filepath);
}

//////////////////////////////////////////////////////////////////////////
