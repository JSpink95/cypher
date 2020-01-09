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

#include <array>
#include <unordered_map>

//////////////////////////////////////////////////////////////////////////

class Object;
class VertexArray;
class Material;
class Framebuffer;

//////////////////////////////////////////////////////////////////////////

namespace RenderPassType
{
    enum Enum
    {
        Opaque, Transparent, Shadow, Particle, Max,
    };
}

//////////////////////////////////////////////////////////////////////////

class RenderPass
{
public:
    RenderPass(RenderPassType::Enum inType)
        : type(inType)
    {}

    void RegisterObject(Ref<Object> object);
    void DeregisterObject(Ref<Object> object);

    void Perform();

public:
    // final image render is not affected by this flag
    bool enabled = true;

    // set this if you want to use a specific material for this pass
    // e.g. a shadow output material for the shadow pass.
    Ref<Material> renderMaterialOverride = nullptr;

private:
    RenderPassType::Enum type;
    std::unordered_map<ObjectId, WeakRef<Object>> objects;
};

//////////////////////////////////////////////////////////////////////////

class RenderPassManager : public Singleton<RenderPassManager>
{
private:
    struct SsaoInput
    {
        static inline constexpr const u32 KernelSize = 32;
        vec4 kernel[KernelSize];
        vec2 noiseScale;
    };

    struct BlendInput
    {
        vec2 screenSize;
        vec2 blendMode;
    };

public:
    static void Initialise();
    static void Render();

    static void AddObjectToPass(RenderPassType::Enum pass, Ref<Object> object);
    static void RemoveObjectFromPass(RenderPassType::Enum  pass, Ref<Object> object);

public:
    RenderPassManager();

    std::array<RenderPass, RenderPassType::Max> renderPasses;

private:
    void InitialiseImpl();
    void RenderImpl();
    void RenderTask_Initialise();

private:
    bool hasBeenInitialised = false;

	float2 windowSize;
	float2 framebufferSize;

    Ref<Framebuffer> mainFramebuffer;
    Ref<Framebuffer> lightFramebuffer;
    Ref<Framebuffer> sslFramebuffer;
    Ref<Framebuffer> ssaoFramebuffer;
    Ref<Framebuffer> particleFramebuffer;
    Ref<Framebuffer> blurFramebuffer;
    Ref<Framebuffer> blendFramebuffer;

    Ref<VertexArray> screenQuad;
    Ref<Material> sslMaterial;
    Ref<Material> ssaoMaterial;
    Ref<Material> imageBlurMaterial;
    Ref<Material> imageBlendMaterial;
    Ref<Material> imageRenderMaterial;
};

//////////////////////////////////////////////////////////////////////////

static inline Ref<RenderPassManager> GetRenderPassManager() { return RenderPassManager::Get(); }

//////////////////////////////////////////////////////////////////////////
