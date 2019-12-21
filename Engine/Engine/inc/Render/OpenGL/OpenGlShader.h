//////////////////////////////////////////////////////////////////////////
//    File        	: OpenGlShader.h
//    Created By    : Jack Spink
//    Created On 	: [3/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Render/Platform/Shader.h"

//////////////////////////////////////////////////////////////////////////

#include <string>

//////////////////////////////////////////////////////////////////////////

#include "pugixml.hpp"
#include "Core/Types.h"
#include <unordered_map>

//////////////////////////////////////////////////////////////////////////

class OpenGlShader : public Shader
{
private:
    struct UBO
    {
        u32 id;
        s32 index;
        s32 binding;
    };

public:
    OpenGlShader(const std::string& filepath);
    OpenGlShader(const pugi::xml_node& shaderRoot);
    ~OpenGlShader();

    virtual void Bind() override;
    virtual void Unbind() override;

    virtual void SetTexture(const std::string& name, Ref<Texture2D> texture, u32 binding) override;

    virtual void* Lock(const std::string& name, const u32 size) override;
    virtual void Unlock(const std::string& name) override;
    virtual void Upload(const std::string& name, const u32 size, const void* data) override;

    virtual void OnDestroy() override;

public:

    // 
    // opengl specific stuff
    // 

    virtual void UploadParameter(const std::string& name, const float2& vector) override;
    virtual void UploadParameter(const std::string& name, const float3& vector) override;
	virtual void UploadParameter(const std::string& name, const fmat4& matrix) override;
    virtual void UploadParameter(const std::string& name, const uint textureId, const uint textureLocation) override;

	virtual void UploadParameterArray(const std::string& name, const std::vector<f32>& floatArray) override;
	virtual void UploadParameterArray(const std::string& name, const std::vector<float3>& float3Array) override;

private:
    s32 id;
    std::unordered_map<std::string, s32> uniforms;
    std::unordered_map<std::string, UBO> uniformBlocks;

    s32 FindOrAddParameter(const std::string& name);
    UBO FindOrAddBlock(const std::string& name, const u32 allocSize);
};

//////////////////////////////////////////////////////////////////////////
