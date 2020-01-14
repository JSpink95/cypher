//////////////////////////////////////////////////////////////////////////
//    File        	: TransformComponent.cpp
//    Created By    : Jack Spink
//    Created On 	: [18/10/2019]
//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Component/TransformComponent.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Object.h"
#include "Core/RTTI/RTTI.h"
#include "Core/Utility/ParseUtils.h"

//////////////////////////////////////////////////////////////////////////

#include <glm/gtc/quaternion.hpp>

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN(TransformComponent)
    RTTI_PROPERTY(TransformComponent, float3, position)
    RTTI_PROPERTY(TransformComponent, float3, rotation)
    RTTI_PROPERTY(TransformComponent, float3, scale)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

fmat4 TransformComponent::CalculateTransformMatrix() const
{
    fmat4 result(1.0f);

    result = glm::translate(result, position);
    result *= glm::mat4_cast(fquat(glm::radians(rotation)));
    result = glm::scale(result, scale);

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

    static std::unordered_map<std::string, PropertySetterFunction> setters =
    {
        { "position", [](Ref<TransformComponent> transform, const std::string& value) { transform->position = ParseFloat3(value); } },
        { "rotation", [](Ref<TransformComponent> transform, const std::string& value) { transform->rotation = ParseFloat3(value); } },
        { "scale", [](Ref<TransformComponent> transform, const std::string& value) { transform->scale = ParseFloat3(value); } },
    };

    for (auto element : node)
    {
        const std::string elementName = element.name();
        if (elementName == "property")
        {
            const std::string propertyId = element.attribute("id").as_string();
            const std::string propertyValue = element.attribute("value").as_string();

            auto it = setters.find(propertyId);
            if (it != setters.end())
            {
                it->second(transform, propertyValue);
            }
        }
    }

    return transform;
}

//////////////////////////////////////////////////////////////////////////
