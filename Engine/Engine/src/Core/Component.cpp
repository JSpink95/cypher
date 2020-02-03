//////////////////////////////////////////////////////////////////////////
//    File        	: Component.cpp
//    Created By    : Jack Spink
//    Created On 	: [17/9/2019]
//////////////////////////////////////////////////////////////////////////

#include "Core/Component.h"
#include "Core/Object.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/RTTI/RTTI.h"

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(Component, RTTIObject)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

Component::Component()
{
}

//////////////////////////////////////////////////////////////////////////

Component::~Component()
{

}

//////////////////////////////////////////////////////////////////////////

const std::string Component::GetInstanceName() const
{
    std::string fullname = id.GetStringId();
    return fullname.substr(fullname.find_first_of(':') + 1);
}

void Component::SetId(const std::string& newStringId)
{
    id = ComponentId::Create(owner->GetId(), newStringId);
}

//////////////////////////////////////////////////////////////////////////
