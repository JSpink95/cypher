//////////////////////////////////////////////////////////////////////////
//    File        	: ComponentRef.h
//    Created By    : Jack Spink
//    Created On 	: [16/1/2020]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Object.h"

//////////////////////////////////////////////////////////////////////////

class ComponentRefBase
{
    DEFINE_CLASS_UID(ComponentRefBase)
public:
    virtual void OnConstruct(Object* owner) {}

public:
    std::string componentName;
};

//////////////////////////////////////////////////////////////////////////

template<typename T>
class ComponentRef: public ComponentRefBase
{
public:

    virtual void OnConstruct(Object* owner) override
    {
        ComponentRefBase::OnConstruct(owner);

        // componentName should be populated by this point
        if (owner)
        {
            component = owner->FindComponentAsType<T>(componentName);
        }
    }

public:

    // typical smart pointer overrides

    operator bool() const
    {
        return !component.expired();
    }

    Ref<T> get()
    {
        return component.lock();
    }

    Ref<T> operator->()
    {
        return component.lock();
    }

    const Ref<T> operator->() const
    {
        return component.lock();
    }

private:
    WeakRef<T> component;
};

//////////////////////////////////////////////////////////////////////////
