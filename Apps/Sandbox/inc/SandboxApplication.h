//////////////////////////////////////////////////////////////////////////
//    File        	: SandboxApplication.h
//    Created By    : Jack Spink
//    Created On 	: [24/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Application.h"
#include "GameFramework/LevelLoader.h"

//////////////////////////////////////////////////////////////////////////

class PhysicsWorld;
class PhysicsObject;
class Material;
class Framebuffer;
class ComputeShader;
class VertexArray;
class Texture2D;
class LightObject;
class StaticMeshObject;
class GameObject;
class FpsCameraController;

//////////////////////////////////////////////////////////////////////////

class Player;
class DungeonRoom;
class Weapon;

//////////////////////////////////////////////////////////////////////////

class SandboxApp : public Application
{
public:
    virtual void OnPreUpdate(const f32 dt) override;

protected:

    virtual void OnRenderCreate() override;
    virtual void OnPostCreate() override;
    virtual void OnDestroy() override;

private:
    Ref<PhysicsWorld> physics;

    Level level;
    Ref<Player> player;
    Ref<GameObject> particleObject;
};

//////////////////////////////////////////////////////////////////////////
