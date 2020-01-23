//////////////////////////////////////////////////////////////////////////
//    File        	: PhysicsObject.h
//    Created By    : Jack Spink
//    Created On 	: [29/11/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "GameFramework/Object/GameObject.h"

//////////////////////////////////////////////////////////////////////////

class btRigidBody;
class BaseCollisionShape;

//////////////////////////////////////////////////////////////////////////

class PhysicsObject : public GameObject
{
    DECLARE_OBJECT(PhysicsObject, GameObject)
private:
    friend class PhysicsWorld;

public:
    virtual void OnConstruct() override;
    virtual void OnDestruct() override;

    virtual void OnUpdate(const f32 dt) override;
    virtual void OnPhysicsUpdate(const f32 dt) override;

    void Initialise(f32 mass, const float3& position, const float3& rotation, Ref<BaseCollisionShape> col);
    void SetGravityEnabled(bool gravity);
    void SetPositionAndRotation(const float3& position, const float3& euler);

    void ApplyDirectImpulse(const float3& direction, const f32 strength);

protected:
    Ref<BaseCollisionShape> collision;
    btRigidBody* body = nullptr;
};

//////////////////////////////////////////////////////////////////////////
