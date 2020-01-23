//////////////////////////////////////////////////////////////////////////
//    File        	: PhysicsWorld.cpp
//    Created By    : Jack Spink
//    Created On 	: [1/10/2019]
//////////////////////////////////////////////////////////////////////////

#include "Physics/PhysicsWorld.h"
#include "Physics/PhysicsObject.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/Thread/PhysicsThread.h"

//////////////////////////////////////////////////////////////////////////

#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h"

//////////////////////////////////////////////////////////////////////////

#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "BulletCollision/CollisionShapes/btSphereShape.h"
#include "BulletCollision/CollisionShapes/btCapsuleShape.h"
#include "BulletCollision/CollisionShapes/btStaticPlaneShape.h"
#include "BulletCollision/BroadphaseCollision/btDbvtBroadphase.h"
#include "BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

//////////////////////////////////////////////////////////////////////////

#include "LinearMath/btDefaultMotionState.h"

//////////////////////////////////////////////////////////////////////////

void PhysicsWorld::OnConstruct()
{
    Super::OnConstruct();

    // make a singleton

    config = new btDefaultCollisionConfiguration;
    collisionDispatcher = new btCollisionDispatcher(config);
    broadphase = new btDbvtBroadphase;
    ghostPairCallback = new btGhostPairCallback;
    constraintSolver = new btSequentialImpulseConstraintSolver;

    // setup data
    broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(ghostPairCallback);

    world = new btDiscreteDynamicsWorld(collisionDispatcher, broadphase, constraintSolver, config);
    world->setGravity(btVector3(0.0f, -9.81f, 0.0f));
}

//////////////////////////////////////////////////////////////////////////

void PhysicsWorld::OnPhysicsUpdate(const f32 dt)
{
    Super::OnPhysicsUpdate(dt);
    
    if (simulatePhysics && world != nullptr)
    {
        world->stepSimulation(dt);
    }
}

//////////////////////////////////////////////////////////////////////////

btRigidBody* PhysicsWorld::CreateRigidBody(f32 mass, const float3& position, const float3& rotation, btCollisionShape* shape)
{
    const bool isStatic = (glm::abs(mass) <= glm::epsilon<f32>());

    btVector3 localInertia(0.0f, 0.0f, 0.0f);
    if (!isStatic)
        shape->calculateLocalInertia(mass, localInertia);
    
    fquat rot = fquat(glm::radians(rotation));
    btTransform transform(btQuaternion(rot.x, rot.y, rot.z, rot.w), btVector3(position.x, position.y, position.z));
    btDefaultMotionState* motionState = new btDefaultMotionState(transform);
    btRigidBody::btRigidBodyConstructionInfo constructionInfo(mass, motionState, shape, localInertia);

    btRigidBody* rb = new btRigidBody(constructionInfo);
    rb->setUserIndex(-1);
    world->addRigidBody(rb);

    return rb;
}

//////////////////////////////////////////////////////////////////////////

// 
// #todo - make a physics thread memory allocator?
// 

btCollisionShape* PhysicsWorld::CreatePlane(const float3& normal, const f32 d)
{
    btStaticPlaneShape* shape = new btStaticPlaneShape(btVector3(normal.x, normal.y, normal.z), d);
    return shape;
}

//////////////////////////////////////////////////////////////////////////

btCollisionShape* PhysicsWorld::CreateBox(const float3& hs)
{
    btBoxShape* shape = new btBoxShape(btVector3(hs.x, hs.y, hs.z));
    return shape;
}

//////////////////////////////////////////////////////////////////////////

btCollisionShape* PhysicsWorld::CreateSphere(const f32 r)
{
    btSphereShape* shape = new btSphereShape(r);
    return shape;
}

//////////////////////////////////////////////////////////////////////////

btCollisionShape* PhysicsWorld::CreateCapsule(const f32 r, const f32 h)
{
    btCapsuleShape* shape = new btCapsuleShape(r, h);
    return shape;
}

//////////////////////////////////////////////////////////////////////////

