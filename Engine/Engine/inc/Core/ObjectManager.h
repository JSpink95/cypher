//////////////////////////////////////////////////////////////////////////
//    File        	: ObjectManager.h
//    Created By    : Jack Spink
//    Created On 	: [21/11/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Singleton.h"
#include "Core/TypeId.h"
#include "Core/Object.h"

//////////////////////////////////////////////////////////////////////////

#include <unordered_map>

//////////////////////////////////////////////////////////////////////////

class Object;

//////////////////////////////////////////////////////////////////////////

class ObjectManager : public Singleton<ObjectManager>
{
public:
    void RegisterObject(Ref<Object> newObject);
    void DeregisterObject(Ref<Object> guid);

    Ref<Object> GetStrongRef(const ObjectId& id);
    WeakRef<Object> GetWeakRef(const ObjectId& id);

public:
    virtual void OnDelete() override;

private:
    std::unordered_map<ObjectId, Ref<Object>> objects;
};

//////////////////////////////////////////////////////////////////////////

static inline Ref<ObjectManager> GetObjectManager() { return ObjectManager::Get(); }

//////////////////////////////////////////////////////////////////////////

template<typename TObject>
static inline Ref<TObject> CreateObject(const ObjectId& id)
{
    Ref<ObjectManager> manager = GetObjectManager();
    if (manager)
    {
        Ref<TObject> newObject = std::make_shared<TObject>();
        newObject->SetId(id);

        manager->RegisterObject(newObject);
        newObject->OnConstruct();

        return newObject;
    }
    
    LOG_ERROR("Failed to create new Object (%s)! [ObjectManager] has not been initialised!\n", id.GetStringId().c_str());
    return nullptr;
}

//////////////////////////////////////////////////////////////////////////

static inline void DestroyObject(const ObjectId& id)
{
    Ref<ObjectManager> manager = GetObjectManager();
    if (manager)
    {
        Ref<Object> object = manager->GetStrongRef(id);
        //object->OnDestroy(); #todo - implement

        manager->DeregisterObject(object);
    }
}

//////////////////////////////////////////////////////////////////////////
