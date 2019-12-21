//////////////////////////////////////////////////////////////////////////
//    File        	: Renderer.h
//    Created By    : Jack Spink
//    Created On 	: [18/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"
#include "Render/Platform/Material.h"
#include "Render/Platform/CoreMaterialParameterBlocks.h"

//////////////////////////////////////////////////////////////////////////

class Shader;
class VertexArray;
class StaticMeshObject;
class Framebuffer;

//////////////////////////////////////////////////////////////////////////

class Renderer
{
private:
    struct RendererSceneData
    {
        Ref<Framebuffer> framebuffer;

        RendererSceneData()
            : framebuffer(nullptr)
        {}
    };

public:

    static void BeginScene(Ref<Framebuffer> framebuffer);
    static void EndScene();

    static void Submit(Ref<Material> material, Ref<VertexArray> mesh, const fmat4& transform = fmat4(1.0f));

#ifdef _OPENGL
    static void SetRenderMode(s32 const mode);
    static s32 renderMode;
#endif

    static const fmat4& GetViewMatrix() { return perFrameProperties.view; }
    static const fmat4& GetProjectionMatrix() { return perFrameProperties.projection; }
    static const fmat4& GetViewProjectionMatrix() { return perFrameProperties.viewProjection; }

    // 
    // PerFrame property setters
    // 

    static void SetViewMatrix(const fmat4& view);
    static void SetProjectionMatrix(const fmat4& projection);
    static void SetViewProjectionMatrix(const fmat4& view, const fmat4& projection);

    // 
    // Time property setters
    // 

    static void SetTimeAndDeltaTime(const f32 time, const f32 dt);

private:
    static inline RendererSceneData activeSceneData;
    static inline MaterialFrameProperties perFrameProperties;
    static inline MaterialTimeProperties timeProperties;
};

//////////////////////////////////////////////////////////////////////////

// 
// #todo - RenderCommand: move to a separate file
// 

class RenderCommand
{
public:
    static void Clear(const s32 code);
};

//////////////////////////////////////////////////////////////////////////
