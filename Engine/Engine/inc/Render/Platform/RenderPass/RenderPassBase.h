//////////////////////////////////////////////////////////////////////////
//    File        	: RenderPass.h
//    Created By    : Jack Spink
//    Created On 	: [23/10/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"
#include "Core/TypeId.h"
#include "Core/Singleton.h"

//////////////////////////////////////////////////////////////////////////

#include "Core/RTTI/RTTI.h"
#include "Core/RTTI/RTTIObject.h"

//////////////////////////////////////////////////////////////////////////

#include <array>
#include <unordered_map>

//////////////////////////////////////////////////////////////////////////

class Object;
class VertexArray;
class Material;
class Framebuffer;
class FramebufferAttachment;

//////////////////////////////////////////////////////////////////////////

namespace GBuffer
{
    enum Color;
}

//////////////////////////////////////////////////////////////////////////

namespace RenderPassType
{
    enum Enum
    {
        Lit, Unlit, Shadow, Particle, SSAO, SSL, Debug, Custom, Final, Max,
    };
}

//////////////////////////////////////////////////////////////////////////

class RenderFunction: public RTTIObject
{
    DECLARE_DERIVED(RenderFunction, RTTIObject)
public:
    virtual void ExecuteRender(RenderPassType::Enum pass, Ref<Material> matOverride = nullptr) {}

public:
    bool enabled = false;
};

//////////////////////////////////////////////////////////////////////////

class RenderPassBase: public RTTIObject
{
    DECLARE_DERIVED(RenderPassBase, RTTIObject)
public:
    RenderPassBase();
    virtual ~RenderPassBase() {}

public:
    virtual void OnRenderCreate();

public:
    virtual void OnBegin();
    virtual void OnPerform();
    virtual void OnFinish();

public:
    virtual const HashedString& GetId() const { return id; }
    virtual s32 GetPriority() const { return 0; }
    virtual RenderPassType::Enum GetType() const { return RenderPassType::Max; }
    virtual Ref<FramebufferAttachment> GetAttachment(GBuffer::Color attachment) { return nullptr; }

private:
    HashedString id;
};

//////////////////////////////////////////////////////////////////////////
