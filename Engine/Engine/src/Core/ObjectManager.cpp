//////////////////////////////////////////////////////////////////////////
//    File        	: ObjectManager.cpp
//    Created By    : Jack Spink
//    Created On 	: [21/11/2019]
//////////////////////////////////////////////////////////////////////////

#include "Core/ObjectManager.h"

//////////////////////////////////////////////////////////////////////////

void ObjectManager::RegisterObject(Ref<Object> newObject)
{
    objects.emplace(newObject->GetId(), newObject);
}

//////////////////////////////////////////////////////////////////////////

void ObjectManager::DeregisterObject(Ref<Object> object)
{
    objects.erase(object->GetId());
}

//////////////////////////////////////////////////////////////////////////

Ref<Object> ObjectManager::GetStrongRef(const ObjectId& id)
{
    auto it = objects.find(id);
    if (it != objects.end())
    {
        return it->second;
    }

    return nullptr;
}

//////////////////////////////////////////////////////////////////////////

WeakRef<Object> ObjectManager::GetWeakRef(const ObjectId& id)
{
    auto it = objects.find(id);
    if (it != objects.end())
    {
        return it->second;
    }

    return WeakRef<Object>();
}

//////////////////////////////////////////////////////////////////////////

void ObjectManager::OnDelete()
{
    Singleton::OnDelete();

    // erase all objects
    for (auto object : objects)
    {
        object.second = nullptr;
    }
}

//////////////////////////////////////////////////////////////////////////
