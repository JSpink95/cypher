//////////////////////////////////////////////////////////////////////////
//    File        	: TransformComponent.cpp
//    Created By    : Jack Spink
//    Created On 	: [18/10/2019]
//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Component/TransformComponent.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Object.h"
#include "Core/ComponentRef.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/RTTI/RTTI.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Utility/ParseUtils.h"

//////////////////////////////////////////////////////////////////////////

#include <glm/gtc/quaternion.hpp>

//////////////////////////////////////////////////////////////////////////

// expose to ComponentRef type
RTTI_TEMPLATE_WITH_BASE(ComponentRef, ComponentRefBase, TransformComponent);

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(TransformComponent, Component)
    RTTI_PROPERTY(TransformComponent, float3, position)
    RTTI_PROPERTY(TransformComponent, float3, rotation)
    RTTI_PROPERTY(TransformComponent, float3, scale)
    RTTI_PROPERTY(TransformComponent, ComponentRef<TransformComponent>, parentTransform)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

void TransformComponent::OnConstruct()
{
    Super::OnConstruct();

    parentTransform.OnConstruct(owner);
}

//////////////////////////////////////////////////////////////////////////

fmat4 TransformComponent::CalculateTransformMatrix() const
{
    fmat4 result(1.0f);

    result = glm::translate(result, position);
    result *= glm::mat4_cast(fquat(glm::radians(rotation)));
    result = glm::scale(result, scale);

    if (parentTransform)
    {
        result = parentTransform->CalculateTransformMatrix() * result;
    }
    
    return result;
}

//////////////////////////////////////////////////////////////////////////

Ref<Component> TransformComponent::Create(Ref<Object> owner, pugi::xml_node node)
{
    Ref<TransformComponent> transform = owner->CreateComponent<TransformComponent>("Transform");

    for (pugi::xml_attribute attribute : node.attributes())
    {
        std::string name = attribute.name();
        if (name == "position")
        {
            transform->position = ParseFloat3(attribute.as_string());
        }
        else if (name == "rotation")
        {
            transform->rotation = ParseFloat3(attribute.as_string());
        }
    }

    return transform;
}

//////////////////////////////////////////////////////////////////////////

Ref<Component> TransformComponent::CreateComponent(Ref<Object> owner, const std::string& id, pugi::xml_node node)
{
    Ref<TransformComponent> transform = owner->CreateComponent<TransformComponent>(id);

    return transform;
}

//////////////////////////////////////////////////////////////////////////
