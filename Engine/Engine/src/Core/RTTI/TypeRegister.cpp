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

//////////////////////////////////////////////////////////////////////////

PropertyBase* TypeBase::FindProperty(const std::string& id)
{
    auto it = properties.find(id);
    if (it == properties.end())
    {
        if (TypeBase* base = GetBaseType())
        {
            return base->FindProperty(id);
        }
        else
        {
            return nullptr;
        }
    }
    else
    {
        return it->second;
    }
}

//////////////////////////////////////////////////////////////////////////

size_t TypeBase::GetPropertyCount() const
{
    return properties.size();
}

//////////////////////////////////////////////////////////////////////////

TypeBase* TypeBase::GetBaseType() const
{
    return TypeRegister::GetRegisteredType(baseTypeName);
}

//////////////////////////////////////////////////////////////////////////

bool TypeBase::IsTypeOf(const std::string& className) const
{
    if (typeName == className)
        return true;

    if (TypeBase* baseType = GetBaseType())
        return baseType->IsTypeOf(className);

    return false;
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

void TypeRegister::GetRegisteredTypesOfBaseImpl(const std::string& baseTypeName, std::vector<TypeBase*>& outTypes)
{
    for (auto& it : types)
    {
        if (it.second && it.second->IsTypeOf(baseTypeName))
        {
            outTypes.push_back(it.second);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
