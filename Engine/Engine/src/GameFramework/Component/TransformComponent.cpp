//////////////////////////////////////////////////////////////////////////
//    File        	: TransformComponent.cpp
//    Created By    : Jack Spink
//    Created On 	: [18/10/2019]
//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Component/TransformComponent.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Object.h"
#include "Core/Utility/ParseUtils.h"

//////////////////////////////////////////////////////////////////////////

#include <glm/gtc/quaternion.hpp>

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
    Ref<TransformComponent> transform = owner->NewComponent<TransformComponent>();

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