void PhysicsWorld::FreeRigidBody(btRigidBody*& rb)
{
    if (rb != nullptr)
    {
        world->removeRigidBody(rb);

        delete rb;
        rb = nullptr;
    }
}

//////////////////////////////////////////////////////////////////////////

void PhysicsWorld::FreeCollisionShape(btCollisionShape*& shape)
{
    if (shape)
    {
        delete shape;
        shape = nullptr;
    }
}

//////////////////////////////////////////////////////////////////////////

PhysicsWorld::RayHit PhysicsWorld::RayCast(const float3& start, const float3& direction, const f32 length, const IgnoreList& ignoreList/* = {}*/)
{
    //if (!GetPhysicsThread()->IsCurrentThread())
    //{
    //    printf("Attempted raycast from outside of physics thread!");
    //    return {};
    //}

    btVector3 rayFromWorld = btVector3(start.x, start.y, start.z);
    btVector3 rayToWorld = rayFromWorld + btVector3(direction.x, direction.y, direction.z) * length;
    btCollisionWorld::ClosestRayResultCallback rayCb(rayFromWorld, rayToWorld);
    world->rayTest(rayFromWorld, rayToWorld, rayCb);

    PhysicsWorld::RayHit hit;
    if (rayCb.hasHit())
    {
        hit.valid = true;
        hit.object = (Object*)rayCb.m_collisionObject->getUserPointer();
        hit.impact = float3(rayCb.m_hitPointWorld.x(), rayCb.m_hitPointWorld.y(), rayCb.m_hitPointWorld.z());
        hit.impactNormal = float3(rayCb.m_hitNormalWorld.x(), rayCb.m_hitNormalWorld.y(), rayCb.m_hitNormalWorld.z());
        hit.distance = length * rayCb.m_closestHitFraction;
    }

    return hit;
}

//////////////////////////////////////////////////////////////////////////

void PhysicsWorld::ApplyImpulseToObject(Ref<PhysicsObject> object, const float3& direction, const f32 strength)
{
    if (object->body == nullptr || object->body->isStaticOrKinematicObject())
        return;

    auto impulseTask = [this, object, direction, strength]() -> void
    {
        btVector3 d = btVector3(direction.x, direction.y, direction.z);
        object->body->applyCentralImpulse(d * strength);
    };

    GetPhysicsThread()->PushThreadTask(impulseTask);
}

//////////////////////////////////////////////////////////////////////////

void PhysicsWorld::ApplyImpulseInRadius(const float3& point, const f32 radius, const f32 strength, const IgnoreList& ignorelist/* = {}*/)
{
    auto impulseTask = [this, point, radius, strength, ignorelist]() -> void
    {
        btVector3 impulseOrigin = btVector3(point.x, point.y, point.z);
        btTransform transform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f), impulseOrigin);
        btSphereShape sphere(radius);

        btPairCachingGhostObject ghost;
        ghost.setCollisionShape(&sphere);
        ghost.setWorldTransform(transform);

        world->addCollisionObject(&ghost, btBroadphaseProxy::SensorTrigger, btBroadphaseProxy::AllFilter & ~btBroadphaseProxy::SensorTrigger);

        if (ghost.getBroadphaseHandle())
            world->updateSingleAabb(&ghost);

        for (s32 idx = 0; idx < ghost.getNumOverlappingObjects(); ++idx)
        {
            btCollisionObject* collision = ghost.getOverlappingObject(idx);
            if (collision->isStaticObject())    // ignore static objects
                continue;

            if (btRigidBody* body = btRigidBody::upcast(collision))
            {
                auto it = std::find_if(ignorelist.begin(), ignorelist.end(), [body](PhysicsObject* po) -> bool { return po->body == body; });
                btVector3 impulse = (body->getWorldTransform().getOrigin() - impulseOrigin);

                if (!impulse.fuzzyZero() && it == ignorelist.end())
                {
                    if (!body->isActive())
                        body->activate();

                    body->applyCentralImpulse(impulse.normalized() * strength);
                }
            }
        }

        world->removeCollisionObject(&ghost);
    };

    GetPhysicsThread()->PushThreadTask(impulseTask);
}

//////////////////////////////////////////////////////////////////////////
