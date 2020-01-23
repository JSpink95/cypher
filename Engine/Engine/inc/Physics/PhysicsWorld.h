//////////////////////////////////////////////////////////////////////////
//    File        	: PhysicsWorld.h
//    Created By    : Jack Spink
//    Created On 	: [1/10/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Object.h"

//////////////////////////////////////////////////////////////////////////

#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"

//////////////////////////////////////////////////////////////////////////

#include <vector>
#include <functional>

//////////////////////////////////////////////////////////////////////////

class PhysicsObject;
class btGhostPairCallback;

//////////////////////////////////////////////////////////////////////////

using IgnoreList = std::vector<PhysicsObject*>;

//////////////////////////////////////////////////////////////////////////

class PhysicsWorld: public Object
{
    DECLARE_OBJECT(PhysicsWorld, Object)
public:

    struct RayHit
    {
        bool valid = false;
        Object* object = nullptr;
        float3 impact, impactNormal;
        f32 distance;
    };

public:

    virtual void OnConstruct() override;
    virtual void OnPhysicsUpdate(const f32 dt) override;     // called from the physics thread

    btRigidBody* CreateRigidBody(f32 mass, const float3& position, const float3& rotation, btCollisionShape* shape);
    btCollisionShape* CreatePlane(const float3& normal, const f32 d);
    btCollisionShape* CreateBox(const float3& hs);
    btCollisionShape* CreateSphere(const f32 r);
    btCollisionShape* CreateCapsule(const f32 r, const f32 h);

    void FreeRigidBody(btRigidBody*& rb);
    void FreeCollisionShape(btCollisionShape*& shape);

    inline void SetSimulatePhysics(bool simulate) { simulatePhysics = simulate; }

    PhysicsWorld::RayHit RayCast(const float3& start, const float3& direction, const f32 length, const IgnoreList& ignoreList = {});

    void ApplyImpulseToObject(Ref<PhysicsObject> object, const float3& direction, const f32 strength);
    void ApplyImpulseInRadius(const float3& point, const f32 radius, const f32 strength, const IgnoreList& ignoreList = {});

private:
    btCollisionConfiguration* config;
    btCollisionDispatcher* collisionDispatcher;
    btBroadphaseInterface* broadphase;
    btConstraintSolver* constraintSolver;
    btGhostPairCallback* ghostPairCallback;
    btDiscreteDynamicsWorld* world;
    bool simulatePhysics = false;

private:
    static inline Ref<PhysicsWorld> activeInstance;

public:

    static Ref<PhysicsWorld> GetActiveInstance()
    {
        return activeInstance;
    }

    static void SetActiveInstance(Ref<PhysicsWorld> newActiveInstance)
    {
        activeInstance = newActiveInstance;
    }

};

//////////////////////////////////////////////////////////////////////////

class Physics
{
public:
    static inline btRigidBody* CreateRigidBody(f32 mass, const float3& position, const float3& rotation, btCollisionShape* shape)
    {
        if (Ref<PhysicsWorld> world = PhysicsWorld::GetActiveInstance())
        {
            return world->CreateRigidBody(mass, position, rotation, shape);
        }

        return nullptr;
    }

    static inline void ApplyImpulseInRadius(const float3& centre, const f32 radius, const f32 strength, const IgnoreList& ignorelist = {})
    {
        if (Ref<PhysicsWorld> world = PhysicsWorld::GetActiveInstance())
        {
            world->ApplyImpulseInRadius(centre, radius, strength, ignorelist);
        }
    }

    static inline PhysicsWorld::RayHit RayCast(const float3& start, const float3& direction, const f32 length, const IgnoreList& ignorelist = {})
    {
        if (Ref<PhysicsWorld> world = PhysicsWorld::GetActiveInstance())
        {
            return world->RayCast(start, direction, length, ignorelist);
        }
    };
};

//////////////////////////////////////////////////////////////////////////
