//////////////////////////////////////////////////////////////////////////
//    File        	: TypeRegister.h
//    Created By    : Jack Spink
//    Created On 	: [13/1/2020]
//////////////////////////////////////////////////////////////////////////

#include "Core/RTTI/TypeRegister.h"
#include "Core/RTTI/PropertyRegister.h"

//////////////////////////////////////////////////////////////////////////

void TypeBase::AddProperty(PropertyBase* newProperty)
{
    const std::string& propertyName = newProperty->propertyName;

    auto it = properties.find(propertyName);
    if (it == properties.end())
    {
        properties.emplace(propertyName, newProperty);
    }
}

TypeBase* TypeBase::GetBaseType()
{
    return TypeRegister::GetRegisteredType(baseTypeName);
}

//////////////////////////////////////////////////////////////////////////

void TypeRegister::RegisterTypeImpl(TypeBase* type)
{
    types.emplace(type->GetTypeName(), type);
}

//////////////////////////////////////////////////////////////////////////

bool TypeRegister::IsRegisteredTypeImpl(const std::string& typeName)
{
    auto it = types.find(typeName);
    return (it != types.end());
}

//////////////////////////////////////////////////////////////////////////

bool TypeRegister::IsRegisteredTypeOfImpl(const std::string& typeName, ClassId classId)
{
    auto it = types.find(typeName);
    if (it != types.end())
    {
        
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////

TypeBase* TypeRegister::GetRegisteredTypeImpl(const std::string& typeName)
{
    auto it = types.find(typeName);
    if (it != types.end())
    {
        return it->second;
    }

    return nullptr;
}

//////////////////////////////////////////////////////////////////////////
