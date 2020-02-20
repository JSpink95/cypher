//////////////////////////////////////////////////////////////////////////
//    File        	: ComponentRef.h
//    Created By    : Jack Spink
//    Created On 	: [16/1/2020]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Object.h"

//////////////////////////////////////////////////////////////////////////

class ComponentRefBase: public RTTIObject
{
    DECLARE_OBJECT(ComponentRefBase, RTTIObject)
public:
    virtual void OnConstruct(Object* owner);
    virtual void OnComponentChanged(Object* owner);

public:
    std::string componentName = "";
};

//////////////////////////////////////////////////////////////////////////

template<typename T>
class ComponentRef: public ComponentRefBase
{
    DECLARE_TEMPLATE_OBJECT(ComponentRef, T, ComponentRefBase)
public:

    virtual void OnConstruct(Object* owner) override
    {
        ComponentRefBase::OnConstruct(owner);

        // componentName should be populated by this point
        OnComponentChanged(owner);
    }

    virtual void OnComponentChanged(Object* owner) override
    {
        ComponentRefBase::OnComponentChanged(owner);

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
