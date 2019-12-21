//////////////////////////////////////////////////////////////////////////
//    File        	: Shader.h
//    Created By    : Jack Spink
//    Created On 	: [3/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"
#include "Core/Disposable.h"

//////////////////////////////////////////////////////////////////////////

class Texture2D;

//////////////////////////////////////////////////////////////////////////

class Shader: public IDisposable
{
public:
    virtual ~Shader() = default;

    virtual void Bind() = 0;
    virtual void Unbind() = 0;

    virtual void SetTexture(const std::string& name, Ref<Texture2D> texture, u32 binding) = 0;

    virtual void* Lock(const std::string& name, const u32 size) = 0;
    virtual void Unlock(const std::string& name) = 0;
    virtual void Upload(const std::string& name, const u32 size, const void* data) = 0;


    template<typename T>
    inline T* Lock(const std::string& name, const u32 size)
    {
        return static_cast<T*>(Lock(name, size));
    }

public:

    // 
    // opengl specific stuff, this should be removed and replaced with a better solution
    // 

    virtual void UploadParameter(const std::string& name, const float2& vector) = 0;
    virtual void UploadParameter(const std::string& name, const float3& vector) = 0;
	virtual void UploadParameter(const std::string& name, const fmat4& matrix) = 0;
    virtual void UploadParameter(const std::string& name, const uint textureId, const uint textureLocation) = 0;

	virtual void UploadParameterArray(const std::string& name, const std::vector<f32>& floatArray) = 0;
	virtual void UploadParameterArray(const std::string& name, const std::vector<float3>& float3Array) = 0;
};

//////////////////////////////////////////////////////////////////////////
