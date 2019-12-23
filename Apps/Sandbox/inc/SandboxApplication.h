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
    virtual void OnPreRender() override;

protected:

    virtual void OnCreate() override;
    virtual void OnDestroy() override;

private:
    void LoadRenderResources();

private:
    bool hasLoadedRenderResources = false;

    Ref<PhysicsWorld> physics;

    Level level;
    Ref<Player> player;
};

//////////////////////////////////////////////////////////////////////////
