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

RTTI_BEGIN_WITH_BASE(ComponentTickFunction, TickFunction)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

RTTI_BEGIN_WITH_BASE(Component, RTTIObject)
    RTTI_PROPERTY(Component, ComponentTickFunction, tickFunction)
RTTI_END()

//////////////////////////////////////////////////////////////////////////

void ComponentTickFunction::ExecuteTick(const f32 dt)
{
    Super::ExecuteTick(dt);

    if (component != nullptr)
    {
        component->OnTick(dt);
    }
}

//////////////////////////////////////////////////////////////////////////

Component::Component()
{
}

//////////////////////////////////////////////////////////////////////////

Component::~Component()
{

}

//////////////////////////////////////////////////////////////////////////

void Component::OnConstruct()
{
    tickFunction.component = this;
    GameThread::RegisterTickFunction(&tickFunction);
}

//////////////////////////////////////////////////////////////////////////

void Component::OnDestruct()
{
    GameThread::DeregisterTickFunction(&tickFunction);
}

//////////////////////////////////////////////////////////////////////////

void Component::OnTick(const f32 dt)
{

}

//////////////////////////////////////////////////////////////////////////

void Component::SetTickEnabled(const bool enabled)
{
    tickFunction.enabled = enabled;
}

//////////////////////////////////////////////////////////////////////////

const std::string Component::GetInstanceName() const
{
    std::string fullname = id.GetStringId();
    return fullname.substr(fullname.find_first_of(':') + 1);
}

//////////////////////////////////////////////////////////////////////////

void Component::SetId(const std::string& newStringId)
{
    id = ComponentId::Create(owner->GetId(), newStringId);
}

//////////////////////////////////////////////////////////////////////////
