//////////////////////////////////////////////////////////////////////////
//    File        	: Property.cpp
//    Created By    : Jack Spink
//    Created On 	: [4/12/2019]
//////////////////////////////////////////////////////////////////////////

#include "Core/Property.h"

//////////////////////////////////////////////////////////////////////////

void PropertyContainer::AddProperty(IProperty* prop)
{
    properties.emplace(prop->GetPropertyId(), prop);
}

//////////////////////////////////////////////////////////////////////////

void PropertyContainer::RemoveProperty(IProperty* prop)
{
    properties.erase(prop->GetPropertyId());
}

//////////////////////////////////////////////////////////////////////////
