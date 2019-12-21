//////////////////////////////////////////////////////////////////////////
//    File        	: TransformComponent.cpp
//    Created By    : Jack Spink
//    Created On 	: [18/10/2019]
//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Component/TransformComponent.h"
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
