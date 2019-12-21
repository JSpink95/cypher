//////////////////////////////////////////////////////////////////////////
//    File        	: MaterialParameter.h
//    Created By    : Jack Spink
//    Created On 	: [23/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Core/Types.h"
#include "Render/Platform/Shader.h"

//////////////////////////////////////////////////////////////////////////

#include <string>

//////////////////////////////////////////////////////////////////////////

class Shader;

//////////////////////////////////////////////////////////////////////////

#define DECLARE_MATERIAL_PARAMETER(type) public: using value_type = type;

//////////////////////////////////////////////////////////////////////////

class MaterialParameter
{
public:
    MaterialParameter(const std::string& id);
    virtual ~MaterialParameter() = default;

    virtual void SetParameter(const Ref<Shader> &shader) = 0;
    virtual Ref<MaterialParameter> Clone() = 0;

protected:
    std::string parameterId;
};

//////////////////////////////////////////////////////////////////////////

class MaterialParameterTexture2D : public MaterialParameter
{
    DECLARE_MATERIAL_PARAMETER(Ref<Texture2D>)
public:
    MaterialParameterTexture2D(const std::string& id);

    virtual void SetParameter(const Ref<Shader>& shader) override;
    virtual Ref<MaterialParameter> Clone() override;

public:
    void SetBlockBinding(const u32 newBinding);
    void SetData(const Ref<Texture2D>& newTexture);

private:
    u32 binding;
    Ref<Texture2D> texture;
};

//////////////////////////////////////////////////////////////////////////

struct BlockBuffer
{
    size_t size = 0u;
    void* data = nullptr;
};

//////////////////////////////////////////////////////////////////////////

class MaterialParameterBlock : public MaterialParameter
{
    DECLARE_MATERIAL_PARAMETER(BlockBuffer)
public:
    MaterialParameterBlock(const std::string& id);

    virtual void SetParameter(const Ref<Shader> &shader) override;
    virtual Ref<MaterialParameter> Clone() override;

public:
    void SetBlockBinding(const u32 newBinding);
    void SetDataGeneric(const BlockBuffer& newBuffer);

    template<typename T>
    void SetData(T& newData)
    {
        BlockBuffer newBuffer;
        newBuffer.size = sizeof(T);
        newBuffer.data = &newData;

        SetDataGeneric(newBuffer);
    }

private:
    u32 binding;
    BlockBuffer buffer;
};

//////////////////////////////////////////////////////////////////////////
